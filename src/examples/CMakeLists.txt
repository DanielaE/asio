add_executable(
    example.completion_executor
    cpp20/invocation/completion_executor.cpp
)
target_link_libraries(example.completion_executor PRIVATE asio::asio)
if (ASIO_HAS_IMPORT_STD)
  set_property(TARGET example.completion_executor PROPERTY CXX_MODULE_STD ON)
endif()
add_test(NAME example.completion_executor COMMAND example.completion_executor)

add_library(M STATIC)
target_sources(
    M
    PRIVATE Impl.cpp impl_part.cpp
    PUBLIC FILE_SET CXX_MODULES FILES M.cppm interface_part.cppm
)
target_compile_features(M PUBLIC cxx_std_${CMAKE_CXX_STANDARD})

add_executable(example.User User.cpp)
target_link_libraries(example.User PRIVATE M)
add_test(NAME example.User COMMAND example.User)
