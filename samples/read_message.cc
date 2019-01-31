
#include <iostream>
#include <stdexcept>
#include <string>

#include <hive/cluster.h>
#include <hive/message.h>
#include <hive/node.h>
#include <hive/test/utils.h>

int main(int, char**) {
  // init DStore for message
  DStore DStore("localhost", 9095, true);

  // Please replace this line
  std::string peerId = "QmSdaZzzSoch28Y6oVnCw2zDM6jUbrLRpaEiTzbppjHxmK";

  std::shared_ptr<std::vector<std::string>> keys =
      DStore.get_remote_keys(peerId);

  for (auto& key : *keys) {
    std::shared_ptr<std::vector<std::shared_ptr<DMessage>>> msgs =
        DStore.get_remote_values(peerId, key);

    std::cout << "key: " << key << std::endl;
    for (auto& msg : *msgs) {
      std::cout << "received: " << msg->timestamp() << " => " << msg->value()
                << std::endl;
    }
  }

  return 0;
}
