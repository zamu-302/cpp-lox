#include "error_reporter.h"
#include "expression.h"
#include "token.h"
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
class Parser {
public:
  Parser(const std::vector<Token> &tokens, const ErrorReporter &reporter)
      : _tokens{tokens}, reporter{reporter} {}

private:
  ErrorReporter reporter;
  struct ParseError : public std::runtime_error {
    ParseError(const std::string &msg) : std::runtime_error(msg) {}
  };
  std::vector<Token> _tokens;
  int curr = 0;
  // grammar
  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> equality();
  std::unique_ptr<Expr> comparsion();
  std::unique_ptr<Expr> term();
  std::unique_ptr<Expr> factor();
  std::unique_ptr<Expr> unary();
  std::unique_ptr<Expr> primary();

  bool match(std::initializer_list<TokenType> types);
  bool isAtEnd();
  bool check(TokenType type);

  Token advance();
  Token peek() const;
  Token previous() const;
  Token consume(TokenType type, const std::string &message);

  void synchronize();

  ParseError error(Token token, const std::string &str);
};
