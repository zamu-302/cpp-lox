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
  ErrorReporter reporter;
private:
  void runPrompt();
  void run(std::string &line);
};
