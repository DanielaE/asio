if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

if(CMAKE_CXX_STANDARD LESS 20)
    message(
        FATAL_ERROR
        "At least C++20 required but have ${CMAKE_CXX_STANDARD}"
    )
endif()

if(ASIO_IMPORT_STD)
    # ---------------------------------------------------------------------------
    # Experimental Setup (MUST be before project() for compiler detection)
    # ---------------------------------------------------------------------------

    # 1. Enable experimental 'import std;' support
    if(NOT DEFINED CMAKE_EXPERIMENTAL_CXX_IMPORT_STD)
        if(
            CMAKE_VERSION VERSION_GREATER_EQUAL 4.3
            AND CMAKE_VERSION VERSION_LESS 4.4
        )
            set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
                "451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
            )
        elseif(
            CMAKE_VERSION VERSION_GREATER_EQUAL 4.2
            AND CMAKE_VERSION VERSION_LESS 4.3
        )
            set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
                "d0edc3af-4c50-42ea-a356-e2862fe7a444"
            )
        elseif(CMAKE_VERSION VERSION_GREATER 4.3)
            message(
                WARNING
                "This version of CMake has not been tested for 'import std;'."
            )
        endif()
    endif()

    # 3. Ensure required C++ standard is set
    if(CMAKE_CXX_STANDARD VERSION_LESS 23)
        message(
            WARNING
            "import std requires C++23 but have ${CMAKE_CXX_STANDARD}. Overriding CMAKE_CXX_STANDARD to 23..."
        )
        set(CMAKE_CXX_STANDARD 23)
    endif()

    # 2. Clang libc++ Modules Path Setup
    # before project() we must detect clang by inspecting CMAKE_CXX_COMPILER
    if(
        NOT CMAKE_CXX_STDLIB_MODULES_JSON
        AND (
            CMAKE_CXX_COMPILER MATCHES "clang"
            OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
        )
    )
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -print-resource-dir
            OUTPUT_VARIABLE _clang_resource_dir
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(_clang_resource_dir)
            # libc++.modules.json is in the lib directory within LLVM/Clang root
            cmake_path(SET _modules_json_path "${_clang_resource_dir}")
            cmake_path(GET _modules_json_path PARENT_PATH _modules_json_path) # .../lib/clang
            cmake_path(GET _modules_json_path PARENT_PATH _modules_json_path) # .../lib
            cmake_path(APPEND _modules_json_path "libc++.modules.json")

            if(EXISTS "${_modules_json_path}")
                set(CMAKE_CXX_STDLIB_MODULES_JSON
                    "${_modules_json_path}"
                    CACHE PATH
                    "Path to libc++.modules.json"
                )
                message(
                    STATUS
                    "Found libc++.modules.json: ${CMAKE_CXX_STDLIB_MODULES_JSON}"
                )
            endif()
        endif()
    else()
        message(
            VERBOSE
            "CMAKE_CXX_STDLIB_MODULES_JSON: ${CMAKE_CXX_STDLIB_MODULES_JSON}"
        )
    endif()
endif()
