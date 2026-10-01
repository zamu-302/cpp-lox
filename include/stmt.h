#pragma once
#include "expression.h"
#include <memory>
#include <vector>

class PrintStmt;
class ExprStmt;
class VarStmt;
class Block;

class StmtVisitor {
public:
  virtual void visitExprStmt(const ExprStmt &stmt) = 0;
  virtual void visitPrintStmt(const PrintStmt &stmt) = 0;
  virtual void visitVarStmt(const VarStmt &stmt) = 0;
  virtual void visitBlockStmt(const Block &stmt) = 0;
  virtual ~StmtVisitor() = default;
};

class Stmt {
public:
  virtual void accept(StmtVisitor &visitor) = 0;
  virtual ~Stmt() = default;
};

class ExprStmt : public Stmt {
public:
  std::unique_ptr<Expr> expr;
  ExprStmt(std::unique_ptr<Expr> expr) : expr{std::move(expr)} {}

  void accept(StmtVisitor &visitor) override {
    return visitor.visitExprStmt(*this);
  }
};

class PrintStmt : public Stmt {
public:
  std::unique_ptr<Expr> expr;
  PrintStmt(std::unique_ptr<Expr> expr) : expr{std::move(expr)} {}
  void accept(StmtVisitor &visitor) override {
    return visitor.visitPrintStmt(*this);
  }
};
class VarStmt : public Stmt {
public:
  Token name;
  std::unique_ptr<Expr> initalizer;
  VarStmt(const Token &name, std::unique_ptr<Expr> initalizer)
      : name{name}, initalizer{std::move(initalizer)} {}

  void accept(StmtVisitor &visitor) override { visitor.visitVarStmt(*this); }
};
class Block : public Stmt {
public:
  std::vector<std::unique_ptr<Stmt>> state;
  Block(std::vector<std::unique_ptr<Stmt>> state) : state{std::move(state)} {}

  void accept(StmtVisitor &visitor) override {
    return visitor.visitBlockStmt(*this);
  }
};
