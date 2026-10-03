
#include "loxcallable.h"
#include "stmt.h"
#include <any>
#include <memory>
#include <vector>
class Interpreter;

class LoxFunction : public LoxCallable {
public:
  LoxFunction(std::unique_ptr<Stmt> &declaration)
      : declaration{std::move(declaration)} {}
  std::any call(Interpreter &interpreter,
                const std::vector<std::any> &args) override;
  int arity() override;

private:
  std::unique_ptr<Stmt> declaration;
};
