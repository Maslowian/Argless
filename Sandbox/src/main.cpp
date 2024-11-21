#include <Argless.hpp>

#include <iostream>

using namespace maslowian::argless;

enum ExampleEnum
{
	None,
	Active,
	Inactive = 17
};

int main(int argc, const char** argv)
{
	Argless argless;

	argless.set_enum_map<ExampleEnum>({ { "on", ExampleEnum::Active }, { "off", ExampleEnum::Inactive }, { "disable", ExampleEnum::Inactive } });

	argless.set_rule<void>("--void", { "-v" }, "Void value");
	argless.set_rule<bool>("--bool", { "-b" }, "Boolean value");
	argless.set_rule<int>("--int", { "-i" }, "Integer value");
	argless.set_rule<float>("--float", { "-f" }, "Floating point value");
	argless.set_rule<std::string>("--string", { "-s" }, "String value");
	argless.set_rule<char>("--char", { "-c" }, "Character value");
	argless.set_rule<std::filesystem::path>("--path", { "-p" }, "Path value");
	argless.set_rule<std::vector<int>>("--vector", { "-v" }, "Vector of integers");
	argless.set_rule<std::array<char, 3>>("--array", { "-a" }, "Array of characters");
	argless.set_rule<std::tuple<int, char, float>>("--tuple", { "-t" }, "Tuple of int, char and float");
	argless.set_rule<ExampleEnum>("--enum", { "-e" }, "Enum value");
	argless.set_rule<std::optional<int>>("--optional", { "-o" }, "Optional integer value");

	argless.set_rule<RuleVector<std::string>>(nullptr);
	argless.set_rule<std::optional<RuleBasicString<char>>>("--help", { "help", "-h", "?" });

	auto opt_result = argless.parse(argc, argv);
	if (!opt_result)
		return -1;
	auto& result = *opt_result;

	if (auto value = result.get<std::optional<RuleBasicString<char>>>("--help"))
	{
		if (value->has_value())
			argless.help(value->value());
		else
			argless.help();
		return 0;
	}

	if (auto value = result.get<RuleVector<std::string>>(nullptr))
	{
		auto& vec = *value;

		std::cout << "Other arguments:" << std::endl;
		for (auto& str : vec)
		{
			std::cout << '\t' << str << std::endl;
		}
	}

	return 0;
}
