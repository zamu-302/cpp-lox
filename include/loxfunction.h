
#pragma once
#include "expression.h"
#include "loxcallable.h"
#include "stmt.h"
#include <any>
#include <memory>
#include <vector>
class Interpreter;

class LoxFunction : public LoxCallable {
public:
  LoxFunction(Function *declaration) : declaration{declaration} {}
  std::any call(Interpreter &interpreter,
                const std::vector<std::any> &args) override;
  int arity() override;
  std::string toString() override;

private:
  const Function *declaration;
};
