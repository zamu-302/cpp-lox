#include "../include/resolver.h"
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
  resolveFunction(&stmt);
}
void Resolver::resolveFunction(const Function *stmt) {
  beginScope();
  for (const auto &param : stmt->params) {
  }
}
