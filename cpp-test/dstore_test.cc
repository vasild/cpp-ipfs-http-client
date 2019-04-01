
#include <iostream>
#include <stdexcept>
#include <string>

#include <hive/cluster.h>
#include <hive/message.h>
#include <hive/node.h>

void usage(const char *cmd) {
  std::cerr << "usage: " << std::endl <<
  "message send: \n\t" << cmd << " send <host> <uid> <key> <value>" << std::endl <<
  "message read: \n\t" << cmd << " read <host> <peerId> <key>" << std::endl <<
  "message remove: \n\t" << cmd << " remove <host> <uid> <key>" << std::endl;
}

int msg_send(std::string &host, std::string &uid, const std::string &key, const std::string &value) {
  ipfs::Json result;

  // init DStore for message
  DStore DStore(host, 9095, false);

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, sizeof(buffer), "%d-%m-%YT%H:%M:%S", timeinfo);
  std::string title(buffer);

  std::shared_ptr<DMessage> msg(new DMessage(title, value));

  DStore.set_sender_UID(uid);

  DStore.add_value(key, msg);

  std::shared_ptr<std::vector<std::shared_ptr<DMessage>>> msgs =
      DStore.get_values(key);

  std::cout << "SEND: " << key;
  for (auto& msg : *msgs) {
    std::cout << " DATE: " << msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  return 0;
}

int msg_read(std::string &host, std::string &peerId, const std::string &key) {
  // init DStore for message
  DStore DStore(host, 9095, false);

  std::shared_ptr<std::vector<std::string>> keys =
      DStore.get_remote_keys(peerId);

  for (auto& key : *keys) {
    std::shared_ptr<std::vector<std::shared_ptr<DMessage>>> msgs =
        DStore.get_remote_values(peerId, key);

    std::cout << "READ: " << "key: " << key << std::endl;
    for (auto& msg : *msgs) {
      std::cout << " DATE: " << msg->timestamp() << " => " << msg->value()
                << std::endl;
    }
  }

  return 0;
}

int msg_remove(std::string &host, std::string &uid, const std::string &key) {
  ipfs::Json result;

  // init DStore for message
  DStore DStore(host, 9095, false);

  DStore.set_sender_UID(uid);

  DStore.remove_values(key);

  std::shared_ptr<std::vector<std::shared_ptr<DMessage>>> msgs = DStore.get_values(key);

  for (auto& msg : *msgs) {
    std::cout << "REMOVE after:  " << key << " DATE: "<< msg->timestamp() << " => " << msg->value()
              << std::endl;
  }

  return 0;
}

int main(int argc, char *argv[]) {

  if (argc < 4) {
    usage(argv[0]);
    return -1;
  }

  std::string host = argv[2];
  std::string uid_or_peerId = argv[3];

  if (strcmp(argv[1], "send") == 0) {
    // send <host> <uid> <key> <value>

    return msg_send(host, uid_or_peerId, argv[4], argv[5]);
  } else if (strcmp(argv[1], "read") == 0) {
    return msg_read(host, uid_or_peerId, argv[4]);
  } else if (strcmp(argv[1], "remove") == 0) {
    return msg_remove(host, uid_or_peerId, argv[4]);
  } else { 
    usage(argv[0]);
  }
  
  return 0;
}
