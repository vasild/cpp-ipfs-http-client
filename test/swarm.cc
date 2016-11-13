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
              << addresses.dump(2) << std::endl;
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
    */
    ipfs::Json::iterator it = addresses["Addrs"].begin();
    std::string peer = it.value()[0];
    peer += "/ipfs/";
    peer += it.key();
    /** [ipfs::Client::SwarmConnect] */
    /* std::string peer =
     * "/ip4/104.131.131.81/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ"
     * for example */
    client.SwarmConnect(peer);
    /** [ipfs::Client::SwarmConnect] */
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
