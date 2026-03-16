set(target_prefix asio_unit_)

set(test_PROGRAMS
    associated_allocator
    associated_executor
    async_result
    awaitable
    basic_datagram_socket
    basic_deadline_timer
    basic_raw_socket
    basic_seq_packet_socket
    basic_serial_port
    basic_signal_set
    basic_socket
    basic_socket_acceptor
    basic_stream_socket
    basic_streambuf
    basic_waitable_timer
    # XXX bind_executor
    # XXX buffered_read_stream
    # XXX buffered_stream
    # XXX buffered_write_stream
    buffer
    buffers_iterator
    co_spawn
    completion_condition
    compose
    connect
    coroutine
    deadline_timer
    defer
    detached
    dispatch
    error
    execution_context
    executor
    executor_work_guard
    generic/basic_endpoint
    # XXX generic/datagram_protocol
    # XXX generic/raw_protocol
    # XXX generic/seq_packet_protocol
    # XXX generic/stream_protocol
    high_resolution_timer
    io_context
    # XXX io_context_strand
    ip/address
    ip/address_v4
    ip/address_v4_iterator
    ip/address_v4_range
    ip/address_v6
    ip/address_v6_iterator
    ip/address_v6_range
    ip/basic_endpoint
    ip/basic_resolver
    ip/basic_resolver_entry
    ip/basic_resolver_iterator
    ip/basic_resolver_query
    ip/host_name
    # XXX ip/icmp
    ip/multicast
    ip/network_v4
    ip/network_v6
    ip/resolver_query_base
    # XXX ip/tcp
    # XXX ip/udp
    ip/unicast
    ip/v6_only
    is_read_buffered
    is_write_buffered
    local/basic_endpoint
    local/connect_pair
    local/datagram_protocol
    local/stream_protocol
    packaged_task
    placeholders
    posix/basic_descriptor
    posix/basic_stream_descriptor
    posix/descriptor
    posix/descriptor_base
    # XXX posix/stream_descriptor
    post
    # XXX read
    # XXX read_at
    # XXX read_until
    # XXX redirect_error
    # XXX serial_port
    serial_port_base
    # XXX signal_set
    socket_base
    steady_timer
    strand
    streambuf
    system_context
    system_executor
    system_timer
    this_coro
    thread
    time_traits
    ts/buffer
    ts/executor
    ts/internet
    ts/io_context
    ts/net
    ts/netfwd
    ts/socket
    ts/timer
    use_awaitable
    # XXX use_future
    uses_executor
    wait_traits
    windows/basic_object_handle
    windows/basic_overlapped_handle
    windows/basic_random_access_handle
    windows/basic_stream_handle
    # FIXME: windows/object_handle
    windows/overlapped_handle
    # FIXME: windows/overlapped_ptr
    # FIXME: windows/random_access_handle
    # FIXME: windows/stream_handle
    # XXX write
    # XXX write_at
)

set(test_header ../latency/high_res_clock.hpp unit_test.hpp)

# if(OpenSSL_FOUND)
#     list(APPEND test_PROGRAMS
#         ssl/context_base
#         ssl/context
#         ssl/error
#         ssl/stream_base
#         ssl/stream
#     )
# endif()

foreach(program ${test_PROGRAMS})
    string(REPLACE "/" "_" target ${program})
    add_executable(${target_prefix}${target} ${program}.cpp ${test_header})
    # XXX set_target_properties(${target_prefix}${target} PROPERTIES CXX_STANDARD 17)

    target_link_libraries(${target_prefix}${target} asio::asio_header)

    add_test(NAME ${target_prefix}${target} COMMAND "${target_prefix}${target}")
endforeach()

if(CMAKE_SKIP_INSTALL_RULES)
    return()
endif()

if(NOT LINUX)
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
            "-D CMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS}"
            "-D CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
            "-D CMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED}"
            "-D CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON}"
            "-D CMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/stagedir"
    )
endif()
