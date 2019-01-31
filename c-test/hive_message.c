
#include <hive/c-api.h>
#include <stdio.h>
#include <stdlib.h>

bool callback(const char *key, const uint8_t *value, size_t length, void *ctx) {
  printf("message: ctx=%p, key= %s, value = %p, length=%lu\n",
         ctx, key, value, length);
  return 0;
}

int main() {
  DStoreC *ds = dstore_create("hive.conf");

  dstore_get_values(ds, "key-001", callback, ds);

  const uint8_t message[] = "hello from key-001";
  dstore_add_value(ds, "key-001", message, sizeof message);

  dstore_remove_values(ds, "key-001");

  dstore_destroy(ds);

  return 0;
}
