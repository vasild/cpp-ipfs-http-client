
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <ipfs/cluster.h>
#include <ipfs/http/transport-curl.h>
#include <ipfs/http/transport.h>

namespace ipfs {

Cluster::Cluster(const std::string& host, long port)
    : url_prefix_("http://" + host + ":" + std::to_string(port)) {
  http_ = new http::TransportCurl();
}

Cluster::~Cluster() { delete http_; }

void Cluster::Id(Json* id) { FetchAndParseJson(MakeUrl("id"), id); }

void Cluster::Version(Json* version) {
  FetchAndParseJson(MakeUrl("version"), version);
}

void Cluster::Peers(Json* json) { FetchAndParseJson(MakeUrl("peers"), json); }

void Cluster::PeerRm(const std::string& peerId, Json* json) {
  DeleteAndParseJson(MakeUrl("peers/" + peerId), json);
}

void Cluster::PeerAdd(const std::string& peerId, Json* reponse) {
  FetchAndParseJson(MakeUrl("add", {{"Id", peerId}}), reponse);
}

void Cluster::FetchAndParseJson(const std::string& url, Json* response) {
  std::stringstream body;

  http_->Get(url, &body);

  ParseJson(body.str(), response);
}

void Cluster::FetchAndParseJson(const std::string& url,
                                const std::vector<http::FileUpload>& files,
                                Json* response) {
  std::stringstream body;

  http_->Fetch(url, files, &body);

  ParseJson(body.str(), response);
}

void Cluster::DeleteAndParseJson(const std::string& url, Json* response) {
  std::stringstream body;

  http_->Delete(url, &body);

  ParseJson(body.str(), response);
}

void Cluster::ParseJson(const std::string& input, Json* result) {
  try {
    *result = Json::parse(input);
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string(e.what()) + "\nInput JSON:\n" + input);
  }
}

template <class PropertyType>
void Cluster::GetProperty(const Json& input, const std::string& property_name,
                          size_t line_number, PropertyType* property_value) {
  if (input.find(property_name) == input.end()) {
    throw std::runtime_error(
        std::string("Unexpected reply: valid JSON, but without the \"") +
        property_name + "\" property on line " + std::to_string(line_number) +
        ":\n" + input.dump());
  }

  *property_value = input[property_name];
}

std::string Cluster::MakeUrl(
    const std::string& path,
    const std::vector<std::pair<std::string, std::string>>& parameters) {
  std::string url = url_prefix_ + "/" + path +
                    "?stream-channels=true&json=true&encoding=json";

  for (auto& parameter : parameters) {
    std::string name_url_encoded;
    http_->UrlEncode(parameter.first, &name_url_encoded);

    std::string value_url_encoded;
    http_->UrlEncode(parameter.second, &value_url_encoded);

    url += "&" + name_url_encoded + "=" + value_url_encoded;
  }

  return url;
}
}  // namespace ipfs
