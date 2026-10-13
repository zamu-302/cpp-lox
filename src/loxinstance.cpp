#include "../include/loxinstance.h"
#include "../include/loxclass.h"

std::string LoxInstance::toString() { return klass->toString() + "instance"; }
