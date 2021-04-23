#include "types.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
  AnyVal v1;
  v1.asInt = 12;
  AnyVal v2;
  v2.asList = list(&v1);
  printf("%d\n",v2.asList.head->asInt);
}
