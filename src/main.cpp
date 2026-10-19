#include "../include/main.h"
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

void Lox::runPrompt() {
  std::string line;
  std::cout << "> ";
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      std::cout << "> ";
      continue;
    }
    run(line);
    ErrorReporter::hadError = false;
    std::cout << "> ";
  }
}
void Lox::runFile(const std::string &path) {

  std::ifstream file(path);
  if (!file) {
    std::cerr << "could not open file: " << path << "\n";
    std::exit(74);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  run(buffer.str());
  ErrorReporter::hadError = false;
}
void Lox::run(const std::string &line) {
  Scanner scan(line, reporter);
  std::vector<Token> tokens = scan.scanTokens();
  Parser parser(tokens, reporter);
  std::vector<std::unique_ptr<Stmt>> statments = parser.parse();

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

  if (ErrorReporter::hadError) {
    std::exit(65);
  }
}

int main(int argc, char *argv[]) {
  Lox lox;
  if (argc > 2) {
    std::cerr << "Too many arguments." << "\n";
    std::exit(64);
  } else if (argc == 2) {
    lox.runFile(argv[1]);
  } else {
    lox.runPrompt();
  }
  return 0;
}
