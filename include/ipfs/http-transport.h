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

#include <ostream>
#include <string>

namespace ipfs {

/**
 * HTTP status code.
 * @see https://en.wikipedia.org/wiki/List_of_HTTP_status_codes.
 */
class HttpStatus {
 public:
  /**
   * Check if the status code is 2xx Success.
   * @return true if 2xx
   */
  inline bool IsSuccess();

  /** The HTTP status code. */
  int code_;
};

/**
 * HTTP response as a string. The whole body of the response is saved into
 * the `body` member. Suitable for fetching small objects.
 */
struct HttpResponseString {
  /** HTTP status code. */
  HttpStatus http_status_;

  /** Body of the HTTP response. */
  std::string body;
};

/**
 * HTTP response streamed. The body of the response is streamed into the `body`
 * member as it arrives. Suitable for fetching large objects to disk without
 * having the entire response in the main memory.
 */
struct HttpResponseStream {
  /** HTTP status code. */
  HttpStatus http_status_;

  /** Body of the HTTP response. */
  std::ostream* body;
};

/** Convenience class for talking basic HTTP. */
class HttpTransport {
 public:
  /** Constructor. */
  HttpTransport();

  /** Destructor. */
  ~HttpTransport();

  /** Fetch the contents of a given URL into a string. */
  void Fetch(
      /** [in] URL to fetch. */
      const std::string& url,
      /** [out] Response, as a string. */
      HttpResponseString* response);

  /** Stream the contents of a given URL into the provided stream. */
  void Stream(
      /** [in] URL to fetch. */
      const std::string& url,
      /** [out] Stream to write the response to. */
      HttpResponseStream* response);

 private:
  /** Setup CURL objects. */
  void CurlSetup();

  /** Destroy CURL objects. */
  void CurlDestroy();

  /** CURL handle. */
  void* curl_;

  /** Designates whether the CURL objects have been set up. */
  bool curl_is_setup_;

  /**
   * The CURL error buffer. CURL_ERROR_SIZE from curl/curl.h is 256. We do not
   * want to include that header here to avoid imposing that requirement on the
   * users of this library. We have a static_assert in http-transport.cc to
   * ensure that this is big enough.
   */
  char curl_error_[256];
};

inline bool HttpStatus::IsSuccess() { return code_ >= 200 && code_ <= 299; }
}
#endif /* IPFS_HTTP_TRANSPORT_H */
