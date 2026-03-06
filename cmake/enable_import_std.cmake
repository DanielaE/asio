
if(NOT DEFINED CMAKE_CXX_STANDARD)
    message(STATUS "CMAKE_CXX_STANDARD is not defined. Setting it to 20...")
    set(CMAKE_CXX_STANDARD 20)
endif()

if(CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "At least C++20 required but have ${CMAKE_CXX_STANDARD}")
endif()

if(ASIO_IMPORT_STD)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.3 AND CMAKE_VERSION VERSION_LESS 4.4)
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
    elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.2 AND CMAKE_VERSION VERSION_LESS 4.3)
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
    elseif(NOT DEFINED CMAKE_EXPERIMENTAL_CXX_IMPORT_STD)
        if(CMAKE_VERSION GREATER 4.3)  # last checked cmake version
        message(WARNING "This version of CMake have not been tested. Please set CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
                        to a valid UUID for this version if experimental gate is still a thing in the future.")
        else()
        message(FATAL_ERROR "CMAKE_EXPERIMENTAL_CXX_IMPORT_STD is not defined. Please set it to a valid UUID.")
        endif()
    endif()

    if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD VERSION_LESS 23)
        message(WARNING "import std requires C++23 but have ${CMAKE_CXX_STANDARD}. Overriding to 23...")
        set(CMAKE_CXX_STANDARD 23)
    endif()

    if("${CMAKE_CXX_STANDARD}" IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
        message(STATUS "Compiler supports 'import std;' for C++${CMAKE_CXX_STANDARD}")
    else() # msvc may not show support as they dont have c++23 formal flag yet, but it works
        message(WARNING "Compiler does NOT support 'import std;' for C++${CMAKE_CXX_STANDARD}")
    endif()
endif()
