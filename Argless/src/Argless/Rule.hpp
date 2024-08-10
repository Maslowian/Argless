#pragma once
#include "Core.hpp"
#include "Args.hpp"

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
	EmptyRule(Parser parser, size_t type_hash, Deleter deleter, bool is_rule_vector, std::function<std::basic_string<CharT>(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)> type_description, std::optional<std::basic_string<CharT>> short_description, std::optional<std::basic_string<CharT>> description) : parser(parser), type_hash(type_hash), deleter(deleter), is_rule_vector(is_rule_vector), short_description(short_description), description(description), type_description(type_description)
	{}

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
	Rule(Parser parser, size_t type_hash, Deleter deleter, bool is_rule_vector, std::function<std::basic_string<CharT>(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)> type_description, std::optional<std::basic_string<CharT>> short_description, std::optional<std::basic_string<CharT>> description, const std::vector<std::basic_string<CharT>>& shortcuts) : EmptyRule<CharT>(parser, type_hash, deleter, is_rule_vector, type_description, short_description, description), shortcuts(shortcuts)
	{}

public:
	std::vector<std::basic_string<CharT>> shortcuts;
};

template <typename CharT>
struct EnumRule
{
public:
	using EnumMap = std::unordered_map<std::basic_string<CharT>, intmax_t>;

public:
	EnumRule(EnumMap enum_map) : enum_map(enum_map)
	{}

public:
	EnumMap enum_map;
};

_ARGLESS_PRIVATE_END
