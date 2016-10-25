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

#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

#include <ipfs/client.h>
#include <ipfs/http/transport-curl.h>

namespace ipfs {

Client::Client(const std::string& host, long port)
    : url_prefix_("http://" + host + ":" + std::to_string(port) + "/api/v0") {
  http_ = new http::TransportCurl();
}

Client::~Client() { delete http_; }

void Client::Id(Json* id) {
  FetchAndParseJson(url_prefix_ + "/id?stream-channels=true", id);
}

void Client::Version(Json* version) {
  FetchAndParseJson(url_prefix_ + "/version?stream-channels=true", version);
}

void Client::Get(const std::string& path, std::iostream* response) {
  http::Response http_response(response);

  std::string path_url_encoded;
  http_->UrlEncode(path, &path_url_encoded);

  const std::string url =
      url_prefix_ + "/cat?stream-channels=true&arg=" + path_url_encoded;

  http_->Get(url, &http_response);
}

void Client::FetchAndParseJson(const std::string& url, Json* response) {
  std::stringstream body_stream;
  http::Response http_response(&body_stream);

  http_->Get(url, &http_response);

  ParseJson(body_stream.str(), response);
}

void Client::ParseJson(const std::string& input, Json* result) {
  try {
    *result = Json::parse(input);
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string(e.what()) + "\nInput JSON:\n" + input);
  }
}
}
