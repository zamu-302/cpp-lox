#include "../include/parser.h"
#include <memory>
#include <string>
#include <vector>

std::unique_ptr<Expr> Parser::expression() { return assignment(); }

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
  return call();
}
std::unique_ptr<Expr> Parser::call() {
  std::unique_ptr<Expr> expr = primary();

  while (true) {
    if (match({TokenType::LEFT_PAREN})) {
      expr = finishCall(std::move(expr));
    } else {
      break;
    }
  }
  return expr;
}
std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
  std::vector<std::unique_ptr<Expr>> arguments;
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      arguments.emplace_back(expression());
    } while (match({TokenType::COMMA}));
  }
  Token paren = consume(TokenType::RIGHT_PAREN, "expected ')' after arguments");
  return std::make_unique<Call>(std::move(callee), paren, std::move(arguments));
}
std::unique_ptr<Expr> Parser::primary() {
  if (match({TokenType::FALSE})) {
    return std::make_unique<Literals>("false");
  }
  if (match({TokenType::TRUE})) {
    return std::make_unique<Literals>("true");
  }
  if (match({TokenType::NUMBER, TokenType::STRING})) {
    auto val = previous().getLit();
    if (std::holds_alternative<double>(val)) {
      double d = std::get<double>(val);
      return std::make_unique<Literals>(d);
    } else if (std::holds_alternative<std::string>(val)) {
      std::string s = std::get<std::string>(val);
      return std::make_unique<Literals>(s);
    }
  }
  if (match({TokenType::IDENTFIERS})) {
    return std::make_unique<Variable>(previous());
  }
  if (match({TokenType::LEFT_PAREN})) {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
    return std::make_unique<Grouping>(std::move(expr));
  }
  throw error(peek(), "Expect expression");
}
std::unique_ptr<Expr> Parser::assignment() {
  std::unique_ptr<Expr> expr = equality();
  if (match({TokenType::EQUAL})) {
    Token equals = previous();
    std::unique_ptr<Expr> value = assignment();
    if (auto *var = dynamic_cast<Variable *>(expr.get())) {
      Token name = var->name;
      return std::make_unique<Assign>(name, std::move(value));
    }
    reporter.error(equals, "Invalid assignment target.");
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
    default:
      break;
    }
    advance();
  }
}
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
  std::vector<std::unique_ptr<Stmt>> statements;
  while (!isAtEnd()) {
    statements.emplace_back(declaration());
  }
  return statements;
}
std::unique_ptr<Stmt> Parser::statement() {
  if (match({TokenType::PRINT})) {
    return printStatement();
  }
  if (match({TokenType::LEFT_BRACE})) {
    return std::make_unique<Block>(block());
  }
  return expressionStatement();
}
std::unique_ptr<Stmt> Parser::printStatement() {
  std::unique_ptr<Expr> value = expression();
  consume(TokenType::SEMICOLON, "Expected ';' after value");
  return std::make_unique<PrintStmt>(std::move(value));
}
std::unique_ptr<Stmt> Parser::expressionStatement() {
  std::unique_ptr<Expr> expr = expression();
  consume(TokenType::SEMICOLON, "Expeceted ';' after value");
  return std::make_unique<ExprStmt>(std::move(expr));
}
Token Parser::advance() {
  if (!isAtEnd()) {
    curr++;
  }
  return previous();
}
std::unique_ptr<Stmt> Parser::declaration() {
  try {
    if (match({TokenType::FUN})) {
      return function("function");
    }
    if (match({TokenType::VAR})) {
      return varDeclaration();
    }
    return statement();
  } catch (ParseError error) {
    synchronize();
    return nullptr;
  }
}
std::unique_ptr<Stmt> Parser::varDeclaration() {
  Token name = consume(TokenType::IDENTFIERS, "expected variable name.");
  std::unique_ptr<Expr> initalizer = nullptr;
  if (match({TokenType::EQUAL})) {
    initalizer = expression();
  }
  consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
  return std::make_unique<VarStmt>(name, std::move(initalizer));
}
std::vector<std::unique_ptr<Stmt>> Parser::block() {
  std::vector<std::unique_ptr<Stmt>> statement;
  while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    statement.push_back(declaration());
  }
  consume(TokenType::RIGHT_BRACE, "expected '}' after block.");
  return statement;
}
std::unique_ptr<Stmt> Parser::function(const std::string &kind) {
  Token name = consume(TokenType::IDENTFIERS, "expect" + kind + " name.");
  consume(TokenType::LEFT_PAREN, "expected '(' after " + kind + " name.");
  std::vector<Token> paramaters;
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (paramaters.size() >= 255) {
        error(peek(), "can't have more than 255 paramaters");
      }
      paramaters.push_back(
          consume(TokenType::IDENTFIERS, "expected parameter name."));
    } while (match({TokenType::COMMA}));
  }
  consume(TokenType::RIGHT_PAREN, "expected '}' after parameters");

  consume(TokenType::LEFT_BRACE, "Expected '{' before " + kind + " body.");
  std::vector<std::unique_ptr<Stmt>> body = block();
  return std::make_unique<Function>(name, paramaters, body);
}
