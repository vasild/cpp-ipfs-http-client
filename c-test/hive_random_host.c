

#include <hive/c-api.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  const char* random_host = hive_random_host("hive.conf");

  printf("hive_random_host = %s\n", random_host);

  if (random_host)
  	free(random_host);

  return 0;
}
