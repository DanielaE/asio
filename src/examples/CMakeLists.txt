add_executable(
    example.completion_executor
    cpp20/invocation/completion_executor.cpp
)
target_link_libraries(example.completion_executor PRIVATE asio::asio)
add_test(NAME example.completion_executor COMMAND example.completion_executor)
