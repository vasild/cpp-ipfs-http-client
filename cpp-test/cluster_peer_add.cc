
#include <iostream>
#include <stdexcept>
#include <string>

#include <ipfs/cluster.h>

int main(int, char**) {
  try {
    ipfs::Cluster cluster("localhost", 9094);
    ipfs::Json reponse;

    std::string peerId = "QmYihPNDiK4qMo4GL2Yfz6C1dusTSuGNVAxCUu2WH5R9kN";

    cluster.PeerAdd(peerId, &reponse);

    std::cout << "Add a cluster peer: " << peerId << std::endl
              << reponse.dump(2).substr(0, 8192) << std::endl;

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
