#ifndef RSTL_TUPLE_FWD_DECLS_H
#define RSTL_TUPLE_FWD_DECLS_H

#pragma once

#include "config.h"

#include <cstddef>
#include <type_traits>

RSTL_NAMESPACE_BEGIN

template <typename... T>
class tuple;

template <typename Tuple>
class tuple_size;

template <size_t I, typename Tuple>
class tuple_element;

template <size_t I, typename Tuple>
using tuple_element_t = typename tuple_element<I, Tuple>::type;

template <size_t I, typename Tuple>
using const_tuple_element_t = std::conditional_t<std::is_lvalue_reference_v<tuple_element_t<I, Tuple>>, std::add_lvalue_reference_t<const std::remove_reference_t<tuple_element_t<I, Tuple>>>, const tuple_element_t<I, Tuple>>;

template <size_t I, typename... Ts_>
tuple_element_t<I, tuple<Ts_...>>& get(tuple<Ts_...>& t);

template <size_t I, typename... Ts_>
const_tuple_element_t<I, tuple<Ts_...>>& get(const tuple<Ts_...>& t);

template <size_t I, typename... Ts_>
tuple_element_t<I, tuple<Ts_...>>&& get(tuple<Ts_...>&& t);

template <typename T, typename... ts_>
T& get(tuple<ts_...>& t);

template <typename T, typename... ts_>
const T& get(const tuple<ts_...>& t);

template <typename T, typename... ts_>
T&& get(tuple<ts_...>&& t);


RSTL_NAMESPACE_END

#endif //RSTL_TUPLE_FWD_DECLS_H
