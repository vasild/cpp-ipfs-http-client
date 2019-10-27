/* Copyright (c) 2016-2019, The C++ IPFS client library developers

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
#include <stdexcept>

#include <ipfs/client.h>
#include <ipfs/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

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
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
