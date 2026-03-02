# Transitioning to [StandardCPlusPlusModules](https://clang.llvm.org/docs/StandardCPlusPlusModules.html#transitioning-to-modules)

## Transitioning to modules

It is best for new code and libraries to use modules from the start if
possible. However, it may be a breaking change for existing code or
libraries to switch to modules. As a result, many existing libraries
need to provide both _headers_ and _module interfaces_ for a while to not
break existing users.

This section provides some suggestions on how to ease the transition
process for existing libraries.

**NOTE:** that this information is only intended as guidance, rather than as
requirements to use modules in Clang. It presumes the project is starting with
no module-based dependencies.

### ABI non-breaking styles

#### export-using style

```cpp
module;
#include "header_1.h"
#include "header_2.h"
...
#include "header_n.h"
export module your_library;
export namespace your_namespace {
  using decl_1;
  using decl_2;
  ...
  using decl_n;
}
```

This example shows how to include all the headers containing
declarations which need to be exported, and uses **using declarations** in
an _export block_ to produce the _module interface_.

#### export extern-C++ style

```cpp
module;
#include "third_party/A/headers.h"
#include "third_party/B/headers.h"
...
#include "third_party/Z/headers.h"
export module your_library;
#define IN_MODULE_INTERFACE
extern "C++" {
  #include "header_1.h"
  #include "header_2.h"
  ...
  #include "header_n.h"
}
```

Headers (from `header_1.h` to `header_n.h`) need to define the macro:

```c
#ifdef IN_MODULE_INTERFACE
#define EXPORT export
#else
#define EXPORT
#endif
```

It is also recommended to refactor headers to include third-party
headers conditionally:

```c
#ifndef IN_MODULE_INTERFACE
#include "third_party/A/headers.h"
#endif

#include "header_x.h"
...
```

This approach works because the declarations with _language linkage_ are
attached to the _global module_. Thus, the **ABI** of the modular form of the
library does not change.

**NOTE:** that the _private module fragment_ can only be in the _primary module
interface unit_ and the _primary module interface unit_ containing the
_private module fragment_ should be the only _module unit_ of the
corresponding module.

In this case, source files (.cpp files) must be converted to _module
implementation units_:

```cpp
#ifndef IN_MODULE_INTERFACE
// List all the includes here.
#include "third_party/A/headers.h"
...
#include "header.h"
#endif

module your_library;

// Following off should be unchanged.
...
```

The _module implementation unit_ will import the _primary module_
implicitly.

**HINT:** Do not include any headers in the _module implementation units_ as it
avoids duplicated declarations between _translation units_.

This is why **non-exported using declarations** should be added from
third-party libraries in the _primary module interface unit_.

If the library is provided as `libyour_library.so`, a modular library
(e.g., `libyour_library_modules.so`) may also need to be provided for
**ABI** compatibility.

### Importing modules

When there are library dependencies providing modules, the module
dependencies should be imported in your module as well. Many existing
libraries will fall into this category once the `std` module is more
widely available.

#### All library dependencies providing modules

Headers need to be converted to include third-party headers
conditionally. Then, for the export-using style:

```cpp
module;
import modules_from_third_party;
#define IN_MODULE_INTERFACE
#include "header_1.h"
#include "header_2.h"
...
#include "header_n.h"
export module your_library;
export namespace your_namespace {
  using decl_1;
  using decl_2;
  ...
  using decl_n;
}
```

or, for the export **extern-C++ style**:

```cpp
export module your_library;
import modules_from_third_party;
#define IN_MODULE_INTERFACE
extern "C++" {
  #include "header_1.h"
  #include "header_2.h"
  ...
  #include "header_n.h"
}
```

or, for the **ABI-breaking style**,

```cpp
export module your_library;
import modules_from_third_party;
#define IN_MODULE_INTERFACE
#include "header_1.h"
#include "header_2.h"
...
#include "header_n.h"

#if the number of .cpp files in your project are small
module :private;
#include "source_1.cpp"
#include "source_2.cpp"
...
#include "source_n.cpp"
#endif
```

**Non-exported** `using` declarations are unnecessary if using
_implementation module units_. Instead, third-party modules can be
imported directly in _implementation module units_.

#### Partial library dependencies providing modules

If the library has to mix the use of `include` and `import` in its
module, the primary goal is still the removal of duplicated declarations
in _translation units_ as much as possible.

**HINT:** If the imported modules provide headers to skip parsing their headers,
those should be included after the import:

```cpp
// a.cpp
import B;
int main()
{
    g<void>();
}

// b.cppm
export module B;
import :C;
export template <typename T> inline void g() noexcept
{
    return f<T>();
}

// c.cppm
module B:C;
template<typename> inline void f() noexcept {}
```

The _internal partition unit_ `c.cppm` is not necessarily reachable by
`a.cpp` because `c.cppm` is not a _module interface unit_ and `a.cpp`
doesn't import `c.cppm`. This leaves it up to the compiler to decide if
`c.cppm` is reachable by `a.cpp` or not. Clang's behavior is that
indirectly imported _internal partition units_ are not reachable.

The suggested approach for using an _internal partition unit_ in Clang is
to only import them in the _implementation unit_.
