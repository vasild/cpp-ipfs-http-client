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

#ifndef IPFS_API_H
#define IPFS_API_H

#include <nlohmann/json.hpp>
#include <ostream>
#include <string>

#include <ipfs/http-transport.h>

namespace ipfs {

/// Type of the output of some methods, aliased for convenience.
using Json = nlohmann::json;

class Ipfs {
 public:
  Ipfs(const std::string& host, long port);

  void Id(Json* response);

  void Version(Json* response);

  void Get(const std::string& path, std::ostream* response);

 private:
  void FetchJson(const std::string& url, Json* response);

  std::string url_prefix_;
  long port_;
  HttpTransport http_;
};
}

#endif /* IPFS_API_H */
