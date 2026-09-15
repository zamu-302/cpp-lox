#include "../include/expression.h"
#include <string>

int main() {

  auto expression = std::make_unique<Binary>(
      std::make_unique<Unary>(Token(TokenType::MINUS, "-", std::string{}, 1),
                              std::make_unique<Literals>("123")),
      std::make_unique<Grouping>(std::make_unique<Literals>("45.67")),

      Token(TokenType::STAR, "*", std::string{}, 1));
  AstPrinter ast;
  std::cout << ast.print(*expression);
}
