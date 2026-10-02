#include "../include/scanner.h"
#include <cctype>
#include <string>

std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = curr;
    scanToken();
  }

  tokens.emplace_back(Token(TokenType::Eof, "", std::string{}, line));
  line++;
  return tokens;
}
void Scanner::scanToken() {
  char c = advance();
  switch (c) {
  case '(':
    addToken(TokenType::LEFT_PAREN);
    break;
  case ')':
    addToken(TokenType::RIGHT_PAREN);
    break;
  case '{':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '}':
    addToken(TokenType::RIGHT_BRACE);
    break;
  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '-':
    addToken(TokenType::MINUS);
    break;
  case '+':
    addToken(TokenType::PLUS);
    break;
  case ';':
    addToken(TokenType::SEMICOLON);
    break;
  case '*':
    addToken(TokenType::STAR);
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
  case '/':
    if (match('/')) {
      while (peek() != '\n' && !isAtEnd()) {
        advance();
      }
    }

    else if (match('*')) {
      while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
          advance();
          advance();
          break;
        } else if (peek() == '\n') {
          line++;
        }
        advance();
      }

      if (isAtEnd()) {
        reporter.error(line, "Undefined Comment Scope");
      }

    }

    else {
      addToken(TokenType::SLASH);
    }
    break;
  case ' ':
  case '\r':
  case '\t':
    break;

  case '\n':
    line++;
    break;
  case '"':
    is_string();
    break;

  default:
    if (std::isdigit(c)) {
      number();
    } else if (std::isalpha(c) || c == '_') {
      identifier();
    } else {
      reporter.error(line, "unexpected character");
    }
    break;
  }
}
bool Scanner::isAtEnd() { return curr >= source.length(); }

char Scanner::advance() {
  curr++;
  return source[curr - 1];
}
void Scanner::addToken(TokenType type, const Literal &l) {
  std::string text = source.substr(start, curr - start);
  tokens.emplace_back(Token(type, text, l, line));
}
void Scanner::addToken(TokenType type) { addToken(type, std::string{}); }
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
char Scanner::peek() {
  if (isAtEnd()) {
    return '\0';
  }
  return source[curr];
}
void Scanner::is_string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      line++;
    }
    advance();
  }
  if (isAtEnd()) {
    reporter.error(line, "Unterminated String.");
    return;
  }
  advance();
  std::string value = source.substr(start + 1, curr - start - 2);
  addToken(TokenType::STRING, value);
}

void Scanner::number() {
  while (std::isdigit(peek())) {
    advance();
  }
  if (peek() == '.' && std::isdigit(peekNext())) {
    advance();
    while (std::isdigit(peek())) {
      advance();
    }
  }
  addToken(TokenType::NUMBER, std::stod(source.substr(start, curr - start)));
}
char Scanner::peekNext() {
  if (curr + 1 >= source.length()) {
    return '\0';
  }
  return source[curr + 1];
}
void Scanner::identifier() {
  while (std::isalnum(peek()) || peek() == '_') {
    advance();
  }
  std::string text = source.substr(start, curr - start);
  TokenType type = TokenType::IDENTFIERS;
  if (keywords.contains(text)) {
    type = keywords.at(text);
  }
  addToken(type);
}
