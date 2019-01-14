

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include <c-api/c-api.h>
#include <ipfs/cluster.h>

#include <stdio.h>

extern "C" const char* hive_generate_conf(const char* host = "localhost",
                                          int port = 9094) {
  ipfs::Json jconf = "{ \"hosts\": []}"_json;

  try {
    ipfs::Cluster cluster(host, port);
    ipfs::Json peers;

    cluster.Peers(&peers);

    std::string cluster_peers;
    for (ipfs::Json::iterator it = peers.begin(); it != peers.end(); ++it) {
      const ipfs::Json& cluster_peer = it.value();

      ipfs::Json addresses = cluster_peer["addresses"];
      for (ipfs::Json::iterator it = addresses.begin(); it != addresses.end();
           ++it) {
        const ipfs::Json& address = it.value();

        // /ip4/10.10.1.20/tcp/32388/
        std::string a = address.dump(2);
        std::size_t pos = a.find("/ip4/");
        if (pos == std::string::npos) {
          pos = a.find("/ip6/");
        }
        if (pos != std::string::npos) {
          std::size_t pos2 = a.find("/tcp/", pos + 5);
          if (pos2 == std::string::npos) {
            goto __end;
          }
          // int port = atoi(a.substr(pos2 + 5).c_str());

          char addr[100];
          if (sprintf(addr, "%s", a.substr(pos + 5, pos2 - pos - 5).c_str()) >
              0) {
            // if (strncmp(addr, "127", 3) != 0 && strncmp(addr, "::1", 3) != 0) {
            	jconf["hosts"].push_back(addr);
            // }
          }
        }

        // std::cout << "cluster peer address:" << std::endl
        //   << address. << std::endl;
      }
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
  srand (time(NULL));

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

	    std::cerr << "fetch conf from " << addr << std::endl;
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
