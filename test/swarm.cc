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
#include <string>

#include <ipfs/client.h>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    /** [ipfs::Client::SwarmAddrs] */
    ipfs::Json addresses;

    client.SwarmAddrs(&addresses);

    std::cout << "Known addresses of each peer:" << std::endl
              << addresses.dump(2).substr(0, 8192) << std::endl;
    /* An example output:
    Known addresses of each peer:
    {
      "Addrs": {
        "QmNRV7kyUxYaQ4KQxFXPYm8EfuzJbtGn1wSFenjXL6LD8y": [
          "/ip4/127.0.0.1/tcp/4001",
          "/ip4/172.17.0.2/tcp/4001",
          "/ip4/5.9.33.222/tcp/1040",
          "/ip4/5.9.33.222/tcp/4001",
          "/ip6/::1/tcp/4001"
        ],
        "QmNYXVn17mHCA1cdTh2DF5KmD9RJ72QkJQ6Eo7HyAuMYqG": [
          "/ip4/10.12.0.5/tcp/4001",
          "/ip4/104.131.144.16/tcp/4001",
          "/ip4/127.0.0.1/tcp/4001",
          "/ip6/::1/tcp/4001"
        ],
        ...
      }
    }
    */
    /** [ipfs::Client::SwarmAddrs] */

    /* Craft a string like
    /ip4/127.0.0.1/tcp/4001/ipfs/QmNRV7kyUxYaQ4KQxFXPYm8EfuzJbtGn1wSFenjXL6LD8y
    from
    {
      "Addrs": {
        "QmNRV7kyUxYaQ4KQxFXPYm8EfuzJbtGn1wSFenjXL6LD8y": [
          "/ip4/127.0.0.1/tcp/4001",
          ...
        ]
      }
    }
    and also pick up an IPv4 address because the environment where these tests
    will be run may not support IPv6.
    */
    std::string peer;
    for (ipfs::Json::iterator it = addresses["Addrs"].begin();
         it != addresses["Addrs"].end(); ++it) {
      const ipfs::Json& addresses = it.value();
      for (const std::string& address :addresses) {
        if (address.substr(0, 5) == "/ip4/") {
          peer = address + "/ipfs/" + it.key();
          break;
        }
      }
    }
    if (peer.empty()) {
      throw std::runtime_error("Could not find a peer with IPv4 address.");
    }
    std::cout << "Connecting to " << peer << std::endl;

    try {
      /** [ipfs::Client::SwarmConnect] */
      /* std::string peer =
       * "/ip4/104.131.131.81/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ"
       * for example */
      client.SwarmConnect(peer);
      /** [ipfs::Client::SwarmConnect] */
      std::cout << "Connected to " << peer << std::endl;
    } catch (const std::exception&) {
      /* Connect and disconnect occasionally fail due to circumstances beyond
       * the control of this test. */
    }

    try {
      /** [ipfs::Client::SwarmDisconnect] */
      /* std::string peer =
       * "/ip4/104.131.131.81/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ"
       * for example */
      client.SwarmDisconnect(peer);
      /** [ipfs::Client::SwarmDisconnect] */
      std::cout << "Disconnected from " << peer << std::endl;
    } catch (const std::exception&) {
      /* Connect and disconnect occasionally fail due to circumstances beyond
       * the control of this test. */
    }

    /** [ipfs::Client::SwarmPeers] */
    ipfs::Json peers;

    client.SwarmPeers(&peers);

    std::cout << "Peers:" << std::endl << peers.dump(2) << std::endl;
    /* An example output:
    Peers:
    {
      "Strings": [
        "/ip4/104.131.131.82/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ",
        "/ip4/104.131.144.16/tcp/4001/ipfs/QmNYXVn17mHCA1cdTh2DF5KmD9RJ72QkJQ6Eo7HyAuMYqG",
        "/ip4/104.223.59.174/tcp/4001/ipfs/QmeWdgoZezpdHz1PX8Ly8AeDQahFkBNtHn6qKeNtWP1jB6",
        ...
      ]
    }
    */
    /** [ipfs::Client::SwarmPeers] */
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
