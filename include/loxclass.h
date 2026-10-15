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
           std::unordered_map<std::string, LoxFunction *> methods,
           LoxClass *superClass)
      : name{name}, methods{std::move(methods)}, superClass{superClass} {}
  std::string toString() override { return name; }
  int arity() override;
  LoxFunction *findMethod(const std::string &name);

private:
  std::string name;
  LoxClass *superClass;
  std::unordered_map<std::string, LoxFunction *> methods;
};
