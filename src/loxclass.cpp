#include "../include/loxclass.h"
#include "../include/interpreter.h"
#include "../include/loxinstance.h"
std::any LoxClass::call(Interpreter &interpreter,
                        const std::vector<std::any> &args) {
  LoxInstance *instance = new LoxInstance(this);
  return instance;
}

int LoxClass::arity() { return 0; }
