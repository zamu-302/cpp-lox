#include "../include/parser.h"
#include <memory>

std::unique_ptr<Expr> Parser::expression() { return equality(); }
std::unique_ptr<Expr> Parser::comparsion() {
  std::unique_ptr<Expr> expr = term();
  while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                TokenType::LESS_EQUAL})) {
    Token opr = previous();
    std::unique_ptr<Expr> right = term();
    expr = std::make_unique<Binary>(std::move(expr), std::move(right), opr);
  }
  return expr;
}
std::unique_ptr<Expr> Parser::equality() {
  std::unique_ptr<Expr> expr = comparsion();
  while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
    Token opr = previous();
    std::unique_ptr<Expr> right = comparsion();
    expr = std::make_unique<Binary>(std::move(expr), std::move(right), opr);
  }
  return expr;
}
bool Parser::match(std::initializer_list<TokenType> types) {
  for (const auto &type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

bool Parser::check(TokenType type) {
  if (isAtEnd()) {
    return false;
  }
  return peek().getType() == type;
}

bool Parser::isAtEnd() { return peek().getType() == TokenType::Eof; }
Token Parser::peek() const { return _tokens[curr]; }

Token Parser::previous() const { return _tokens[curr - 1]; }
