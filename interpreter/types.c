#include "types.h"
#include <stdlib.h>
#include <stdio.h>

List new_list(Value* head){
  List rtn;
  rtn.head = head;
  rtn.tail = NULL;
  return rtn;
}

Tuple tuple(int size, ...)  {
  Tuple rtn;
  Value** elems = (Value**)malloc(size * sizeof(Value*));
  va_list args;
  va_start(args,size);
  for (int i = 0; i < size; i++) {
    elems[i] = va_arg(args,Value*);
  }
  va_end(args);
  rtn.size = size;
  rtn.elems = elems;
  return rtn;
}

int main() {
  Value v1;
  v1.asInt = 10;
  Value v2;
  v2.asTuple = tuple(1,&v1);
  printf("%d, %d\n",v2.asTuple.size, v2.asTuple.elems[0]->asInt);
  free(v2.asTuple.elems);
}
