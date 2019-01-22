

#include <hive/c-api.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

  const char* refresh_conf = hive_refresh_conf("hive.conf");
  printf("hive_refresh_conf =\n %s\n", refresh_conf);

  if (refresh_conf)
  	free(refresh_conf);

  return 0;
}
