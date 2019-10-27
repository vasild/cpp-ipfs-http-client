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

    ipfs::Json add_result;
    client.FilesAdd(
        {{"foo.txt", ipfs::http::FileUpload::Type::kFileContents, "abcd"}},
        &add_result);
    const std::string& hash = add_result[0]["hash"];

    /** [ipfs::Client::DhtFindProvs] */
    /* std::string hash = "QmWPyMW2u7J2Zyzut7TcBMT8pG6F...Bt1nP" for example. */
    ipfs::Json providers;

    client.DhtFindProvs(hash, &providers);

    std::cout << "Providers for " << hash << ":" << std::endl
              << providers.dump(2).substr(0, 512) << "..." << std::endl;
    /* An example output:
    [
      {
        "Extra": "",
        "ID": "",
        "Responses": [
          {
            "Addrs": null,
            "ID": "QmZSb7SYajaEEbJU2FB4XJWWfxX9AjwmdreK5MDu9zuQvn"
          }
        ],
        "Type": 4
      },
      {
        "Extra": "",
        "ID": "QmYoQjsZeSyzeeX9yCu5jfETgKinR2Udeej385FzPprsGg",
        "Responses": null,
        "Type": 6
      },
      {
        "Extra": "",
        "ID": "QmdsoQ8xhfkaEpAYCLJ75LEiup9E4ybUdbqDkZZKggfWjS",
        "Responses": null,
        "Type": 6
      },...
    */
    /** [ipfs::Client::DhtFindProvs] */

    std::string peer_id;
    /* Find an actual peer. */
    for (auto& p : providers) {
      if (p["Responses"].is_array()) {
        for (auto& r : p["Responses"]) {
          if (r["Addrs"].is_array() && !r["Addrs"].empty() && !r["ID"].empty()) {
            peer_id = r["ID"];
          }
        }
      }
    }
    if (peer_id.empty()) {
      throw std::runtime_error(
          "Could not find a peer with an address in the DhtFindProvs() "
          "response: " +
          providers.dump().substr(0, 1024));
    }

    /** [ipfs::Client::DhtFindPeer] */
    /* std::string peer_id = "QmcZrBqWBYV3RGsPuhQX11Qzp...maDYF" for example. */
    ipfs::Json peer_addresses;

    client.DhtFindPeer(peer_id, &peer_addresses);

    std::cout << "Peer " << peer_id << " info:" << std::endl
              << peer_addresses.dump(2) << std::endl;
    /* An example output:
    Peer QmcZrBqWBYV3RGsPuhQX11QzpKAQ8SYfMYL1dGXuPmaDYF info:
    [
      "/ip6/::1/tcp/4001",
      "/ip4/212.227.249.191/tcp/4001",
      "/ip4/127.0.0.1/tcp/4001"
    ]
    */
    /** [ipfs::Client::DhtFindPeer] */

    ipfs::test::must_fail("client.DhtFindPeer()", [&client, &peer_addresses]() {
      const std::string nonexistent =
          "QmWo9xNSxHAyYV9A2c5kgjyCWTUD17Wk8f7uTFdvJWB6Zb";
      client.DhtFindPeer(nonexistent, &peer_addresses);
    });
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
