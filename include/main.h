#include "error_reporter.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
class Lox {
public:
  void trials();
  ErrorReporter reporter;

private:
  static Interpreter interpreter;
  void runPrompt();
  void run(std::string &line);
};
