#include "token.h"
#include <memory>
#include <string>
#include <utility>
class Binary;
class Unary;
class Literals;
class Grouping;

class Visitor {
public:
  virtual void visitBinary(const Binary &binary) = 0;
  virtual void visitUnary(const Unary &unary) = 0;
  virtual void visitLiteral(const Literals &literal) = 0;
  virtual void visitGrouping(const Grouping &grouping) = 0;
  virtual ~Visitor() = default;
};

class Expr {
public:
  virtual void accept(Visitor &visitor) = 0;
  virtual ~Expr() = default;
};

class Binary : public Expr {
public:
  Binary(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, Token opr)
      : left{std::move(left)}, right{std::move(right)}, opr{opr} {}
  void accept(Visitor &visitor) override { visitor.visitBinary(*this); }

  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  Token opr;
};

class Unary : public Expr {
public:
  Unary(Token opr, std::unique_ptr<Expr> right)
      : opr{opr}, right{std::move(right)} {}
  void accept(Visitor &visitor) override { visitor.visitUnary(*this); }
  Token opr;
  std::unique_ptr<Expr> right;
};
class Literals : public Expr {
public:
  std::string value;

  Literals(std::string value) : value(value) {}

  void accept(Visitor &visitor) override { visitor.visitLiteral(*this); }
};

class Grouping : public Expr {
public:
  std::unique_ptr<Expr> expression;

  Grouping(std::unique_ptr<Expr> expression)
      : expression(std::move(expression)) {}

  void accept(Visitor &visitor) override { visitor.visitGrouping(*this); }
};

class AstPrinter : public Visitor {
public:
  std::string result;

  void visitBinary(const Binary &expr) override {
    std::string left_str, right_str;
    expr.left->accept(*this);
    left_str = result;

    expr.right->accept(*this);
    right_str = result;

    result =
        "(" + expr.opr.getLexeme() + " " + left_str + " " + right_str + ")";
  }

  void visitUnary(const Unary &expr) override {
    expr.right->accept(*this);
    result = "(" + expr.opr.getLexeme() + " " + result + ")";
  }
  void visitLiteral(const Literals &expr) override { result = expr.value; }
  void visitGrouping(const Grouping &expr) override {
    expr.expression->accept(*this);
    result = "(group " + result + ")";
  }

  std::string print(Expr &expr) {
    expr.accept(*this);
    return result;
  }
};
