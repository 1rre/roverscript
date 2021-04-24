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
  Instruction inst[7];
  inst[0].opcode = CREATE_VARIABLE;
  inst[0].operand.cvo = INT;

  t.asLiteral.asInt = 5;
  inst[1].opcode = ASSIGN_VARIABLE;
  inst[1].operand.avo = avo(0, value(INT_LITERAL, t));

  inst[2].opcode = CREATE_VARIABLE;
  inst[2].operand.cvo = INT;

  t.asVarIndex = 0;
  inst[3].opcode = ASSIGN_VARIABLE;
  inst[3].operand.avo = avo(-1, value(VARIABLE, t));

  t.asLiteral.asInt = 12;
  inst[4].opcode = ASSIGN_VARIABLE;
  inst[4].operand.avo = avo(-2, value(INT_LITERAL, t));

  t.asFunction = fun(ADD_BIF, -1);
  inst[5].opcode = ASSIGN_VARIABLE;
  inst[5].operand.avo = avo(0, value(FUNCTION_CALL, t));

  t.asVarIndex = 0;
  inst[6].opcode = RETURN;
  inst[6].operand.rvo = value(VARIABLE, t);

  int rtnVal = interpret(inst, 0, NULL, 0, NULL).asInt;
  fwrite("%d\n", rtnVal);
  return rtnVal;
}

#endif
