
#include <iostream>
#include <stdexcept>
#include <string>

#include <ipfs/cluster-client.h>

int main(int, char**) {
  try {
    ipfs::Cluster cluster("localhost", 9094);

    ipfs::Json json;
    
    cluster.Version(&json);
    std::string version = json["Version"];
    
    std::cout << "json output:" << std::endl
              << json.dump(2).substr(0, 8192) << std::endl;
    
    std::cout << "cluster version:" << std::endl
        << version << std::endl;

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
