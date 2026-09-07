#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Lox {
public:
  void trials() { runPrompt(); }
  void error(int line, const std::string &message) {
    report(line, "", message);
  }

private:
  bool hadError = false;
  void runPrompt() {
    std::string input;
    std::cin >> input;

    std::fstream file(input);
    std::string line;

    while (std::getline(file, line)) {
      std::cout << "> ";
      run(line);
      hadError = false;
    }
  }
  void run(std::string &line) {
    std::istringstream ss(line);
    for (std::string token; ss >> token;) {
      std::cout << token << '\n';
    }
    if (hadError) {
      std::exit(65);
    }
  }
  void report(int line, const std::string &where, const std::string &message) {
    std::cerr << "[line " << line << " ] Error " << where << ": " << message;
    hadError = true;
  }
};

int main() {

  Lox l1;
  l1.trials();

  return 0;
}
