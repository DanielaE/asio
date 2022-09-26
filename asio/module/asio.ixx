module;
#include "asio-module.h"

export module asio;

#ifdef _MSC_VER
#	pragma comment(lib, "asio.lib")
#endif

#ifdef ASIO_ATTACH_TO_GLOBAL_MODULE
extern "C++" {
#endif

export {
#include "asio.hpp"

#include "asio/ts/buffer.hpp"
#include "asio/ts/executor.hpp"
#include "asio/ts/internet.hpp"
#include "asio/ts/io_context.hpp"
#include "asio/ts/net.hpp"
#include "asio/ts/netfwd.hpp"
#include "asio/ts/socket.hpp"
#include "asio/ts/timer.hpp"

#include "asio/experimental/as_single.hpp"
#include "asio/experimental/as_tuple.hpp"
#include "asio/experimental/awaitable_operators.hpp"
#include "asio/experimental/cancellation_condition.hpp"
#include "asio/experimental/co_spawn.hpp"
#include "asio/experimental/coro.hpp"
#include "asio/experimental/parallel_group.hpp"
#include "asio/experimental/promise.hpp"
#include "asio/experimental/use_coro.hpp"
} // export

#ifdef ASIO_ATTACH_TO_GLOBAL_MODULE
} // extern "C++"
#endif

module :private;

#include "asio/impl/src.hpp"
