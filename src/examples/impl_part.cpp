// impl_part.cppm
// Internal module partition unit
// is this a module partition implementations? CK

module;

#include <iostream>
#include <string_view>

module M;

constexpr std::string_view W{"World."};
// MSVC error C7619: cannot export 'World' as module partition 'impl_part' does not contribute to the exported interface of module unit 'M'
void World() {
    std::cout << W << '\n';
}
