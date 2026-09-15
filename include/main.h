#include "error_reporter.h"
#include "parser.h"
#include "scanner.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Lox {
public:
  ErrorReporter report;
  Lox() {}
  void trials();

private:
  bool hadError = false;
  void runPrompt();
  void run(std::string &line);
};
