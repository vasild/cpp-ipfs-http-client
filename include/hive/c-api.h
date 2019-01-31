
#ifndef _HIVE_API_H_
#define _HIVE_API_H_

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char* hive_generate_conf(const char* host, int port);
const char* hive_refresh_conf(const char* defaultConf);
const char* hive_random_host(const char* volatileConf);

typedef void DStoreC;

DStoreC *dstore_create(const char *hive_conf);
void dstore_destroy(DStoreC *dstore);
int dstore_get_values(DStoreC *dstore, const char *key,
                      int (*cb)(const char *key, const uint8_t *value,
                                 size_t length, void *ctx),
                      void *ctx);
int dstore_add_value(DStoreC *dstore, const char *key,
                     const uint8_t *value, size_t len);
int dstore_remove_values(DStoreC *dstore, const char *key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HIVE_API_H_ */
