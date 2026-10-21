### 5. Tree-Walk Interpreter

is the execution of code right after parsing it to AST. where the interpreter goes one branch and leaf at a time evaluating each node as it goes. it doesn't compile the code to bytecode or machine code. The AST itself is the executable form.

it inherits from visitors(statement and expression) so it have to implement all the virtual methods at runtime. Those are what it implements...

### Expressions

1. visitLiteralExpr: Unwraps and returns the literal value directly (e.g., numbers, strings, booleans, or null).

2. visitGroupingExpr: Recursively evaluates the inner expression inside parentheses and returns its result.

3. visitUnaryExpr: Evaluates the operand, performs runtime type checks, and applies unary operators (like - for arithmetic negation or ! for logical not).

4.visitBinaryExpr: Evaluates the left and right operands, performs strict type checking, and executes arithmetic (+, -, *, /) or comparison (<, >, ==) operations.

5.visitVariableExpr: Queries the current Environment scope chain to look up and retrieve a variable's stored value.

6.visitAssignExpr: Evaluates the right-hand value and updates an existing variable in the appropriate lexical scope environment.

7.visitLogicalExpr: Implements short-circuit evaluation for and and or operators (e.g., skipping the right-hand side of an or if the left-hand side is truthy).

8.visitCallExpr: Evaluates the callee and its arguments, casts the callee to a LoxCallable interface, and invokes its .call() method.

1. visitGetExpr: Evaluates an expression, verifies it is a LoxInstance, and returns the matching property field or method bound to that instance.

2. visitSetExpr: Evaluates the target object and assigned value, ensures the object is a LoxInstance, and updates or creates the target property field.

3. visitThisExpr: Looks up and returns the current class instance using the statically resolved environment distance for the this keyword.

4. visitSuperExpr: Looks up the superclass and current instance from resolved scope distances, then returns the superclass method bound to the current instance.

### Statements

1. visitExpressionStmt: Evaluates an expression statement (like a function call or standalone expression) purely for its side effects, then discards the result.

2. visitPrintStmt: Evaluates an expression, converts the resulting object into a human-readable string via a helper stringify() method, and prints it to standard output.

3. visitVarStmt: Evaluates an optional initializer expression and defines a new variable within the current scope's environment.

4. visitBlockStmt: Creates a brand-new child Environment scoped to that block, executes a list of statements within it, and automatically restores the previous environment afterward.

5. visitIfStmt: Evaluates a condition. If truthy, it executes the "then" branch; otherwise, it executes the "else" branch (if present).

6. visitWhileStmt: Continuously evaluates a condition and loops through its body as long as the condition evaluates to a truthy value.

7. visitFunctionStmt: Allocates a new LoxFunction object—capturing the current lexical scope environment as a closure—and binds it to a name in the environment.

8. visitReturnStmt: Evaluates the return value and throws a special Return exception. this exception bubbles all the way out through nested loops and conditionals to catch blocks inside LoxFunction.call().

9. visitClassStmt: Creates a LoxClass object holding its methods and superclass references, binding it to the designated class name.

it's also responsible for environment management where it holds a pointer to the global environment and maintains a mutable environment pointer to know the scope it is in when traversing, truth operation(what's true is decided here) and it's also responsible for runtime error handling.
