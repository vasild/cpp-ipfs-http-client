
#include <iostream>
#include <stdexcept>
#include <string>

#include <hive/cluster.h>

int main(int, char**) {
  try {
    ipfs::Cluster cluster("localhost", 9094);

    ipfs::Json peers;

    cluster.Peers(&peers);

    std::string cluster_peers;
    for (ipfs::Json::iterator it = peers.begin();
         it != peers.end(); ++it) {
      const ipfs::Json& cluster_peer = it.value();

      ipfs::Json addresses = cluster_peer["addresses"];
      for (ipfs::Json::iterator it = addresses.begin();
         it != addresses.end(); ++it) {
        const ipfs::Json& address = it.value();
        std::cout << "cluster peer address:" << std::endl
          << address << std::endl;
      }
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
