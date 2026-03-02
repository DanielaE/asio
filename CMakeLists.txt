cmake_minimum_required(VERSION 3.30...4.3)

# gersemi: off
if(CMAKE_CXX_STANDARD LESS 20)
    message( FATAL_ERROR "At least C++20 required but have ${CMAKE_CXX_STANDARD}")
endif()

# ---------------------------------------------------------------------------
# check if import std; is supported by CMAKE_CXX_COMPILER
# ---------------------------------------------------------------------------
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.3 AND CMAKE_VERSION VERSION_LESS 4.4)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.2 AND CMAKE_VERSION VERSION_LESS 4.3)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
endif()
# gersemi: on

project(
    asio
    DESCRIPTION "Modularized Asio for C++20"
    HOMEPAGE_URL "https://github.com/DanielaE/asio/tree/module"
    LANGUAGES CXX
)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_SKIP_TEST_ALL_DEPENDENCY OFF)

# ---------------------------------------------------------------------------
# Ensure binaries find the correct libstdc++ at runtime when using a
# non-system GCC (e.g. /opt/gcc-16). Without this, CTest fails because the
# dynamic linker finds the system's older libstdc++ which lacks required
# GLIBCXX symbols.
# ---------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++.so
        OUTPUT_VARIABLE _gcc_stdcxx_path
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(_gcc_stdcxx_path)
        cmake_path(GET _gcc_stdcxx_path PARENT_PATH _gcc_lib_dir)
        get_filename_component(_gcc_lib_dir "${_gcc_lib_dir}" REALPATH)
        list(APPEND CMAKE_BUILD_RPATH "${_gcc_lib_dir}")
        message(STATUS "GCC libstdc++ rpath: ${_gcc_lib_dir}")
    endif()
endif()

message(
    STATUS
    "Cmake is: ${CMAKE_VERSION} modules scan: ${CMAKE_CXX_SCAN_FOR_MODULES}"
)

# ---- Declare libraries ----

file(
    GLOB_RECURSE _asio_implementation
    "include/asio/*.hpp"
    "include/asio/*.ipp"
)

#==============================
# asio interface library
#==============================
add_library(asio_header INTERFACE)
add_library(asio::asio_header ALIAS asio_header)
target_sources(
    asio_header
    INTERFACE FILE_SET HEADERS BASE_DIRS include FILES ${_asio_implementation}
)
target_compile_features(asio_header INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
target_compile_definitions(asio_header INTERFACE ASIO_DISABLE_BOOST_CONTEXT_FIBER ASIO_STANDALONE ASIO_NO_DEPRECATED)

#==============================
# asio module library
#==============================
file(GLOB_RECURSE _asio_headers "include/asio/*.hpp")
list(FILTER _asio_headers EXCLUDE REGEX [=[.*/spawn.*\.hpp]=])

# FIXME: some header in include/asio/detail fails VERIFY_INTERFACE_HEADER_SETS!
# NOTE: we need them installed, but we used them install with asio_header INTERFACE! CK
set(_public_headers include/asio.hpp ${_asio_headers})
list(FILTER _public_headers EXCLUDE REGEX [=[.*/detail/.*\.hpp]=])
list(FILTER _public_headers EXCLUDE REGEX [=[.*/impl/.*\.hpp]=])
# foreach(header in LISTS ${_public_headers})
#   message(DEBUG "${header}")
# endforeach()

add_library(asio STATIC)
add_library(asio::asio ALIAS asio)
target_sources(
    asio
    PUBLIC
        FILE_SET modules TYPE CXX_MODULES BASE_DIRS module FILES module/asio.ixx
        FILE_SET HEADERS BASE_DIRS include FILES ${_public_headers}
)
target_compile_features(asio PUBLIC cxx_std_${CMAKE_CXX_STANDARD})
set_target_properties(
    asio
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${PROJECT_IS_TOP_LEVEL}
)
target_link_libraries(asio PUBLIC asio_header)

option(ASIO_IMPORT_STD "Enable 'import std;' support for ASIO module" OFF)
if(ASIO_IMPORT_STD)
    message(
        STATUS
        "CMAKE_CXX_COMPILER_IMPORT_STD=${CMAKE_CXX_COMPILER_IMPORT_STD}"
    )
    if(${CMAKE_CXX_STANDARD} IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
        option(
            ASIO_HAS_IMPORT_STD
            "Build with import std; is possible and used!"
            ON
        )
        message(STATUS "CMAKE_CXX_MODULE_STD=${CMAKE_CXX_MODULE_STD}")
    endif()
    message(STATUS "ASIO_HAS_IMPORT_STD=${ASIO_HAS_IMPORT_STD}")
endif()
if(ASIO_HAS_IMPORT_STD)
    # -------------------------------------------------------------------------
    # Tell CMake that we explicitly want `import std`.
    # This will initialize the property CXX_MODULE_STD of asio targets to 1
    # -------------------------------------------------------------------------
    target_compile_definitions(asio PUBLIC ASIO_HAS_IMPORT_STD)
    set_target_properties(asio PROPERTIES CXX_MODULE_STD ON)
    message(STATUS "Build asio with target PROPERRY CXX_MODULE_STD ON")
endif()

# Optional SSL support (can be controlled via -DASIO_ENABLE_SSL=OFF to disable)
option(ASIO_ENABLE_SSL "Enable SSL support for ASIO module" ON)
if(ASIO_ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
    target_compile_definitions(asio PUBLIC ASIO_USE_SSL)
    # Add OpenSSL include directories for module compilation
    # Use SYSTEM to mark as system headers and PRIVATE for module internal use
    target_include_directories(asio SYSTEM PRIVATE ${OPENSSL_INCLUDE_DIR})
    # Also add as PUBLIC for consumers
    target_include_directories(asio SYSTEM PUBLIC ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(asio PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    message(
        STATUS
        "ASIO: SSL support enabled with OpenSSL at ${OPENSSL_INCLUDE_DIR}"
    )
else()
    message(
        STATUS
        "ASIO: SSL support disabled (use -DASIO_ENABLE_SSL=ON to enable)"
    )
endif()

if(MSVC)
    # right, but please don't complain about #includes in the module purview
    target_compile_options(asio PRIVATE /wd5244)
    # I do mean C++23
    target_compile_options(
        asio
        PUBLIC
            /utf-8
            /Zc:__cplusplus
            /Zc:throwingNew
            /Zc:inline
            /Zc:externConstexpr
            /Zc:templateScope
            /Zc:checkGwOdr
            /Zc:enumTypes
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # I do mean C++20
    target_compile_options(
        asio
        PUBLIC
            -fsized-deallocation
            -faligned-allocation
            -Wno-include-angled-in-module-purview
    )
endif()

if(MINGW)
    target_link_libraries(asio PUBLIC bcrypt mswsock ws2_32)
endif()

enable_testing()
add_subdirectory(src/examples)

#
# Install the libraries and its FILE_SET's.
#
include(GNUInstallDirs)

set(package_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/asio)

install(
    TARGETS asio asio_header
    # EXPORT ${targets_export_name}
    COMPONENT asio_Development
    FILE_SET HEADERS
    FILE_SET CXX_MODULES
    CXX_MODULES_BMI
        DESTINATION
            ${package_install_dir}/bmi-${CMAKE_CXX_COMPILER_ID}_$<CONFIG>
    FILE_SET asio_header
)

# TODO(CK): install the cmake config packages too
