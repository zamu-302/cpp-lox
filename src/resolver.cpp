#include "../include/resolver.h"
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
