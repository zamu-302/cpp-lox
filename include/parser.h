#include "error_reporter.h"
#include "expression.h"
#include "stmt.h"
#include "token.h"
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

struct ParseError : public std::runtime_error {
  ParseError(const std::string &msg) : std::runtime_error(msg) {}
};

class Parser {
public:
  Parser(const std::vector<Token> &tokens, const ErrorReporter &reporter)
      : _tokens{tokens}, reporter{reporter} {}
  friend Stmt;
  std::vector<std::unique_ptr<Stmt>> parse();

private:
  ErrorReporter reporter;
  std::vector<Token> _tokens;
  int curr = 0;
  // grammar
  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> assignment();
  std::unique_ptr<Expr> equality();
  std::unique_ptr<Expr> comparsion();
  std::unique_ptr<Expr> term();
  std::unique_ptr<Expr> factor();
  std::unique_ptr<Expr> unary();
  std::unique_ptr<Expr> primary();

  // helper
  bool match(std::initializer_list<TokenType> types);
  bool isAtEnd();
  bool check(TokenType type);

  Token advance();
  Token peek() const;
  Token previous() const;
  Token consume(TokenType type, const std::string &message);

  void synchronize();
  std::unique_ptr<Stmt> statement();
  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> printStatement();
  std::unique_ptr<Stmt> expressionStatement();
  std::unique_ptr<Stmt> varDeclaration();
  ParseError error(Token token, const std::string &str);
};
