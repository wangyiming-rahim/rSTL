#ifndef RSTL_INTEGER_SEQUENCE_H
#define RSTL_INTEGER_SEQUENCE_H

#include "config.h"

#include <type_traits>

RSTL_NAMESPACE_BEGIN

template <typename T, T... Ints>
class integer_sequence
{
public:
	using value_type = T;
	static_assert(std::is_integral_v<T>, "rstl::integer_sequence can only be instantiated with an integral type");

	static constexpr size_t size() noexcept
	{
		return sizeof...(Ints);
	}
};

template <size_t... Is>
using index_sequence = integer_sequence<size_t, Is...>;

template <typename T, T N>
using make_integer_sequence = __make_integer_seq<integer_sequence, T, N>;

template <size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

template <typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

namespace internal {

	template <typename T>
	struct integer_sequence_size_helper;

	template <typename T, T... Ints>
	struct integer_sequence_size_helper<rstl::integer_sequence<T, Ints...>> : public std::integral_constant<size_t, sizeof...(Ints)>
	{

	};

	template <typename T>
	struct integer_sequence_size : public integer_sequence_size_helper<std::remove_cv_t<T>>
	{

	};

	template <typename T>
	struct index_sequence_size : public integer_sequence_size_helper<std::remove_cv_t<T>>
	{

	};

	template <typename T>
	inline constexpr size_t integer_sequence_size_v = integer_sequence_size<T>::value;

	template <typename T>
	inline constexpr size_t index_sequence_size_v = index_sequence_size<T>::value;

} // namespace internal end


RSTL_NAMESPACE_END

#endif //RSTL_INTEGER_SEQUENCE_H
