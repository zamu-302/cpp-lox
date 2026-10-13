#include "../include/main.h"
#include <memory>
#include <vector>

void Lox::trials() { runPrompt(); }

void Lox::runPrompt() {
  std::string input;
  std::cin >> input;

  std::fstream file(input);
  std::string line;

  while (std::getline(file, line)) {
    std::cout << "> ";
    run(line);
    ErrorReporter::hadError = false;
  }
}

void Lox::run(const std::string &line) {
  Scanner scan(line, reporter);
  std::vector<Token> tokens = scan.scanTokens();
  Parser parser(tokens, reporter);
  std::vector<std::unique_ptr<Stmt>> statments = parser.parse();

  if (ErrorReporter::hadError) {
    std::exit(65);
  }

  Interpreter interpreter(reporter);
  Resolver resolver(interpreter, reporter);
  resolver.resolve(statments);
  if (ErrorReporter::hadError) {
    return;
  }
  interpreter.interpret(std::move(statments));

  if (ErrorReporter::hadRuntimeError) {
    std::exit(70);
  }
}

int main() {
  Lox lox;
  lox.runPrompt();
  return 0;
}
