
#include <iostream>
#include <stdexcept>
#include <string>

#include <hive/node.h>

int UidInfo(std::string& uid, ipfs::Json* uidInfo) {
  try {
    ipfs::Node node("localhost", 9095);

    node.UidInfo(uid, uidInfo);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

int main(int, char**) {
  ipfs::Json userInfo;
  std::string uid = "uid-132fdc89-4e85-47f5-b942-fd7ba515a6f3";

  UidInfo(uid, &userInfo);

  std::cout << "uid info: " << userInfo["Name"] << " => " << userInfo["Id"]
            << std::endl;

  return 0;
}
