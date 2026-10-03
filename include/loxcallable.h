#include <any>
#include <chrono>
#include <string>
#include <vector>

class Interpreter;

class LoxCallable {
public:
  virtual std::any call(const Interpreter &interpreter,
                        const std::vector<std::any> &arguments) = 0;
  virtual int arity() = 0;
  virtual std::string toString() = 0;
  virtual ~LoxCallable() = default;
};
class ClockCallable : public LoxCallable {
public:
  int arity() override { return 0; }
  std::any call(const Interpreter &interpreter,
                const std::vector<std::any> &arguments) override {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return (double)std::chrono::duration_cast<std::chrono::seconds>(now)
        .count();
  }
  std::string toString() override { return "<native fn>"; }
};
