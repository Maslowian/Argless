#pragma once
#include <memory>
#include "Parser.hpp"

_ARGLESS_PRIVATE_BEGIN

class Pointer
{
	template <typename T>
	struct is_normal_pointer : std::false_type {};
	template <typename T>
	struct is_normal_pointer<T*> : std::true_type {};

	template <typename T>
	struct is_unique_pointer : std::false_type {};
	template <typename T, typename D>
	struct is_unique_pointer<std::unique_ptr<T, D>> : std::true_type {};

	template <typename T>
	struct is_shared_pointer : std::false_type {};
	template <typename T>
	struct is_shared_pointer<std::shared_ptr<T>> : std::true_type {};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_normal_pointer<T>::value || is_shared_pointer<T>::value || is_unique_pointer<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		static_assert(is_t<T>::value, "Not implemented yet");
	}
};

_ARGLESS_PRIVATE_END
