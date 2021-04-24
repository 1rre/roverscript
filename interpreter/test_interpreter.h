#ifndef TEST_INTERPRETER_H
#define TEST_INTERPRETER_H

#include "interpret.h"

Value value(ValueType t, ValueRef r) {
  Value rtn;
  rtn.value = r;
  rtn.type = t;
  return rtn;
}

AssignVariableOp avo(int i, Value v) {
  AssignVariableOp rtn;
  rtn.index = i;
  rtn.new_value = v;
  return rtn;
}

Function fun(int i, Positional l) {
  Function rtn;
  rtn.id = i;
  rtn.location = l;
  return rtn;
}

int test_interpret() {
  ValueRef t;
  Instruction inst[16];
  int i = 0;
  inst[i].opcode = CREATE_VARIABLE;
  inst[i++].operand.cvo = INT;
  
  t.asLiteral.asInt = 5;
  inst[i].opcode = ASSIGN_VARIABLE;
  inst[i++].operand.avo = avo(0, value(INT_LITERAL, t));

  inst[i].opcode = CREATE_VARIABLE;
  inst[i++].operand.cvo = LIST;

  t.asLiteral.asInt = 12;
  inst[i].opcode = ASSIGN_VARIABLE;
  inst[i++].operand.avo = avo(-1, value(INT_LITERAL, t));
  
  t.asVarIndex = 1;
  inst[i].opcode = ASSIGN_VARIABLE;
  inst[i++].operand.avo = avo(-2, value(VARIABLE, t));

  t.asFunction = fun(CONS_BIF,-1);
  inst[i].opcode = ASSIGN_VARIABLE;
  inst[i++].operand.avo = avo(1, value(FUNCTION_CALL, t));
  
  t.asVarIndex = 0;
  inst[i].opcode = ASSIGN_VARIABLE;
  inst[i++].operand.avo = avo(-1, value(VARIABLE, t));
  
  t.asVarIndex = 1;
  inst[i].opcode = ASSIGN_VARIABLE;
  inst[i++].operand.avo = avo(-2, value(VARIABLE, t));

  t.asFunction = fun(CONS_BIF, -1);
  inst[i].opcode = ASSIGN_VARIABLE;
  inst[i++].operand.avo = avo(1, value(FUNCTION_CALL, t));
  t.asVarIndex = 0;

  t.asVarIndex = 1;
  inst[i].opcode = RETURN;
  inst[i++].operand.rvo = value(HEAD_OF_LIST, t);

  int rtnVal = interpret(inst, 0, NULL, 0, NULL).asInt;
  fwrite("%d\n", rtnVal);
  return rtnVal;
}

#endif
