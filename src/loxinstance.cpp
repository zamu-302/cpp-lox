#include "../include/loxinstance.h"
#include "../include/loxclass.h"
#include <any>

std::string LoxInstance::toString() { return klass->toString() + "instance"; }
std::any LoxInstance::get(Token name) {
  if (fields.contains(name.getLexeme())) {
    return fields[name.getLexeme()];
  }
  return RuntimeError(name, "property not in fields.");
}
