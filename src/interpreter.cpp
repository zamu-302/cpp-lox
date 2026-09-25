#include "../include/interpreter.h"
#include <any>

bool Interpreter::isTruly(std::any obj) {
  if (!obj.has_value()) {
    return false;
  }
  if (obj.type() == typeid(bool)) {
    return std::any_cast<bool>(obj);
  }
  return true;
}

bool Interpreter::isEqual(std::any left, std::any right) {
  if (!left.has_value() && !right.has_value()) {
    return true;
  }
  if (!left.has_value() || !right.has_value()) {
    return false;
  }
  if (left.type() != right.type()) {
    return false;
  }
  if (left.type() == typeid(double))
    return std::any_cast<double>(left) == std::any_cast<double>(right);

  if (left.type() == typeid(bool))
    return std::any_cast<bool>(left) == std::any_cast<bool>(right);

  if (left.type() == typeid(std::string))
    return std::any_cast<std::string>(left) ==
           std::any_cast<std::string>(right);

  return false;
}
void Interpreter::checkNumberOperator(Token token, std::any operand) {
  if (operand.type() == typeid(double)) {
    return;
  }
  throw RuntimeError(token, "operator  must be an number.");
}
