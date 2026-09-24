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
    switch (expr.opr.getType()) {
    case TokenType::MINUS:
      return -1 * std::stod(expr.right);
    case TokenType::BANG:
      return !isTruly(expr.right);

      return NULL;
    }
  }

private:
  std::any evaluate(const std::unique_ptr<Expr> &expr) {
    return expr->accept(*this);
  }
  bool isTruly(std::unique_ptr<Expr> expr);
};
