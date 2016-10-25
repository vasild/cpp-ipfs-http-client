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

#include <iostream>
#include <string>

namespace ipfs {

namespace http {

/**
 * HTTP status code.
 * @see https://en.wikipedia.org/wiki/List_of_HTTP_status_codes.
 */
class Status {
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
 * HTTP response. The body of the response is streamed into the `body_`
 * member as it arrives.
 */
class Response {
 public:
  /** Constructor to prevent creating objects uninitialized `body_`. */
  explicit Response(
      /** [in] pointer to body with which to initialize the `body_` member. */
      std::iostream* body);

  /** HTTP status code. */
  Status status_;

  /** Body of the HTTP response. */
  std::iostream* body_;
};

/** Convenience interface for talking basic HTTP. */
class Transport {
 public:
  /** Destructor. */
  virtual inline ~Transport();

  /**
   * Fetch the contents of a given URL using the HTTP GET method and stream it
   * into `response.body_`.
   */
  virtual void Get(
      /** [in] URL to get. */
      const std::string& url,
      /** [out] Output to save the response body and status code to. */
      Response* response) = 0;

  /** URL encode a string. */
  virtual void UrlEncode(
      /** [in] Input string to encode. */
      const std::string& raw,
      /** [out] URL encoded result. */
      std::string* encoded) = 0;
};

inline bool Status::IsSuccess() { return code_ >= 200 && code_ <= 299; }

inline Response::Response(std::iostream* body) : body_(body) {}

inline Transport::~Transport() {}

} /* namespace http */
} /* namespace ipfs */

#endif /* IPFS_HTTP_TRANSPORT_H */
