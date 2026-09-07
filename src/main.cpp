#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Lox {
public:
  void trials() { runPrompt(); }

private:
  void runPrompt() {
    std::string input;
    std::cin >> input;

    std::fstream file(input);
    std::string line;

    while (std::getline(file, line)) {
      std::cout << "> ";
      run(line);
    }
  }
  void run(std::string &line) {
    std::istringstream ss(line);
    for (std::string token; ss >> token;) {
      std::cout << token << '\n';
    }
  }
};

int main() {

  Lox l1;
  l1.trials();

  return 0;
}
