#include "../include/loxfunction.h"
#include "../include/interpreter.h"
#include "../include/loxinstance.h"
std::any LoxFunction::call(Interpreter &interpreter,
                           const std::vector<std::any> &args) {
  Environment *enviroment(closure);
  for (int i = 0; i < declaration->params.size(); i++) {
    enviroment->define(declaration->params[i].getLexeme(), args[i]);
  }
  try {
    interpreter.executeBlock(declaration->body, enviroment);
  } catch (ReturnException returnvalue) {
    return returnvalue.value;
  }
  delete enviroment;
  return std::any{};
}
int LoxFunction::arity() { return declaration->params.size(); }
std::string LoxFunction::toString() {
  return "<fn " + declaration->name.getLexeme() + ">";
}
LoxFunction *LoxFunction::bind(LoxInstance *instance) {
  Environment enviroment(closure);
  enviroment.define("this", instance);
  return new LoxFunction(declaration, &enviroment);
}
