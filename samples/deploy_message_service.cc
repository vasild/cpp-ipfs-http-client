
#include <iostream>
#include <stdexcept>
#include <string>

#include <hive/cluster.h>
#include <hive/message.h>
#include <hive/node.h>

int main(int, char**) {
  ipfs::Node node("localhost", 9095);

  ipfs::Json result;

  node.UIDNew(&result);

  std::string uid = result["UID"].get<std::string>();
  std::string peerId = result["PeerID"].get<std::string>();

  std::cout << "Deploy message serive ... " << std::endl
            << "Please record the following info" << std::endl
            << "Your UID is: " << uid << std::endl
            << "Your PeerId is: " << peerId << std::endl
            << "the UID is for to create message account and send messages." << std::endl
            << "the PeerId is for to the receiver side that can get messages."<< std::endl
            << "Please use the upward exact info to update the sample source code."<< std::endl;
  return 0;
}
