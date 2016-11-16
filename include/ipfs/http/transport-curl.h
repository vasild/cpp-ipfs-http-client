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
#include <iostream>
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

  /** Fetch the contents of a given URL. If any files are provided in `files`,
   * they are submitted using "Content-Type: multipart/form-data".
   *
   * @throw std::exception if any error occurs including erroneous HTTP status
   * code */
  void Fetch(
      /** [in] URL to get. */
      const std::string& url,
      /** [in] List of files to upload. */
      const std::vector<FileUpload>& files,
      /** [out] Output to save the response body to. */
      std::iostream* response) override;

  /** URL encode a string. */
  void UrlEncode(
      /** [in] Input string to encode. */
      const std::string& raw,
      /** [out] URL encoded result. */
      std::string* encoded) override;

  /** Test the internals that are hard to execute from the public API, like
   * error handling of some exceptional cases. */
  void Test();

 private:
  /** Setup the CURL handle `curl_`. */
  void HandleSetup();

  /** Do the actual HTTP request. The CURL handle must have been configured when
   * this method is called. The method will also check for successful HTTP
   * status code and throw an exception if something goes wrong. */
  void Perform(
      /** [in] URL to retrieve. */
      const std::string& url,
      /** [in,out] Response from the web server. */
      std::iostream* response);

  /** Destroy the CURL handle `curl_`. */
  void HandleDestroy();

  /** CURL handle. */
  CURL* curl_;

  /** Designates whether the CURL handle `curl_` has been set up. */
  bool curl_is_setup_;

  /** The CURL error buffer. */
  char curl_error_[CURL_ERROR_SIZE];

  /** Flag to cause `UrlEncode()` to fail miserably. */
  bool url_encode_injected_failure = false;

  /** Flag to cause `HandleSetup()` to fail miserably. */
  bool handle_setup_injected_failure = false;

  /** Flag to cause `Perform()` to fail miserably. */
  bool perform_injected_failure = false;
};

} /* namespace http */
} /* namespace ipfs */

#endif /* IPFS_HTTP_TRANSPORT_CURL_H */
