#include "../include/resolver.h"
#include "../include/interpreter.h"

#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

void Resolver::visitBlockStmt(const Block &stmt) {
  beginScope();
  resolve(stmt.state);
  endScope();
}

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>> &stmts) {
  for (const auto &stmt : stmts) {
    resolve(stmt);
  }
}
void Resolver::resolve(const std::unique_ptr<Stmt> &stmt) {
  stmt->accept(*this);
}
void Resolver::resolve(const std::unique_ptr<Expr> &expr) {
  expr->accept(*this);
}

void Resolver::beginScope() { scopes.emplace_back(); }
void Resolver::endScope() { scopes.pop_back(); }

void Resolver::visitVarStmt(const VarStmt &stmt) {
  declare(stmt.name);
  if (stmt.initalizer != nullptr) {
    resolve(stmt.initalizer);
  }
  define(stmt.name);
}
void Resolver::define(Token name) {
  if (scopes.empty())
    return;
  auto &scope = scopes.back();
  scope[name.getLexeme()] = true;
}
void Resolver::declare(Token name) {
  if (scopes.empty()) {
    return;
  }
  auto &scope = scopes.back();
  if (scope.contains(name.getLexeme())) {
    reporter.error(name, "Already assigned variable in this scope");
  }
  scope[name.getLexeme()] = false;
}
std::any Resolver::visitVariable(const Variable &expr) {
  if (!scopes.empty() && scopes.back()[expr.name.getLexeme()] == false) {
    reporter.error(expr.name,
                   "can't read local variable in its own initalizer.");
  }
  resolveLocal(&expr, expr.name);
  return std::any{};
}
void Resolver::resolveLocal(const Expr *expr, Token token) {
  for (int i = scopes.size() - 1; i >= 0; i--) {
    if (scopes[i].contains(token.getLexeme())) {
      interpreter.resolve(expr, scopes.size() - 1 - i);
      return;
    }
  }
}
std::any Resolver::visitAssign(const Assign &expr) {
  resolve(expr.value);

  resolveLocal(&expr, expr.name);
  return std::any{};
}
void Resolver::visitFunctionStmt(const Function &stmt) {
  declare(stmt.name);
  define(stmt.name);
  resolveFunction(&stmt, FunctionType::FUNCTION);
}
void Resolver::resolveFunction(const Function *stmt, FunctionType type) {
  FunctionType enclosing = curr;
  curr = type;
  beginScope();
  for (const auto &param : stmt->params) {
    declare(param);
    define(param);
  }
  resolve(stmt->body);
  endScope();
  curr = enclosing;
}
void Resolver::visitExprStmt(const ExprStmt &stmt) { resolve(stmt.expr); }
void Resolver::visitIfStmt(const If &stmt) {
  resolve(stmt.condition);
  resolve(stmt.thenBranch);
  if (stmt.elseBranch != nullptr) {
    resolve(stmt.elseBranch);
  }
}
void Resolver::visitPrintStmt(const PrintStmt &stmt) { resolve(stmt.expr); }
void Resolver::visitReturnStmt(const Return &stmt) {
  if (curr == FunctionType::NONE) {
    reporter.error(stmt.keyword, "can't return from Top level code.");
  }
  if (stmt.value == nullptr) {
    return;
  }
  resolve(stmt.value);
}
void Resolver::visitWhileStmt(const While &stmt) {
  resolve(stmt.condition);
  resolve(stmt.body);
}
std::any Resolver::visitBinary(const Binary &expr) {
  resolve(expr.left);
  resolve(expr.right);
  return std::any{};
}
std::any Resolver::visitCall(const Call &expr) {
  resolve(expr.callee);
  for (auto &args : expr.arguments) {
    resolve(args);
  }
  return std::any{};
}
std::any Resolver::visitGrouping(const Grouping &expr) {
  resolve(expr.expression);
  return std::any{};
}
std::any Resolver::visitLiteral(const Literals &expr) { return std::any{}; }

std::any Resolver::visitUnary(const Unary &expr) {
  resolve(expr.right);
  return std::any{};
}
std::any Resolver::visitLogical(const Logical &expr) {
  resolve(expr.left);
  resolve(expr.right);
  return std::any{};
}
void Resolver::visitClassStmt(const CLASS &stmt) {
  ClassType enclosingClass = currClass;
  currClass = ClassType::CLASS;
  declare(stmt.token);
  define(stmt.token);
  beginScope();
  scopes.back()["this"] = true;
  for (auto &method : stmt.methods) {
    FunctionType declaration = FunctionType::METHOD;
    resolveFunction(method.get(), declaration);
  }
  endScope();
  currClass = ClassType::NONE;
}
std::any Resolver::visitGet(const Get &expr) {
  resolve(expr.obj);
  return std::any{};
}
std::any Resolver::visitSet(const Set &expr) {
  resolve(expr.obj);
  resolve(expr.right);
  return std::any{};
}
std::any Resolver::visitThis(const This &expr) {
  resolveLocal(&expr, expr.name);
  return std::any{};
}
