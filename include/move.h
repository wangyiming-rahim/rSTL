#ifndef RSTL_MOVE_H
#define RSTL_MOVE_H

#include "config.h"
#include "type_traits.h"

#pragma once

RSTL_NAMESPACE_BEGIN

/*
 * forward
 * */

template <typename T>
constexpr T&& forward(remove_reference_t<T> &x) noexcept
{
	return static_cast<T&&>(x);
}

/*
 * move
 * */

template <typename T>
constexpr remove_reference_t<T>&& move(T &&x) noexcept
{
	return static_cast<remove_reference_t<T>&&>(x);
}


RSTL_NAMESPACE_END

#endif //RSTL_MOVE_H
