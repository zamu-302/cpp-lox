#include "main.h"
#include "token.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
class Scanner {
public:
  Scanner(std::string source) : source{source} {}
  auto scanTokens();

private:
  Lox lox;
  bool isAtEnd();
  void scanToken();
  std::string source;
  std::vector<Token> tokens;
  int start = 0;
  int curr = 0;
  int line = 1;
  inline static const std::unordered_map<std::string, TokenType> keywords{
      {"and", TokenType::AND},       {"class", TokenType::CLASS},
      {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
      {"for", TokenType::FOR},       {"fun", TokenType::FUN},
      {"if", TokenType::IF},         {"nil", TokenType::NIL},
      {"or", TokenType::OR},         {"print", TokenType::PRINT},
      {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
      {"this", TokenType::THIS},     {"true", TokenType::TRUE},
      {"var", TokenType::VAR},       {"while", TokenType::WHILE},
  };

  // helpers
  char advance();
  void addToken(TokenType type);
  void addToken(TokenType type, Literal &literal);
  void is_string();
  char peek();
  bool match(char c);
  char peekNext();
  void number();
  void identifier();
};
