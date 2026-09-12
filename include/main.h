#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Lox {
public:
  Lox(){
    
  }
  void trials();
  void error(int line, const std::string &message);

private:
  bool hadError = false;
  void runPrompt();
  void run(std::string &line);
  void report(int line, const std::string &where, const std::string &message);
};
