#pragma once
#include "error_reporter.h"
#include "interpreter.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"
#include "stmt.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
class Lox {
public:
  Lox() {}
  friend Scanner;
  void runFile(const std::string &path);
  ErrorReporter reporter;
  void runPrompt();
  void run(const std::string &line);
};
