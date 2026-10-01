#pragma once
#include "token.h"
#include <any>
#include <memory>
#include <string>
#include <utility>
class Binary;
class Unary;
class Literals;
class Grouping;
class Variable;
class Assign;

class Visitor {
public:
  virtual std::any visitBinary(const Binary &binary) = 0;
  virtual std::any visitUnary(const Unary &unary) = 0;
  virtual std::any visitLiteral(const Literals &literal) = 0;
  virtual std::any visitGrouping(const Grouping &grouping) = 0;
  virtual std::any visitVariable(const Variable &variable) = 0;
  virtual std::any visitAssign(const Assign &assign) = 0;
  virtual ~Visitor() = default;
};

class Expr {
public:
  virtual std::any accept(Visitor &visitor) = 0;
  virtual ~Expr() = default;
};

class Binary : public Expr {
public:
  Binary(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, Token opr)
      : left{std::move(left)}, right{std::move(right)}, opr{opr} {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitBinary(*this);
  }

  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  Token opr;
};

class Unary : public Expr {
public:
  Unary(Token opr, std::unique_ptr<Expr> right)
      : opr{opr}, right{std::move(right)} {}
  std::any accept(Visitor &visitor) override {
    return visitor.visitUnary(*this);
  }
  Token opr;
  std::unique_ptr<Expr> right;
};
class Literals : public Expr {
public:
  std::string value;

  Literals(std::string value) : value(value) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitLiteral(*this);
  }
};

class Grouping : public Expr {
public:
  std::unique_ptr<Expr> expression;

  Grouping(std::unique_ptr<Expr> expression)
      : expression(std::move(expression)) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitGrouping(*this);
  }
};

class AstPrinter : public Visitor {
public:
  std::string result;

  std::any visitBinary(const Binary &expr) override {
    std::string left_str, right_str;
    expr.left->accept(*this);
    left_str = result;

    expr.right->accept(*this);
    right_str = result;

    result =
        "(" + expr.opr.getLexeme() + " " + left_str + " " + right_str + ")";
    return result;
  }

  std::any visitUnary(const Unary &expr) override {
    expr.right->accept(*this);
    result = "(" + expr.opr.getLexeme() + " " + result + ")";
    return result;
  }
  std::any visitLiteral(const Literals &expr) override {
    if (expr.value.empty()) {
      result = "nil";
    } else {
      result = expr.value;
    }
    return result;
  }
  std::any visitGrouping(const Grouping &expr) override {
    expr.expression->accept(*this);
    result = "(group " + result + ")";
    return result;
  }

  std::string print(Expr &expr) {
    expr.accept(*this);
    return result;
  }
};
class Variable : public Expr {
public:
  Token name;
  Variable(const Token &name) : name{name} {}
  std::any accept(Visitor &visitor) override {
    return visitor.visitVariable(*this);
  }
};
class Assign : public Expr {
public:
  Token name;
  std::unique_ptr<Expr> value;
  Assign(const Token &name, std::unique_ptr<Expr> value)
      : name{name}, value{std::move(value)} {}

  std::any accept(Visitor &visit) override { return visit.visitAssign(*this); }
};
