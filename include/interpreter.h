#include "environment.h"
#include "error_reporter.h"
#include "expression.h"
#include "loxcallable.h"
#include "stmt.h"
#include "token.h"
#include <any>
#include <charconv>
#include <memory>
#include <string>
#include <vector>

class Interpreter : public Visitor, public StmtVisitor {
public:
  Environment globals;
  ClockCallable clockFn;
  Interpreter(const ErrorReporter &reporter) : reporter{reporter} {
    globals.define("clock", (LoxCallable *)&clockFn);
  }
  void interpret(std::vector<std::unique_ptr<Stmt>> statements);
  std::any visitLiteral(const Literals &expr) override {
    if (expr.value == "") {
      return expr.val;
    }
    if (expr.value == "true") {
      return true;
    }
    if (expr.value == "false") {
      return false;
    }
    return expr.value;
  }
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
    environment->define(stmt.name.getLexeme(), val);
  }
  std::any visitAssign(const Assign &expr) override {
    std::any value = evaluate(expr.value);
    environment->assign(expr.name, value);
    return value;
  }
  std::any visitVariable(const Variable &variable) override {
    return environment->get(variable.name);
  }
  void visitBlockStmt(const Block &stmt) override {
    executeBlock(stmt.state, new Environment(environment));
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
    return std::any{};
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
        std::cout << "it's a string" << std::endl;
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
    return std::any{};
  }
  std::any visitCall(const Call &expr) override {
    std::any callee = evaluate(expr.callee);
    std::vector<std::any> args;
    for (auto &arg : expr.arguments) {
      args.emplace_back(evaluate(std::move(arg)));
    }

    if (callee.type() != typeid(LoxCallable *)) {
      throw RuntimeError(expr.paren, "can only call functions and classes.");
    }
    LoxCallable *function = std::any_cast<LoxCallable *>(callee);
    if (args.size() != function->arity()) {
      throw RuntimeError(expr.paren, "Expected " +
                                         std::to_string(function->arity()) +
                                         " arguments but got " +
                                         std::to_string(args.size()) + ".");
    }

    return function->call(*this, args);
  }

private:
  ErrorReporter reporter;
  Environment *environment = &globals;

  std::any evaluate(const std::unique_ptr<Expr> &expr) {
    return expr->accept(*this);
  }
  void executeBlock(const std::vector<std::unique_ptr<Stmt>> &statement,
                    Environment *env) {
    Environment *prev = this->environment;

    this->environment = env;
    for (const auto &s : statement) {
      execute(s);
    }
    this->environment = prev;
  }
  bool isTruly(std::any expr);
  bool isEqual(std::any left, std::any right);
  void checkNumberOperands(Token token, std::any left, std::any right);
  void checkNumberOperands(Token token, std::any operand);
  std::string stringify(std::any obj);
  void execute(const std::unique_ptr<Stmt> &stmt);
};
