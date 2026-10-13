#pragma once
#include <memory>
class LoxClass;
class LoxInstance {
public:
  LoxInstance(LoxClass *klass) : klass{klass} {};
  std::string toString();

private:
  LoxClass *klass;
};
