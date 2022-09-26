//
// detail/is_executor.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_IS_EXECUTOR_HPP
#define ASIO_DETAIL_IS_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/config.hpp"
#include "asio/detail/type_traits.hpp"

#include "asio/detail/push_options.hpp"

namespace asio {
namespace detail {

template <typename T>
constexpr inline bool is_executor_class_v =
  requires(T t) {
    { t.context()          };
    { t.on_work_started()  };
    { t.on_work_finished() };
    { t.dispatch()         };
    { t.post()             };
    { t.defer()            };
  };

template <typename T>
struct is_executor_class
  : integral_constant<bool, is_executor_class_v<T>> {};

template <typename T>
struct is_executor
  : conditional<is_class<T>::value,
      is_executor_class<T>,
      false_type>::type
{
};

} // namespace detail
} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_DETAIL_IS_EXECUTOR_HPP
