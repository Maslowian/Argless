/* Argless - https://github.com/Maslowian/Argless */

/*
The MIT License (MIT)

Copyright (c) 2024 Piotr Ginalski

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

// User defined optional macros
// ARGLESS_DO_NOT_UNDEF - If defined, all macros will not be undefined

#define ARGLESS_CORE_HPP

#define _ARGLESS_BEGIN namespace maslowian { namespace argless {
#define _ARGLESS_END } }
#define _ARGLESS maslowian::argless::

#define _ARGLESS_PRIVATE_BEGIN _ARGLESS_BEGIN namespace core {
#define _ARGLESS_PRIVATE_END _ARGLESS_END }
#define _ARGLESS_PRIVATE _ARGLESS core::

#define _ARGLESS_STATIC_ASSERT(expr, msg) static_assert(expr, msg)

#if defined(_DEBUG) || defined(DEBUG)
#define _ARGLESS_ASSERT(expr, msg) do { if (!(expr)) throw std::runtime_error(msg); } while (0)
#else
#define _ARGLESS_ASSERT(expr, msg)
#endif

#define _ARGLESS_CONVERT_CHAR(char_type, to_char_type, value) [](char_type c) constexpr -> to_char_type { return static_cast<to_char_type>(c); }(value)

#define _ARGLESS_CHAR(char_type, value) _ARGLESS_CONVERT_CHAR(char, char_type, value)

// TODO: make it constexpr
#define _ARGLESS_CONVERT_TEXT(char_type, to_char_type, value) [](std::basic_string_view<char_type> str) -> std::basic_string<to_char_type> \
{ \
	std::basic_string<to_char_type> convert_str(str.size(), _ARGLESS_CHAR(to_char_type, '\0')); \
	for (size_t i = 0; i < str.size(); i++) \
		convert_str[i] = _ARGLESS_CHAR(to_char_type, str[i]); \
	return convert_str; \
}(value)

#define _ARGLESS_TEXT(char_type, value) _ARGLESS_CONVERT_TEXT(char, char_type, value)

// C++ version detection

#ifdef _MSVC_LANG
	#define _ARGLESS_CPP_VERSION _MSVC_LANG
#else
	#define _ARGLESS_CPP_VERSION __cplusplus
#endif

#if _ARGLESS_CPP_VERSION >= 201103L
	#define _ARGLESS_HAS_CPP11 1
#else
	#define _ARGLESS_HAS_CPP11 0
#endif

#if _ARGLESS_CPP_VERSION >= 201402L
	#define _ARGLESS_HAS_CPP14 1
#else
	#define _ARGLESS_HAS_CPP14 0
#endif

#if _ARGLESS_CPP_VERSION >= 201703L
	#define _ARGLESS_HAS_CPP17 1
#else
	#define _ARGLESS_HAS_CPP17 0
#endif

#if _ARGLESS_CPP_VERSION >= 202002L
	#define _ARGLESS_HAS_CPP20 1
#else
	#define _ARGLESS_HAS_CPP20 0
#endif

#undef _ARGLESS_CPP_VERSION

// C++ version specific macros

#if _ARGLESS_HAS_CPP20
#define _ARGLESS_HAS_CHAR8_T 1
#else
#define _ARGLESS_HAS_CHAR8_T 0
#endif

#include <string>
#include <string_view>
#include <vector>

#include <vector>

_ARGLESS_BEGIN

template <typename T, typename AT = std::allocator<T>>
class RuleVector : public std::vector<T, AT>
{
public:
	RuleVector() = default;
	~RuleVector() = default;
};

_ARGLESS_END

_ARGLESS_PRIVATE_BEGIN

template <typename T>
struct is_rule_vector : std::false_type {};
template <typename T, typename A>
struct is_rule_vector<RuleVector<T, A>> : std::true_type {};

template <typename T>
struct type_under
{
	using type = T;
};
template <typename T, typename A>
struct type_under<RuleVector<T, A>>
{
	using type = T;
};

_ARGLESS_PRIVATE_END

#include <functional>
#include <optional>
#include <vector>
#include <string_view>

_ARGLESS_PRIVATE_BEGIN

template <typename CharT>
struct Args
{
	std::vector<std::basic_string_view<CharT>> args;
	size_t index = 0;
	std::optional<size_t> limit = std::nullopt;

	std::function<bool()> is_rule;
	std::function<std::optional<intmax_t>(size_t)> is_enum_value;

public:
	inline std::optional<std::basic_string_view<CharT>*> peak()
	{
		if (is_rule())
			return std::nullopt;
		return peak_rule();
	}

	inline void consume()
	{
		index++;
	}

public:
	inline std::optional<std::basic_string_view<CharT>*> peak_rule()
	{
		if (index < args.size() && (!limit.has_value() || index < limit.value()))
			return &args[index];
		return std::nullopt;
	}
};

_ARGLESS_PRIVATE_END

#include <unordered_map>
#include <string>
#include <optional>

_ARGLESS_PRIVATE_BEGIN

template <typename CharT>
struct EmptyRule
{
public:
	using Parser = std::function<std::optional<void*>(Args<CharT>&)>;
	using Deleter = std::function<void(void*)>;

public:
	Parser parser;
	size_t type_hash;
	Deleter deleter;
	bool is_rule_vector;
	std::optional<std::basic_string<CharT>> short_description;
	std::optional<std::basic_string<CharT>> description;
	std::function<std::basic_string<CharT>(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)> type_description;
};

template <typename CharT>
struct Rule : EmptyRule<CharT>
{
public:
	using Parser = typename EmptyRule<CharT>::Parser;
	using Deleter = typename EmptyRule<CharT>::Deleter;

public:
	std::vector<std::basic_string<CharT>> shortcuts;
};

template <typename CharT>
struct EnumRule
{
public:
	using EnumMap = std::unordered_map<std::basic_string<CharT>, intmax_t>;

public:
	EnumMap enum_map;
};

_ARGLESS_PRIVATE_END

#include <unordered_map>
#include <optional>
#include <string>

_ARGLESS_PRIVATE_BEGIN

template <typename CharT>
struct ArglessData
{
	std::unordered_map<std::basic_string<CharT>, _ARGLESS_PRIVATE Rule<CharT>> rules; 
	std::unordered_map<size_t, _ARGLESS_PRIVATE EnumRule<CharT>> enum_rules;
	std::optional<_ARGLESS_PRIVATE EmptyRule<CharT>> empty_rule;
};

_ARGLESS_PRIVATE_END

#include <algorithm>
#include <locale>

#include <string>
#include <string_view>
#include <type_traits>
#include <stdexcept>

_ARGLESS_PRIVATE_BEGIN

template <typename T, typename CharT>
T stot(std::basic_string_view<CharT> str)
{
	if constexpr (std::is_same_v<T, float>)
		return static_cast<T>(std::stof(std::string(str)));
	else if constexpr (std::is_same_v<T, double>)
		return static_cast<T>(std::stod(std::string(str)));
	else if constexpr (std::is_same_v<T, long double>)
		return static_cast<T>(std::stold(std::string(str)));
	else if constexpr (std::is_integral_v<T>)
	{
		if constexpr (std::is_signed_v<T>)
			return static_cast<T>(std::stoll(std::string(str)));
		else
			return static_cast<T>(std::stoull(std::string(str)));
	}

	throw std::invalid_argument("invalid stot argument");
}

_ARGLESS_PRIVATE_END

_ARGLESS_PRIVATE_BEGIN

enum class ArgType
{
	Boolean,
	Integer,
	FloatingPoint,
	String,
	Character,
	Enum,
	Variant,
	Path,
};

_ARGLESS_PRIVATE_END

_ARGLESS_PRIVATE_BEGIN

class FundamentalType
{
	template <typename T>
	struct is_character
	{
		static constexpr inline bool value = std::is_same_v<T, char> || std::is_same_v<T, wchar_t> ||
#if _ARGLESS_HAS_CHAR8_T
			std::is_same_v<T, char8_t> ||
#endif
			std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>;
	};
	template <typename T>
	struct is_integer
	{
		static constexpr inline bool value = std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, short> || std::is_same_v<T, unsigned short> || std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_same_v<T, long> || std::is_same_v<T, unsigned long> || std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long>;
	};
	template <typename T>
	struct is_floating_point
	{
		static constexpr inline bool value = std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>;
	};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = std::is_same_v<T, bool> || is_character<T>::value || is_integer<T>::value || is_floating_point<T>::value;
	};

public:	
	template <typename CharT, typename T, std::enable_if_t<std::is_same<T, bool>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Boolean;
		auto& value = *arg.value();

		std::basic_string<CharT> value_lower(value.size(), 0);
		std::locale loc;
		std::transform(value.begin(), value.end(), value_lower.begin(), [&loc](CharT c) { return std::tolower(c, loc); });

		bool result;

		if (value_lower == _ARGLESS_TEXT(CharT, "true"))
			result = true;
		else if (value_lower == _ARGLESS_TEXT(CharT, "false"))
			result = false;
		else
			throw ArgType::Boolean;

		args.consume();
		return result;
	}
	template <typename CharT, typename T, std::enable_if_t<is_character<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Character;
		auto& value = *arg.value();

		if (value.size() != 1)
			throw ArgType::Character;

		T result = _ARGLESS_CONVERT_CHAR(CharT, T, value[0]);

		args.consume();
		return result;
	}
	template <typename CharT, typename T, std::enable_if_t<is_integer<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Integer;
		auto& value = *arg.value();

		T result;
		try
		{
			result = stot<T>(value);
		}
		catch (...)
		{
			throw ArgType::Integer;
		}

		args.consume();
		return result;
	}
	template <typename CharT, typename T, std::enable_if_t<is_floating_point<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::FloatingPoint;
		auto& value = *arg.value();

		T result;
		try
		{
			result = stot<T>(value);
		}
		catch (...)
		{
			throw ArgType::FloatingPoint;
		}

		args.consume();
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<std::is_same<T, bool>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Boolean");
	}
	template <typename T, typename CharT, std::enable_if_t<is_character<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Character");
	}
	template <typename T, typename CharT, std::enable_if_t<is_integer<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Integer");
	}
	template <typename T, typename CharT, std::enable_if_t<is_floating_point<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Float");
	}
};

_ARGLESS_PRIVATE_END

#include <array>

#include <functional>
#include <string>

_ARGLESS_PRIVATE_BEGIN

template <typename CharT, typename T>
constexpr std::function<T(Args<CharT>&)> get_parser();

template <typename T, typename CharT>
std::basic_string<CharT> get_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info);

_ARGLESS_PRIVATE_END

_ARGLESS_PRIVATE_BEGIN

class Array
{
	template <typename T>
	struct is_array : std::false_type {};
	template <typename T, size_t N>
	struct is_array<std::array<T, N>> : std::true_type {};

	template <typename T>
	struct array_info;
	template <typename T, size_t N>
	struct array_info<std::array<T, N>>
	{
		using value_type = T;
		static constexpr size_t size = N;
	};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_array<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		T result;

		for (size_t i = 0; i < result.size(); i++)
		{
			auto value = get_parser<CharT, typename T::value_type>()(args);
			result[i] = value;
		}

		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '<') + get_type_description<typename T::value_type, CharT>(enum_info) + _ARGLESS_TEXT(CharT, ">[") + std::to_string(array_info<T>::size) + _ARGLESS_CHAR(CharT, ']');
	}
};

_ARGLESS_PRIVATE_END

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <unordered_set>

_ARGLESS_PRIVATE_BEGIN

class Optional
{
	template <typename T>
	struct is_optional : std::false_type {};
	template <typename T>
	struct is_optional<std::optional<T>> : std::true_type {};

	friend class DynamicArray;

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_optional<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		try
		{
			return get_parser<CharT, typename T::value_type>()(args);
		}
		catch (ArgType)
		{
			return std::nullopt;
		}
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '<') + get_type_description<typename T::value_type, CharT>(enum_info) + _ARGLESS_TEXT(CharT, ">?");
	}
};

_ARGLESS_PRIVATE_END

_ARGLESS_PRIVATE_BEGIN

class DynamicArray
{
	template <typename T>
	struct is_vector : std::false_type {};
	template <typename T>
	struct is_vector<std::vector<T>> : std::true_type {};

	template <typename T>
	struct is_deque : std::false_type {};
	template <typename T>
	struct is_deque<std::deque<T>> : std::true_type {};

	template <typename T>
	struct is_list : std::false_type {};
	template <typename T>
	struct is_list<std::list<T>> : std::true_type {};

	template <typename T>
	struct is_set : std::false_type {};
	template <typename T>
	struct is_set<std::set<T>> : std::true_type {};

	template <typename T>
	struct is_unordered_set : std::false_type {};
	template <typename T>
	struct is_unordered_set<std::unordered_set<T>> : std::true_type {};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_vector<T>::value || is_deque<T>::value || is_list<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		T result;

		while (true)
		{
			try
			{
				auto value = get_parser<CharT, typename T::value_type>()(args);

				if constexpr (Optional::is_optional<typename T::value_type>::value)
					if (!value.has_value())
						break;

				result.push_back(value);
			} 
			catch (ArgType) 
			{
				break;
			}
		}

		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '<') + get_type_description<typename T::value_type, CharT>(enum_info) + _ARGLESS_TEXT(CharT, ">[...]");
	}
};

_ARGLESS_PRIVATE_END

#include <tuple>

_ARGLESS_PRIVATE_BEGIN

class Tuple
{
	template <typename T>
	struct is_filled_tuple : std::false_type {};
	template <typename T, typename ...Rest>
	struct is_filled_tuple<std::tuple<T, Rest...>> : std::true_type {};
	
	template <typename T>
	struct is_empty_tuple : std::false_type {};
	template <>
	struct is_empty_tuple<std::tuple<>> : std::true_type {};

	template <typename T>
	struct is_single_tuple : std::false_type {};
	template <typename T>
	struct is_single_tuple<std::tuple<T>> : std::true_type {};

	template <typename T>
	struct is_tail_tuple
	{
		static constexpr inline bool value = is_filled_tuple<T>::value && !is_single_tuple<T>::value;
	};

	template <typename T>
	struct is_tuple : std::false_type {};
	template <typename ...Args>
	struct is_tuple<std::tuple<Args...>> : std::true_type {};

	template <typename T>
	struct tuple_info
	{
		_ARGLESS_STATIC_ASSERT(!is_empty_tuple<T>::value, "Cannot get tuple info from empty tuple");
	};
	template <typename T, typename ...Args>
	struct tuple_info<std::tuple<T, Args...>>
	{
		using type = T;
		using tail = std::tuple<Args...>;
	};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_tuple<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_tail_tuple<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto start_index = args.index;
		auto orginal_limit = args.limit;
		std::optional<size_t> limit = orginal_limit;
		typename Tuple::tuple_info<T>::type value;
		std::optional<typename Tuple::tuple_info<T>::tail> rest;

		std::optional<std::pair<ArgType, size_t>> e_before;

		do
		{
			args.index = start_index;
			args.limit = limit;

			try
			{
				value = get_parser<CharT, typename Tuple::tuple_info<T>::type>()(args);
			}
			catch (ArgType e)
			{
				if (e_before.has_value())
				{
					args.index = e_before.value().second;
					throw e_before.value().first;
				}
				else
					throw e;
			}

			if (!limit.has_value())
				limit = args.index;

			args.limit = orginal_limit;
			try
			{
				rest = get_parser<CharT, typename Tuple::tuple_info<T>::tail>()(args);
			}
			catch (ArgType e)
			{
				if (limit.value() <= start_index)
					throw e;
				limit.value()--;

				e_before = std::make_pair(e, args.index);
			}
			if (rest.has_value())
				break;
		} 
		while (true);

		args.limit = orginal_limit;

		T result = std::tuple_cat(std::make_tuple(value), rest.value());
		return result;
	}
	template <typename CharT, typename T, std::enable_if_t<is_single_tuple<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto value = get_parser<CharT, typename Tuple::tuple_info<T>::type>()(args);
		T result = std::make_tuple(value);
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '<') + get_simple_type_description<T, CharT>(enum_info) + _ARGLESS_CHAR(CharT, '>');
	}

private:
	template <typename T, typename CharT, std::enable_if_t<is_tail_tuple<T>::value, int> = 0>
	static std::basic_string<CharT> get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return get_type_description<typename Tuple::tuple_info<T>::type, CharT>(enum_info) + _ARGLESS_TEXT(CharT, ", ") + get_simple_type_description<typename Tuple::tuple_info<T>::tail, CharT>(enum_info);
	}

	template <typename T, typename CharT, std::enable_if_t<is_single_tuple<T>::value, int> = 0>
	static std::basic_string<CharT> get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return get_type_description<typename Tuple::tuple_info<T>::type, CharT>(enum_info);
	}

	template <typename T, typename CharT, std::enable_if_t<!is_t<T>::value, int> = 0>
	static std::string get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return get_type_description<T, CharT>(enum_info);
	}
};

_ARGLESS_PRIVATE_END

_ARGLESS_PRIVATE_BEGIN

class Enum
{
public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = std::is_enum_v<T>;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Enum;

		auto value = args.is_enum_value(typeid(T).hash_code());
		if (!value.has_value())
			throw ArgType::Enum;

		T result = static_cast<T>(value.value());

		args.consume();
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0> 
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		auto values = enum_info(typeid(T).hash_code());

		std::basic_string<CharT> result;
		result += _ARGLESS_CHAR(CharT, '[');
		for (size_t i = 0; i < values.size(); i++)
		{
			if (i != 0)
				result += _ARGLESS_CHAR(CharT, '|');

			result += _ARGLESS_CHAR(CharT, '\'');
			result += values[i];
			result += _ARGLESS_CHAR(CharT, '\'');
		}
		result += _ARGLESS_CHAR(CharT, ']');

		return result;
	}
};

_ARGLESS_PRIVATE_END

#include <string>
#include <algorithm>
#include <iterator>

_ARGLESS_PRIVATE_BEGIN

class String
{
	template <typename T>
	struct is_string : std::false_type {};
	template <typename T>
	struct is_string<std::basic_string<T>> : std::true_type {};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_string<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::String;
		auto& value = *arg.value();

		T result;
		std::transform(value.begin(), value.end(), std::back_inserter(result), [](CharT c) -> typename T::value_type { return _ARGLESS_CONVERT_CHAR(CharT, typename T::value_type, c); });

		args.consume();
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Word");
	}
};

_ARGLESS_PRIVATE_END

#include <string>
#include <algorithm>
#include <iterator>

#include <string>

_ARGLESS_BEGIN

template <typename CT, typename T = std::char_traits<CT>, typename AT = std::allocator<CT>> 
class RuleBasicString : public std::basic_string<CT, T, AT>
{
public:
	using non_rule_basic_string = std::basic_string<CT, T, AT>;

	RuleBasicString() = default;
	~RuleBasicString() = default;
};

_ARGLESS_END

_ARGLESS_PRIVATE_BEGIN

template <typename T>
struct is_rule_basic_string : std::false_type {};
template <typename CT, typename T, typename AT>
struct is_rule_basic_string<RuleBasicString<CT, T, AT>> : std::true_type {};

_ARGLESS_PRIVATE_END

_ARGLESS_PRIVATE_BEGIN

class RuleString
{
public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_rule_basic_string<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak_rule();
		if (!arg.has_value())
			throw ArgType::String;
		auto& value = *arg.value();

		T result;
		std::transform(value.begin(), value.end(), std::back_inserter(result), [](CharT c) -> typename T::value_type { return _ARGLESS_CONVERT_CHAR(CharT, typename T::value_type, c); });

		args.consume();
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return String::get_description<typename T::non_rule_basic_string, CharT>(enum_info);
	}
};

_ARGLESS_PRIVATE_END

#include <variant>

_ARGLESS_PRIVATE_BEGIN

class Variant
{
	template <typename T>
	struct is_variant : std::false_type {};
	template <typename ...Args>
	struct is_variant<std::variant<Args...>> : std::true_type {};

	template <typename T>
	struct is_filled_variant : std::false_type {};
	template <typename T, typename ...Args>
	struct is_filled_variant<std::variant<T, Args...>> : std::true_type {};

	template <typename T>
	struct is_empty_variant : std::false_type {};
	template <>
	struct is_empty_variant<std::variant<>> : std::true_type {};

	template <typename T>
	struct is_monostate : std::false_type {};
	template <>
	struct is_monostate<std::monostate> : std::true_type {};

	template <typename T>
	struct variant_info
	{
		_ARGLESS_STATIC_ASSERT(!is_empty_variant<T>::value, "Cannot get variant info from empty variant");
	};
	template <typename T, typename ...Args>
	struct variant_info<std::variant<T, Args...>>
	{
		using type = T;
		using tail = std::variant<Args...>;
	};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_variant<T>::value || is_monostate<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_variant<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		return VariantParser<CharT, T, std::variant_size_v<T>>::get(args);
	}

	template <typename CharT, typename T, std::enable_if_t<is_monostate<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		return T{};
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value && !is_monostate<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '[') + VariantDescription<CharT, T, std::variant_size_v<T>>::get_simple_type_description(enum_info) + _ARGLESS_CHAR(CharT, ']');
	}

	template <typename T, typename CharT, std::enable_if_t<is_monostate<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "None");
	}

private:
	template <typename CharT, typename T, size_t I>
	struct VariantParser
	{
		static T get(Args<CharT>& args)
		{
			auto index = args.index;
			try 
			{
				return get_parser<CharT, std::variant_alternative_t<I - 1, T>>()(args);
			}
			catch (ArgType e)
			{
				args.index = index;
				return VariantParser<CharT, T, I - 1>::get(args);
			}
		}

	};

	template <typename CharT, typename T>
	struct VariantParser<CharT, T, 0>
	{
		static T get(Args<CharT>& args)
		{
			throw ArgType::Variant;
		}
	};

private:
	template <typename CharT, typename T, size_t I>
	struct VariantDescription
	{
		static std::basic_string<CharT> get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
		{
			return get_type_description<std::variant_alternative_t<I - 1, T>, CharT>(enum_info) + _ARGLESS_CHAR(CharT, '|') + VariantDescription<CharT, T, I - 1>::get_simple_type_description(enum_info);
		}
	};

	template <typename CharT, typename T>
	struct VariantDescription<CharT, T, 1>
	{
		static std::basic_string<CharT> get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
		{
			return get_type_description<std::variant_alternative_t<0, T>, CharT>(enum_info);
		}
	};
};

_ARGLESS_PRIVATE_END

#include <filesystem>
#include <string>
#include <algorithm>
#include <iterator>

_ARGLESS_PRIVATE_BEGIN

class Path
{
	template <typename T>
	struct is_path : std::false_type {};
	template <>
	struct is_path<std::filesystem::path> : std::true_type {};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_path<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Path;
		auto& value = *arg.value();

		typename T::string_type result;
		std::transform(value.begin(), value.end(), std::back_inserter(result), [](CharT c) -> typename T::value_type { return _ARGLESS_CONVERT_CHAR(CharT, typename T::value_type, c); });

		args.consume();
		return T(result);
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Path");
	}
};

_ARGLESS_PRIVATE_END

_ARGLESS_PRIVATE_BEGIN

#define _ARGLESS_PARSERS \
	FundamentalType, \
	Array, \
	DynamicArray, \
	Tuple, \
	Enum, \
	String, \
	Optional, \
	Variant, \
	RuleString, \
	Path

template <typename... Types>
struct GetSize 
{
	static constexpr inline size_t value = std::tuple_size_v<std::tuple<Types...>>;
};

template <size_t I, typename... Types>
struct GetType 
{
	using type = std::tuple_element_t<I, std::tuple<Types...>>;
};

template <size_t I, typename T, bool Succeed, typename... Parsers>
struct GetParser
{
private:
	using CurrentParser = typename GetType<I - 1, Parsers...>::type;
	static constexpr inline bool Success = CurrentParser::template is_t<T>::value;
	using NextParser = typename GetParser<I - 1, T, Success || Succeed, Parsers...>::type;
public:
	using type = std::conditional_t<Success, CurrentParser, NextParser>;
};

template <typename T, bool Succeed, typename... Parsers>
struct GetParser<0, T, Succeed, Parsers...>
{
	_ARGLESS_STATIC_ASSERT(Succeed, "Type not supported");
	using type = void;
};

template <typename CharT, typename T>
constexpr std::function<T(Args<CharT>&)> get_parser()
{
	return std::function<T(Args<CharT>&)>(&GetParser<GetSize<_ARGLESS_PARSERS>::value, T, false, _ARGLESS_PARSERS>::type::template get<CharT, T>);
}

template <typename T, typename CharT>
std::basic_string<CharT> get_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
{
	return GetParser<GetSize<_ARGLESS_PARSERS>::value, T, false, _ARGLESS_PARSERS>::type::template get_description<T, CharT>(enum_info);
}

_ARGLESS_PRIVATE_END

#include <string>
#include <optional>
#include <unordered_map>
#include <memory>
#include <vector>

_ARGLESS_PRIVATE_BEGIN

struct ResultValue
{
	ResultValue (size_t type_hash) : type_hash(type_hash)
	{}

	size_t type_hash;
	std::vector<std::shared_ptr<void>> values;
};

_ARGLESS_PRIVATE_END

_ARGLESS_BEGIN

template <typename CharT>
class BasicArgless;

template <typename CharT>
class BasicResult
{
	using string = std::basic_string<CharT>;

private:
	BasicResult() = default;

public:
	template <typename T>
	std::shared_ptr<T> get(const string& name) const
	{
		auto value = values.find(name);
		
		if (value == values.end())
			return nullptr;

		return get<T>(value->second);
	}

	template <typename T>
	std::shared_ptr<T> get(std::nullptr_t) const
	{
		if (!empty_rule_value.has_value())
			return nullptr;

		return get<T>(empty_rule_value.value());
	}

public:
	size_t get(const string& name) const
	{
		auto value = values.find(name);
		if (value == values.end())
			return 0;
		
		return value->second.values.size();
	}

	size_t get(std::nullptr_t) const
	{
		if (!empty_rule_value.has_value())
			return 0;

		return empty_rule_value.value().values.size();
	}

public:
	string path() const
	{
		return executable_path;
	}

private:
	template <typename T>
	std::shared_ptr<T> get(const _ARGLESS_PRIVATE ResultValue& result_value) const
	{
		_ARGLESS_ASSERT(result_value.type_hash == typeid(T).hash_code(), "Type mismatch");

		if constexpr (_ARGLESS_PRIVATE is_rule_vector<T>::value)
		{
			T result;
			for (size_t i = 0; i < result_value.values.size(); i++)
				result.push_back(*reinterpret_cast<typename _ARGLESS_PRIVATE type_under<T>::type*>(result_value.values[i].get()));

			auto test = std::shared_ptr<T>(new T(result));
			return test;
		}
		else
		{
			if (result_value.values.empty())
				return nullptr;
			return std::static_pointer_cast<T>(result_value.values[0]);
		}
	}

private:
	std::unordered_map<string, _ARGLESS_PRIVATE ResultValue> values;
	std::optional<_ARGLESS_PRIVATE ResultValue> empty_rule_value;
	string executable_path;

private:
	friend class BasicArgless<CharT>;
};

using Result = BasicResult<char>;
using WResult = BasicResult<wchar_t>;

_ARGLESS_END

#include <string>
#include <exception>
#include <variant>

_ARGLESS_PRIVATE_BEGIN

enum class ArglessExceptionType
{
	WrongArgument,
	StrayArgument,
	ValueDuplication,
};

struct NoRule {};

template <typename CharT>
struct ArglessException : public std::exception
{
	ArglessException(ArglessExceptionType type, std::variant<std::monostate, std::basic_string<CharT>, NoRule> rule_name, Args<CharT> args, const ArglessData<CharT>& data, std::optional<ArgType> arg_type = std::nullopt)
		: type(type), rule_name(std::move(rule_name)), args(std::move(args)), data(data), arg_type(arg_type)
	{}

	const char* what() const noexcept override
	{
		switch (type)
		{
		case ArglessExceptionType::WrongArgument:
			return "Wrong argument";
		case ArglessExceptionType::StrayArgument:
			return "Stray argument";
		case ArglessExceptionType::ValueDuplication:
			return "Value duplication";
		}

		return "Unknown exception";
	}
	
	ArglessExceptionType type;
	std::variant<std::monostate, std::basic_string<CharT>, NoRule> rule_name;
	Args<CharT> args;
	const ArglessData<CharT>& data;
	std::optional<ArgType> arg_type;
};

_ARGLESS_PRIVATE_END

#include <iostream>
#include <variant>
#include <filesystem>

_ARGLESS_PRIVATE_BEGIN

template <typename CharT>
class Helper
{
public:
	Helper() = default;
	virtual ~Helper() = default;

public:
	virtual void help(const ArglessException<CharT>& e) = 0;
	virtual void info(const ArglessData<CharT>& data) = 0;
	virtual void info(const std::basic_string<CharT>& name, const ArglessData<CharT>& data) = 0;
};

template <typename T>
struct OutStream
{
	static inline std::ostream& out = std::cout;
};
template <>
struct OutStream<wchar_t>
{
	static inline std::wostream& out = std::wcout;
};

template <typename CharT, typename OutT = OutStream<CharT>>
class DefaultHelper : public Helper<CharT>
{
	static inline auto& out = OutT::out;

public:
	DefaultHelper(const std::optional<std::basic_string<CharT>>& description) : Helper<CharT>(), m_description(description)
	{}

public:
	void help(const ArglessException<CharT>& e) override
	{
		error_description(e);
		hint(e);
		where(e);
	}

	void info(const ArglessData<CharT>& data) override
	{
		if (m_description.has_value())
			out << m_description.value() << std::endl << std::endl;

		out << _ARGLESS_TEXT(CharT, "Options:") << std::endl;

		for (const auto& [name, rule] : data.rules)
		{
			out << _ARGLESS_TEXT(CharT, "\t");

			out << name;

			if (rule.short_description.has_value())
			{
				out << _ARGLESS_TEXT(CharT, " - ");
				out << rule.short_description.value();
			}

			out << std::endl;
		}
	}

	void info(const std::basic_string<CharT>& name, const ArglessData<CharT>& data) override
	{
		auto rule = data.rules.find(name);
		if (rule == data.rules.end())
			for (const auto& [name_, rule_] : data.rules)
				if (std::find(rule_.shortcuts.begin(), rule_.shortcuts.end(), name) != rule_.shortcuts.end())
				{
					rule = data.rules.find(name_);
					break;
				}
		if (rule == data.rules.end())
		{
			out << _ARGLESS_TEXT(CharT, "Option '") << name << _ARGLESS_TEXT(CharT, "' not found") << std::endl;
			return;
		}

		out << _ARGLESS_TEXT(CharT, "Option: ") << rule->first << std::endl;

		auto enum_info = [&data](size_t enum_hash)-> std::vector<std::basic_string<CharT>> 
		{ 
			auto it = data.enum_rules.find(enum_hash);

			_ARGLESS_ASSERT(it != data.enum_rules.end(), "Enum not found");

			auto& enum_map = it->second.enum_map;

			std::vector<std::basic_string<CharT>> result;

			for (const auto& [name, value] : enum_map)
				result.push_back(name);

			return result;
		};

		out << _ARGLESS_TEXT(CharT, "Value type: ") << rule->second.type_description(enum_info) << std::endl;

		if (rule->second.description.has_value())
			out << _ARGLESS_TEXT(CharT, "Description: ") << rule->second.description.value() << std::endl;
		else if (rule->second.short_description.has_value())
			out << _ARGLESS_TEXT(CharT, "Description: ") << rule->second.short_description.value() << std::endl;

		if (rule->second.shortcuts.size() > 0)
		{
			out << _ARGLESS_TEXT(CharT, "Shortcuts: ");

			for (const auto& shortcut : rule->second.shortcuts)
			{
				if (shortcut == rule->first)
					continue;

				if (shortcut != rule->second.shortcuts.front())
					out << _ARGLESS_TEXT(CharT, ", ");

				out << shortcut;
			}
		}

		out << std::endl;
	}

protected:
	std::optional<std::basic_string<CharT>> m_description;

protected:
	void error_description(const ArglessException<CharT>& e) const
	{
		if (e.type == ArglessExceptionType::WrongArgument)
		{
			if (e.arg_type.has_value() && e.arg_type.value() == ArgType::Variant)
			{
				out << _ARGLESS_TEXT(CharT, "Wrong argument: ");

				out << _ARGLESS_TEXT(CharT, "Given arguments do not match any of the expected types");
			}
			else
			{
				if (e.args.index >= e.args.args.size())
					out << _ARGLESS_TEXT(CharT, "Missing argument: ");
				else
					out << _ARGLESS_TEXT(CharT, "Wrong argument: ");

				out << _ARGLESS_TEXT(CharT, "Expected argument of type '");

				if (e.arg_type.has_value())
					out << get_arg_type_name(e.arg_type.value());
				else
					out << _ARGLESS_TEXT(CharT, "Unknown type");

				out << _ARGLESS_TEXT(CharT, "'");
			}
		}
		else if (e.type == ArglessExceptionType::StrayArgument)
		{
			out << _ARGLESS_TEXT(CharT, "Unexpected argument: Argument does not belong to any option");
		}
		else if	(e.type == ArglessExceptionType::ValueDuplication)
		{
			out << _ARGLESS_TEXT(CharT, "Value duplication: ");

			if (auto value = std::get_if<std::basic_string<CharT>>(&e.rule_name))
				out << _ARGLESS_TEXT(CharT, "Option '") << *value << '\'';
			else if (std::holds_alternative<NoRule>(e.rule_name))
				out << _ARGLESS_TEXT(CharT, "No option specified");

			out << _ARGLESS_TEXT(CharT, " does not allow being specified more than once");
		}

		out	<< std::endl;
	}

	std::basic_string<CharT> get_arg_type_name(ArgType arg_type) const
	{
		std::function<std::vector<std::basic_string<CharT>>(size_t)> false_func;
		switch (arg_type)
		{
		case ArgType::Boolean:
			return get_type_description<bool, CharT>(false_func);
		case ArgType::Integer:
			return get_type_description<int, CharT>(false_func);
		case ArgType::FloatingPoint:
			return get_type_description<float, CharT>(false_func);
		case ArgType::String:
			return get_type_description<std::basic_string<CharT>, CharT>(false_func);
		case ArgType::Character:
			return get_type_description<CharT, CharT>(false_func);
		case ArgType::Enum:
			return "Option from the list";
		case ArgType::Variant:
			return get_type_description<bool, CharT>(false_func);
		case ArgType::Path:
			return get_type_description<std::filesystem::path, CharT>(false_func);
		}
	}

	void hint(const ArglessException<CharT>& e) const
	{
		if (!(e.type == ArglessExceptionType::StrayArgument))
			return;

		out << _ARGLESS_TEXT(CharT, "Hint: ");

		if (e.type == ArglessExceptionType::StrayArgument)
			out << _ARGLESS_TEXT(CharT, "Check expected arguments for the specified option");

		out << std::endl;
	}

	void where(const ArglessException<CharT>& e) const
	{
		size_t target_index = e.args.index;

		if (target_index > 0 && e.type == ArglessExceptionType::ValueDuplication)
			target_index--;

		size_t space = 0;

		for (size_t i = 0; i < e.args.args.size(); i++)
		{
			if (i != 0)
			{
				out << _ARGLESS_TEXT(CharT, " ");

				if (i <= target_index)
					space++;
			}

			out << e.args.args[i];

			if (i < target_index)
				space += e.args.args[i].size();
		}
		out << std::endl;

		bool is_arg = target_index < e.args.args.size() && !(e.type == ArglessExceptionType::ValueDuplication && std::holds_alternative<NoRule>(e.rule_name));
		size_t arg_size = 0;
		if (is_arg)
		{
			arg_size = e.args.args[target_index].size();
		}

		if (!is_arg)
			space++;

		for (size_t i = 0; i < space; i++)
			out << _ARGLESS_TEXT(CharT, " ");

		if (!is_arg)
			out << _ARGLESS_TEXT(CharT, "^");
		else
			for (size_t i = 0; i < arg_size; i++)
				out << _ARGLESS_TEXT(CharT, "~");

		out << std::endl;
	}
};

_ARGLESS_PRIVATE_END

_ARGLESS_BEGIN

template <typename CharT>
class BasicArgless
{
	using string_view = std::basic_string_view<CharT>;
	using string = std::basic_string<CharT>;

	struct void_t {};

public:
	template <typename OutT = _ARGLESS_PRIVATE OutStream<CharT>>
	constexpr BasicArgless()
		: m_helper(new _ARGLESS_PRIVATE DefaultHelper<CharT, OutT>(std::nullopt))
	{}

	template <typename OutT = _ARGLESS_PRIVATE OutStream<CharT>>
	constexpr BasicArgless(const string& description)
		: m_helper(new _ARGLESS_PRIVATE DefaultHelper<CharT, OutT>(description))
	{}

	constexpr BasicArgless(std::unique_ptr<_ARGLESS_PRIVATE Helper<CharT>> helper)
		: m_helper(std::move(helper))
	{}

public:
	template <typename T>
	constexpr void set_enum_map(const std::unordered_map<string, T>& map)
	{
		_ARGLESS_STATIC_ASSERT(std::is_enum_v<T>, "T must be an enum type");

		typename _ARGLESS_PRIVATE EnumRule<CharT>::EnumMap enum_map;
		for (const auto& [key, value] : map)
			enum_map[key] = static_cast<intmax_t>(value);

		m_data.enum_rules.emplace(typeid(T).hash_code(), _ARGLESS_PRIVATE EnumRule<CharT>{ .enum_map = enum_map });
	}
	
	template <typename T>
	constexpr void set_rule(const string& name, const std::vector<string>& shortcuts, const std::optional<string>& short_description = {}, const std::optional<string>& description = {})
	{
		_ARGLESS_PRIVATE Rule<CharT> rule;

		rule.parser = typename _ARGLESS_PRIVATE Rule<CharT>::Parser(&parser_caller<typename _ARGLESS_PRIVATE type_under<T>::type>);
		rule.type_hash = typeid(T).hash_code();
		rule.deleter = &deleter<typename _ARGLESS_PRIVATE type_under<T>::type>;
		rule.is_rule_vector = _ARGLESS_PRIVATE is_rule_vector<T>::value;
		rule.type_description = std::function<std::basic_string<CharT>(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)>(&type_description<typename _ARGLESS_PRIVATE type_under<T>::type>);
		rule.short_description = short_description;
		rule.description = description;

		rule.shortcuts = shortcuts;

		m_data.rules.emplace(name, rule);
	}

	template <typename T>
	constexpr void set_rule(std::nullptr_t, const std::optional<string>& short_description = {}, const std::optional<string>& description = {})
	{
		_ARGLESS_PRIVATE EmptyRule<CharT> rule;

		rule.parser = typename _ARGLESS_PRIVATE Rule<CharT>::Parser(&parser_caller<typename _ARGLESS_PRIVATE type_under<T>::type>);
		rule.type_hash = typeid(T).hash_code();
		rule.deleter = &deleter<typename _ARGLESS_PRIVATE type_under<T>::type>;
		rule.is_rule_vector = _ARGLESS_PRIVATE is_rule_vector<T>::value;
		rule.type_description = std::function<std::basic_string<CharT>(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)>(&type_description<typename _ARGLESS_PRIVATE type_under<T>::type>);
		rule.short_description = short_description;
		rule.description = description;

		m_data.empty_rule = rule;
	}

public:
	template <typename T>
	constexpr void remove_enum_map()
	{
		_ARGLESS_STATIC_ASSERT(std::is_enum_v<T>, "T must be an enum type");

		m_data.enum_rules.erase(typeid(T).hash_code());
	}

	constexpr void remove_rule(const string& name)
	{
		m_data.rules.erase(name);
	}

	constexpr void remove_rule(std::nullptr_t)
	{
		m_data.empty_rule = std::nullopt;
	}

public:
	constexpr void set_helper(std::unique_ptr<_ARGLESS_PRIVATE Helper<CharT>> helper)
	{
		m_helper = std::move(helper);
	}

	void help()
	{
		_ARGLESS_ASSERT(m_helper, "Helper is not set");
		m_helper->info(m_data);
	}

	void help(const string& name)
	{
		_ARGLESS_ASSERT(m_helper, "Helper is not set");
		m_helper->info(name, m_data);
	}

public:
	std::optional<Result> parse(int argc, const CharT* const* argv) const 
	{
		_ARGLESS_PRIVATE Args<CharT> args;
		std::vector<string_view> args_vector;
		args_vector.reserve(argc);
		for (int i = 0; i < argc; i++)
			args_vector.push_back(argv[i]);
		args.args = args_vector;
		args.is_rule = std::bind(&BasicArgless::is_rule, this, &args);
		args.is_enum_value = std::bind(&BasicArgless::is_enum_value, this, &args, std::placeholders::_1);

		Result result;

		if (args.index < argc)
		{
			result.executable_path = args.args[args.index];
			args.consume();
		}

		try
		{
			while (args.index < argc)
			{
				const _ARGLESS_PRIVATE EmptyRule<CharT>* rule;
				std::variant<std::monostate, string, _ARGLESS_PRIVATE NoRule> rule_name;
				std::function<void(std::shared_ptr<void>)> add_value;
				std::function<size_t()> rule_value_size;

				auto named_rule = find_rule(string(args.args[args.index]));
				if (named_rule != m_data.rules.end())
				{
					args.consume();

					rule = &named_rule->second;
					rule_name = named_rule->first;

					add_value = [&result, &named_rule](std::shared_ptr<void> value)
					{
						auto result_value = result.values.find(named_rule->first);
						if (result_value == result.values.end())
						{
							result.values.insert(std::make_pair(named_rule->first, _ARGLESS_PRIVATE ResultValue(named_rule->second.type_hash)));
							result_value = result.values.find(named_rule->first);
						}
						result_value->second.values.push_back(value);
					};

					rule_value_size = [&result, &named_rule]()
					{
						auto result_value = result.values.find(named_rule->first);
						if (result_value == result.values.end())
							return size_t(0);
						return result_value->second.values.size();
					};
				}
				else
				{
					if (!m_data.empty_rule.has_value())
						throw _ARGLESS_PRIVATE ArglessException<CharT>(_ARGLESS_PRIVATE ArglessExceptionType::StrayArgument, std::monostate(), args, m_data);

					rule = &m_data.empty_rule.value();
					rule_name = _ARGLESS_PRIVATE NoRule();
					add_value = [this, &result](std::shared_ptr<void> value)
					{
						if (!result.empty_rule_value.has_value())
							result.empty_rule_value = _ARGLESS_PRIVATE ResultValue(this->m_data.empty_rule.value().type_hash);
						result.empty_rule_value->values.push_back(value);
					};

					rule_value_size = [&result]()
					{
						if (!result.empty_rule_value.has_value())
							return size_t(0);
						return result.empty_rule_value->values.size();
					};
				}

				if (!rule->is_rule_vector && rule_value_size() > 0)
					throw _ARGLESS_PRIVATE ArglessException<CharT>(_ARGLESS_PRIVATE ArglessExceptionType::ValueDuplication, rule_name, args, m_data);

				try 
				{
					auto value = rule->parser(args);
					add_value(std::shared_ptr<void>(value.value(), rule->deleter));
				} 
				catch (_ARGLESS_PRIVATE ArgType e)
				{
					throw _ARGLESS_PRIVATE ArglessException<CharT>(_ARGLESS_PRIVATE ArglessExceptionType::WrongArgument, rule_name, args, m_data, e);
				}
			}
		}
		catch (const _ARGLESS_PRIVATE ArglessException<CharT>& e)
		{
			if (m_helper)
			{
				m_helper->help(e);
				return std::nullopt;
			}
			else
				throw e;
		}

		return result;
	}

private:
	template <typename T>
	static void* parser_caller(_ARGLESS_PRIVATE Args<CharT>& args)
	{
		auto result = _ARGLESS_PRIVATE get_parser<CharT, T>()(args);
		return static_cast<void*>(new T(result));
	}

	template <>
	static void* parser_caller<void>(_ARGLESS_PRIVATE Args<CharT>& args)
	{
		return static_cast<void*>(new void_t());
	}

private:
	template <typename T>
	static void deleter(void* ptr)
	{
		delete static_cast<T*>(ptr);
	}

	template <>
	static void deleter<void>(void* ptr)
	{
		delete static_cast<void_t*>(ptr);
	}

private:
	template <typename T>
	static string type_description(const std::function<std::vector<string>(size_t)>& enum_info)
	{
		return _ARGLESS_PRIVATE get_type_description<T, CharT>(enum_info);
	}

	template <>
	static string type_description<void>(const std::function<std::vector<string>(size_t)>& enum_info)
	{
		return _ARGLESS_TEXT(CharT, "Flag");
	}

private:
	bool is_rule(_ARGLESS_PRIVATE Args<CharT>* args) const
	{
		if (args->index >= args->args.size())
			return false;
		return find_rule(string(args->args[args->index])) != m_data.rules.end();
	}

	std::optional<intmax_t> is_enum_value(_ARGLESS_PRIVATE Args<CharT>* args, size_t enum_hash) const
	{
		auto enum_rule = m_data.enum_rules.find(enum_hash);
		_ARGLESS_ASSERT(enum_rule != m_data.enum_rules.end(), "Enum is not set");

		auto value = enum_rule->second.enum_map.find(string(args->args[args->index]));
		if (value == enum_rule->second.enum_map.end())
			return std::nullopt;

		return value->second;
	}

private:
	typename std::unordered_map<string, _ARGLESS_PRIVATE Rule<CharT>>::const_iterator find_rule(const string& arg) const
	{
		{
			auto rule = m_data.rules.find(arg);
			if (rule != m_data.rules.end())
				return rule;
		}

		for (auto rule = m_data.rules.begin(); rule != m_data.rules.end(); rule++)
		{
			if (std::find(rule->second.shortcuts.begin(), rule->second.shortcuts.end(), arg) != rule->second.shortcuts.end())
				return rule;
		}

		return m_data.rules.end();
	}

private:
	_ARGLESS_PRIVATE ArglessData<CharT> m_data;
	std::unique_ptr<_ARGLESS_PRIVATE Helper<CharT>> m_helper;
};

typedef BasicArgless<char> Argless;
typedef BasicArgless<wchar_t> WArgless;

_ARGLESS_END

#if !ARGLESS_DO_NOT_UNDEF

#ifdef ARGLESS_CORE_HPP

#undef _ARGLESS_BEGIN
#undef _ARGLESS_END
#undef _ARGLESS

#undef _ARGLESS_PRIVATE_BEGIN
#undef _ARGLESS_PRIVATE_END
#undef _ARGLESS_PRIVATE

#undef _ARGLESS_STATIC_ASSERT
#undef _ARGLESS_ASSERT

#undef _ARGLESS_CONVERT_CHAR
#undef _ARGLESS_CHAR
#undef _ARGLESS_CONVERT_TEXT
#undef _ARGLESS_TEXT

#undef _ARGLESS_HAS_CPP11
#undef _ARGLESS_HAS_CPP14
#undef _ARGLESS_HAS_CPP17
#undef _ARGLESS_HAS_CPP20

#undef _ARGLESS_HAS_CHAR8_T

// defined in other headers

#undef _ARGLESS_PARSERS

#endif

#endif
