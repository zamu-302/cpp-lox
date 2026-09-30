#pragma once
#include "error_reporter.h"
#include "token.h"
#include <any>
#include <string>
#include <unordered_map>

class Environment {
public:
  void define(const std::string &name, std::any obj) { values[name] = obj; }
  std::any get(const Token &name) {
    if (values.contains(name.getLexeme())) {
      return values[name.getLexeme()];
    }
    throw RuntimeError(name, "undefined Variable '" + name.getLexeme() + "'.");
  }

private:
  std::unordered_map<std::string, std::any> values;
};
