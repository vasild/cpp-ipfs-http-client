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

#ifndef IPFS_HTTP_TRANSPORT_H
#define IPFS_HTTP_TRANSPORT_H

#include <string>

namespace ipfs {

struct HttpResponse {
  std::string body;
  int status_code;
};

class HttpTransport {
 public:
  HttpTransport();
  ~HttpTransport();

  void Fetch(const std::string& url, HttpResponse* response);

 private:
  void CurlSetup();
  void CurlDestroy();

  void* curl_;
  bool curl_is_setup_;

  /** The CURL error buffer. CURL_ERROR_SIZE from curl/curl.h is 256. We do not
   * want to include that header here to avoid imposing that requirement on the
   * users of this library. We have a static_assert in http-transport.cc to
   * ensure that this is big enough. */
  char curl_error_[256];
};
}

#endif /* IPFS_HTTP_TRANSPORT_H */
