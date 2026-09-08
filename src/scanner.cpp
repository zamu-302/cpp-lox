#include "../include/scanner.h"

auto Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = curr;
    scanToken();
  }

  tokens.emplace_back(Token(TokenType::Eof, "", std::string{}, line));
  return tokens;
}
void Scanner::scanToken() {
  char c = advance();
  switch (c) {
  case '(':
    addToken(TokenType::LEFT_BRACE);
    break;
  case ')':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '{':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '}':
    addToken(TokenType::LEFT_BRACE);
    break;
  case ',':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '.':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '-':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '+':
    addToken(TokenType::LEFT_BRACE);
    break;
  case ';':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '*':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '!':
    addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    break;
  case '=':
    addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    break;
  case '<':
    addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    break;
  case '>':
    addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    break;
  default:
    lox.error(line, "unexpected character");

    break;
  }
}
bool Scanner::isAtEnd() { return curr >= source.length(); }

char Scanner::advance() {
  curr++;
  return source[curr - 1];
}
void Scanner::addToken(TokenType type, Literal &l) {
  std::string text = source.substr(start, curr);
  tokens.emplace_back(Token(type, text, l, line));
}
bool Scanner::match(char c) {
  if (isAtEnd()) {
    return false;
  }
  if (source[curr] != c) {
    return false;
  }
  curr++;
  return true;
}
