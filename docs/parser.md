# 2. Parser and grammar

## Idea

The parser takes the list of the tokens from the scanner and builds a tree like structure that mirrors the nested nature of grammar. It is called syntax trees or abstract syntax trees.
It gives us the ability to create the higher level statements using out smaller parts(Tokens). which looks like this.
![Alt Text Description](../images/Ast.png)
This Parser will also help us figure out syntax errors when the next token doesn't fit the grammar, it reports an error. It's also used by Resolver,Interpret and LSP(Language sever protocol) which guesses what you are trying to do when coding by seeing the whereabouts of the cursor and the AST(abstract syntax tree).

### context free Grammar

The context-free grammar is a set of formal rules(Productions) for generating strings in a language.where those strings define what a valid expression, statements or program looks like.

It contains bodies that describe the rule of the grammar which are terminal,non terminal and productions were terminal are tokens, an endpoint that needs no further rules.

whereas non terminal can be seen as rules that point to other rules till you find a terminal and return it back to the main rule.

Production which has a non terminal -> sequence of terminal and/or non terminal. but left side must be one non terminal.

**Precedence is built into the grammar.** Lower-precedence operators are matched first, so
they sit _higher_ in the grammar; higher-precedence operators sit _lower_. That is why `*`
ends up deeper in the tree than `+`.

Precedence, lowest to highest:

| Level | Operators               | Associativity | Parser function |
| ----- | ----------------------- | ------------- | --------------- |
| 1     | `=`                     | right         | `assignment`    |
| 2     | `or`                    | left          | `orExpr`        |
| 3     | `and`                   | left          | `andExpr`       |
| 4     | `==` `!=`               | left          | `equality`      |
| 5     | `<` `<=` `>` `>=`       | left          | `comparsion`    |
| 6     | `+` `-`                 | left          | `term`          |
| 7     | `*` `/`                 | left          | `factor`        |
| 8     | `!` `-` (unary)         | right         | `unary`         |
| 9     | `()` call, `.` property | left          | `call`          |

![Alt Text Description](../images/Statements.png)![Alt Text Description](../images/Expression.png)

### Recursive descent

Recursive descent is a **top-down** parsing technique: write one function per non-terminal.
Each function consumes tokens, calls the functions for other non-terminals when its rule says
so, and returns a **tree node**. The code mirrors the grammar's recursion, and it is called
"descent" because it starts at the top rule and goes down towards the terminals.

### Errors and panic mode

If the parser kept going after an error as if nothing had happened, one mistake would cause
a flood of fake errors (**cascading errors**). So after reporting an error the parser enters
**panic mode**: it _throws away tokens_ until it reaches the start of the next statement
(**synchronization**), then continues from there. Nothing is saved or restored; it just skips ahead
to a point where parsing can safely resume.

## Example: `1 + 2 * 3`

Tree:

```
Binary(+)
├── Literal(1)
└── Binary(*)
    ├── Literal(2)
    └── Literal(3)
```

How the functions produce it:

```
expression → assignment → orExpr → andExpr → equality → comparsion → term
term:   expr = factor()  → ... → primary gives 1
term:   match(PLUS) ✓    right = factor()
          factor: expr = unary() → primary gives 2
          factor: match(STAR) ✓  right = unary() → 3   → Binary(2 * 3)
term:   expr = Binary(1 + (2 * 3))
```

`*` binds tighter because `factor` is called _inside_ `term`: by the time `term` gets its
right operand, `factor` has already grouped `2 * 3`.
