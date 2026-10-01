#pragma once
#include "error_reporter.h"
#include "token.h"
#include <any>
#include <memory>
#include <string>
#include <unordered_map>

class Environment {
public:
  Environment *enclosing;

  Environment() : enclosing{nullptr} {}
  Environment(Environment *enclosing) : enclosing{enclosing} {}

  void define(const std::string &name, std::any obj) { values[name] = obj; }
  std::any get(const Token &name) {
    if (values.contains(name.getLexeme())) {
      return values[name.getLexeme()];
    }
    if (enclosing != nullptr) {
      return enclosing->get(name);
    }
    throw RuntimeError(name, "undefined Variable '" + name.getLexeme() + "'.");
  }
  void assign(const Token &name, std::any value) {
    if (values.contains(name.getLexeme())) {
      values[name.getLexeme()] = value;
      return;
    }
    if (enclosing != nullptr) {
      enclosing->assign(name, value);
      return;
    }
    throw RuntimeError(name, "undefined Variable '" + name.getLexeme() + "'.");
  }

private:
  std::unordered_map<std::string, std::any> values;
};
