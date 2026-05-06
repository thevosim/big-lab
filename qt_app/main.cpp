#include <iostream>
#include "chain_hash_table.hpp"
#include "poly_parser.hpp"

int main() {
    std::string str = "(x+z+y^3)*x^2";
    auto p = PolyParser::parse(str);
    ChainHashTable<std::string, Polynomus> table;
    table.insert(str, p);
    std::cout << "kluch        |    znachenie\n";
    auto lambda = [](const std::string& key, const Polynomus& poly) {
        std::cout << key << " | " << poly << '\n';
    };
    table.for_each(lambda);
    return 0;
}