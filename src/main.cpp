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
  for (const auto &token : tokens) {
    std::cout << (int)token.getType() << " " << token.getLexeme() << " "
              << token.getLine() << '\n';
  }
  Parser parser(tokens, reporter);
  std::vector<std::unique_ptr<Stmt>> statments = parser.parse();

  if (ErrorReporter::hadError) {
    std::exit(65);
  }
  Interpreter interpreter(reporter);
  interpreter.interpret(std::move(statments));

  if (ErrorReporter::hadRuntimeError) {
    std::exit(70);
  }
}

int main() {
  Lox lox;
  lox.run("var name= 1+1; \n print name;");
  return 0;
}
