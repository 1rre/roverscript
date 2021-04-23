#include "types.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
  Value v1;
  v1.asInt = 12;
  Value v2;
  v2.asList = list(&v1);
  printf("%d\n",v2.asList.head->asInt);
}
