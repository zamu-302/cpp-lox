#pragma once
#include "error_reporter.h"
#include "token.h"
#include <any>
#include <memory>
#include <unordered_map>

class LoxClass;
class LoxInstance {
public:
  LoxInstance(LoxClass *klass, ErrorReporter &reporter) : klass{klass} {};
  std::any get(Token name);
  std::string toString();

private:
  LoxClass *klass;
  std::unordered_map<std::string, std::any> fields;
};
