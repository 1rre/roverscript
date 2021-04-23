#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdarg.h>


typedef struct _List List;

struct _List {
  union AnyVal* head;
  List* tail;
  int size;
};


typedef struct {
  int size;
  union AnyVal** elems;
} Tuple;

typedef union AnyVal {
  int asInt;
  float asFloat;

  List asList;
  Tuple asTuple;

  struct Bits {
    int size;
    void* start;
  } asBits;

} Value;

// Lists
List cons(Value* head, List* tail) {
  List rtn;
  rtn.head = head;
  rtn.tail = tail;
  rtn.size = tail==NULL?1:tail->size+1;
  return rtn;
}
List list(Value* head) {
  return cons(head,NULL);
}

// Tuple
Tuple tuple(int size, ...) {
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
void del_tuple(Value* val) {
  free(val->asTuple.elems);
}





#endif
