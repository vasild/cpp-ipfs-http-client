/*
Copyright (c) 2016-2016, Vasil Dimov

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
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <curl/curl.h>
#include <ostream>
#include <stdexcept>
#include <string>

#include <ipfs/http-transport.h>

namespace ipfs {

/**
 * CURL global initialization.
 * `curl_global_init()` needs to be called before any other threads have
 * started executing because it is not thread safe and may call other
 * non-thread safe functions. Thus we use a global variable and call
 * `curl_global_init()` in its constructor.
 */
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

/**
 * Call curl_global_init() before any other threads have started executing.
 * This does not work with DLLs on Windows,
 * @see https://github.com/curl/curl/issues/586.
 */
static const CurlGlobal curl_global;

/** CURL callback for saving the result as a string. */
static size_t curl_cb_string(
    /** [in] Pointer to the result. */
    char* ptr,
    /** [in] Size each chunk of the result. */
    size_t size,
    /** [in] Number of chunks in the result. */
    size_t nmemb,
    /** [out] Response (a pointer to `std::string`). */
    void* response_void) {
  std::string* response = static_cast<std::string*>(response_void);

  const size_t n = size * nmemb;

  response->append(ptr, n);

  return n;
}

/** CURL callback for writing the result to a stream. */
static size_t curl_cb_stream(
    /** [in] Pointer to the result. */
    char* ptr,
    /** [in] Size each chunk of the result. */
    size_t size,
    /** [in] Number of chunks in the result. */
    size_t nmemb,
    /** [out] Response (a pointer to a pointer to `std::ostream`). */
    void* response_void) {
  std::ostream** response = static_cast<std::ostream**>(response_void);

  const size_t n = size * nmemb;

  (*response)->write(ptr, n);

  return n;
}

HttpTransport::HttpTransport() : curl_is_setup_(false) {
  if (curl_global.result_ != CURLE_OK) {
    throw std::runtime_error("curl_global_init() failed");
  }
  static_assert(sizeof(curl_error_) >= CURL_ERROR_SIZE,
                "The size of curl_error_ is too small");
}

HttpTransport::~HttpTransport() { CurlDestroy(); }

void HttpTransport::Fetch(const std::string& url,
                          HttpResponse<std::string>* response) {
  CurlPerform(url, curl_cb_string, response);
}

void HttpTransport::Stream(const std::string& url,
                           HttpResponse<std::ostream*>* response) {
  CurlPerform(url, curl_cb_stream, response);
}

void HttpTransport::CurlSetup() {
  if (curl_is_setup_) {
    return;
  }

  CURL** curl = static_cast<CURL**>(&curl_);

  *curl = curl_easy_init();
  if (*curl == NULL) {
    throw std::runtime_error("curl_easy_init() failed");
  }

  /* https://curl.haxx.se/libcurl/c/CURLOPT_ERRORBUFFER.html */
  curl_easy_setopt(*curl, CURLOPT_ERRORBUFFER, curl_error_);

  /* Enable TCP keepalive.
  https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPALIVE.html */
  curl_easy_setopt(*curl, CURLOPT_TCP_KEEPALIVE, 1);

  /* Seconds to wait before sending keep-alive packets.
  https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPIDLE.html */
  curl_easy_setopt(*curl, CURLOPT_TCP_KEEPIDLE, 30);

  /* Seconds between keep-alive probes.
  https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPINTVL.html */
  curl_easy_setopt(*curl, CURLOPT_TCP_KEEPINTVL, 10);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_POST.html */
  curl_easy_setopt(*curl, CURLOPT_POST, 1);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_USERAGENT.html */
  curl_easy_setopt(*curl, CURLOPT_USERAGENT, "cpp-ipfs-api");

  curl_is_setup_ = true;
}

template <typename Body>
void HttpTransport::CurlPerform(const std::string& url,
                                size_t (*curl_cb)(char* ptr, size_t size,
                                                  size_t nmemb, void* userdata),
                                HttpResponse<Body>* response) {
  CurlSetup();

  CURL** curl = static_cast<CURL**>(&curl_);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_URL.html */
  curl_easy_setopt(*curl, CURLOPT_URL, url.c_str());

  /* https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html */
  curl_easy_setopt(*curl, CURLOPT_WRITEFUNCTION, curl_cb);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html */
  curl_easy_setopt(*curl, CURLOPT_WRITEDATA, &response->body_);

  CURLcode res = curl_easy_perform(*curl);
  if (res != CURLE_OK) {
    const std::string generic_error(curl_easy_strerror(res));
    CurlDestroy();
    throw std::runtime_error(generic_error + std::string(": ") + curl_error_);
  }

  static_assert(sizeof(long) == sizeof(response->http_status_.code_),
                "Unexpected size of HTTP status code");

  res = curl_easy_getinfo(*curl, CURLINFO_RESPONSE_CODE,
                          &response->http_status_.code_);
  if (res != CURLE_OK) {
    CurlDestroy();
    throw std::runtime_error(
        std::string("Can't get the HTTP status code from CURL: ") +
        curl_easy_strerror(res));
  }
}

void HttpTransport::CurlDestroy() {
  if (!curl_is_setup_) {
    return;
  }

  curl_is_setup_ = false;

  CURL** curl = static_cast<CURL**>(&curl_);

  curl_easy_cleanup(*curl);
}
}
