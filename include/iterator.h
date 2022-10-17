#ifndef RSTL_ITERATOR_H
#define RSTL_ITERATOR_H

#include <stddef.h>

#include "../internal/type_void_t.h"

#pragma once

RSTL_NAMESPACE_BEGIN

enum iterator_status_flag
{
	isf_none = 0x00,
	isf_valid = 0x01,
	isf_current = 0x02,
	isf_con_dereference = 0x04
};

struct input_iterator_tag
{

};

struct output_iterator_tag
{

};

struct forward_iterator_tag : public input_iterator_tag
{

};
struct bidirectional_iterator_tag : public forward_iterator_tag
{

};

struct random_access_iterator_tag : public bidirectional_iterator_tag
{

};

struct contiguous_iterator_tag    : public random_access_iterator_tag
{

};

template <typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*, typename Reference = T&>
struct iterator
{
	using iterator_category = Category;
	using value_type = T;
	using difference_type = Distance;
	using pointer = Pointer;
	using reference = Reference ;
};

namespace internal
{
	template <typename Iterator, typename = void>
	struct default_iterator_traits
	{

	};

	template <typename Iterator>
	struct default_iterator_traits<Iterator, void_t<typename Iterator::iterator_category, typename Iterator::value_type,
			typename Iterator::difference_type, typename Iterator::pointer, typename Iterator::reference>>
	{
		using iterator_category = typename Iterator::iterator_category;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;
	};
}

template <typename Iterator>
struct iterator_traits : internal::default_iterator_traits<Iterator>
{

};

template <typename T>
struct iterator_traits<T*>
{
	typedef random_access_iterator_tag iterator_category;     // To consider: Change this to contiguous_iterator_tag for the case that
	typedef T                                        value_type;            //              EASTL_ITC_NS is "eastl" instead of "std".
	typedef ptrdiff_t                                difference_type;
	typedef T*                                       pointer;
	typedef T&                                       reference;
};

template <typename T>
struct iterator_traits<const T*>
{
	typedef random_access_iterator_tag iterator_category;
	typedef T                                        value_type;
	typedef ptrdiff_t                                difference_type;
	typedef const T*                                 pointer;
	typedef const T&                                 reference;
};



RSTL_NAMESPACE_END

#endif //RSTL_ITERATOR_H
