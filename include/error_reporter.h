#pragma once
#include "token.h"
#include <any>
#include <iostream>
#include <stdexcept>
#include <string>
class ErrorReporter;
class RuntimeError : public std::runtime_error {

public:
  friend ErrorReporter;
  RuntimeError(Token token, const std::string &message)
      : std::runtime_error(message), token{std::move(token)}, message{message} {
  }

private:
  Token token;
  std::string message;
};
class ReturnException : public std::runtime_error {
public:
  std::any value;
  ReturnException(std::any value)
      : std::runtime_error{"return"}, value{value} {}
};

class ErrorReporter {
public:
  void error(int, const std::string &message);
  void error(Token token, const std::string &message);
  static void runtime_error(RuntimeError error);
  static bool hadError;
  static bool hadRuntimeError;

private:
  void report(int line, const std::string &where, const std::string &message);
};
