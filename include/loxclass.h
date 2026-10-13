#include "loxcallable.h"
class LoxInstance;
class Interpreter;

class LoxClass : public LoxCallable {
public:
  std::any call(Interpreter &interpreter,
                const std::vector<std::any> &args) override;
  LoxClass(const std::string &name) : name{name} {}
  std::string toString() override { return name; }
  int arity() override;

private:
  std::string name;
};
