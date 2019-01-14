#include <iostream>
#include <sstream>
#include <stdexcept>

#include <ipfs/node.h>
#include <ipfs/cluster.h>
#include <ipfs/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Node client("localhost", 9095);

    /** [ipfs::Node::FilesLs] */
    ipfs::Json ls_result;
    client.FilesLs("/ipfs/QmZJwDYpFZrdLHVy6Zhzd1bxq87zBcW8beR2eidihXPQ4j",
                   &ls_result);
    std::cout << "FilesLs() result:" << std::endl
              << ls_result.dump(2) << std::endl;

    /* An example output:
    {
      "Arguments": {
        "/ipfs/QmZJwDYpFZrdLHVy6Zhzd1bxq87zBcW8beR2eidihXPQ4j":
    "QmZJwDYpFZrdLHVy6Zhzd1bxq87zBcW8beR2eidihXPQ4j"
      },
      "Objects": {
        "QmZJwDYpFZrdLHVy6Zhzd1bxq87zBcW8beR2eidihXPQ4j": {
          "Hash": "QmZJwDYpFZrdLHVy6Zhzd1bxq87zBcW8beR2eidihXPQ4j",
          "Links": null,
          "Size": 1028,
          "Type": "File"
        }
      }
    }
    */
    /** [ipfs::Node::FilesLs] */
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}