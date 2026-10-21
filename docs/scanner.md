Scanning(Lexing)

## Idea

The scanner takes linear stream of characters and chucks them them together into a series of something more of a word(Token) which is basically the smallest meaningful units of the language. some character or sequence of characters have meaning the others like white spaces, comments doesn't have meaning thus they are ignored.

## In this project

Files: `include/scanner.h`, `src/scanner.cpp`, `include/token.h`.

The scanner keeps three pointers:

| Field   | Meaning                               |
| ------- | ------------------------------------- |
| `start` | index where the current lexeme began  |
| `curr`  | index of the next character to read   |
| `line`  | current line, used for error messages |

The main loop is just:

```
while not at end:
    start = curr
    scanToken()
add an Eof token
```

`scanToken()` reads one character with `advance()` and decides what to do with a `switch`:

| Kind                  | Examples              | How it is handled                                                                     |
| --------------------- | --------------------- | ------------------------------------------------------------------------------------- |
| Single character      | `( ) { } , . - + ; *` | add the token immediately                                                             |
| One or two characters | `! != = == < <= > >=` | `match('=')` peeks at the next character and consumes it only if it is `=`            |
| Slash                 | `/`, `//`, `/* */`    | `//` skips to end of line, `/* */` skips to the closing `*/`, otherwise it is `SLASH` |
| Ignored               | space, `\r`, `\t`     | do nothing                                                                            |
| Newline               | `\n`                  | `line++`                                                                              |
| String                | `"hi"`                | `is_string()` reads until the closing `"`, stores the text without the quotes         |
| Number                | `10`, `3.14`          | `number()`                                                                            |
| Identifier or keyword | `x`, `while`          | `identifier()` reads the whole word, then looks it up in the `keywords` map           |

The helper functions are small but worth knowing:

- `advance()` returns the current character and moves `curr` forward.
- `peek()` looks at the current character without consuming it (`'\0'` at the end).
- `peekNext()` looks one further ahead. Numbers need it to decide whether a `.` starts a fraction.
- `match(c)` consumes the next character only if it equals `c`.

## Example

Input: `var x = 10; // hi`

```
VAR('var')  IDENTIFIER('x')  EQUAL('=')  NUMBER('10')  SEMICOLON(';')  Eof
```

The comment produced no token.
