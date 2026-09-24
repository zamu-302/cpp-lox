#pragma once
#include "token.h"
#include <iostream>
#include <string>

class ErrorReporter {
public:
  void error(int, const std::string &message);
  void error(Token token, const std::string &message);

private:
  bool hadError = false;
  void report(int line, const std::string &where, const std::string &message);
};
