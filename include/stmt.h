#pragma once
#include "expression.h"
#include <memory>
#include <vector>

class PrintStmt;
class ExprStmt;
class VarStmt;
class Block;
class Function;
class If;

class StmtVisitor {
public:
  virtual void visitExprStmt(const ExprStmt &stmt) = 0;
  virtual void visitPrintStmt(const PrintStmt &stmt) = 0;
  virtual void visitVarStmt(const VarStmt &stmt) = 0;
  virtual void visitBlockStmt(const Block &stmt) = 0;
  virtual void visitFunctionStmt(const Function &stmt) = 0;
  virtual void visitIfStmt(const If &stmt) = 0;
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
class Function : public Stmt {
public:
  Token name;
  std::vector<Token> params;
  std::vector<std::unique_ptr<Stmt>> body;
  Function(const Token &name, const std::vector<Token> &params,
           std::vector<std::unique_ptr<Stmt>> &body)
      : name{name}, params{params}, body{std::move(body)} {}
  void accept(StmtVisitor &visitor) { visitor.visitFunctionStmt(*this); }
};
class If : public Stmt {
public:
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> thenBranch;
  std::unique_ptr<Stmt> elseBranch;
  If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,
     std::unique_ptr<Stmt> elseBranch)
      : condition{std::move(condition)}, thenBranch{std::move(thenBranch)},
        elseBranch{std::move(elseBranch)} {}
  void accept(StmtVisitor &visitor) { visitor.visitIfStmt(*this); }
};
