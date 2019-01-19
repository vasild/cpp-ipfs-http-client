
#include <iostream>
#include <stdexcept>
#include <string>

#include <ipfs/cluster.h>
#include <ipfs/node.h>
#include <ipfs/test/utils.h>

#include "DMessage.h"

int main(int, char**) {
  ipfs::Node node("localhost", 9095);

  ipfs::Json result;

  // create a new UID

  node.UIDNew(&result);

  std::cout << "UIDNew() result:" << std::endl
            << result.dump(2) << std::endl;

  std::string uid = result["UID"].get<std::string>();


  // init DCache for message
  DCache DCache("localhost", 9095, uid);

  std::shared_ptr<DMessage> msg1(new DMessage("20190111-1", "hello-1"));
  std::shared_ptr<DMessage> msg2(new DMessage("20190111-2", "hello-2"));
  std::shared_ptr<DMessage> msg3(new DMessage("20190111-3", "hello-3"));

  DCache.init();

  DCache.add_value("msg0", msg1);
  DCache.add_value("msg1", msg2);
  DCache.add_value("msg2", msg3);

  std::shared_ptr<std::vector<std::shared_ptr<DMessage>>> msgs =
      DCache.get_values("msg0");

  for (auto& msg : *msgs) {
    std::cout << "msg0" << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  msgs = DCache.get_values("msg1");

  for (auto& msg : *msgs) {
    std::cout << "msg1" << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  msgs = DCache.get_values("msg2");

  for (auto& msg : *msgs) {
    std::cout << "msg2" << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  DCache.remove_values("msg2");

  msgs = DCache.get_values("msg2");

  for (auto& msg : *msgs) {
    std::cout << "msg2" << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  return 0;
}
