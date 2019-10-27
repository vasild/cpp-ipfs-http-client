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

    std::string object_id;

    client.ObjectNew(&object_id);

    /** [ipfs::Client::PinAdd] */
    /* std::string object_id = "QmdfTbBqBPQ7VNxZEYEj14V...1zR1n" for example. */
    client.PinAdd(object_id);

    std::cout << "Pinned object: " << object_id << std::endl;
    /* An example output:
    Pinned object: QmdfTbBqBPQ7VNxZEYEj14VmRuZBkqFbiwReogJgS1zR1n
    */
    /** [ipfs::Client::PinAdd] */

    /** [ipfs::Client::PinLs__a] */
    ipfs::Json pinned;

    client.PinLs(&pinned);

    std::cout << "List of all pinned objects:" << std::endl
              << pinned.dump(2) << std::endl;
    /* An example output:
    List of all pinned objects:
    {
      "Keys": {
        "QmNYaS23te5Rja36U94JoSTuMxJZmBEnHN8KEcjR6rGRGn": {
          "Type": "indirect"
        },
        "QmUNLLsPACCz1vLxQVkXqqLX5R1X345qqfHbsf67hvA3Nn": {
          "Type": "recursive"
        },
        "Qmf5t6BgYbiT2usHRToVzLu5DNHfH39S4dq6JTxf69Npzt": {
          "Type": "indirect"
        }
      }
    }
    */
    /** [ipfs::Client::PinLs__a] */

    /** [ipfs::Client::PinLs__b] */
    /* std::string object_id = "QmdfTbBqBPQ7VNxZEYEj14V...1zR1n" for example. */
    client.PinLs(object_id, &pinned);

    std::cout << "List pinned objects under " << object_id << ":" << std::endl
              << pinned.dump(2) << std::endl;
    /* An example output:
    List pinned objects under QmdfTbBqBPQ7VNxZEYEj14VmRuZBkqFbiwReogJgS1zR1n:
    {
      "Keys": {
        "QmdfTbBqBPQ7VNxZEYEj14VmRuZBkqFbiwReogJgS1zR1n": {
          "Type": "recursive"
        }
      }
    }
    */
    /** [ipfs::Client::PinLs__b] */

    /** [ipfs::Client::PinRm] */
    /* std::string object_id = "QmdfTbBqBPQ7VNxZEYEj14V...1zR1n" for example. */

    bool unpinned;
    try {
      client.PinRm(object_id, ipfs::Client::PinRmOptions::NON_RECURSIVE);
      unpinned = true;
    } catch (const std::exception&) {
      unpinned = false;
    }

    if (unpinned) {
      throw std::runtime_error(
          "Unpinning " + object_id +
          " non-recursively succeeded but should have failed.");
    }

    client.PinRm(object_id, ipfs::Client::PinRmOptions::RECURSIVE);
    /** [ipfs::Client::PinRm] */
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
