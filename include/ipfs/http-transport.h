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
  long code_;
};

/**
 * HTTP response. The body of the response is either streamed into the `body_`
 * member as it arrives (`std::ostream*`) or appended to it (`std::string`).
 * @tparam Body Desired type in which to return the body of the HTTP response.
 * Either `std::string` or `std::ostream*`.
 */
template <typename Body>
class HttpResponse {
 public:
  /** HTTP status code. */
  HttpStatus http_status_;

  /** Body of the HTTP response. Either `std::string` or `std::ostream*`. */
  Body body_;
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
      HttpResponse<std::string>* response);

  /** Stream the contents of a given URL into the provided stream. */
  void Stream(
      /** [in] URL to stream. */
      const std::string& url,
      /** [out] Stream to write the response to. */
      HttpResponse<std::ostream*>* response);

 private:
  /** Setup the CURL handle `curl_`. */
  void CurlSetup();

  /** CURL perform and check the HTTP status code. */
  template <typename Body>
  void CurlPerform(
      /** [in] URL to retrieve. */
      const std::string& url,
      /** [in,out] CURL callback function to write to `response`. */
      size_t (*curl_cb)(char* ptr, size_t size, size_t nmemb, void* userdata),
      /**
       * [in,out] Response from the web server. The body is either a
       * `std::string` or `std::ostream*`.
       */
      HttpResponse<Body>* response);

  /** Destroy the CURL handle `curl_`. */
  void CurlDestroy();

  /** CURL handle. */
  void* curl_;

  /** Designates whether the CURL handle `curl_` has been set up. */
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
