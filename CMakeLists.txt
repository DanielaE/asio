cmake_minimum_required(VERSION 3.30...4.3)

if(CMAKE_CXX_STANDARD LESS 20)
    message(
        FATAL_ERROR
        "At least C++20 required but have ${CMAKE_CXX_STANDARD}"
    )
endif()

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

# ---- Declare libraries ----

file(
    GLOB_RECURSE _asio_implementation
    "include/asio/*/*.hpp"
    "include/asio/*.ipp"
)
list(FILTER _asio_implementation EXCLUDE REGEX [=[.*/experimental/.*]=])


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

#==============================
# asio module library
#==============================
file(GLOB_RECURSE _asio_headers "include/asio/*.hpp")
list(FILTER _asio_headers EXCLUDE REGEX [=[.*/experimental/.*\.hpp]=])
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
        FILE_SET modules
            TYPE CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/module
            FILES module/asio.ixx
        FILE_SET HEADERS BASE_DIRS include FILES ${_public_headers}
)
target_compile_features(asio PUBLIC cxx_std_${CMAKE_CXX_STANDARD})
set_target_properties(
    asio
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${PROJECT_IS_TOP_LEVEL}
)

target_link_libraries(asio PUBLIC asio_header)

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
