#include "../include/interpreter.h"
#include "../include/loxclass.h"
#include "../include/loxfunction.h"
#include "../include/loxinstance.h"
#include <any>
#include <memory>
#include <string>
#include <unordered_map>
void Interpreter::visitReturnStmt(const Return &stmt) {
  std::any value = nullptr;
  if (stmt.value != nullptr) {
    value = evaluate(stmt.value);
  }
  throw ReturnException(value);
}
std::any Interpreter::visitGet(const Get &expr) {
  std::any obj = evaluate(expr.obj);
  if (typeid(LoxInstance *) == obj.type()) {
    LoxInstance *instances = std::any_cast<LoxInstance *>(obj);
    return instances->get(expr.token);
  }
  return RuntimeError(expr.token, "Only instances have properties.");
}
std::any Interpreter::visitSet(const Set &expr) {
  std::any obj = evaluate(expr.obj);
  if (obj.type() != typeid(LoxInstance *)) {
    return RuntimeError(expr.name, "Only Instances have fields.");
  }
  std::any value = evaluate(expr.right);
  LoxInstance *instances = std::any_cast<LoxInstance *>(value);
  instances->set(expr.name, value);

  return value;
}
std::any Interpreter::lookupVariable(Token name, const Expr *expr) {
  if (locals.contains(expr)) {
    return environment->getAt(locals[expr], name.getLexeme());
  } else {
    return globals.get(name);
  }
}
std::any Interpreter::visitThis(const This &expr) {
  return lookupVariable(expr.name, &expr);
}
std::any Interpreter::visitLogical(const Logical &logical) {
  std::any left = evaluate(logical.left);

  if (logical.token.getType() == TokenType::OR) {
    if (isTruly(left)) {
      return left;
    }
  } else {
    if (!isTruly(left)) {
      return left;
    }
  }
  return evaluate(logical.right);
}
void Interpreter::visitClassStmt(const CLASS &stmt) {
  LoxClass *superClass = nullptr;
  if (stmt.superClass != nullptr) {
    std::any eval = evaluate(stmt.superClass.get());
    try {
      superClass = std::any_cast<LoxClass *>(eval);
    } catch (const std::bad_any_cast &) {
      throw RuntimeError(stmt.superClass->name, "Superclass must be a class.");
    }
  }
  environment->define(stmt.token.getLexeme(), std::any{});

  if (stmt.superClass != nullptr) {
    environment = new Environment(environment);
    environment->define("super", superClass);
  }

  std::unordered_map<std::string, LoxFunction *> methods;
  for (auto &method : stmt.methods) {
    LoxFunction *function = new LoxFunction(method.get(), environment,
                                            method->name.getLexeme() == "init");
    methods[method->name.getLexeme()] = function;
  }
  LoxClass klass(stmt.token.getLexeme(), methods, superClass);
  if (superClass != nullptr) {
    environment = environment->enclosing;
  }
  environment->assign(stmt.token, klass);
}
std::any Interpreter::evaluate(Expr *expr) { return expr->accept(*this); }
void Interpreter::resolve(const Expr *expr, int depth) { locals[expr] = depth; }
void Interpreter::visitIfStmt(const If &stmt) {
  if (isTruly(evaluate(stmt.condition))) {
    execute(stmt.elseBranch);
  } else if (stmt.elseBranch != nullptr) {
    execute(stmt.elseBranch);
  }
}
std::any Interpreter::visitSuper(const Super &expr) {
  int distance = locals.at(&expr);
  LoxClass *superClass =
      std::any_cast<LoxClass *>(environment->getAt(distance, "super"));

  LoxInstance *instance =
      std::any_cast<LoxInstance *>(environment->getAt(distance - 1, "this"));

  LoxFunction *method = superClass->findMethod(expr.method.getLexeme());

  if (method == nullptr) {
    throw RuntimeError(expr.method,
                       "Undefined property '" + expr.method.getLexeme() + "'.");
  }

  return method->bind(instance);
}
void Interpreter::visitWhileStmt(const While &stmt) {
  while (isTruly(evaluate(stmt.condition))) {
    execute(stmt.body);
  }
}
void Interpreter::visitFunctionStmt(const Function &stmt) {

  LoxFunction *function = new LoxFunction(&stmt, environment, false);
  environment->define(stmt.name.getLexeme(), (LoxCallable *)function);
}

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
void Interpreter::executeBlock(
    const std::vector<std::unique_ptr<Stmt>> &statement, Environment *env) {
  Environment *prev = this->environment;

  this->environment = env;
  for (const auto &s : statement) {
    execute(s);
  }
  this->environment = prev;
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
