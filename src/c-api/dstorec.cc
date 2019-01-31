#include <hive/c-api.h>
#include <hive/node.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>

#include <hive/c-api.h>
#include <hive/message.h>

extern "C"
DStoreC *dstore_create(const char *hive_conf) {
  try {
    auto host_rand = hive_random_host(hive_conf);
    if (!host_rand) return nullptr;

    auto ds = new DStore{host_rand, 9095};

    // Please replace this line
    std::string uid = "uid-00ec6a5e-a36d-4313-a579-5ce46be3eadb";
    ds->set_sender_UID(uid);

    return reinterpret_cast<DStoreC *>(ds);
  } catch (...) {
    return nullptr;
  }
}

extern "C"
void dstore_destroy(DStoreC *dstore) {
  delete reinterpret_cast<DStore *>(dstore);
}

extern "C"
int dstore_get_values(DStoreC *dstore, const char *key,
                      bool (*callback)(const char *key, const uint8_t *value,
                                       size_t length, void *context),
                      void *context) {
  auto ds = reinterpret_cast<DStore *>(dstore);

  if (!dstore || !key || !*key || !callback) return -1;

  try {
    const auto &dmsgs = ds->get_values(key);
    for (auto &dmsg : *dmsgs) {
      auto value = dmsg->value();
      bool cont = callback(key, reinterpret_cast<const uint8_t *>(value.data()),
                           value.size(), context);
      if (!cont) break;
    }
    return 0;
  } catch (...) {
    return -1;
  }
}

extern "C"
int dstore_add_value(DStoreC *dstore, const char *key, const uint8_t *value,
                     size_t len) {
  static int cnt = 0;
  char buf[128];
  auto ds = reinterpret_cast<DStore *>(dstore);

  if (!dstore || !key || !*key || !value || !len) return -1;

  try {
    snprintf(buf, sizeof(buf), "%d", cnt++);
    auto dmsg = std::make_shared<DMessage>(
        buf, std::string{reinterpret_cast<const char *>(value), len});
    ds->add_value(key, dmsg);
    return 0;
  } catch (...) {
    return -1;
  }
}

extern "C"
int dstore_remove_values(DStoreC *dstore, const char *key) {
  auto ds = reinterpret_cast<DStore *>(dstore);

  if (!dstore || !key || !*key) return -1;

  try {
    ds->remove_values(key);
    return 0;
  } catch (...) {
    return -1;
  }
}
