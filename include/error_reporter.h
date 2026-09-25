#pragma once
#include "token.h"
#include <iostream>
#include <stdexcept>
#include <string>

class ErrorReporter {
public:
  void error(int, const std::string &message);
  void error(Token token, const std::string &message);

private:
  bool hadError = false;
  void report(int line, const std::string &where, const std::string &message);
};

class RuntimeError : public std::runtime_error {

public:
  RuntimeError(Token token, const std::string &message)
      : std::runtime_error(message), token{std::move(token)} {}

private:
  Token token;
};
