#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "opcode.h"
#include "interpret.h"

void test_interpret() {
  ValueRef t;
  Instruction inst[6];
  inst[0].opcode = CREATE_VARIABLE;
  inst[0].operand.cvo = CreateVariableOp {0,INT};
  
  t.asLiteral.asInt = 5;
  inst[1].opcode = ASSIGN_VARIABLE;
  inst[1].operand.avo = AssignVariableOp{0, Value{INT_LITERAL, t}};

  t.asVarIndex = 0;
  inst[2].opcode = ASSIGN_VARIABLE;
  inst[2].operand.avo = AssignVariableOp{-1, Value{VARIABLE, t}};

  t.asLiteral.asInt = 3;
  inst[3].opcode = ASSIGN_VARIABLE;
  inst[3].operand.avo = AssignVariableOp{-2, Value{INT_LITERAL, t}};
  
  t.asFunction = Function {ADD_BIF};
  inst[4].opcode = ASSIGN_VARIABLE;
  inst[4].operand.avo = AssignVariableOp{0, Value{FUNCTION_CALL, t}};

  t.asVarIndex = 0;
  inst[5].opcode = RETURN;
  inst[5].operand.rvo = ReturnValueOp{Value{VARIABLE, t}};

  int rtnVal = interpret(inst,0,NULL,0,NULL);

  Serial.println(rtnVal);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  test_interpret();
}

void loop() {
  
}
