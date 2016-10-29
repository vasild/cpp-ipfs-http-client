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
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <ipfs/client.h>

/** Check if a given set of properties exist in a JSON. */
static inline void check_if_properties_exist(
    /** [in] Label to use when throwing an exception if a failure occurs. */
    const std::string& label,
    /** [in] JSON to check for the properties. */
    const ipfs::Json& j,
    /** [in] List of properties. */
    const std::vector<const char*>& properties) {
  for (const char* property : properties) {
    if (j.find(property) == j.end()) {
      throw std::runtime_error(label + ": the property \"" + property +
                               "\" was not found in the response:\n" +
                               j.dump(2));
    }
  }
}

/** Check if a string contains another string and throw an exception if it does
 * not. */
static inline void check_if_string_contains(
    /** [in] Label to use when throwing an exception if a failure occurs. */
    const std::string& label,
    /** [in] String to search into. */
    const std::string& big,
    /** [in] String to search for. */
    const std::string& needle) {
  if (big.find(needle) == big.npos) {
    throw std::runtime_error(label + ": \"" + needle +
                             "\" was not found in the response:\n" + big);
  }
}

/** Convert a string to hex. For example: "abcd" -> "61626364". */
static inline std::string string_to_hex(
    /** [in] String to convert. */
    const std::string& input) {
  std::stringstream ss;
  ss << std::hex;
  for (size_t i = 0; i < input.length(); ++i) {
    ss << std::setw(2) << std::setfill('0') << (int)input[i];
  }
  return ss.str();
}

int main(int, char**) {
  try {
    /** [ipfs::Client::Client] */
    ipfs::Client client("localhost", 5001);
    /** [ipfs::Client::Client] */

    /** [ipfs::Client::Id] */
    ipfs::Json id;
    client.Id(&id);
    std::cout << "Peer's public key: " << id["PublicKey"] << std::endl;
    /* An example output:
    Peer's public key: "CAASpgIwggEiMA0GCSqGSIb3DQN/ImJDE/CN1eHE....gMBAAE="
    */
    /** [ipfs::Client::Id] */
    check_if_properties_exist("client.Id()", id,
                              {"Addresses", "ID", "PublicKey"});

    /** [ipfs::Client::Version] */
    ipfs::Json version;
    client.Version(&version);
    std::cout << "Peer's version: " << version << std::endl;
    /* An example output:
    Peer's version: {"Commit":"ee6dd5e","Golang":"go1.7.3","Repo":"4",
    "System":"amd64/freebsd","Version":"0.4.3"}
    */
    /** [ipfs::Client::Version] */
    check_if_properties_exist("client.Version()", version,
                              {"Repo", "System", "Version"});

    /** [ipfs::Client::ConfigSet] */
    client.ConfigSet("Datastore.StorageMax", "20GB");

    client.ConfigSet("Datastore.StorageGCWatermark", 80);

    client.ConfigSet("Mounts",
                     R"({
                          "FuseAllowOther": false,
                          "IPFS": "/ipfs",
                          "IPNS": "/ipns"
                        })"_json);
    /** [ipfs::Client::ConfigSet] */

    /** [ipfs::Client::ConfigGet] */
    ipfs::Json config;
    client.ConfigGet("Datastore", &config);
    std::cout << R"(Config "Datastore":)" << std::endl
              << config.dump(2) << std::endl;
    /* An example output:
    Config "Datastore":
    {
      "BloomFilterSize": 0,
      "GCPeriod": "1h",
      "HashOnRead": false,
      "NoSync": false,
      "Params": null,
      "Path": "/home/vd/.ipfs/datastore",
      "StorageGCWatermark": 90,
      "StorageMax": "10GB",
      "Type": "leveldb"
    }
    */

    client.ConfigGet("" /* fetch the entire config */, &config);
    std::cout << "Config max storage: " << config["Datastore"]["StorageMax"]
              << std::endl;
    /* An example output:
    Config max storage: "10GB"
    */
    /** [ipfs::Client::ConfigGet] */

    /** [ipfs::Client::ConfigReplace] */
    ipfs::Json entire_config;
    client.ConfigGet("" /* fetch the entire config */, &entire_config);

    client.ConfigReplace(entire_config);
    /** [ipfs::Client::ConfigReplace] */

    /** [ipfs::Client::BlockPut] */
    ipfs::Json block;
    client.BlockPut(
        {"" /* no file name */, ipfs::http::FileUpload::Type::kFileContents,
         "Block put test."},
        &block);
    std::cout << "Stored block key: " << block["Key"] << std::endl;
    /* An example output:
    Stored block key: "QmQpWo5TL9nivqvL18Bq8bS34eewAA6jcgdVsUu4tGeVHo"
    */
    /** [ipfs::Client::BlockPut] */
    check_if_properties_exist("client.BlockPut()", block, {"Key", "Size"});

    /** [ipfs::Client::BlockGet] */
    std::stringstream block_contents;
    /* E.g. block["Key"] is "QmQpWo5TL9nivqvL18Bq8bS34eewAA6jcgdVsUu4tGeVHo". */
    client.BlockGet(block["Key"], &block_contents);
    std::cout << "Block (hex): " << string_to_hex(block_contents.str())
              << std::endl;
    /* An example output:
    Block (hex): 426c6f636b2070757420746573742e
    */
    /** [ipfs::Client::BlockGet] */

    /** [ipfs::Client::BlockStat] */
    ipfs::Json stat_result;
    client.BlockStat(block["Key"], &stat_result);
    std::cout << "Stat: " << stat_result << std::endl;
    /* An example output:
    Stat: {"Key":"QmQpWo5TL9nivqvL18Bq8bS34eewAA6jcgdVsUu4tGeVHo","Size":15}
    */
    /** [ipfs::Client::BlockStat] */
    check_if_properties_exist("client.BlockStat()", stat_result,
                              {"Key", "Size"});

    /** [ipfs::Client::FilesGet] */
    std::stringstream contents;
    client.FilesGet(
        "/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG/readme",
        &contents);
    std::cout << "Retrieved contents: " << contents.str().substr(0, 8) << "..."
              << std::endl;
    /* An example output:
    Retrieved contents: Hello an...
    */
    /** [ipfs::Client::FilesGet] */
    check_if_string_contains("client.FilesGet()", contents.str(),
                             "Hello and Welcome to IPFS!");

    /** [ipfs::Client::FilesAdd] */
    ipfs::Json add_result;
    client.FilesAdd(
        {{"foo.txt", ipfs::http::FileUpload::Type::kFileContents, "abcd"},
         {"bar.txt", ipfs::http::FileUpload::Type::kFileName,
          "compile_commands.json"}},
        &add_result);
    std::cout << "FilesAdd() result:" << std::endl
              << add_result.dump(2) << std::endl;
    /* An example output:
    [
      {
        "path": "foo.txt",
        "hash": "QmWPyMW2u7J2Zyzut7TcBMT8pG6F2cB4hmZk1vBJFBt1nP",
        "size": 4
      }
      {
        "path": "bar.txt",
        "hash": "QmVjQsMgtRsRKpNM8amTCDRuUPriY8tGswsTpo137jPWwL",
        "size": 1176
      },
    ]
    */
    /** [ipfs::Client::FilesAdd] */
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
