#include "../include/main.h"

void Lox::trials() { runPrompt(); }

void Lox::runPrompt() {
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
void Lox::run(std::string &line) {
  std::istringstream ss(line);
  for (std::string token; ss >> token;) {
    std::cout << token << '\n';
  }
  if (hadError) {
    std::exit(65);
  }
}
