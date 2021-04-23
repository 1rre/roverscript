#ifndef INTERPRET_H
#define INTERPRET_H

#include "opcode.h"
#include "types.h"
#include <stdio.h>

Variable get_literal(Value val, Variable* vars, int* n_args, Variable** args) {
  Variable rtn;
  switch (val.type) {
    case INT_LITERAL:
      rtn.type = INT;
      goto load_lit;
    case FLOAT_LITERAL:
      rtn.type = FLOAT;
      goto load_lit;
    case ATOMIC_LITERAL:
      rtn.type = ATOM;
      goto load_lit;
    case VARIABLE:
      return vars[val.value.asVarIndex];
    case TUPLE_ELEM:
      // TODO: Get Type
      rtn.val = *vars[val.value.asElem.varIndex].val.asTuple.elems[val.value.asElem.elem];
    case HEAD_OF_LIST:
      // TODO: Get type
      rtn.val = *vars[val.value.asVarIndex].val.asList.head;
    case FUNCTION_CALL:
      if (val.value.asFunction.id < DEFFUN_START) {
        switch (val.value.asFunction.id) {
          case ADD_BIF:
            if ((*args)[1].type == INT && (*args)[1].type == INT) {
              rtn.type = INT;
              rtn.val.asInt = (*args)[0].val.asInt + (*args)[1].val.asInt;
            } else {
              rtn.type = FLOAT;
              rtn.val.asFloat = (*args)[0].type == FLOAT? (*args)[0].val.asFloat: (*args)[0].val.asInt + (*args)[1].type == FLOAT? (*args)[1].val.asFloat: (*args)[1].val.asInt;
            }
          break;
        }
      }
      free(*args);
      *args = NULL;
      *n_args = 0;
    break;
  }
  return rtn;
load_lit:
  rtn.val = val.value.asLiteral;
  return rtn;
}

int interpret(Instruction* inst, int n_vars, Variable* vars, int n_args, Variable* args) {
  Serial.print(inst->opcode);
  switch(inst->opcode) {
    case CREATE_VARIABLE:
      vars = (Variable*)realloc(vars, (n_vars+1) * sizeof(Variable));
      vars[n_vars].type = inst->operand.cvo.type;
    break;
    case DELETE_VARIABLE:
      vars = (Variable*)realloc(vars, (n_vars-inst->operand.dvo) * sizeof(Variable));
    break;
    case ASSIGN_VARIABLE:
      if (inst->operand.avo.index < 0) {
        args = (Variable*)realloc(args, (n_args+1)*sizeof(Variable));
        args[n_args] = get_literal(inst->operand.avo.new_value,vars,&n_args,&args);
      } else {
        vars[inst->operand.avo.index] = get_literal(inst[0].operand.avo.new_value,vars,&n_args,&args);
      }
    break;
    case PREPEND_LIST:
      // TODO: Lists
    break;
    case JUMP_IF:
      // TODO: Jump
    break;
    case JUMP:
      // TODO: Jump
    break;
    case RETURN:
    //int r_val = get_literal(inst->operand.rvo,vars,&n_args,&args).val.asInt;
    //free(vars);
    //return r_val;
    return 0;
  }
  interpret(inst+1,n_vars,vars,n_args,args);
}



#endif
