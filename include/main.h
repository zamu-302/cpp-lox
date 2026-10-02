#include "error_reporter.h"
#include "interpreter.h"
#include "parser.h"
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
  void trials();
  ErrorReporter reporter;
  void runPrompt();
  void run(const std::string &line);
};
