

#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include <hive/c-api.h>
#include <hive/cluster.h>

#include <stdio.h>

static std::mutex mtx;

static bool test_reachable(ipfs::Json* jconf, const char* host, int port) {
  ipfs::Cluster cluster(host, port);
  ipfs::Json id;

  try {
    cluster.Id(&id);
    std::string rpc_protocol_version =
        id["rpc_protocol_version"].get<std::string>();
    if (rpc_protocol_version.find("hivecluster") != std::string::npos) {
      mtx.lock();
      (*jconf)["hosts"].push_back(host);
      mtx.unlock();
    }

    free((void *)host);
  } catch (const std::exception& e) {
    return false;
  }

  return true;
}

extern "C" const char* hive_generate_conf(const char* host = "localhost",
                                          int port = 9094) {
  ipfs::Json jconf = "{ \"hosts\": []}"_json;

  try {
    ipfs::Cluster cluster(host, port);
    ipfs::Json peers;

    cluster.Peers(&peers);

    std::string cluster_peers;
    std::vector<std::shared_ptr<std::thread>> threads;
    for (ipfs::Json::iterator it = peers.begin(); it != peers.end(); ++it) {
      const ipfs::Json& cluster_peer = it.value();

      ipfs::Json addresses = cluster_peer["addresses"];
      for (ipfs::Json::iterator it = addresses.begin(); it != addresses.end();
           ++it) {
        const ipfs::Json& address = it.value();

        // /ip4/10.10.1.20/tcp/32388/
        std::string a = address.get<std::string>();
        std::size_t pos = a.find("/ip4/");
        if (pos == std::string::npos) {
          pos = a.find("/ip6/");
        }
        if (pos != std::string::npos) {
          std::size_t pos2 = a.find("/tcp/", pos + 5);
          if (pos2 == std::string::npos) {
            goto __end;
          }

          std::string addr = a.substr(pos + 5, pos2 - pos - 5);
          if (addr.compare(0, 3, "127") != 0 &&
              addr.compare(0, 3, "::1") != 0) {
            std::shared_ptr<std::thread> th(new std::thread(
                test_reachable, &jconf, strdup(addr.c_str()), 9094));
            threads.push_back(th);
          }
        }
      }
    }

    for (std::vector<std::shared_ptr<std::thread>>::iterator it =
             threads.begin();
         it != threads.end(); ++it) {
      (*it)->join();
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return NULL;
  }

__end:
  return strdup(jconf.dump(4).c_str());
}

extern "C" const char* hive_refresh_conf(
    const char* defaultConf = "hive.conf") {
  srand(time(NULL));

  try {
    ipfs::Json jconf;
    std::ifstream conf_file(defaultConf);
    const char* new_conf = NULL;

    conf_file.seekg(0, std::ios::end);
    size_t size = conf_file.tellg();
    std::string buffer(size, ' ');
    conf_file.seekg(0);
    conf_file.read(&buffer[0], size);

    jconf = ipfs::Json::parse(buffer);

    ipfs::Json jhosts = jconf["hosts"];
    int count = std::distance(jhosts.begin(), jhosts.end());

    do {
      std::string addr = jhosts[rand() % count];
      new_conf = hive_generate_conf(addr.c_str(), 9094);

      // std::cerr << "fetch conf from " << addr << std::endl;
    } while (new_conf == NULL);

    return strdup(new_conf);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return NULL;
  }
  return NULL;
}

extern "C" const char* hive_random_host(
    const char* volatileConf = "hive.conf") {
  try {
    ipfs::Json jconf;
    const char* hosts = hive_refresh_conf(volatileConf);
    // std::cerr << "to parse:" << hosts << std::endl;
    jconf = ipfs::Json::parse(hosts);

    ipfs::Json jhosts = jconf["hosts"];
    int index = rand() % std::distance(jhosts.begin(), jhosts.end());
    std::string addr = jhosts[index];
    // std::cerr << "to parse index:" << index << addr << std::endl;

    return strdup(addr.c_str());
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return NULL;
  }
  return NULL;
}
