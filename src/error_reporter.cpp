#include "../include/error_reporter.h"
bool ErrorReporter::hadRuntimeError = false;
bool ErrorReporter::hadError = false;

void ErrorReporter::runtime_error(RuntimeError error) {
  std::cerr << error.message << '\n'
            << "[Line" << error.token.getLine() << "]\n";
  hadRuntimeError = true;
}
void ErrorReporter::error(int line, const std::string &message) {
  report(line, "", message);
}

void ErrorReporter::error(Token token, const std::string &message) {
  if (token.getType() == TokenType::Eof) {
    report(token.getLine(), " at end", message);
  } else {
    report(token.getLine(), " at '" + token.getLexeme() + "'", message);
  }
}

void ErrorReporter::report(int line, const std::string &where,
                           const std::string &message) {
  std::cerr << "[line " << line << " ] Error " << where << ": " << message
            << '\n';
  hadError = true;
}
