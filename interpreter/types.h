#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdarg.h>

char buf[32];

typedef enum
{
  ADD_BIF,
  SUB_BIF,
  MUL_BIF,
  IDIV_BIF,
  FDIV_BIF,
  DEFFUN_START
} BIF;

typedef enum
{
  CREATE_VARIABLE,
  DELETE_VARIABLE,
  ASSIGN_VARIABLE,
  PREPEND_LIST,
  FUNCTION_HEADER,
  JUMP_IF,
  JUMP,
  RETURN
} Opcode;

typedef enum
{
  INT,
  FLOAT,
  ATOM,
  LIST,
  TUPLE
} TypeCode;

typedef struct _List List;

struct _List {
  union AnyVal* head;
  List* tail;
  int size;
  TypeCode type;
};

typedef struct {
  int size;
  void* start;
} Bits;


typedef struct {
  int size;
  union AnyVal* elems;
  TypeCode* types;
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

void del_tuple(Tuple* val) {
  free(val->elems);
  free(val->types);
}
/*
void del_list(List* val) {
  if (val->size > 1) del_list(val->tail);
  free(val);
  return;
}
*/




#endif
