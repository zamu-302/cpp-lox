#include "loxcallable.h"
#include "loxfunction.h"
#include <algorithm>
#include <unordered_map>
class LoxInstance;
class Interpreter;

class LoxClass : public LoxCallable {
public:
  std::any call(Interpreter &interpreter,
                const std::vector<std::any> &args) override;
  LoxClass(const std::string &name,
           std::unordered_map<std::string, LoxFunction *> methods)
      : name{name}, methods{std::move(methods)} {}
  std::string toString() override { return name; }
  int arity() override;
  LoxFunction *findMethod(const std::string &name);

private:
  std::string name;
  std::unordered_map<std::string, LoxFunction *> methods;
};
