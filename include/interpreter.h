#include "expression.h"
#include "token.h"
#include <any>
#include <memory>
#include <string>
class Interpreter : public Visitor {
public:
  std::any visitLiteral(const Literals &expr) override { return expr.value; }
  std::any visitGrouping(const Grouping &expr) override {
    return evaluate(expr.expression);
  }
  std::any visitUnary(const Unary &expr) override {
    std::any right = evaluate(expr.right);
    switch (expr.opr.getType()) {
    case TokenType::MINUS:
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
      break;

    case TokenType::SLASH:
      return std::any_cast<double>(left) / std::any_cast<double>(right);

    case TokenType::STAR:
      return std::any_cast<double>(left) * std::any_cast<double>(right);
    case TokenType::GREATER:
      return std::any_cast<double>(left) > std::any_cast<double>(right);
    case TokenType::GREATER_EQUAL:
      return std::any_cast<double>(left) >= std::any_cast<double>(right);
    case TokenType::LESS:
      return std::any_cast<double>(left) < std::any_cast<double>(right);
    case TokenType::LESS_EQUAL:
      return std::any_cast<double>(left) <= std::any_cast<double>(right);
    case TokenType::BANG_EQUAL:
      return !isEqual(left, right);
    case TokenType::EQUAL_EQUAL:
      return isEqual(left, right);
    }
    return NULL;
  }

private:
  std::any evaluate(const std::unique_ptr<Expr> &expr) {
    return expr->accept(*this);
  }
  bool isTruly(std::any expr);
  bool isEqual(std::any left, std::any right);
};
