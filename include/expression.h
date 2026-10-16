#pragma once
#include "token.h"
#include <any>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

// forward declaration
class Binary;
class Unary;
class Literals;
class Logical;
class Grouping;
class Variable;
class Assign;
class Call;
class Get;
class Set;
class This;
class Super;

class Visitor {
public:
  virtual std::any visitBinary(const Binary &binary) = 0;
  virtual std::any visitUnary(const Unary &unary) = 0;
  virtual std::any visitLiteral(const Literals &literal) = 0;
  virtual std::any visitGrouping(const Grouping &grouping) = 0;
  virtual std::any visitVariable(const Variable &variable) = 0;
  virtual std::any visitAssign(const Assign &assign) = 0;
  virtual std::any visitCall(const Call &call) = 0;
  virtual std::any visitLogical(const Logical &logical) = 0;
  virtual std::any visitGet(const Get &get) = 0;
  virtual std::any visitSet(const Set &set) = 0;
  virtual std::any visitThis(const This &expr) = 0;
  virtual std::any visitSuper(const Super &expr) = 0;
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
class Get : public Expr {
public:
  std::unique_ptr<Expr> obj;
  Token token;
  Get(std::unique_ptr<Expr> obj, Token token)
      : obj{std::move(obj)}, token{token} {}
  std::any accept(Visitor &visitor) override { return visitor.visitGet(*this); }
};
class Set : public Expr {
public:
  std::unique_ptr<Expr> obj;
  Token name;
  std::unique_ptr<Expr> right;
  Set(std::unique_ptr<Expr> obj, Token name, std::unique_ptr<Expr> right)
      : obj{std::move(obj)}, name{name}, right{std::move(right)} {}
  std::any accept(Visitor &visitor) override { return visitor.visitSet(*this); }
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
  double val = 0;
  std::string value = "";
  bool boolean = false;
  enum class Kind { Nil, Bool, Number, String };
  Kind kind = Kind::Nil;

  Literals() = default;
  Literals(bool boolean) : kind{Kind::Bool}, boolean{boolean} {}
  Literals(std::string value) : kind{Kind::String}, value(value) {}
  Literals(double val) : kind{Kind::Number}, val{val} {}
  Literals(const char *) = delete;

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
class Call : public Expr {
public:
  std::unique_ptr<Expr> callee;
  Token paren;
  std::vector<std::unique_ptr<Expr>> arguments;
  Call(std::unique_ptr<Expr> callee, const Token &paren,
       std::vector<std::unique_ptr<Expr>> arguments)
      : callee{std::move(callee)}, paren{paren},
        arguments(std::move(arguments)) {}
  std::any accept(Visitor &visitor) override {
    return visitor.visitCall(*this);
  }
};
class Logical : public Expr {
public:
  std::unique_ptr<Expr> left;
  Token token;
  std::unique_ptr<Expr> right;
  Logical(std::unique_ptr<Expr> left, Token token, std::unique_ptr<Expr> right)
      : left{std::move(left)}, token{token}, right{std::move(right)} {}
  std::any accept(Visitor &visitor) override {
    return visitor.visitLogical(*this);
  }
};
class This : public Expr {
public:
  Token name;
  This(Token name) : name{name} {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitThis(*this);
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
    switch (expr.kind) {
    case Literals::Kind::Nil:
      result = "nil";
      break;
    case Literals::Kind::Bool:
      result = expr.boolean ? "true" : "false";
      break;
    case Literals::Kind::Number:
      result = std::to_string(expr.val);
      break;
    case Literals::Kind::String:
      result = expr.value;
      break;
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
class Super : public Expr {
public:
  Token keyword;
  Token method;
  Super(Token keyword, Token method) : keyword{keyword}, method{method} {}
  std::any accept(Visitor &vistor) { return vistor.visitSuper(*this); }
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
