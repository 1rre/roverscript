#ifndef OPCODE_H
#define OPCODE_H

/*
Do we want variable length instructions so that we can have 32 bit values?

Opcode Fields:
  opcode <- bitfield representing an operation
  variable ref <- sequential ID for variables (AnyVal* stored in an array)
  label ref <- sequential ID for labels (index of char in received data)
  type code <- bitfield representing a type (one of Int,Float,List,Tuple,Atom,Variable?)
  value <- raw value for type

Opcodes:
  Create variable of type
  {opcode} {variable ref} {}

  Delete variable of type
  {opcode} {variable ref}

  Assign to variable
  {opcode} {variable ref} {type code} {value}

  Prepend to list
  {opcode} {variable ref} {type code} {value}

  +, *, >=, etc.
  {opcode} {type code} {value} {type code} {value}

  label/function header?
    Maybe store as start position in array to be interpreted? (ie variable ref?)
  {opcode} {label ref}

  unconditional jump
  {opcode} {label ref}

  conditional jump
  {opcode} {type code} {value}
*/

typedef unsigned char Opcode;
typedef unsigned int Variable;
typedef unsigned char TypeCode;

typedef enum {
  INT_LITERAL,
  FLOAT_LITERAL,
  ATOMIC_LITERAL,
  TUPLE_ELEM,
  HEAD_OF_LIST
} ValueType;

typedef struct {
  unsigned int elem;
  Variable var;
} TupleValue;
 
typedef union {
  int asInt;
  float asFloat;
  Variable asVariable;
  TupleValue asTupleElem;
} ValueRef;

typedef struct {
  ValueType type;
  ValueRef val;
} Value;

typedef union {

} Operand;

typedef struct {
  Opcode opcode;
  Operand operand;
} Instruction;



#endif
