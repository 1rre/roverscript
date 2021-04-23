#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdarg.h>


typedef struct {
  union AnyVal* head;
  struct List* tail;
} List;

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


// List
List list(Value* head);
List cons(Value* head, struct List* tail);

// Tuple
Tuple tuple(int size, ...);
void del_tuple(Value* val) {
  free(val->asTuple.elems);
}





#endif
