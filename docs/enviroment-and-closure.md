# Environment & Closures

## Environment

An environment is a map from variable names to their values. Every scope — global, block, function body — gets its own environment. Environments are chained: each one holds a pointer to the one that encloses it.

```cpp
class Environment {
public:
    Environment* enclosing;  // raw pointer — we don't own this

    Environment()                    : enclosing{nullptr} {}
    Environment(Environment* parent) : enclosing{parent}  {}

private:
    std::unordered_map<std::string, std::any> values;
};
```

`enclosing` is a raw pointer because the inner scope **observes** the outer one — it doesn't own it. The outer scope always outlives the inner one, so the pointer is always valid.

### Variable lookup walks the chain

```cpp
std::any get(const Token& name) {
    if (values.contains(name.getLexeme()))
        return values[name.getLexeme()];  // found here

    if (enclosing != nullptr)
        return enclosing->get(name);      // try one level up

    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}
```

### The chain at runtime

```
GLOBAL   { a: "global", Cake: <class> }
    └── FUNCTION   { x: 5 }
              └── BLOCK   { b: 2 }
```

When the block looks up `a` it checks itself (not found), then the function scope (not found), then global (found). The whole walk happens in `get()` without the interpreter knowing how deep it goes.

---

## Closures

A closure is a function that captures the environment it was **defined in**, not the one it is **called in**.

```cpp
class LoxFunction : public LoxCallable {
private:
    const Function* declaration;
    Environment*    closure;     // captured at definition time
public:
    LoxFunction(const Function* declaration, Environment* closure)
        : declaration{declaration}, closure{closure} {}
};
```

When a function is defined, the interpreter stores the **current environment** as the closure:

```cpp
void Interpreter::visitFunctionStmt(const Function& stmt) {
    LoxFunction* fn = new LoxFunction(&stmt, environment);
    //                                        ↑
    //                           env at DEFINITION time — this is the closure
    environment->define(stmt.name.getLexeme(), (LoxCallable*)fn);
}
```

When the function is **called**, a new environment is created with the closure as its parent — not the caller's current environment:

```cpp
std::any LoxFunction::call(Interpreter& interpreter,
                           const std::vector<std::any>& args) {
    Environment* env = new Environment(closure);  // parent = closure, not caller
    for (int i = 0; i < declaration->params.size(); i++)
        env->define(declaration->params[i].getLexeme(), args[i]);

    interpreter.executeBlock(declaration->body, env);
    delete env;
    return std::any{};
}
```

### Why this matters

```
fun makeCounter() {
    var count = 0;
    fun increment() {
        count = count + 1;
        return count;
    }
    return increment;
}

var c = makeCounter();
c();  // 1
c();  // 2
c();  // 3
```

When `makeCounter` returns, its local scope would normally be gone. But `increment` captured that scope as its closure. The environment chain at each call looks like:

```
GLOBAL   { c: <fn increment> }
    └── makeCounter scope   { count: 1 }   ← kept alive by the closure
              └── increment body   { }
```

Each call to `c()` modifies `count` in the captured scope. The scope lives as long as the closure references it.

### Two closures from the same factory are independent

```cpp
var c1 = makeCounter();
var c2 = makeCounter();

c1();  // 1  — c1's count
c1();  // 2
c2();  // 1  — c2's own count, separate scope
```

Each call to `makeCounter` creates a **new** environment for `count`. Each returned `increment` captures a different environment. They share the same function code but hold independent state.

---

## The Variable Resolution Problem

Dynamic lookup through the environment chain has a subtle bug:

```
var a = "global";
{
    fun showA() { print a; }

    showA();        // "global" ✓
    var a = "block";
    showA();        // "block"  ✗ — should still print "global"
}
```

The second call finds `a = "block"` in the block scope because the chain now has it one hop closer. The **same function** returns different values depending on what has been declared after it.

### The fix — resolve at parse time

At runtime, `lookUpVariable` uses the stored depth instead of walking the chain:

```cpp
std::any lookUpVariable(const Token& name, const Expr& expr) {
    if (locals.contains(&expr))
        return environment->getAt(locals[&expr], name.getLexeme());
    else
        return globals.get(name);
}
```

`getAt` jumps **exactly** that many hops — no more, no less:

```cpp
std::any Environment::getAt(int distance, const std::string& name) {
    return ancestor(distance)->values[name];
}

Environment* Environment::ancestor(int distance) {
    Environment* env = this;
    for (int i = 0; i < distance; i++)
        env = env->enclosing;
    return env;
}
```

Now `showA` always jumps exactly 2 hops to reach `a` in global — the block-scoped `a` declared later is one hop away and is never seen.

---

## The full picture

```
Source code parsed
    └── Resolver walks AST
            └── records hop depth for every variable use
                        └── Interpreter executes
                                └── lookUpVariable jumps exactly N hops
                                        └── always finds the right scope
```
