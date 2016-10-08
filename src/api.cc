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

#include <nlohmann/json.hpp>
#include <ostream>
#include <stdexcept>
#include <string>

#include <ipfs/api.h>

namespace ipfs {

Ipfs::Ipfs(const std::string& host, long port)
    : url_prefix_("http://" + host + ":" + std::to_string(port) + "/api/v0") {}

void Ipfs::Id(Json* id) {
  FetchJson(url_prefix_ + "/id?stream-channels=true", id);
}

void Ipfs::Version(Json* version) {
  FetchJson(url_prefix_ + "/version?stream-channels=true", version);
}

void Ipfs::Get(const std::string& path, std::ostream* response) {
  HttpResponse<std::ostream*> http_response;
  http_response.body_ = response;

  const std::string url = url_prefix_ + "/cat?stream-channels=true&arg=" + path;

  http_.Stream(url, &http_response);
}

void Ipfs::FetchJson(const std::string& url, Json* response) {
  HttpResponse<std::string> http_response;

  http_.Fetch(url, &http_response);

  if (!http_response.http_status_.IsSuccess()) {
    throw std::runtime_error("HTTP request failed with status code " +
                             std::to_string(http_response.http_status_.code_) +
                             ". Response body:\n" + http_response.body_);
  }

  try {
    *response = Json::parse(http_response.body_);
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string(e.what()) + "\nresponse body:\n" +
                             http_response.body_);
  }
}
}
