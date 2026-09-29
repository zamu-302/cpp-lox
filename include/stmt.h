#pragma once
#include "expression.h"
#include <memory>

class PrintStmt;
class ExprStmt;

class StmtVisitor {
public:
  virtual void visitExprStmt(const ExprStmt &stmt) = 0;
  virtual void visitPrintStmt(const PrintStmt &stmt) = 0;
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
