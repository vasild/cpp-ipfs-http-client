/* Copyright (c) 2016-2016, Vasil Dimov

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

#ifndef IPFS_HTTP_TRANSPORT_CURL_H
#define IPFS_HTTP_TRANSPORT_CURL_H

#include <curl/curl.h>
#include <string>
#include <vector>

#include <ipfs/http/transport.h>

namespace ipfs {

namespace http {

/** Convenience class for talking basic HTTP, implemented using CURL. */
class TransportCurl : public Transport {
 public:
  /** Constructor. */
  TransportCurl();

  /** Destructor. */
  ~TransportCurl();

  /** Fetch the contents of a given URL using the HTTP GET method and stream it
   * into `response.body_`. */
  void Get(
      /** [in] URL to get. */
      const std::string& url,
      /** [out] Output to save the response body and status code to. */
      Response* response) override;

  /** Submit a given content to a given URL using the HTTP POST method.
   * Use "Content-Type: multipart/form-data". */
  void Post(
      /** [in] URL to post to. */
      const std::string& url,
      /** [in] List of files to upload. */
      const std::vector<FileUpload>& files,
      /** [out] Output to save the response body and status code to. */
      Response* response) override;

  /** URL encode a string. */
  void UrlEncode(
      /** [in] Input string to encode. */
      const std::string& raw,
      /** [out] URL encoded result. */
      std::string* encoded) override;

 private:
  /** Setup the CURL handle `curl_`. */
  void HandleSetup();

  /** Do the actual HTTP request. The CURL handle must have been configured when
   * this method is called. The method will also check for successful HTTP status
   * code and throw an exception if something goes wrong. */
  void Perform(
      /** [in] URL to retrieve. */
      const std::string& url,
      /** [in,out] Response from the web server. */
      Response* response);

  /** Destroy the CURL handle `curl_`. */
  void HandleDestroy();

  /** CURL handle. */
  CURL* curl_;

  /** Designates whether the CURL handle `curl_` has been set up. */
  bool curl_is_setup_;

  /** The CURL error buffer. CURL_ERROR_SIZE from curl/curl.h is 256. We do not
   * want to include that header here to avoid imposing that requirement on the
   * users of this library. We have a static_assert in http/transport.cc to
   * ensure that this is big enough. */
  char curl_error_[256];
};

} /* namespace http */
} /* namespace ipfs */

#endif /* IPFS_HTTP_TRANSPORT_CURL_H */
