#include <iostream>
#include <sstream>
#include <stdexcept>

#include <hive/node.h>
#include <hive/cluster.h>
#include <hive/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Node client("localhost", 9095);

    /** [ipfs::Node::FilesGet] */
    std::stringstream contents;
    client.FileGet(
        "/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG/readme",
        &contents);
    std::cout << "Retrieved contents: " << contents.str().substr(0, 20) << "..."
              << std::endl;
    /* An example output:
    Retrieved contents: Hello an...
    */
    /** [ipfs::Node::FilesGet] */
    ipfs::test::check_if_string_contains("client.FilesGet()", contents.str(),
                                         "Hello and Welcome to IPFS!");
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
