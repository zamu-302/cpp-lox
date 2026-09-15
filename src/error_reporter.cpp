#include "../include/error_reporter.h"

void ErrorReporter::error(Token token, const std::string &message) {
  if (token.getType() == TokenType::Eof) {
    report(token.getLine(), " at end", message);
  } else {
    report(token.getLine(), " at '" + token.getLexeme() + "'", message);
  }
}

void ErrorReporter::report(int line, const std::string &where,
                           const std::string &message) {
  std::cerr << "[line " << line << " ] Error " << where << ": " << message;
  hadError = true;
}
