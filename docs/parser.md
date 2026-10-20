Parsing
a parser takes the list of the tokens from the scanner and produces a tree like structure that mirrors the nested nature of grammar. they are called syntax trees or abstract syntax trees. it gives us the ability to create the higher level statements using out smaller parts(Tokens). which looks like this.
![Alt Text Description](./images/Ast.png)
This Syntax tree will also help us figure out syntax errors. it's also used by LSP(Language sever protocol) which guesses what you are trying to do when coding by seeing the whereabouts of the cursor and the AST(abstract syntax tree).

The way the parser handles syntax error is entering panic mode which sounds too much but is necessary to avoid cascading error which is having more errors around because the parser continuing to parse without resolving the previous error causing it to have more error than necessary and to avoid this we use Synchronization which basically get the parser to safe state before going to the next tokens. saves before state-> resolves->goes back.

context free Grammar
-is a set of formal rules(Productions) for generating strings in a language.where those strings define what a valid expression, statements or program looks like.it contains bodies that describe the rule of the grammar which are terminal,non terminal and productions were terminal are tokens which are like literal values and are called terminals because they are endpoints whereas non terminal can be seen as rules that point to other rules till you find a terminal and return it.Production which has a non terminal -> sequence of terminal and/or non terminal. but left side must be one non terminal. This is also where precedence is designed where lower precedence operations appear upper in the grammar and higher ones appear lower in the grammar.

![Alt Text Description](./images/Statements.png)![Alt Text Description](./images/Expression.png)

to implement those grammars we have a recursive decent parser which is a top-down parser where we write one function for each non terminal in the grammar. were it consumes tokens and calls other non terminal functions when needed and returns a parsed value. which mirrors the grammar's recursion.

![Alt Text Description](./images/matrix.png)
