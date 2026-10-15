#pragma once
#include "error_reporter.h"
#include "expression.h"
#include "stmt.h"
#include "token.h"
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
enum class FunctionType {
  NONE,
  FUNCTION,
  METHOD,
};
enum class ClassType {
  NONE,
  CLASS,
};
class Interpreter;
class Resolver : public Visitor, public StmtVisitor {
public:
  Resolver(Interpreter &interpreter, ErrorReporter reporter)
      : interpreter{interpreter}, reporter{reporter} {}
  void resolve(const std::vector<std::unique_ptr<Stmt>> &stmts);

  // stmt visitors
  void visitBlockStmt(const Block &stmt) override;
  void visitVarStmt(const VarStmt &stmt) override;
  void visitFunctionStmt(const Function &stmt) override;
  void visitExprStmt(const ExprStmt &stmt) override;
  void visitIfStmt(const If &stmt) override;
  void visitPrintStmt(const PrintStmt &stmt) override;
  void visitWhileStmt(const While &stmt) override;
  void visitClassStmt(const CLASS &stmt) override;
  void visitReturnStmt(const Return &stmt) override;

  // expr visitors
  std::any visitVariable(const Variable &expr) override;
  std::any visitAssign(const Assign &expr) override;
  std::any visitBinary(const Binary &expr) override;
  std::any visitCall(const Call &expr) override;
  std::any visitGrouping(const Grouping &expr) override;
  std::any visitLiteral(const Literals &expr) override;
  std::any visitLogical(const Logical &expr) override;
  std::any visitUnary(const Unary &expr) override;
  std::any visitGet(const Get &expr) override;
  std::any visitSet(const Set &expr) override;
  std::any visitThis(const This &expr) override;

private:
  Interpreter &interpreter;
  ErrorReporter reporter;
  std::vector<std::unordered_map<std::string, bool>> scopes;
  // helpers
  void beginScope();
  void endScope();
  void resolve(const std::unique_ptr<Stmt> &stmt);
  void resolve(const std::unique_ptr<Expr> &expr);
  void resolveLocal(const Expr *expr, Token token);
  void resolveFunction(const Function *stmt, FunctionType type);
  void declare(Token name);
  void define(Token name);

  FunctionType curr = FunctionType::NONE;
  ClassType currClass = ClassType::NONE;
};
