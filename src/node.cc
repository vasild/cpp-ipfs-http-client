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

#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <hive/http/transport-curl.h>
#include <hive/http/transport.h>
#include <hive/node.h>

namespace ipfs {

Node::Node(const std::string& host, long port)
    : url_prefix_("http://" + host + ":" + std::to_string(port) + "/api/v0") {
  http_ = new http::TransportCurl();
}

Node::~Node() { delete http_; }

void Node::Id(Json* id) { FetchAndParseJson(MakeUrl("id"), id); }

void Node::Version(Json* version) {
  FetchAndParseJson(MakeUrl("version"), version);
}

void Node::ConfigGet(const std::string& key, Json* config) {
  std::string url;

  if (key.empty()) {
    url = MakeUrl("config/show");
  } else {
    url = MakeUrl("config", {{"arg", key}});
  }

  FetchAndParseJson(url, config);

  if (!key.empty()) {
    /* Convert
    {
      "Key": "Datastore",
      "Value": { "BloomFilterSize": 0, "GCPeriod": "1h", ... }
    }

    to

    { "BloomFilterSize": 0, "GCPeriod": "1h", ... }
    */
    GetProperty(*config, "Value", 0, config);
  }
}

void Node::ConfigSet(const std::string& key, const Json& value) {
  Json unused;
  FetchAndParseJson(MakeUrl("config", {{"arg", key}, {"arg", value.dump()}}),
                    &unused);
}

void Node::ConfigReplace(const Json& config) {
  std::stringstream unused;
  http_->Fetch(MakeUrl("config/replace"),
               {{"new_config.json", http::FileUpload::Type::kFileContents,
                 config.dump()}},
               &unused);
}

void Node::DhtFindPeer(const std::string& peer_id, Json* addresses) {
  std::stringstream body;

  http_->Fetch(MakeUrl("dht/findpeer", {{"arg", peer_id}}), {}, &body);

  /* Find the addresses of the requested peer in the response. It consists
  of many lines like this:

  {..., "Responses":[{"Addrs":["...","..."],"ID":"peer_id"}], ...}

  */
  std::string line;
  while (std::getline(body, line)) {
    Json json_chunk;

    ParseJson(line, &json_chunk);

    if (json_chunk["Responses"].is_array()) {
      for (auto& r : json_chunk["Responses"]) {
        if (r["ID"] == peer_id) {
          *addresses = r["Addrs"];
          return;
        }
      }
    }
  }

  throw std::runtime_error("Could not find info for peer " + peer_id +
                           " in response: " + body.str());
}

void Node::DhtFindProvs(const std::string& hash, Json* providers) {
  std::stringstream body;

  http_->Fetch(MakeUrl("dht/findprovs", {{"arg", hash}}), {}, &body);

  /* The reply consists of multiple lines, each one of which is a JSON, for
  example:

  {"Extra":"","ID":"QmfPZcnVAEjXABiA7StETRUKkS8FzNt968Z8HynbJR7oci","Responses":null,"Type":6}
  {"Extra":"","ID":"QmfSUo8FkKDTE8T3uhXfQUiyTz7JuMrkUFpTwLM7LLidXG","Responses":null,"Type":6}
  {"Extra":"","ID":"QmWmJvCpjMuBZX4MYWupb9GB3qNYVa1igYCsAQfSHmFJde","Responses":null,"Type":0}

  we convert that into a single JSON like:

  [
    {"Extra":"","ID":"QmfPZcnVAEjXABiA7StETRUKkS8FzNt968Z8HynbJR7oci","Responses":null,"Type":6},
    {"Extra":"","ID":"QmfSUo8FkKDTE8T3uhXfQUiyTz7JuMrkUFpTwLM7LLidXG","Responses":null,"Type":6},
    {"Extra":"","ID":"QmWmJvCpjMuBZX4MYWupb9GB3qNYVa1igYCsAQfSHmFJde","Responses":null,"Type":0}
  ]
  */

  std::string line;
  while (std::getline(body, line)) {
    Json json_chunk;

    ParseJson(line, &json_chunk);

    providers->push_back(json_chunk);
  }
}

void Node::BlockGet(const std::string& block_id, std::iostream* block) {
  http_->Fetch(MakeUrl("block/get", {{"arg", block_id}}), {}, block);
}

void Node::BlockPut(const http::FileUpload& block, Json* stat) {
  FetchAndParseJson(MakeUrl("block/put"), {block}, stat);
}

void Node::BlockStat(const std::string& block_id, Json* stat) {
  FetchAndParseJson(MakeUrl("block/stat", {{"arg", block_id}}), stat);
}

void Node::FileLs(const std::string& path, Json* fileList) {
  FetchAndParseJson(MakeUrl("file/ls", {{"arg", path}}), fileList);
}

void Node::FileGet(const std::string& path, std::iostream* response) {
  http_->Fetch(MakeUrl("cat", {{"arg", path}}), {}, response);
}

void Node::FileAdd(const std::vector<http::FileUpload>& files, Json* result) {
  std::stringstream body;

  http_->Fetch(MakeUrl("add", {{"progress", "true"}}), files, &body);

  /* The reply consists of multiple lines, each one of which is a JSON, for
  example:

  {"Name":"foo.txt","Bytes":4}
  {"Name":"foo.txt","Hash":"QmWPyMW2u7J2Zyzut7TcBMT8pG6F2cB4hmZk1vBJFBt1nP"}
  {"Name":"bar.txt","Bytes":1176}
  {"Name":"bar.txt","Hash":"QmVjQsMgtRsRKpNM8amTCDRuUPriY8tGswsTpo137jPWwL"}

  we convert that into a single JSON like:

  [
    { "path": "foo.txt", "hash": "QmWP...", "size": 4 },
    { "path": "bar.txt", "hash": "QmVj...", "size": 1176 }
  ]

  and return it to the caller. */

  /* A temporary JSON object to facilitate creating the result in case the
  reply lines are out of order. This one looks like:
  {
    "foo.txt": { "path": "foo.txt", "hash": "QmWP...", "size": 4 }
    "bar.txt": { "path": "foo.txt", "hash": "QmVj...", "size": 1176 }
  }
  */
  Json temp;

  std::string line;
  for (size_t i = 1; std::getline(body, line); ++i) {
    Json json_chunk;

    ParseJson(line, &json_chunk);

    std::string path;
    GetProperty(json_chunk, "Name", i, &path);

    temp[path]["path"] = path;

    static const char* hash = "Hash";
    if (json_chunk.find(hash) != json_chunk.end()) {
      temp[path]["hash"] = json_chunk[hash];
    }

    static const char* bytes = "Bytes";
    if (json_chunk.find(bytes) != json_chunk.end()) {
      temp[path]["size"] = json_chunk[bytes];
    }
  }

  for (Json::iterator it = temp.begin(); it != temp.end(); ++it) {
    result->push_back(it.value());
  }
}

void Node::UIDNew(Json* json) {
  FetchAndParseJson(MakeUrl("uid/new", {}), {}, json);
}

void Node::UidLogin(const std::string& uid, Json* json) {
  FetchAndParseJson(MakeUrl("uid/login", {{"uid", uid}}), {}, json);
}

void Node::UidInfo(const std::string& uid, Json* json) {
  FetchAndParseJson(MakeUrl("uid/info", {{"uid", uid}}), {},  json);
}

void Node::FilesLs(const std::string& uid, const std::string& path,
                   Json* json) {
  FetchAndParseJson(MakeUrl("files/ls", {{"uid", uid}, {"path", path}}), {},
                    json);
}

void Node::FilesCp(const std::string& uid, const std::string& source,
                   const std::string& destination, Json* json) {
  FetchAndParseJson(
      MakeUrl("files/cp",
              {{"uid", uid}, {"source", source}, {"dest", destination}}),
      {}, json);
}

void Node::FilesMkdir(const std::string& uid, const std::string& path,
                      Json* json) {
  FetchAndParseJson(MakeUrl("files/mkdir", {{"uid", uid}, {"path", path}}), {},
                    json);
}

void Node::FilesMv(const std::string& uid, const std::string& path,
                   const std::string& dest, Json* json) {
  FetchAndParseJson(
      MakeUrl("files/mv", {{"uid", uid}, {"source", path}, {"dest", dest}}), {},
      json);
}

void Node::FilesRead(const std::string& uid, const std::string& path,
                     int64_t offset, int64_t count,
                     std::stringstream* content) {
  http_->Fetch(MakeUrl("files/read", {{"uid", uid},
                                      {"path", path},
                                      {"offset", std::to_string(offset)},
                                      {"count", std::to_string(count)}}),
               {}, content);
}

void Node::FilesRm(const std::string& uid, const std::string& path,
                   bool recursive, Json* json) {
  FetchAndParseJson(
      MakeUrl("files/rm", {{"uid", uid},
                           {"path", path},
                           {"recursive", recursive ? "true" : "false"}}),
      {}, json);
}

void Node::FilesStat(const std::string& uid, const std::string& path,
                     Json* json) {
  FetchAndParseJson(MakeUrl("files/stat", {{"uid", uid}, {"path", path}}), {},
                    json);
}

void Node::FilesWrite(const std::string& uid, const std::string& path,
                      const std::string& data, int64_t offset, bool create,
                      bool truncate, Json* json) {
  std::stringstream body;

  http_->Fetch(
      MakeUrl("files/write", {{"uid", uid},
                              {"path", path},
                              {"offset", std::to_string(offset)},
                              {"create", create ? "true" : "false"},
                              {"truncate", truncate ? "true" : "false"}}),
      {{"data", ipfs::http::FileUpload::Type::kFileContents, data}}, &body);
}

void Node::ObjectNew(std::string* object_id) {
  Json response;

  FetchAndParseJson(MakeUrl("object/new"), &response);

  GetProperty(response, "Hash", 0, object_id);
}

void Node::ObjectPut(const Json& object, Json* object_stored) {
  FetchAndParseJson(
      MakeUrl("object/put", {{"inputenc", "json"}}),
      {{"node.json", http::FileUpload::Type::kFileContents, object.dump()}},
      object_stored);
}

void Node::ObjectGet(const std::string& object_id, Json* object) {
  FetchAndParseJson(MakeUrl("object/get", {{"arg", object_id}}), object);
}

void Node::ObjectData(const std::string& object_id, std::string* data) {
  std::stringstream body;

  http_->Fetch(MakeUrl("object/data", {{"arg", object_id}}), {}, &body);

  *data = body.str();
}

void Node::ObjectLinks(const std::string& object_id, Json* links) {
  Json response;

  FetchAndParseJson(MakeUrl("object/links", {{"arg", object_id}}), &response);

  GetProperty(response, "Links", 0, links);
}

void Node::ObjectStat(const std::string& object_id, Json* stat) {
  FetchAndParseJson(MakeUrl("object/stat", {{"arg", object_id}}), stat);
}

void Node::ObjectPatchAddLink(const std::string& source,
                              const std::string& link_name,
                              const std::string& link_target,
                              std::string* cloned) {
  Json response;

  FetchAndParseJson(
      MakeUrl("object/patch/add-link",
              {{"arg", source}, {"arg", link_name}, {"arg", link_target}}),
      &response);

  GetProperty(response, "Hash", 0, cloned);
}

void Node::ObjectPatchRmLink(const std::string& source,
                             const std::string& link_name,
                             std::string* cloned) {
  Json response;

  FetchAndParseJson(
      MakeUrl("object/patch/rm-link", {{"arg", source}, {"arg", link_name}}),
      &response);

  GetProperty(response, "Hash", 0, cloned);
}

void Node::ObjectPatchAppendData(const std::string& source,
                                 const http::FileUpload& data,
                                 std::string* cloned) {
  Json response;

  FetchAndParseJson(MakeUrl("object/patch/append-data", {{"arg", source}}),
                    {data}, &response);

  GetProperty(response, "Hash", 0, cloned);
}

void Node::ObjectPatchSetData(const std::string& source,
                              const http::FileUpload& data,
                              std::string* cloned) {
  Json response;

  FetchAndParseJson(MakeUrl("object/patch/set-data", {{"arg", source}}), {data},
                    &response);

  GetProperty(response, "Hash", 0, cloned);
}

void Node::PinAdd(const std::string& object_id) {
  Json response;

  FetchAndParseJson(MakeUrl("pin/add", {{"arg", object_id}}), &response);

  Json pins_array;
  GetProperty(response, "Pins", 0, &pins_array);

  for (const std::string& pin : pins_array) {
    if (pin == object_id) {
      return;
    }
  }

  throw std::runtime_error(
      "Request to pin \"" + object_id +
      "\" got a result that does not contain it as pinned: " + response.dump());
}

void Node::PinLs(Json* pinned) { FetchAndParseJson(MakeUrl("pin/ls"), pinned); }

void Node::PinLs(const std::string& object_id, Json* pinned) {
  FetchAndParseJson(MakeUrl("pin/ls", {{"arg", object_id}}), pinned);
}

void Node::PinRm(const std::string& object_id, PinRmOptions options) {
  Json response;

  const std::string recursive =
      options == PinRmOptions::RECURSIVE ? "true" : "false";

  FetchAndParseJson(
      MakeUrl("pin/rm", {{"arg", object_id}, {"recursive", recursive}}),
      &response);
}

void Node::SwarmAddrs(Json* addresses) {
  FetchAndParseJson(MakeUrl("swarm/addrs"), addresses);
}

void Node::SwarmConnect(const std::string& peer) {
  Json response;
  FetchAndParseJson(MakeUrl("swarm/connect", {{"arg", peer}}), &response);
}

void Node::SwarmDisconnect(const std::string& peer) {
  Json response;
  FetchAndParseJson(MakeUrl("swarm/disconnect", {{"arg", peer}}), &response);
}

void Node::SwarmPeers(Json* peers) {
  FetchAndParseJson(MakeUrl("swarm/peers"), peers);
}

void Node::NamePublish(const std::string uid, const std::string path,
                       Json* response) {
  FetchAndParseJson(MakeUrl("name/publish", {{"uid", uid},
                                             {"path", path},
                                             {"resolve", "true"},
                                             {"lifetime", "24h"}}),
                    {}, response);
}

void Node::NameResolve(const std::string PeerId, Json* response) {
  FetchAndParseJson(MakeUrl("name/resolve", {{"arg", PeerId}}), {}, response);
}

void Node::FetchAndParseJson(const std::string& url, Json* response) {
  FetchAndParseJson(url, {}, response);
}

void Node::FetchAndParseJson(const std::string& url,
                             const std::vector<http::FileUpload>& files,
                             Json* response) {
  std::stringstream body;

  http_->Fetch(url, files, &body);

  std::string content = body.str();
  if (!content.empty())
    ParseJson(content, response);
  else
    *response = "{ \"Message\": \"No body\" }"_json;
}

void Node::ParseJson(const std::string& input, Json* result) {
  try {
    *result = Json::parse(input);
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string(e.what()) + "\nInput JSON:\n" + input);
  }
}

template <class PropertyType>
void Node::GetProperty(const Json& input, const std::string& property_name,
                       size_t line_number, PropertyType* property_value) {
  if (input.find(property_name) == input.end()) {
    throw std::runtime_error(
        std::string("Unexpected reply: valid JSON, but without the \"") +
        property_name + "\" property on line " + std::to_string(line_number) +
        ":\n" + input.dump());
  }

  *property_value = input[property_name];
}

std::string Node::MakeUrl(
    const std::string& path,
    const std::vector<std::pair<std::string, std::string>>& parameters) {
  // remove params "?stream-channels=true&json=true&encoding=json"
  std::string url = url_prefix_ + "/" + path + "?stream-channels=true";

  for (auto& parameter : parameters) {
    std::string name_url_encoded;
    http_->UrlEncode(parameter.first, &name_url_encoded);

    std::string value_url_encoded;
    http_->UrlEncode(parameter.second, &value_url_encoded);

    url += "&" + name_url_encoded + "=" + value_url_encoded;
  }

  return url;
}
} /* namespace ipfs */
