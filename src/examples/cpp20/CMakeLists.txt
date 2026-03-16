set(target_prefix asio_cpp20_)

set(noinst_PROGRAMS
    invocation/completion_executor.cpp
    # operations/composed_5.cpp
    # operations/composed_4.cpp
    # operations/composed_6.cpp
    # operations/composed_7.cpp
    # operations/composed_3.cpp
    # operations/composed_2.cpp
    # operations/composed_1.cpp
    # operations/composed_8.cpp
    # XXX operations/c_callback_wrapper.cpp
    # XXX operations/callback_wrapper.cpp
    coroutines/echo_server_with_as_tuple_default.cpp
    coroutines/echo_server_with_default.cpp
    coroutines/echo_server.cpp
    coroutines/echo_server_with_as_single_default.cpp
    coroutines/echo_server_with_deferred.cpp
    coroutines/echo_server_with_deferred_default.cpp
    coroutines/refactored_echo_server.cpp
    coroutines/chat_server.cpp
    coroutines/timeout.cpp
    channels/mutual_exclusion_2.cpp
    channels/throttling_proxy.cpp
    channels/mutual_exclusion_1.cpp
)

# add_executable(${target_prefix}_type_erasure
#     type_erasure/main.cpp
#     type_erasure/stdin_line_reader.cpp
#     type_erasure/sleep.cpp
# )
# target_link_libraries(${target_prefix}_type_erasure asio::asio_header)

foreach(source ${noinst_PROGRAMS})
    string(REGEX REPLACE "\.cpp$" "" program ${source})
    string(REPLACE "/" "_" target ${program})

    add_executable(${target_prefix}${target} ${program}.cpp)
    target_link_libraries(${target_prefix}${target} asio::asio_header)
endforeach()
