#include "../include/loxclass.h"
#include "../include/interpreter.h"
#include "../include/loxinstance.h"
std::any LoxClass::call(Interpreter &interpreter,
                        const std::vector<std::any> &args) {
  LoxInstance *instance = new LoxInstance(this);
  LoxFunction *initalizer = findMethod("init");
  if (initalizer != nullptr) {
    initalizer->bind(instance)->call(interpreter, args);
  }
  return instance;
}
LoxFunction *LoxClass::findMethod(const std::string &name) {
  if (methods.contains(name)) {
    return methods[name];
  }
  if (superClass != nullptr) {
    return superClass->findMethod(name);
  }
  return nullptr;
}
int LoxClass::arity() {
  LoxFunction *initalizer = findMethod("init");
  if (initalizer == nullptr) {
    return 0;
  }
  return initalizer->arity();
}
