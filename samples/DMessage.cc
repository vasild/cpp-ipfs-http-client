
#include <iostream>
#include <stdexcept>
#include <string>

#include <hive/cluster.h>
#include <hive/node.h>
#include <hive/test/utils.h>

#include "DMessage.h"

DStore::DStore(const std::string host, int port, const std::string& uid)
    : node(host, port) {
  ipfs::Json uidInfo;

  node.UidInfo(uid, &uidInfo);
  userId = uid;
  peerId = uidInfo["PeerID"].get<std::string>();

  std::cout << "My PeerID is " << peerId << std::endl;

  needPublish = true;
  needResolve = true;
}

std::shared_ptr<std::vector<std::string>> DStore::get_keys() {
  // ipfs name publish --key=mykey
  //  ipfs name resolve QmRMgVUVsVcBCo8XMnsfMtLMaQ1myGWnhv8xy6iCTBccyZ
  // /ipfs/QmWMg1Wm6pyjU6VWrBU3uvY9kfm17X3UbHz91qm2wY559j

  //   YIMAC:cpp-test yiwang$ ipfs ls
  //   /ipns/QmYBvxNNXfjafsV7s88fekuRfkqNNRmB4stur88KsaSsaA
  // QmdFASUwf5ab5pY5ucpamGPAe2uLzLYtksR284uTtYyWne 123 001-001.txt
  // QmcFUHo1DBpydnTSeuXszKgQqnXQCJq6Wu1BUdzEfuRahM 64  001.txt

  // {"Arguments":{"/ipns/QmYBvxNNXfjafsV7s88fekuRfkqNNRmB4stur88KsaSsaA":"Qmf7VYPRhr5v17No8Ppsgo6ShZFHe7J7ukH1QRmqBxA1wZ"},"Objects":{"Qmf7VYPRhr5v17No8Ppsgo6ShZFHe7J7ukH1QRmqBxA1wZ":{"Hash":"Qmf7VYPRhr5v17No8Ppsgo6ShZFHe7J7ukH1QRmqBxA1wZ","Size":0,"Type":"Directory","Links":[{"Name":"001-001.txt","Hash":"QmdFASUwf5ab5pY5ucpamGPAe2uLzLYtksR284uTtYyWne","Size":13,"Type":"File"},{"Name":"001.txt","Hash":"QmcFUHo1DBpydnTSeuXszKgQqnXQCJq6Wu1BUdzEfuRahM","Size":6,"Type":"File"}]}}}

  // ipfs files ls /nodes/peerId/
  // ipfs files read /nodes/peerId/messages/key/001
  // ipfs files read /nodes/peerId/messages/key/002
  // ..

  std::shared_ptr<std::vector<std::string>> vm;

  try {
    ipfs::Json ls_result;
    std::string path = workingPath + "/messages/";

    node.FileLs(path, &ls_result);
    std::string objectsId = ls_result["Arguments"][path];
    ipfs::Json& messages = ls_result["objects"][objectsId]["Links"];
    for (ipfs::Json::iterator it = messages.begin(); it != messages.end();
         ++it) {
      const ipfs::Json& message = it.value();

      std::string key = message["Name"].get<std::string>();
      vm->push_back(key);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return vm;
}

std::shared_ptr<std::vector<std::shared_ptr<DMessage>>> DStore::get_values(
    const std::string& key) {
  // ipfs files ls /nodes/peerId/
  // ipfs files read /nodes/peerId/messages/key/001
  // ipfs files read /nodes/peerId/messages/key/002
  // ..

  std::shared_ptr<std::vector<std::shared_ptr<DMessage>>> vm(
      new std::vector<std::shared_ptr<DMessage>>);
  ipfs::Json ls_result;
  std::string path = workingPath + "/messages/" + key;
  ipfs::Json messages;

  try {
    node.FilesLs(userId, path, &ls_result);
    messages = ls_result["Entries"];
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << "DEBUG: List key path " << path << " for key " << key
              << " failed" << std::endl;
    return vm;
  }

  std::cerr << "DEBUG: List key path " << path << " for key " << key
            << " successfully" << std::endl;

  try {
    for (ipfs::Json::iterator it = messages.begin(); it != messages.end();
         ++it) {
      const ipfs::Json& message = it.value();
      const std::string name = message["Name"].get<std::string>();

      std::stringstream content;
      node.FilesRead(userId, path + "/" + name, 0, 1000, &content);

      std::shared_ptr<DMessage> dmsg(new DMessage(name, content.str()));
      vm->push_back(dmsg);
    }
  } catch (const std::exception& e) {
    std::cerr << "DEBUG: Read key path " << path << " for key " << key
              << " failed" << std::endl;
    std::cerr << e.what() << std::endl;
    return vm;
  }

  std::cerr << "DEBUG: Read key path " << path << " for key " << key
            << " successfully" << std::endl;

  return vm;
}

bool DStore::add_value(const std::string& key,
                       std::shared_ptr<DMessage> message) {
  // > ipfs files write /nodes/peerId/messages/key/timestamp-xxxx

  // > ipfs files stat  /nodes/peerId
  // xxxxxxxxx
  // > ipfs name  publish --key=uid-132fdc89-4e85-47f5-b942-fd7ba515a6f3
  // /ipfs/xxxxxxxxx /ipfs/Qmf7VYPRhr5v17No8Ppsgo6ShZFHe7J7ukH1QRmqBxA1wZ
  // /ipfs/QmatmE9msSfkKxoffpHwNLNKgwZG8eT9Bud6YoPab52vpy

  ipfs::Json result;
  std::string path = workingPath + "/messages/" + key;
  bool keyPath = true;

  try {
    node.FilesLs(userId, path, &result);
  } catch (const std::exception& e) {
    std::cerr << "Not exist path: " << path << ". Creating it ... " << e.what()
              << std::endl;
    keyPath = false;
  }

  // std::cout << "DEBUG: " << result.dump() << std::endl;

  if (!keyPath) {
    try {
      node.FilesMkdir(userId, path, &result);
    } catch (const std::exception& e) {
      std::cerr << "Mkdir: " << path << "; " << e.what() << std::endl;
      return false;
    }
  }

  std::cerr << "DEBUG: Create key path " << path << " for key " << key
            << " successfully" << std::endl;

  std::string messagePath = path + "/" + message->timestamp();
  try {
    ipfs::Json result0;
    node.FilesWrite(userId, messagePath, message->value(), 0, true, false,
                    &result0);
  } catch (const std::exception& e) {
    std::cerr << __LINE__ << " FilesWrite: " << e.what() << std::endl;
    return false;
  }

  std::cerr << "DEBUG: Write message \"" << message->value() << "\" for key "
            << key << " is success, publish now... " << std::endl;

  needPublish = true;
  publish();

  return true;
}

bool DStore::remove_values(const std::string& key) {
  // > ipfs files rm /nodes/peerId/messages/key/xxxxxxxx

  // > ipfs files stat /nodes/peerId
  // xxxxxxxx
  // > ipfs name  publish --key=uid-132fdc89-4e85-47f5-b942-fd7ba515a6f3
  // /ipfs/xxxxxxxxx /ipfs/QmatmE9msSfkKxoffpHwNLNKgwZG8eT9Bud6YoPab52vpy

  try {
    ipfs::Json result;
    std::string path = workingPath + "/messages/" + key;

    node.FilesRm(userId, path, true, &result);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }

  std::cerr << "DEBUG: Remove messages for key " << key
            << " is success, publish now... " << std::endl;

  needPublish = true;
  publish();

  return true;
}

bool DStore::init() {
  // > ipfs files mkdir /nodes/peerId/
  // > ipfs name resolve /ipns/peerId
  // /ipfs/xxxxxxx
  // > ipfs files mv /ipfs/xxxxxxx/messages /nodes/peerId/

  workingPath = "/";
  ipfs::Json result;

  try {
    node.FilesLs(userId, workingPath, &result);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << "Please call UIDNew()  " << std::endl;
    return false;
  }

  // std::cout << "DEBUG: " << result.dump() << std::endl;

  try {
    node.FilesRm(userId, workingPath + "/messages", true, &result);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  std::cerr << "Cleanuped workingPath: " << (workingPath + "/messages")
            << std::endl;

  needResolve = true;
  resolve();

  // verify working path
  bool messagesPath = false;
  for (ipfs::Json::iterator it = result["Entries"].begin();
       it != result["Entries"].end(); ++it) {
    const ipfs::Json& key = it.value();

    if (key["Name"] == "messages") {
      messagesPath = true;
      break;
    }
  }

  if (!messagesPath) {
    try {
      node.FilesMkdir(userId, workingPath + "/messages", &result);
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }

  return true;
}

bool DStore::publish() {
  // > ipfs files stat /nodes/peerId
  // xxxxxxxx
  // > ipfs name  publish --key=uid-132fdc89-4e85-47f5-b942-fd7ba515a6f3
  // /ipfs/xxxxxxxxx /ipfs/QmatmE9msSfkKxoffpHwNLNKgwZG8eT9Bud6YoPab52vpy

  ipfs::Json result;

  if (needPublish) {
    try {
      node.FilesStat(userId, workingPath, &result);
    } catch (const std::exception& e) {
      std::cerr << "publish at FilesStat: " << e.what() << std::endl;
      return false;
    }

    std::string hash = result["Hash"].get<std::string>();
    try {
      node.NamePublish(userId, "/ipfs/" + hash, &result);

      std::cerr << "published with userId:" << userId
                << " path = " << workingPath << " and hash = " << hash
                << std::endl;

      needPublish = false;
    } catch (const std::exception& e) {
      std::cerr << "publish failed, userId:" << userId
                << " path = " << workingPath << " and hash = " << hash << " "
                << e.what() << std::endl;
      return false;
    }
  }

  return true;
}

bool DStore::resolve() {
  // > ipfs name resolve /ipns/peerId
  // /ipfs/xxxxxxx
  // > ipfs files mv /ipfs/xxxxxxx/messages /nodes/peerId/

  ipfs::Json result;

  if (needResolve) {
    try {
      node.NameResolve("/ipns/" + peerId, &result);
      rootPath = result["Path"];
    } catch (const std::exception& e) {
      std::cerr << "NameResolve: " << e.what() << std::endl;
      return false;
    }

    std::cerr << "DEBUG: Fetched rootPath " << rootPath << " for peerId "
              << peerId << " successfully" << std::endl;

    try {
      node.FilesCp(userId, rootPath + "/messages", workingPath, &result);
    } catch (const std::exception& e) {
      std::cerr << "DEBUG: Restore workingPath " << workingPath << " from "
                << (rootPath + "/messages") << " for peerId " << peerId
                << " failed. Continue ..." << std::endl;
      std::cerr << "FilesCp: " << e.what() << std::endl;
      return false;
    }

    std::cerr << "DEBUG: Restored workingPath " << workingPath << " for peerId "
              << peerId << " successfully" << std::endl;

    needResolve = false;
  }

  return true;
}

std::string get_timestamp() {
  std::time_t result = std::time(nullptr);
  return std::to_string(result);
}
