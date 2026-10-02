#include "../include/interpreter.h"
#include <any>
#include <memory>
#include <string>
void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>> statements) {
  try {
    for (const auto &statement : statements) {
      execute(statement);
    }
  } catch (RuntimeError error) {
    throw RuntimeError::runtime_error(error);
  }
}
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
void Interpreter::checkNumberOperands(Token token, std::any operand) {
  if (operand.type() == typeid(double)) {
    return;
  }
  throw RuntimeError(token, "operator must be an number.");
}
void Interpreter::checkNumberOperands(Token token, std::any left,
                                      std::any right) {
  if (left.type() == typeid(double) && right.type() == typeid(double)) {
    return;
  }
  throw RuntimeError(token, "operator must be a number.");
}
std::string Interpreter::stringify(std::any obj) {
  if (!obj.has_value())
    return "nil";
  if (obj.type() == typeid(double)) {
    double d = std::any_cast<double>(obj);
    std::string s = std::to_string(d);
    s.erase(s.find_last_not_of('0') + 1);
    if (s.back() == '.')
      s.pop_back();
    return s;
  }
  if (obj.type() == typeid(bool))
    return std::any_cast<bool>(obj) ? "true" : "false";
  if (obj.type() == typeid(std::string))
    return std::any_cast<std::string>(obj);
  return "nil";
}
void Interpreter::execute(const std::unique_ptr<Stmt> &stmt) {
  stmt->accept(*this);
}
