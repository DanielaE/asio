// M.cppm
// Primary module interface unit
// importable module unit files should use .cppm (or .ccm, .cxxm, or .c++m) as a file extension.

export module M;
export import :interface_part;

// warning: importing an implementation partition unit in a module interface is not recommended.
// NOTE: not needed! import :impl_part;

export void Hello();
