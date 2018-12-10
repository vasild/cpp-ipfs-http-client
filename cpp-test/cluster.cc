
#include <iostream>
#include <stdexcept>
#include <string>

#include <ipfs/cluster-client.h>

int main(int, char**) {
  try {
    ipfs::Cluster cluster("localhost", 9094);

    ipfs::Json id;

    cluster.Id(&id);

    std::cout << "Current cluster id:" << std::endl
              << id.dump(2).substr(0, 8192) << std::endl;

    std::string cluster_peers;
    for (ipfs::Json::iterator it = id["cluster_peers"].begin();
         it != id["cluster_peers"].end(); ++it) {
      const ipfs::Json& cluster_peer = it.value();

      std::cout << "cluster peer id:" << std::endl
        << cluster_peer << std::endl;
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
