#ifndef INTERPRET_H
#define INTERPRET_H

#include "opcode.h"

AnyVal interpret(Instruction*, int, Variable*, int, Variable*);

Variable get_literal(Value val, Instruction* inst, Variable* vars, int* n_args, Variable** args) {
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
      rtn.val = vars[val.value.asElem.varIndex].val.asTuple.elems[val.value.asElem.elem];
      rtn.type = vars[val.value.asElem.varIndex].val.asTuple.types[val.value.asElem.elem];
    case HEAD_OF_LIST:
      rtn.val = vars[val.value.asVarIndex].val.asList->head->value;
      rtn.type = vars[val.value.asVarIndex].val.asList->head->type;
    case FUNCTION_CALL:
      if (val.value.asFunction.id < DEF_FUN_START) {
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

          case CONS_BIF:
            rtn.type = LIST;
            rtn.val.asList = cons(&(*args)[0], (*args)[1].val.asList);
          break;
        }
      } else {
        rtn.val = interpret(inst + val.value.asFunction.location,*n_args,*args,0,NULL);
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

AnyVal interpret(Instruction* inst, int n_vars, Variable* vars, int n_args, Variable* args) {

  Variable temp;
  void* to_free = NULL;

  switch(inst->opcode) {
    case CREATE_VARIABLE:
      vars = (Variable*)realloc(vars, (n_vars+1) * sizeof(Variable));
      vars[n_vars].type = inst->operand.cvo;
      if (inst->operand.cvo == LIST) vars[n_vars].val.asList = &EMPTY_LIST;
      // TODO: Make tuple
      n_vars++;
    break;
    case DELETE_VARIABLE:
      for (int i = n_vars-inst->operand.dvo-1; i < n_vars; i++) 
        if (vars[i].type == TUPLE) del_tuple(&(vars[i].val.asTuple));
        else if (vars[i].type == LIST) del_list(vars[i].val.asList);
      vars = (Variable*)realloc(vars, (n_vars - inst->operand.dvo) * sizeof(Variable));
    break;
    case ASSIGN_VARIABLE:
      if (inst->operand.avo.index < 0) {
        args = (Variable*)realloc(args, (n_args+1)*sizeof(Variable));
        args[n_args] = get_literal(inst->operand.avo.new_value,inst,vars,&n_args,&args);
        n_args++;
      } else {
        // We need to detect whether the list members are still being used (cons)
        if (vars[inst->operand.avo.index].type == LIST) to_free = vars[inst->operand.avo.index].val.asList;
        vars[inst->operand.avo.index] = get_literal(inst->operand.avo.new_value,inst,vars,&n_args,&args);
      }
      free(to_free);
    break;
    case JUMP_IF:
      temp = get_literal(inst->operand.jco.predicate,inst,vars,&n_args,&args);
      if (temp.val.asInt) return interpret(inst + inst->operand.jco.dest, n_vars, vars, n_args, args);
    break;
    case JUMP:
      return interpret(inst + inst->operand.juo.dest,n_args,vars,n_args,args);
    case RETURN:
    temp.val = get_literal(inst->operand.rvo,inst,vars,&n_args,&args).val;
    // If it's a list this may delete parts of it?
    for (int i = 0; i < n_vars; i++)
      if (vars[i].type == TUPLE) del_tuple(&(vars[i].val.asTuple));
      else if (vars[i].type == LIST) del_list(vars[i].val.asList);
    for (int i = 0; i < n_args; i++)
      if (args[i].type == TUPLE) del_tuple(&(args[i].val.asTuple));
      else if (args[i].type == LIST) del_list(args[i].val.asList);
    free(vars);
    free(args);
    return temp.val;
  }
  return interpret(inst+1,n_vars,vars,n_args,args);
}



#endif
