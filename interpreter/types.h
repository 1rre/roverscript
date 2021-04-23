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
    void* start;
  } Bits;


typedef struct {
  int size;
  union AnyVal** elems;
} Tuple;

typedef union AnyVal {

  int asInt;
  float asFloat;

  List asList;
  Tuple asTuple;
  Bits asBits;

} AnyVal;

// Lists
List cons(AnyVal* head, List* tail) {
  List rtn;
  rtn.head = head;
  rtn.tail = tail;
  rtn.size = tail==NULL?1:tail->size+1;
  return rtn;
}
List list(AnyVal* head) {
  return cons(head,NULL);
}

// Tuple
Tuple tuple(int size, ...) {
  Tuple rtn;
  AnyVal** elems = (AnyVal**)malloc(size * sizeof(AnyVal*));
  va_list args;
  va_start(args,size);
  for (int i = 0; i < size; i++) {
    elems[i] = va_arg(args,AnyVal*);
  }
  va_end(args);
  rtn.size = size;
  rtn.elems = elems;
  return rtn;
}
void del_tuple(AnyVal* val) {
  free(val->asTuple.elems);
}





#endif
