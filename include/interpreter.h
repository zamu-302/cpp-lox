#include "error_reporter.h"
#include "expression.h"
#include "stmt.h"
#include "token.h"
#include <any>
#include <memory>
#include <string>
#include <vector>
class Interpreter : public Visitor, public StmtVisitor {
public:
  Interpreter(const ErrorReporter &reporter) : reporter{reporter} {}
  void interpret(std::vector<std::unique_ptr<Stmt>> statements);
  std::any visitLiteral(const Literals &expr) override { return expr.value; }
  std::any visitGrouping(const Grouping &expr) override {
    return evaluate(expr.expression);
  }
  void visitExprStmt(const ExprStmt &stmt) override { evaluate(stmt.expr); }
  void visitPrintStmt(const PrintStmt &stmt) override {
    std::any value = evaluate(stmt.expr);
    std::cout << stringify(value) << '\n';
  }
  void visitVarStmt(const VarStmt &stmt) override {
    std::any val = nullptr;
    if (stmt.initalizer != nullptr) {
      val = evaluate(stmt.initalizer);
    }
  }

  std::any visitUnary(const Unary &expr) override {
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
    return NULL;
  }
  std::any visitBinary(const Binary &expr) override {
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
    return NULL;
  }

private:
  ErrorReporter reporter;
  std::any evaluate(const std::unique_ptr<Expr> &expr) {
    return expr->accept(*this);
  }
  bool isTruly(std::any expr);
  bool isEqual(std::any left, std::any right);
  void checkNumberOperands(Token token, std::any left, std::any right);
  void checkNumberOperands(Token token, std::any operand);
  std::string stringify(std::any obj);
  void execute(const std::unique_ptr<Stmt> &stmt);
};
