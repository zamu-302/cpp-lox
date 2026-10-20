Lox Interpreter in C++

A C++ 20 implementation of the jlox implementation from crafting interpreter which is a tree-walk interpreter implemented the following feature.

features of the language are:

- it's dynamically typed- 'var' can store any type.
- control flow
- lexical scopes
- classes & inheritance
- functions & methods
- constructors and fields

Pipeline
source-> scanner-> Tokens-> Parser-> AST-> Resolver-> Interpreter
!Command line (any OS)

[Alt Text Description](./images/Parts_of_the_language.png)
cmake -B build -G Ninja
cmake --build build --config Release

Usage
python server.py
open <http://localhost:3000>
