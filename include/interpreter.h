#include "error_reporter.h"
#include "expression.h"
#include "token.h"
#include <any>
#include <memory>
#include <string>
class Interpreter : public Visitor {
public:
  Interpreter(const RuntimeError &error, const ErrorReporter &reporter)
      : error{error}, reporter{reporter} {}
  void interpret(std::unique_ptr<Expr> expression);
  std::any visitLiteral(const Literals &expr) override { return expr.value; }
  std::any visitGrouping(const Grouping &expr) override {
    return evaluate(expr.expression);
  }
  std::any visitUnary(const Unary &expr) override {
    std::any right = evaluate(expr.right);
    switch (expr.opr.getType()) {
    case TokenType::MINUS:
      checkNumberOperands(expr.opr, right);
      return -1 * std::any_cast<double>(right);
    case TokenType::BANG:
      return !isTruly(right);
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
    }
    return NULL;
  }

private:
  RuntimeError error;
  ErrorReporter reporter;
  std::any evaluate(const std::unique_ptr<Expr> &expr) {
    return expr->accept(*this);
  }
  bool isTruly(std::any expr);
  bool isEqual(std::any left, std::any right);
  void checkNumberOperands(Token token, std::any left, std::any right);
  void checkNumberOperands(Token token, std::any operand);
  std::string stringify(std::any obj);
};
