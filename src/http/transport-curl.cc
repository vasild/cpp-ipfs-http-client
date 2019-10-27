/* Copyright (c) 2016-2019, The C++ IPFS client library developers

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include <curl/curl.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <ipfs/http/transport-curl.h>
#include <ipfs/test/utils.h>

namespace ipfs {

namespace http {

/** CURL global initialization. `curl_global_init()` needs to be called before
 * any other threads have started executing because it is not thread safe and
 * may call other non-thread safe functions. Thus we use a global variable and
 * call `curl_global_init()` in its constructor. */
class CurlGlobal {
 public:
  /** Constructor that calls curl_global_init(). */
  CurlGlobal();

  /** Destructor that calls curl_global_cleanup(). */
  ~CurlGlobal();

  /** The result from curl_global_init() for checking the outcome later. */
  CURLcode result_;
};

CurlGlobal::CurlGlobal() { result_ = curl_global_init(CURL_GLOBAL_ALL); }
CurlGlobal::~CurlGlobal() { curl_global_cleanup(); }

/** Call curl_global_init() before any other threads have started executing.
 * This does not work with DLLs on Windows,
 * @see https://github.com/curl/curl/issues/586. */
static const CurlGlobal curl_global;

/** Flag to simulate a failure of curl_global_init(). */
static bool curl_global_injected_failure = false;

#ifndef NDEBUG
/** Placeholder to inject a fake http response during testing. */
std::string replace_body;
#endif /* NDEBUG */

/** Check if a HTTP status code is 2xx Success.
 * @return true if 2xx */
inline bool status_is_success(long code) { return code >= 200 && code <= 299; }

/** CURL callback for writing the result to a stream. */
static size_t curl_cb_stream(
    /** [in] Pointer to the result. */
    char* ptr,
    /** [in] Size each chunk of the result. */
    size_t size,
    /** [in] Number of chunks in the result. */
    size_t nmemb,
    /** [out] Response (a pointer to `std::iostream`). */
    void* response_void) {
  std::iostream* response = static_cast<std::iostream*>(response_void);

  const size_t n = size * nmemb;

  response->write(ptr, n);

  return n;
}

TransportCurl::TransportCurl() : curl_is_setup_(false) {
  if (curl_global.result_ != CURLE_OK || curl_global_injected_failure) {
    throw std::runtime_error("curl_global_init() failed");
  }
  static_assert(sizeof(curl_error_) >= CURL_ERROR_SIZE,
                "The size of curl_error_ is too small");
}

TransportCurl::~TransportCurl() { HandleDestroy(); }

void TransportCurl::Fetch(const std::string& url,
                          const std::vector<FileUpload>& files,
                          std::iostream* response) {
  HandleSetup();

  /* https://curl.haxx.se/libcurl/c/CURLOPT_POST.html */
  curl_easy_setopt(curl_, CURLOPT_POST, 1);

  curl_httppost* form_parts = NULL;
  curl_httppost* form_parts_end = NULL;

  for (size_t i = 0; i < files.size(); ++i) {
    const FileUpload& file = files[i];
    const std::string name("file" + std::to_string(i));
    static const char* content_type = "application/octet-stream";

    switch (file.type) {
      case FileUpload::Type::kFileContents:
        /* https://curl.haxx.se/libcurl/c/curl_formadd.html */
        curl_formadd(&form_parts, &form_parts_end,
                     /* name="..."; */
                     CURLFORM_COPYNAME, name.c_str(),
                     /* filename="..."; */
                     CURLFORM_BUFFER, file.path.c_str(),
                     /* File contents. */
                     CURLFORM_BUFFERPTR, file.data.c_str(),
                     /* File contents length. */
                     CURLFORM_BUFFERLENGTH, file.data.length(),
                     /* Content-Type: */
                     CURLFORM_CONTENTTYPE, content_type, CURLFORM_END);
        break;
      case FileUpload::Type::kFileName:
        /* https://curl.haxx.se/libcurl/c/curl_formadd.html */
        curl_formadd(&form_parts, &form_parts_end,
                     /* name="..."; */
                     CURLFORM_COPYNAME, name.c_str(),
                     /* filename="..."; */
                     CURLFORM_FILENAME, file.path.c_str(),
                     /* Read the data from this file. */
                     CURLFORM_FILE, file.data.c_str(),
                     /* Content-Type: */
                     CURLFORM_CONTENTTYPE, content_type, CURLFORM_END);
        break;
    }
  }

  /* Auto free the resources occupied by `form_parts`. */
  std::unique_ptr<curl_httppost, void (*)(curl_httppost*)> form_parts_deleter(
      form_parts, [](curl_httppost* d) {
        /* https://curl.haxx.se/libcurl/c/curl_formfree.html */
        curl_formfree(d);
      });

  /* https://curl.haxx.se/libcurl/c/CURLOPT_HTTPPOST.html */
  curl_easy_setopt(curl_, CURLOPT_HTTPPOST, form_parts);

  curl_slist* headers = NULL;
  /* https://curl.haxx.se/libcurl/c/curl_slist_append.html */
  headers = curl_slist_append(headers, "Expect:");

  /* Auto free the resources occupied by `headers`. */
  std::unique_ptr<curl_slist, void (*)(curl_slist*)> headers_deleter(
      headers, [](curl_slist* d) {
        /* https://curl.haxx.se/libcurl/c/curl_slist_free_all.html */
        curl_slist_free_all(d);
      });

  /* https://curl.haxx.se/libcurl/c/CURLOPT_HTTPHEADER.html */
  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

#ifndef NDEBUG
  if (!replace_body.empty()) {
    *response << replace_body;
    return;
  }
#endif /* NDEBUG */

  Perform(url, response);
}

void TransportCurl::UrlEncode(const std::string& raw, std::string* encoded) {
  HandleSetup();

  char* encoded_c = curl_easy_escape(curl_, raw.c_str(), 0);
  if (encoded_c == NULL || url_encode_injected_failure) {
    throw std::runtime_error("curl_easy_escape() failed on \"" + raw + "\"");
  }
  std::unique_ptr<char, void (*)(char*)> encoded_c_deleter(
      encoded_c, [](char* e) { curl_free(e); });

  encoded->assign(encoded_c);
}

void TransportCurl::HandleSetup() {
  if (curl_is_setup_) {
    return;
  }

  curl_ = curl_easy_init();
  if (curl_ == NULL || handle_setup_injected_failure) {
    throw std::runtime_error("curl_easy_init() failed");
  }

  /* https://curl.haxx.se/libcurl/c/CURLOPT_ERRORBUFFER.html */
  curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, curl_error_);

  /* Enable TCP keepalive.
   * https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPALIVE.html */
  curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1);

  /* Seconds to wait before sending keep-alive packets.
   * https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPIDLE.html */
  curl_easy_setopt(curl_, CURLOPT_TCP_KEEPIDLE, 30);

  /* Seconds between keep-alive probes.
   * https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPINTVL.html */
  curl_easy_setopt(curl_, CURLOPT_TCP_KEEPINTVL, 10);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_USERAGENT.html */
  curl_easy_setopt(curl_, CURLOPT_USERAGENT, "cpp-ipfs-http-client");

  curl_is_setup_ = true;
}

void TransportCurl::Perform(const std::string& url, std::iostream* response) {
  /* https://curl.haxx.se/libcurl/c/CURLOPT_URL.html */
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());

  /* https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html */
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, curl_cb_stream);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html */
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, response);

  curl_error_[0] = '\0';

  CURLcode res = curl_easy_perform(curl_);
  if (res != CURLE_OK) {
    const std::string generic_error(curl_easy_strerror(res));
    throw std::runtime_error(
        generic_error +
        (curl_error_[0] != '\0' ? std::string(": ") + curl_error_ : ""));
  }

  long status_code;

  res = curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &status_code);
  if (res != CURLE_OK || perform_injected_failure) {
    throw std::runtime_error(
        std::string("Can't get the HTTP status code from CURL: ") +
        curl_easy_strerror(res));
  }

  if (!status_is_success(status_code)) {
    std::streambuf* b = response->rdbuf();
    throw std::runtime_error(
        "HTTP request failed with status code " + std::to_string(status_code) +
        ". Response body:\n" +
        /* Read the whole body back from the stream where we wrote it and
         * append it to this error message string. Usually the bodies of
         * HTTP error responses represent a short HTML or JSON that
         * describes the error. */
        static_cast<const std::stringstream&>(std::stringstream() << b).str());
  }
}

void TransportCurl::HandleDestroy() {
  if (!curl_is_setup_) {
    return;
  }

  curl_is_setup_ = false;

  curl_easy_cleanup(curl_);
}

void TransportCurl::Test() {
  curl_global_injected_failure = true;
  test::must_fail("TransportCurl::TransportCurl()",
                  []() { TransportCurl transport_curl; });
  curl_global_injected_failure = false;

  test::must_fail("TransportCurl::UrlEncode()", []() {
    TransportCurl c;
    std::string encoded;
    c.url_encode_injected_failure = true;
    c.UrlEncode("nobody can encode me", &encoded);
  });

  test::must_fail("TransportCurl::HandleSetup()", []() {
    TransportCurl c;
    c.handle_setup_injected_failure = true;
    c.Fetch("http://localhost:1234", {}, nullptr);
  });

#ifndef NDEBUG
  test::must_fail("TransportCurl::Perform()", []() {
    TransportCurl c;
    replace_body = "";
    c.perform_injected_failure = true;
    std::stringstream response;
    c.Fetch("http://google.com", {}, &response);
  });
#endif /* NDEBUG */

  TransportCurl c;
  c.HandleSetup();
  c.HandleSetup();
}

} /* namespace http */
} /* namespace ipfs */
