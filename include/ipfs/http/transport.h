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

#ifndef IPFS_HTTP_TRANSPORT_H
#define IPFS_HTTP_TRANSPORT_H

#include <iostream>
#include <string>
#include <vector>

namespace ipfs {

namespace http {

/** HTTP file upload. */
struct FileUpload {
  /** The type of the `data` member. */
  enum class Type {
    /** The file contents, put into a string by the caller. For small files. */
    kFileContents,
    /** File whose contents is streamed to the web server. For big files. */
    kFileName,
  };

  /** File name to pretend to the web server. */
  const std::string path;

  /** The type of the `data` member. */
  Type type;

  /** The data to be added. Either a file name from which to read the data or
   * the contents itself. */
  const std::string data;
};

/** Convenience interface for talking basic HTTP. */
class Transport {
 public:
  /** Destructor. */
  virtual inline ~Transport();

  /** Fetch the contents of a given URL. If any files are provided in `files`,
   * they are submitted using "Content-Type: multipart/form-data".
   *
   * @throw std::exception if any error occurs including erroneous HTTP status
   * code */
  virtual void Fetch(
      /** [in] URL to get. */
      const std::string& url,
      /** [in] List of files to upload. */
      const std::vector<FileUpload>& files,
      /** [out] Output to save the response body to. */
      std::iostream* response) = 0;

  /** URL encode a string. */
  virtual void UrlEncode(
      /** [in] Input string to encode. */
      const std::string& raw,
      /** [out] URL encoded result. */
      std::string* encoded) = 0;
};

inline Transport::~Transport() {}

} /* namespace http */
} /* namespace ipfs */

#endif /* IPFS_HTTP_TRANSPORT_H */
