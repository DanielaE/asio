cmake_minimum_required(VERSION 3.30...4.3)

include(cmake/enable_import_std.cmake)

project(
    asio
    DESCRIPTION "Modularized Asio for C++20"
    HOMEPAGE_URL "https://github.com/DanielaE/asio/tree/module"
    LANGUAGES CXX
    VERSION 1.36.0
)

# ---------------------------------------------------------------------------
# Ensure binaries find the correct libstdc++ at runtime when using a
# non-system GCC (e.g. /opt/gcc-16). Without this, CTest fails because the
# dynamic linker finds the system's older libstdc++ which lacks required
# GLIBCXX symbols.
# ---------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(_suffix .so)
    if(APPLE)
        set(_suffix .dylib)
    endif()
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++${_suffix}
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
add_library(asio::asio-header ALIAS asio_header)
target_sources(
    asio_header
    INTERFACE FILE_SET HEADERS BASE_DIRS include FILES ${_asio_implementation}
)
target_compile_features(asio_header INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
target_compile_definitions(
    asio_header
    INTERFACE
        ASIO_DISABLE_BOOST_CONTEXT_FIBER
        ASIO_STANDALONE
        ASIO_NO_DEPRECATED
)

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
        FILE_SET HEADERS
            BASE_DIRS include module
            FILES ${_public_headers} module/asio-gmf.h
)
target_compile_features(asio PRIVATE cxx_std_${CMAKE_CXX_STANDARD})
set_target_properties(
    asio
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${PROJECT_IS_TOP_LEVEL}
)
target_link_libraries(asio PUBLIC asio_header)

option(ASIO_IMPORT_STD "Enable 'import std;' support for ASIO module" OFF)
if(ASIO_IMPORT_STD)
    # Tell CMake that we explicitly want `import std`.
    # This initializes the property CXX_MODULE_STD of asio targets to 1 (requires CMake 3.30+)
    target_compile_definitions(asio PUBLIC ASIO_HAS_IMPORT_STD)
    set_target_properties(asio PROPERTIES CXX_MODULE_STD ON)
    message(STATUS "Build asio with target PROPERTY CXX_MODULE_STD ON")
endif()

# Optional SSL support (can be controlled via -DASIO_ENABLE_SSL=OFF to disable)
option(ASIO_ENABLE_SSL "Enable SSL support for ASIO module" ON)
if(ASIO_ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
    target_compile_definitions(asio PUBLIC ASIO_USE_SSL)
    # Add OpenSSL include directories for module compilation
    target_link_libraries(asio PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    message(
        STATUS
        "ASIO: SSL support enabled with OpenSSL at ${OPENSSL_INCLUDE_DIR}"
    )
    set(_dependencies DEPENDENCIES OpenSSL)
else()
    message(
        STATUS
        "ASIO: SSL support disabled (use -DASIO_ENABLE_SSL=ON to enable)"
    )
    set(_dependencies)
endif()

if(MINGW)
    target_link_libraries(asio PUBLIC bcrypt mswsock ws2_32)
endif()

enable_testing()

option(ASIO_BUILD_EXAMPLES "Build Asio examples" ${PROJECT_IS_TOP_LEVEL})
if(ASIO_BUILD_EXAMPLES)
    add_subdirectory(src/examples)
endif()

option(ASIO_BUILD_TESTS "Build Asio tests" ${PROJECT_IS_TOP_LEVEL})
if(ASIO_BUILD_TESTS)
    # test if the targets are usable from the install directory
    add_test(
        NAME install-to-stagedir
        COMMAND
            ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix
            ${CMAKE_BINARY_DIR}/stagedir --config $<CONFIG>
    )
    add_test(
        NAME find-package-test
        COMMAND
            ${CMAKE_CTEST_COMMAND} # --verbose
            --output-on-failure -C $<CONFIG> --build-and-test
            "${CMAKE_SOURCE_DIR}/src/examples"
            "${CMAKE_CURRENT_BINARY_DIR}/find-package-test" --build-generator
            ${CMAKE_GENERATOR} --build-makeprogram ${CMAKE_MAKE_PROGRAM}
            --build-options #
            "-D ASIO_IMPORT_STD=${ASIO_IMPORT_STD}"
            "-D CMAKE_BUILD_TYPE=$<CONFIG>"
            "-D CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-D CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
            "-D CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON}"
            "-D CMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/stagedir"
    )
endif()

#
# Install the libraries and its FILE_SET's.
#
include(cmake/asio-install-library.cmake)

asio_install_library(asio TARGETS asio asio_header
    ${_dependencies}
)
