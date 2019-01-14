
#include <iostream>
#include <stdexcept>
#include <string>

#include <ipfs/node.h>

int main(int, char**) {
  try {
    ipfs::Node node("localhost", 9095);

    ipfs::Json keyList;

    node.KeyList("uid", &keyList);


    std::cout << "Current key list:" << std::endl
              << keyList.dump(2).substr(0, 8192) << std::endl;

    for (ipfs::Json::iterator it = keyList["Keys"].begin();
         it != keyList["Keys"].end(); ++it) {
      const ipfs::Json& key = it.value();

      std::cout << "Key pair: "
        << key["Name"] << " -> " << key["Id"] << std::endl;
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
