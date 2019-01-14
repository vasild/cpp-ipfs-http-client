

#include <c-api/c-api.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  const char* generate_conf = hive_generate_conf("localhost", 9094);
  printf("%s\n", generate_conf);

  if (generate_conf)
  	free(generate_conf);

  return 0;
}
