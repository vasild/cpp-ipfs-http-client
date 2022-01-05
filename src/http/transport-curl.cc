/* Copyright (c) 2016-2022, The C++ IPFS client library developers

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
#include <ipfs/http/transport-curl.h>
#include <ipfs/test/utils.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace ipfs {

namespace http {

/** Flag to simulate a failure of curl_global_init(). */
static bool curl_global_injected_failure = false;

#ifndef NDEBUG
/** Placeholder to inject a fake http response during testing. */
std::string replace_body;
#endif /* NDEBUG */

/** Check if a HTTP status code is 2xx Success.
 * @return true if 2xx HTTP status code */
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

TransportCurl::TransportCurl(bool curlVerbose)
    : keep_perform_running_(true), curl_verbose(curlVerbose) {
  global_init_result_ = curl_global_init(CURL_GLOBAL_ALL);
  if (global_init_result_ != CURLE_OK || curl_global_injected_failure) {
    throw std::runtime_error("curl_global_init() failed");
  }

  /* Init a multi stack
   * https://curl.se/libcurl/c/curl_multi_init.html */
  multi_handle_ = curl_multi_init();

  /* Create a cURL easy handle (which we will reuse)
   * https://curl.se/libcurl/c/curl_easy_init.html */
  curl_ = curl_easy_init();

  if (curl_ == NULL) {
    throw std::runtime_error("curl_easy_init() failed");
  }

  if (curl_verbose) {
    /* https://curl.se/libcurl/c/CURLOPT_VERBOSE.html */
    curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
  }

  /* Enable TCP keepalive.
   * https://curl.se/libcurl/c/CURLOPT_TCP_KEEPALIVE.html */
  curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1);

  /* Seconds to wait before sending keep-alive packets.
   * https://curl.se/libcurl/c/CURLOPT_TCP_KEEPIDLE.html */
  curl_easy_setopt(curl_, CURLOPT_TCP_KEEPIDLE, 30);

  /* Seconds between keep-alive probes.
   * https://curl.se/libcurl/c/CURLOPT_TCP_KEEPINTVL.html */
  curl_easy_setopt(curl_, CURLOPT_TCP_KEEPINTVL, 10);

  /* https://curl.se/libcurl/c/CURLOPT_USERAGENT.html */
  curl_easy_setopt(curl_, CURLOPT_USERAGENT, "cpp-ipfs-http-client");

  /* Avoid race condition when used in threading
   * https://curl.se/libcurl/c/threadsafe.html
   * https://curl.se/libcurl/c/CURLOPT_NOSIGNAL.html */
  curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1L);
}

TransportCurl::~TransportCurl() {
  curl_multi_remove_handle(multi_handle_, curl_);
  curl_easy_cleanup(curl_);
  curl_global_cleanup();
}

void TransportCurl::Fetch(const std::string& url,
                          const std::vector<FileUpload>& files,
                          std::iostream* response) {
  /* https://curl.se/libcurl/c/CURLOPT_POST.html */
  curl_easy_setopt(curl_, CURLOPT_POST, 1);

  curl_httppost* form_parts = NULL;
  curl_httppost* form_parts_end = NULL;

  for (size_t i = 0; i < files.size(); ++i) {
    const FileUpload& file = files[i];
    const std::string name("file" + std::to_string(i));
    static const char* content_type = "application/octet-stream";

    switch (file.type) {
      case FileUpload::Type::kFileContents:
        /* https://curl.se/libcurl/c/curl_formadd.html */
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
        /* https://curl.se/libcurl/c/curl_formadd.html */
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
        /* https://curl.se/libcurl/c/curl_formfree.html */
        curl_formfree(d);
      });

  /* https://curl.se/libcurl/c/CURLOPT_HTTPPOST.html */
  curl_easy_setopt(curl_, CURLOPT_HTTPPOST, form_parts);

  curl_slist* headers = NULL;
  /* https://curl.se/libcurl/c/curl_slist_append.html */
  headers = curl_slist_append(headers, "Expect:");

  /* Auto free the resources occupied by `headers`. */
  std::unique_ptr<curl_slist, void (*)(curl_slist*)> headers_deleter(
      headers, [](curl_slist* d) {
        /* https://curl.se/libcurl/c/curl_slist_free_all.html */
        curl_slist_free_all(d);
      });

  /* https://curl.se/libcurl/c/CURLOPT_HTTPHEADER.html */
  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

#ifndef NDEBUG
  if (!replace_body.empty()) {
    *response << replace_body;
    return;
  }
#endif /* NDEBUG */

  Perform(url, response);
}

void TransportCurl::StopFetch() { keep_perform_running_ = false; }

void TransportCurl::UrlEncode(const std::string& raw, std::string* encoded) {
  char* encoded_c = curl_easy_escape(curl_, raw.c_str(), 0);
  if (encoded_c == NULL || url_encode_injected_failure) {
    throw std::runtime_error("curl_easy_escape() failed on \"" + raw + "\"");
  }
  std::unique_ptr<char, void (*)(char*)> encoded_c_deleter(
      encoded_c, [](char* e) { curl_free(e); });

  encoded->assign(encoded_c);
}

void TransportCurl::Perform(const std::string& url, std::iostream* response) {
  keep_perform_running_ = true; /* Set atomic bool to true by default */
  int still_running = 0;        /* keep number of running handles */
  CURLMsg* msg;  /* for picking up messages with the transfer status */
  int msgs_left; /* how many messages are left */
  char curl_error[CURL_ERROR_SIZE]; /* cURL error message buffer */
  std::string generic_error;
  std::vector<std::string> get_info_errors;
  std::vector<std::string> status_code_errors;

  /* https://curl.se/libcurl/c/CURLOPT_URL.html */
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());

  /* https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html */
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, curl_cb_stream);

  /* https://curl.se/libcurl/c/CURLOPT_WRITEDATA.html */
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, response);

  /* https://curl.se/libcurl/c/CURLOPT_ERRORBUFFER.html */
  curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, curl_error);

  /* End of string (empty string) */
  curl_error[0] = '\0';

  /* Add easy handle to multi stack.
   * https://curl.se/libcurl/c/curl_multi_add_handle.html */
  curl_multi_add_handle(multi_handle_, curl_);

  do {
    /* https://curl.se/libcurl/c/curl_multi_perform.html */
    CURLMcode mc = curl_multi_perform(multi_handle_, &still_running);

    /* Allow to break/stop the perform task at any given moment.
     * Very useful if you want to stop this call when running inside a thread.
     */
    if (!keep_perform_running_) break;

    if (!mc && still_running)
      /* wait for activity, timeout or "nothing"
       * https://curl.se/libcurl/c/curl_multi_poll.html */
      mc = curl_multi_poll(multi_handle_, NULL, 0, 40, NULL);

    if (mc) {
      generic_error = std::string(curl_multi_strerror(mc));
      break;
    }

  } while (still_running);

  /* Check for HTTP status code, only if there are no generic errors and the
   * atomic bool is still true */
  if (generic_error.empty() && keep_perform_running_) {
    /* Future-proof - by looping over each easy handle; altough we only use one
     * handle for now. https://curl.se/libcurl/c/curl_multi_info_read.html */
    while ((msg = curl_multi_info_read(multi_handle_, &msgs_left))) {
      if (msg->msg == CURLMSG_DONE) {
        long status_code;

        /* https://curl.se/libcurl/c/curl_easy_getinfo.html */
        CURLcode res = curl_easy_getinfo(msg->easy_handle,
                                         CURLINFO_RESPONSE_CODE, &status_code);
        if (res != CURLE_OK || perform_injected_failure) {
          get_info_errors.push_back(
              "Can't get the HTTP status code from CURL: " +
              std::string(curl_easy_strerror(res)));
        }
        if (!status_is_success(status_code)) {
          std::streambuf* b = response->rdbuf();
          status_code_errors.push_back(
              "HTTP request failed with status code " +
              std::to_string(status_code) + ". Response body:\n" +
              /* Read the whole body back from the stream where we wrote it and
               * append it to this error message string. Usually the bodies of
               * HTTP error responses represent a short HTML or JSON that
               * describes the error. */
              static_cast<const std::stringstream&>(std::stringstream() << b)
                  .str());
        }
      }
    }
  }

  /* Always execute the curl_multi_remove_handle()!
   * https://curl.se/libcurl/c/curl_multi_remove_handle.html */
  curl_multi_remove_handle(multi_handle_, curl_);

  /* If there were errors, throw them now (if atomic bool is still true) */
  if (keep_perform_running_) {
    if (!generic_error.empty()) {
      throw std::runtime_error(
          generic_error +
          (curl_error[0] != '\0' ? std::string(": ") + curl_error : ""));
    } else {
      // For now we just throw the first error we see
      for (const auto& error : get_info_errors) {
        throw std::runtime_error(error);
      }
      for (const auto& error : status_code_errors) {
        throw std::runtime_error(error);
      }
    }
  }
}

void TransportCurl::Test() {
  curl_global_injected_failure = true;
  test::must_fail("TransportCurl::TransportCurl()",
                  []() { TransportCurl transport_curl(false); });
  curl_global_injected_failure = false;

  test::must_fail("TransportCurl::UrlEncode()", []() {
    TransportCurl c(false);
    std::string encoded;
    c.url_encode_injected_failure = true;
    c.UrlEncode("nobody can encode me", &encoded);
  });

#ifndef NDEBUG
  test::must_fail("TransportCurl::Perform()", []() {
    TransportCurl c(false);
    replace_body = "";
    c.perform_injected_failure = true;
    std::stringstream response;
    c.Fetch("http://google.com", {}, &response);
  });
#endif /* NDEBUG */
}

} /* namespace http */
} /* namespace ipfs */
