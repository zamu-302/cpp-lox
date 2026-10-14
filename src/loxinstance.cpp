#include "../include/loxinstance.h"
#include "../include/loxclass.h"
#include <any>

std::string LoxInstance::toString() { return klass->toString() + "instance"; }
std::any LoxInstance::get(Token name) {
  if (fields.contains(name.getLexeme())) {
    return fields[name.getLexeme()];
  }
  LoxFunction *method = klass->findMethod(name.getLexeme());
  if (method != nullptr) {
    return method->bind(this);
  }
  throw RuntimeError(name, "property not in fields.");
}
void LoxInstance::set(Token name, std::any value) {
  fields[name.getLexeme()] = value;
}
