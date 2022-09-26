//
// detail/is_buffer_sequence.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_IS_BUFFER_SEQUENCE_HPP
#define ASIO_DETAIL_IS_BUFFER_SEQUENCE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/config.hpp"
#include "asio/detail/type_traits.hpp"

#include "asio/detail/push_options.hpp"
#include <concepts>

namespace asio {

class mutable_buffer;
class const_buffer;
class mutable_registered_buffer;
class const_registered_buffer;

namespace detail {

template <typename T, typename Buffer>
constexpr inline bool is_buffer_sequence_class_v =
  requires(T t) {
    { *asio::buffer_sequence_begin(t) } -> std::convertible_to<Buffer>;
    { *asio::buffer_sequence_end(t)   } -> std::convertible_to<Buffer>;
  };

template <typename T, typename Buffer>
struct is_buffer_sequence_class
  : integral_constant<bool, is_buffer_sequence_class_v<T, Buffer>> {};

template <typename T, typename Buffer>
struct is_buffer_sequence
  : conditional<is_class<T>::value,
      is_buffer_sequence_class<T, Buffer>,
      false_type>::type
{
};

template <>
struct is_buffer_sequence<mutable_buffer, mutable_buffer>
  : true_type
{
};

template <>
struct is_buffer_sequence<mutable_buffer, const_buffer>
  : true_type
{
};

template <>
struct is_buffer_sequence<const_buffer, const_buffer>
  : true_type
{
};

template <>
struct is_buffer_sequence<const_buffer, mutable_buffer>
  : false_type
{
};

template <>
struct is_buffer_sequence<mutable_registered_buffer, mutable_buffer>
  : true_type
{
};

template <>
struct is_buffer_sequence<mutable_registered_buffer, const_buffer>
  : true_type
{
};

template <>
struct is_buffer_sequence<const_registered_buffer, const_buffer>
  : true_type
{
};

template <>
struct is_buffer_sequence<const_registered_buffer, mutable_buffer>
  : false_type
{
};

template <typename T>
constexpr inline bool is_dynamic_buffer_class_v1_v =
  requires {
    { T::const_buffers_type   };
    { T::mutable_buffers_type };
  } &&
  requires(const T & t) {
    { t.size()     };
    { t.max_size() };
    { t.capacity() };
    { t.data()     };
  }
  && requires(T & t, std::size_t n) {
    { t.data()     };
    { t.consume(n) };
    { t.prepare(n) };
    { t.commit(n)  };
  }
;

template <typename T>
struct is_dynamic_buffer_class_v1
  : integral_constant<bool, is_dynamic_buffer_class_v1_v<T>> {};

template <typename T>
struct is_dynamic_buffer_v1
  : conditional<is_class<T>::value,
      is_dynamic_buffer_class_v1<T>,
      false_type>::type
{
};

template <typename T>
constexpr inline bool is_dynamic_buffer_class_v2_v =
  requires {
    { T::const_buffers_type   };
    { T::mutable_buffers_type };
  } &&
  requires(const T & t, std::size_t n) {
    { t.size()     };
    { t.max_size() };
    { t.capacity() };
    { t.data(n, n) };
  } &&
  requires(T & t, std::size_t n) {
    { t.consume(n) };
    { t.grow(n)    };
    { t.shrink(n)  };
  }
;

template <typename T>
struct is_dynamic_buffer_class_v2
  : integral_constant<bool, is_dynamic_buffer_class_v2_v<T>> {};

template <typename T>
struct is_dynamic_buffer_v2
  : conditional<is_class<T>::value,
      is_dynamic_buffer_class_v2<T>,
      false_type>::type
{
};

} // namespace detail
} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_DETAIL_IS_BUFFER_SEQUENCE_HPP
