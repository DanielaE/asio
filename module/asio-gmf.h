#pragma once
#define ASIO_IN_GMF

#ifndef ASIO_STANDALONE
#   define ASIO_STANDALONE // sorry, Boost-ified Asio is not yet supported
#endif

#if defined(_WIN32) and __has_include(<SDKDDKVer.h>)
#	include <SDKDDKVer.h>
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

#if __has_include(<LibraryCustomizations.h>)
#   include <LibraryCustomizations.h>
#else
#   ifndef ASIO_NO_DEPRECATED
#       define ASIO_NO_DEPRECATED
#   endif
#   undef ASIO_DEPRECATED_MSG
#   define ASIO_DISABLE_DEPRECATED_MSG
#endif

#define ASIO_MODULE

#include <asio/detail/config.hpp>

#ifndef ASIO_ENABLE_HANDLER_TRACKING
#	define ASIO_STD_CSTDARG_NO_INCLUDE
#endif

#include "asio/detail/std/all.hpp"

#include <sys/stat.h>
#include <sys/types.h>

#if defined(ASIO_WINDOWS_RUNTIME) // expected to not work at all
#	include <robuffer.h>
#	include <windows.storage.streams.h>
#	include <wrl/implements.h>
#	include <codecvt>
#	include <locale>
#elif defined(ASIO_WINDOWS) or defined(__CYGWIN__)
#	include <WS2tcpip.h>
#	include <WinSock2.h>
#	include <Windows.h>
//  do not reorder!
#	include <MSWSock.h>
#	include <process.h>
#else
#	include <arpa/inet.h>
#	include <fcntl.h>
#	include <limits.h>
#	include <net/if.h>
#	include <netdb.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <poll.h>
#	include <signal.h>
#	include <sys/ioctl.h>
#	include <sys/poll.h>
#	include <sys/socket.h>
#	include <sys/uio.h>
#	include <sys/un.h>
#	include <termios.h>
#endif

#ifdef ASIO_HAS_PTHREADS
#	include <pthread.h>
#endif
#if defined(ASIO_HAS_IO_URING)
#	include <liburing.h>
#endif
#if defined(ASIO_HAS_KQUEUE)
#	include <sys/event.h>
#	include <sys/time.h>
#endif
#if defined(ASIO_HAS_DEV_POLL)
#	include <sys/devpoll.h>
#endif
#if defined(ASIO_HAS_EPOLL)
#	include <sys/epoll.h>
#	if defined(ASIO_HAS_TIMERFD)
#		include <sys/timerfd.h>
#	endif
#endif
#if defined(ASIO_HAS_EVENTFD)
#	include <sys/eventfd.h>
#endif
#if defined(ASIO_HAS_PIPE) and defined(ASIO_HAS_IOCP)
#	include <bcrypt.h>
#endif

#if defined(ASIO_USE_SSL)
#	if defined(ASIO_USE_WOLFSSL)
#		include <wolfssl/options.h>
#	endif // defined(ASIO_USE_WOLFSSL)

#	include <openssl/ssl.h>
#	include <openssl/conf.h>
#	if !defined(OPENSSL_NO_ENGINE)
#		include <openssl/engine.h>
#	endif // !defined(OPENSSL_NO_ENGINE)
#	include <openssl/dh.h>
#	include <openssl/err.h>
#	include <openssl/rsa.h>
#	include <openssl/x509.h>
#	include <openssl/x509v3.h>
#endif
#undef ASIO_IN_GMF
