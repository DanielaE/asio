cmake_minimum_required(VERSION 3.30...4.3)

include(../../cmake/enable_import_std.cmake)

project(asio_examples DESCRIPTION "Examples for Asio" LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    find_package(asio REQUIRED)
else()
    if(APPLE)
        # partly deprecated! add_subdirectory(cpp11)
        add_subdirectory(cpp14)
        # CoroutineTS is deperecated!
        # NO! add_subdirectory(cpp17)
    endif()
    add_subdirectory(cpp20)
endif()

add_executable(
    example.completion_executor
    cpp20/invocation/completion_executor.cpp
)
target_link_libraries(example.completion_executor PRIVATE asio::asio)
target_compile_features(
    example.completion_executor
    PRIVATE cxx_std_${CMAKE_CXX_STANDARD}
)
if(ASIO_IMPORT_STD)
    set_property(TARGET example.completion_executor PROPERTY CXX_MODULE_STD ON)
endif()
add_test(NAME example.completion_executor COMMAND example.completion_executor)

add_library(M STATIC)
target_sources(
    M
    PRIVATE Impl.cpp impl_part.cpp
    PUBLIC FILE_SET CXX_MODULES FILES M.cppm interface_part.cppm
)
target_compile_features(M PRIVATE cxx_std_${CMAKE_CXX_STANDARD})

add_executable(example.User User.cpp)
target_link_libraries(example.User PRIVATE M)
add_test(NAME example.User COMMAND example.User)
