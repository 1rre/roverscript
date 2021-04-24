#ifndef OPCODE_H
#define OPCODE_H

#include "arduino_utils.h"

typedef enum {
  // 2 args
  ADD_BIF,    // a + b
  SUB_BIF,    // a - b
  MUL_BIF,    // a · b
  IDIV_BIF,   // a div b
  FDIV_BIF,   // a / b
  BAND_BIF,   // a b· b
  LAND_BIF,   // a l· b
  BOR_BIF,    // a b+ b
  LOR_BIF,    // a l+ b
  BXOR_BIF,   // a x+ b
  CONS_BIF,   // [a | b]
  POW_BIF,    // a ^ b 
  // 1 arg
  BNOT_BIF,   // !a
  LENGTH_BIF, // length(a)
  ABS_BIF,    // abs(a)     // Also to be used for ~a as a can be forced to be int & 1 added
  SQRT_BIF,   // sqrt(a)
  SIN_BIF,    // sin(x)     // cos(x) = sin(x-pi/2)
  // Multi arg
  MK_TUPLE,   // (a,b,...)  // Takes n elems
  // End of BIFs
  DEF_FUN_START
} BIF;

typedef enum {
  CREATE_VARIABLE,
  DELETE_VARIABLE,
  ASSIGN_VARIABLE,
  JUMP_IF,
  JUMP,
  RETURN
} Opcode;

typedef enum {
  INT,
  FLOAT,
  ATOM,
  LIST,
  TUPLE
} TypeCode;

typedef struct ListMember ListMember;
typedef struct List List;
typedef struct Bits Bits;
typedef struct Tuple Tuple;
typedef union AnyVal AnyVal;

struct List {
  ListMember* head;
  int size;
};

struct Bits {
  int size;
  void* start;
};

struct Tuple {
  int size;
  union AnyVal* elems;
  TypeCode* types;
};

union AnyVal {

  int asInt;
  float asFloat;

  List* asList;
  Tuple asTuple;
  Bits asBits;
};


struct ListMember {
  AnyVal value;
  struct ListMember* next;
  TypeCode type;
};

typedef struct {
  AnyVal val;
  TypeCode type;
} Variable;

static ListMember EMPTY_LIST_MEMBER;
static List EMPTY_LIST = {&EMPTY_LIST_MEMBER, 0};

// Lists
List* cons(Variable* head, List* tail) {
  ListMember* new_head = (ListMember*)malloc(sizeof(ListMember));
  List* rtn = (List*)malloc(sizeof (List));
  new_head->next = tail->head;
  new_head->type = head->type;
  new_head->value = head->val;
  rtn->head = new_head;
  rtn->size = tail->size + 1;
  return rtn;
}

void cons_inplace(Variable* head, List* list) {
  ListMember* new_head = (ListMember*)malloc(sizeof(ListMember));
  new_head->next = list->head;
  new_head->type = head->type;
  new_head->value = head->val;
  list->head = new_head;
  list->size++;
}

void del_tuple(Tuple* val) {
  free(val->elems);
  free(val->types);
}

void del_all_members(ListMember* current) {
  if (current->next == NULL) return;
  del_all_members(current->next);
  free(current);
}

void del_list(List* val) {
  if (val == &EMPTY_LIST) return;
  del_all_members(val->head);
  free(val);
  return;
}

typedef unsigned int Positional;

typedef struct {
  unsigned int id;
  Positional location;
  TypeCode returns;
} Function;

typedef enum {
  INT_LITERAL,
  FLOAT_LITERAL,
  ATOMIC_LITERAL,
  VARIABLE,
  TUPLE_ELEM,
  HEAD_OF_LIST,
  FUNCTION_CALL
} ValueType;

typedef struct {
  unsigned int elem;
  unsigned int varIndex;
} TupleValue;
 
typedef union {
  AnyVal asLiteral;
  unsigned int asVarIndex;
  TupleValue asElem;
  Positional asSection;
  Function asFunction;
} ValueRef;

typedef struct {
  ValueType type;
  ValueRef value;
} Value;

typedef TypeCode CreateVariableOp;

typedef unsigned int DeleteVariableOp;

typedef struct {
  int index;
  Value new_value;
} AssignVariableOp;

typedef struct {
  Value predicate;
  Positional dest;
} JumpConditionalOp;

typedef struct {
  Positional dest;
} JumpUnconditionalOp;

typedef Value ReturnValueOp;

typedef union {
  CreateVariableOp cvo;
  DeleteVariableOp dvo;
  AssignVariableOp avo;
  JumpConditionalOp jco;
  JumpUnconditionalOp juo;
  ReturnValueOp rvo;
} Operand;

typedef struct {
  Opcode opcode;
  Operand operand;
} Instruction;

// Tuple
Tuple tuple(int size, ...) {
  Tuple rtn;
  AnyVal* elems = (AnyVal*)malloc(size * sizeof(AnyVal));
  TypeCode* types = (TypeCode*)malloc(size * sizeof(TypeCode));
  va_list args;
  va_start(args,size);
  for (int i = 0; i < size; i++) {
    Variable elem = *va_arg(args, Variable*);
    elems[i] = elem.val;
    types[i] = elem.type;
  }
  va_end(args);
  rtn.size = size;
  rtn.elems = elems;
  rtn.types = types;
  return rtn;
}


#endif
