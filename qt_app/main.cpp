#include <iostream>
#include "../include/poly_parser.hpp"


int main() {
    auto p = PolyParser::parse("(x+z+y^3)*x^2");
    std::cout << p << '\n';
    return 0;
}