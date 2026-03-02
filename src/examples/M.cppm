// M.cppm
// Primary module interface unit
// importable module unit files should use .cppm (or .ccm, .cxxm, or .c++m) as a file extension.

export module M;
export import :interface_part;

// clang warning: importing an implementation partition unit in a module interface is not recommended.
// FIXME: is not needed for clang and gcc and does not work wit MSVC! import :impl_part;
export void Hello();
