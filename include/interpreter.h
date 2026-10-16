#include "environment.h"
#include "error_reporter.h"
#include "expression.h"
#include "loxcallable.h"
#include "stmt.h"
#include "token.h"
#include <any>
#include <charconv>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
class Interpreter : public Visitor, public StmtVisitor {
public:
  Interpreter(const ErrorReporter &reporter) : reporter{reporter} {
    globals.define("clock", (LoxCallable *)&clockFn);
  }

  Environment globals;
  ClockCallable clockFn;

  // Expr
  std::any visitLogical(const Logical &logical) override;
  std::any visitGet(const Get &expr) override;
  std::any visitSet(const Set &expr) override;
  std::any visitThis(const This &expr) override;
  std::any visitSuper(const Super &expr) override;
  std::any visitLiteral(const Literals &expr) override;
  std::any visitGrouping(const Grouping &expr) override;
  std::any visitAssign(const Assign &expr) override;
  std::any visitVariable(const Variable &expr) override;
  std::any visitUnary(const Unary &expr) override;
  std::any visitBinary(const Binary &expr) override;
  std::any visitCall(const Call &expr) override;

  // stmt
  void visitIfStmt(const If &stmt) override;
  void visitReturnStmt(const Return &stmt) override;
  void visitFunctionStmt(const Function &stmt) override;
  void visitWhileStmt(const While &stmt) override;
  void visitClassStmt(const CLASS &stmt) override;
  void visitExprStmt(const ExprStmt &stmt) override;
  void visitPrintStmt(const PrintStmt &stmt) override;
  void visitVarStmt(const VarStmt &stmt) override;
  void visitBlockStmt(const Block &stmt) override;

  // helper
  void interpret(std::vector<std::unique_ptr<Stmt>> statements);
  void resolve(const Expr *expr, int depth);
  std::any lookupVariable(Token name, const Expr *expr);
  void executeBlock(const std::vector<std::unique_ptr<Stmt>> &statement,
                    Environment *env);

private:
  ErrorReporter reporter;
  std::unordered_map<const Expr *, int> locals;
  Environment *environment = &globals;
  std::any evaluate(Expr *expr);
  std::any evaluate(const std::unique_ptr<Expr> &expr);
  bool isTruly(std::any expr);
  bool isEqual(std::any left, std::any right);
  void checkNumberOperands(Token token, std::any left, std::any right);
  void checkNumberOperands(Token token, std::any operand);
  std::string stringify(std::any obj);
  void execute(const std::unique_ptr<Stmt> &stmt);
};
