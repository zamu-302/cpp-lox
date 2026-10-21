# Lox Interpreter

A complete tree-walk interpreter for the [Lox language](https://craftinginterpreters.com), written in C++20. Built by following Robert Nystrom's _Crafting Interpreters_ and translating the Java reference implementation into idiomatic C++.

[Alt Text Description](./images/Parts_of_the_language.png)
---

## Build

```bash
cmake -B build -G Ninja
cmake --build build
```

Run a file:

```bash
./build/cpp-lox script.lox
```

Run in localHost

```bash
python server.py
open <http://localhost:3000>
```

## Language features

**Types** — number (double), string, boolean, nil

**Operators** — `+` `-` `*` `/` `==` `!=` `<` `>` `<=` `>=` `and` `or` `!`

**Control flow** — `if / else`, `while`, `for` (desugared to while)

**Functions**

```
fun add(a, b) { return a + b; }
print add(1, 2); // 3
```

**Closures**

```
fun makeCounter() {
    var count = 0;
    fun inc() { count = count + 1; return count; }
    return inc;
}
var c = makeCounter();
c(); c(); c(); // 1 2 3
```

**Classes**

```
class Animal {
    init(name) { this.name = name; }
    speak() { print this.name; }
}
```

**Inheritance**

```
class Dog < Animal {
    speak() { print this.name + " barks"; }
}
Dog("Rex").speak();
```

**super**

```
class Dog < Animal {
    speak() { super.speak(); }
}
```

## Error handling

**Syntax errors** — thrown as `ParseError` during parsing, caught in `declaration()`, which calls `synchronize()` to recover and continue parsing the rest of the file.

**Runtime errors** — thrown as `RuntimeError` (extends `std::runtime_error`) during execution, caught at the top of `interpret()`, which calls `ErrorReporter::runtimeError()` and sets `hadRuntimeError = true`.

**Static errors** — reported by the Resolver before execution. Programs with static errors never reach the interpreter.

---

## Docs

- [`README.md`](./docs/README.md) — contains the order the read the Docs from and the reasoning behind it.

### Credits

1. Crafting Interpreter book by Robert Nystrom [Visit Book](https://share.google/I8460WRbyi4zYXrce)
2. C++ Software Design by Klaus Iglbereger.
