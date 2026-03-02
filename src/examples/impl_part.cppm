// impl_part.cppm
// Internal module partition unit

module;

#include <iostream>
#include <string_view>

module M:impl_part;
import :interface_part;

constexpr std::string_view W{"World."};
void World() {
    std::cout << W << '\n';
}
