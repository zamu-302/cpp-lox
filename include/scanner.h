#include "token.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
class Scanner {
public:
  Scanner(std::string source) : source{source} {}
  auto scanTokens() {
    while (!isAtEnd()) {
      start = curr;
      scanToken();
    }
    tokens.emplace_back(Token(TokenType::Eof, "", nullptr, line));
    return tokens;
  }

private:
  bool isAtEnd();
  void scanToken();
  std::string source;
  std::vector<Token> tokens;
  int start = 0;
  int curr = 0;
  int line = 1;

  // helpers
  char advance();
  void addToken(TokenType type);
  void addToken(TokenType type, Literal &literal);
};
