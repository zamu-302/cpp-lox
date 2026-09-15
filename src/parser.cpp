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
std::unique_ptr<Expr> Parser::term() {
  std::unique_ptr<Expr> expr = factor();
  while (match({TokenType::MINUS, TokenType::PLUS})) {
    Token opr = previous();
    std::unique_ptr<Expr> right = factor();
    expr = std::make_unique<Binary>(std::move(expr), std::move(right), opr);
  }
  return expr;
}
std::unique_ptr<Expr> Parser::factor() {
  std::unique_ptr<Expr> expr = unary();
  while (match({TokenType::SLASH, TokenType::STAR})) {
    Token opr = previous();
    std::unique_ptr<Expr> right = unary();
    expr = std::make_unique<Binary>(std::move(expr), std::move(right), opr);
  }
  return expr;
}
std::unique_ptr<Expr> Parser::unary() {
  if (match({TokenType::BANG, TokenType::MINUS})) {
    Token opr = previous();
    std::unique_ptr<Expr> right = unary();
    return std::make_unique<Unary>(opr, std::move(right));
  }
  return primary();
}
std::unique_ptr<Expr> Parser::primary() {
  if (match({TokenType::FALSE})) {
    return std::make_unique<Literals>("false");
  }
  if (match({TokenType::TRUE})) {
    return std::make_unique<Literals>("true");
  }
  if (match({TokenType::NUMBER, TokenType::STRING})) {
    return std::make_unique<Literals>(previous().getLiteral());
  }
  if (match({TokenType::LEFT_PAREN})) {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
    return std::make_unique<Grouping>(std::move(expr));
  }
  throw error(peek(), "Expect expression");
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
Token Parser::consume(TokenType type, const std::string &message) {
  if (check(type))
    return advance();
  throw error(peek(), message);
}
Token Parser::previous() const { return _tokens[curr - 1]; }

ParseError Parser::error(Token token, const std::string &message) {
  reporter.error(token, message);
  ParseError parse_error(message);
  return parse_error;
}

void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
    if (previous().getType() == TokenType::SEMICOLON) {
      return;
    }

    switch (peek().getType()) {
    case TokenType::CLASS:
    case TokenType::FUN:
    case TokenType::VAR:
    case TokenType::FOR:
    case TokenType::IF:
    case TokenType::WHILE:
    case TokenType::PRINT:
    case TokenType::RETURN:
      return;
    }
    advance();
  }
}
