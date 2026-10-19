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
    execute(stmt.thenBranch);
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

std::any Interpreter::visitLiteral(const Literals &expr) {
  switch (expr.kind) {
  case Literals::Kind::Bool:
    return expr.boolean;
  case Literals::Kind::Number:
    return expr.val;
  case Literals::Kind::Nil:
    break;
  case Literals::Kind::String:
    return expr.value;
  }
  return std::any{};
}

std::any Interpreter::visitGrouping(const Grouping &expr) {
  return evaluate(expr.expression);
}

void Interpreter::visitExprStmt(const ExprStmt &stmt) { evaluate(stmt.expr); }

void Interpreter::visitPrintStmt(const PrintStmt &stmt) {
  std::any value = evaluate(stmt.expr);
  std::cout << stringify(value) << '\n';
}

void Interpreter::visitVarStmt(const VarStmt &stmt) {
  std::any val;
  if (stmt.initalizer != nullptr) {
    val = evaluate(stmt.initalizer);
  }
  environment->define(stmt.name.getLexeme(), val);
}

std::any Interpreter::visitAssign(const Assign &expr) {
  std::any value = evaluate(expr.value);
  if (locals.contains(&expr)) {
    environment->assignAt(locals[&expr], expr.name, value);
  } else {
    environment->assign(expr.name, value);
  }
  return value;
}

void Interpreter::visitBlockStmt(const Block &stmt) {
  executeBlock(stmt.state, new Environment(environment));
}

std::any Interpreter::visitCall(const Call &expr) {
  std::any callee = evaluate(expr.callee);
  std::vector<std::any> args;
  for (auto &arg : expr.arguments) {
    args.emplace_back(evaluate(std::move(arg)));
  }

  if (callee.type() != typeid(LoxCallable *)) {
    throw RuntimeError(expr.paren, "can only call functions and classes.");
  }
  LoxCallable *function = std::any_cast<LoxCallable *>(callee);
  if (args.size() != function->arity()) {
    throw RuntimeError(expr.paren, "Expected " +
                                       std::to_string(function->arity()) +
                                       " arguments but got " +
                                       std::to_string(args.size()) + ".");
  }

  return function->call(*this, args);
}

std::any Interpreter::evaluate(const std::unique_ptr<Expr> &expr) {
  return expr->accept(*this);
}

std::any Interpreter::visitVariable(const Variable &variable) {
  return lookupVariable(variable.name, &variable);
}

std::any Interpreter::visitUnary(const Unary &expr) {
  std::any right = evaluate(expr.right);
  switch (expr.opr.getType()) {
  case TokenType::MINUS:
    checkNumberOperands(expr.opr, right);
    return -1 * std::any_cast<double>(right);
  case TokenType::BANG:
    return !isTruly(right);
  default:
    break;
  }
  return std::any{};
}

std::any Interpreter::visitBinary(const Binary &expr) {
  std::any left = evaluate(expr.left);
  std::any right = evaluate(expr.right);

  switch (expr.opr.getType()) {
  case TokenType::MINUS:
    checkNumberOperands(expr.opr, left, right);
    return std::any_cast<double>(left) - std::any_cast<double>(right);

  case TokenType::PLUS:
    if ((typeid(double) == left.type()) && (typeid(double) == right.type())) {
      return std::any_cast<double>(left) + std::any_cast<double>(right);
    }

    if ((typeid(std::string) == left.type()) &&
        (typeid(std::string) == right.type())) {
      std::cout << "it's a string" << std::endl;
      return std::any_cast<std::string>(left) +
             std::any_cast<std::string>(right);
    }
    throw RuntimeError(expr.opr,
                       "Operands must be two numbers or two strings.");

  case TokenType::SLASH:
    checkNumberOperands(expr.opr, left, right);
    return std::any_cast<double>(left) / std::any_cast<double>(right);

  case TokenType::STAR:
    checkNumberOperands(expr.opr, left, right);
    return std::any_cast<double>(left) * std::any_cast<double>(right);
  case TokenType::GREATER:
    checkNumberOperands(expr.opr, left, right);
    return std::any_cast<double>(left) > std::any_cast<double>(right);
  case TokenType::GREATER_EQUAL:
    checkNumberOperands(expr.opr, left, right);
    return std::any_cast<double>(left) >= std::any_cast<double>(right);
  case TokenType::LESS:
    checkNumberOperands(expr.opr, left, right);
    return std::any_cast<double>(left) < std::any_cast<double>(right);
  case TokenType::LESS_EQUAL:
    checkNumberOperands(expr.opr, left, right);
    return std::any_cast<double>(left) <= std::any_cast<double>(right);
  case TokenType::BANG_EQUAL:
    return !isEqual(left, right);
  case TokenType::EQUAL_EQUAL:
    return isEqual(left, right);
  default:
    break;
  }
  return std::any{};
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
