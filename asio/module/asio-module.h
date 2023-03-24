#pragma once

#ifdef _WIN32
#	include <SDKDDKVer.h>
#	define _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
#endif

#if defined(ASIO_HEADER_ONLY)
#	error "ASIO_HEADER_ONLY" makes no sense with this module
#endif

#if !defined(ASIO_SEPARATE_COMPILATION)
#	define ASIO_SEPARATE_COMPILATION
#endif

#if !defined(ASIO_DISABLE_BUFFER_DEBUGGING) && !defined(ASIO_ENABLE_BUFFER_DEBUGGING)
#	define ASIO_DISABLE_BUFFER_DEBUGGING
#endif

#define ASIO_NO_DEPRECATED
#define ASIO_MODULE
// #define ASIO_ATTACH_TO_GLOBAL_MODULE

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <crtdbg.h>
#include <csignal>

#include <MSWSock.h>

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <compare>
#include <condition_variable>
#include <coroutine>
#include <deque>
#include <forward_list>
#include <functional>
#include <future>
#include <ios>
#include <iosfwd>
#include <istream>
#include <iterator>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <stop_token>
#include <streambuf>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>
