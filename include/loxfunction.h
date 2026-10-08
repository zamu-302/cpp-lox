
#pragma once
#include "environment.h"
#include "loxcallable.h"
#include "stmt.h"
#include <any>
#include <memory>
#include <vector>
class Interpreter;

class LoxFunction : public LoxCallable {
public:
  LoxFunction(Function *declaration, Environment *closure)
      : declaration{declaration}, closure{closure} {}
  std::any call(Interpreter &interpreter,
                const std::vector<std::any> &args) override;
  int arity() override;
  std::string toString() override;

private:
  Environment *closure;
  const Function *declaration;
};
