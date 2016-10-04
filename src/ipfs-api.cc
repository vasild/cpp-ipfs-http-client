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

#include "ipfs-api.h"

namespace ipfs {

class CurlGlobal {
 public:
  CurlGlobal();
  ~CurlGlobal();

  CURLcode result;
};

CurlGlobal::CurlGlobal() { result = curl_global_init(CURL_GLOBAL_ALL); }
CurlGlobal::~CurlGlobal() { curl_global_cleanup(); }

// Call curl_global_init() before any other threads have started executing.
// This does not work with DLLs on Windows, see https://github.com/curl/curl/issues/586
static const CurlGlobal curl_global;

Ipfs::Ipfs(const std::string& host, long port, Protocol)
    : host_("http://" + host + "/api/v0"), port_(port) {}

static size_t curl_cb(char* ptr, size_t size, size_t nmemb,
                      void* response_void) {
  std::string* response = static_cast<std::string*>(response_void);

  const size_t n = size * nmemb;
  try {
    response->append(ptr, n);
  } catch (...) {
    return 0;
  }

  return n;
}

bool Ipfs::Id(std::string* response, std::string* err) {
  if (curl_global.result != CURLE_OK) {
    *err = "curl_global_init() failed";
    return false;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    *err = "curl_easy_init() failed";
    return false;
  }

  /* https://curl.haxx.se/libcurl/c/CURLOPT_URL.html */
  curl_easy_setopt(curl, CURLOPT_URL, (host_ + "/id?encoding=json").c_str());

  /* https://curl.haxx.se/libcurl/c/CURLOPT_PORT.html */
  curl_easy_setopt(curl, CURLOPT_PORT, port_);

  /* Enable TCP keepalive.
  https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPALIVE.html */
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1);

  /* Seconds to wait before sending keep-alive packets.
  https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPIDLE.html */
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 30);

  /* Seconds between keep-alive probes.
  https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPINTVL.html */
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 10);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_POST.html */
  curl_easy_setopt(curl, CURLOPT_POST, 1);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_USERAGENT.html */
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "cpp-ipfs-api");

  /* https://curl.haxx.se/libcurl/c/CURLOPT_ERRORBUFFER.html */
  char error_buf[CURL_ERROR_SIZE];
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html */
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);

  /* https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html */
  std::string response_;
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    *err = curl_easy_strerror(res) + std::string(": ") + error_buf;
    curl_easy_cleanup(curl);
    return false;
  }

  *response = response_;

  curl_easy_cleanup(curl);

  return true;
}
}
