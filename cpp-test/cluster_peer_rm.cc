
#include <iostream>
#include <stdexcept>
#include <string>

#include <ipfs/cluster.h>

int main(int, char**) {
  try {
    ipfs::Cluster cluster("localhost", 9094);

    std::string peerId = "QmYihPNDiK4qMo4GL2Yfz6C1dusTSuGNVAxCUu2WH5R9kN";
    ipfs::Json peers;

    cluster.PeerRm(peerId, &peers);

    // std::cout << "Current cluster id:" << std::endl
//               << peers.dump(2).substr(0, 8192) << std::endl;

    std::string cluster_peers;
    for (ipfs::Json::iterator it = peers.begin();
         it != peers.end(); ++it) {
      const ipfs::Json& cluster_peer = it.value();

      std::cout << "cluster peer id:" << std::endl
        << cluster_peer["id"] << std::endl;
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
