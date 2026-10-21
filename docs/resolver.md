### 4. Resolver

A single pass over the AST that runs after parsing and before execution. It calculates exactly how many environment hops each variable is from its declaration and stores that in `interpreter.locals`. No code executes — it only reads the AST and writes one integer per variable use.

---

## Why it exists

Without it, runtime lookup walks the chain and finds the wrong variable:

```
var a = "global";
{
    fun showA() { print a; }
    showA();         // "global" ✓
    var a = "block";
    showA();         // "block"  ✗ — same function, different result
}
```

The resolver locks in the hop count at parse time. The interpreter then jumps exactly that many hops every call, ignoring anything declared later.

---

## Scope stack

```cpp
std::vector<std::unordered_map<std::string, bool>> scopes;
```

One map per active scope. The `bool` tracks initialization:

- `false` — declared, initializer not yet evaluated
- `true` — fully initialized, safe to use

This catches `var a = a` — when the initializer tries to read `a`, the flag is still `false`.

```cpp
void beginScope() { scopes.emplace_back(); }
void endScope()   { scopes.pop_back(); }

void declare(const Token& name) { scopes.back()[name.getLexeme()] = false; }
void define(const Token& name)  { scopes.back()[name.getLexeme()] = true;  }
```

---

## resolveLocal

Walks the stack from the inside out and records the depth:

```cpp
void Resolver::resolveLocal(const Expr& expr, const Token& name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].contains(name.getLexeme())) {
            interpreter.resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
    // not found locally — global, no depth stored
}
```

`&expr` is the key — every AST node has a unique stable address for the program's lifetime. At runtime:

```cpp
std::any lookUpVariable(const Token& name, const Expr& expr) {
    if (locals.contains(&expr))
        return environment->getAt(locals[&expr], name.getLexeme());
    return globals.get(name);
}
```

---

## Key visitors

```cpp
// variable declaration — declare before resolving RHS catches: var a = a
void visitVarStmt(const VarStmt& stmt) {
    declare(stmt.name);
    if (stmt.initializer) resolve(*stmt.initializer);
    define(stmt.name);
}

// function — defined before body so recursion works
void visitFunctionStmt(const Function& stmt) {
    declare(stmt.name); define(stmt.name);
    beginScope();
    for (auto& p : stmt.params) { declare(p); define(p); }
    resolve(stmt.body);
    endScope();
}

// class — two extra scopes for this and super
void visitClassStmt(const Class& stmt) {
    if (stmt.superClass) { resolve(*stmt.superClass);
        beginScope(); scopes.back()["super"] = true; }
    beginScope(); scopes.back()["this"] = true;
    for (auto& m : stmt.methods) resolveFunction(*m);
    endScope();
    if (stmt.superClass) endScope();
}
```

The scope layout for classes is intentional:

```
GLOBAL
  └── super scope  { super }   ← depth N
        └── this scope   { this }    ← depth N-1
              └── method body
```

`super` is always one hop further than `this`, so `visitSuper` gets the instance at `distance - 1` without any extra tracking.

---

## Static errors it catches

| Error                      | Caught because                             |
| -------------------------- | ------------------------------------------ |
| `var a = a`                | flag is `false` when initializer reads `a` |
| `return` outside function  | `currentFunction` enum is `NONE`           |
| `this` outside class       | `currentClass` enum is `NONE`              |
| `class Foo < Foo`          | superclass name equals class name          |
| `return value` in `init()` | function type is `INITIALIZER`             |
