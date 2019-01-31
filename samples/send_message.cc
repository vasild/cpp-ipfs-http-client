
#include <iostream>
#include <stdexcept>
#include <string>

#include <hive/cluster.h>
#include <hive/message.h>
#include <hive/node.h>

int main(int, char**) {
  ipfs::Node node("localhost", 9095);

  ipfs::Json result;

  // Please replace this line
  std::string uid = "uid-00ec6a5e-a36d-4313-a579-5ce46be3eadb";

  // init DStore for message
  DStore DStore("localhost", 9095, true);

  std::shared_ptr<DMessage> msg1(new DMessage("20190111-1", "hello-1"));
  std::shared_ptr<DMessage> msg2(new DMessage("20190111-2", "hello-2"));
  std::shared_ptr<DMessage> msg3(new DMessage("20190111-3", "hello-3"));

  DStore.set_sender_UID(uid);

  DStore.add_value("msg0", msg1);
  DStore.add_value("msg1", msg2);
  DStore.add_value("msg2", msg3);

  std::shared_ptr<std::vector<std::shared_ptr<DMessage>>> msgs =
      DStore.get_values("msg0");

  for (auto& msg : *msgs) {
    std::cout << "msg0" << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  msgs = DStore.get_values("msg1");

  for (auto& msg : *msgs) {
    std::cout << "msg1" << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  msgs = DStore.get_values("msg2");

  for (auto& msg : *msgs) {
    std::cout << "msg2" << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  DStore.remove_values("msg2");

  msgs = DStore.get_values("msg2");

  for (auto& msg : *msgs) {
    std::cout << "msg2" << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  return 0;
}
