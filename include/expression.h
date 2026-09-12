#include "token.h"
#include <memory>
#include <utility>
class Binary;
class Unary;

class Visitor {
public:
  virtual void visitBinary(const Binary &binary) = 0;
  virtual void visitUnary(const Unary &unary) = 0;
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
  void accept(Visitor &visitor) { visitor.visitBinary(*this); }
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  Token opr;
};
class Unary : public Expr {
public:
  Unary(Token opr, std::unique_ptr<Expr> right)
      : opr{opr}, right{std::move(right)} {}
  void accept(Visitor &visitor) { visitor.visitUnary(*this); }
  Token opr;
  std::unique_ptr<Expr> right;
};
