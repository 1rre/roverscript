#ifndef OPCODE_H
#define OPCODE_H

typedef struct {
  AnyVal val;
  TypeCode type;
} Variable;

typedef unsigned int Positional;

typedef struct {
  unsigned int id;
  Positional location;
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
  int list;
  Variable new_head;
} PrependListOp;

typedef struct {
  Positional start;
  Positional end;
} FunctionHeaderOp;

typedef struct {
  Value predicate;
  Positional dest;
  Positional r_address;
} JumpConditionalOp;

typedef struct {
  Positional dest;
  Positional r_address;
} JumpUnconditionalOp;

typedef Value ReturnValueOp;

typedef union {
  CreateVariableOp cvo;
  DeleteVariableOp dvo;
  AssignVariableOp avo;
  PrependListOp plo;
  FunctionHeaderOp fho;
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
