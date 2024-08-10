#include <Argless.hpp>

#include <iostream>

using namespace maslowian::argless;

enum ExampleEnum
{
	None,
	Active,
	Inactive = 17
};

int _main(int argc, const char** argv)
{
	RuleBasicString<char> est;

	Argless argless;

	argless.set_enum_map<ExampleEnum>({ { "on", ExampleEnum::Active }, { "off", ExampleEnum::Inactive } });

	argless.set_rule<std::tuple<std::vector<int>, int>>("-test", {"-t"}, "Function used for testing");
	argless.set_rule<std::tuple<bool, int, ExampleEnum>>("--rule", { "-r" });
	argless.set_rule<void>("--flag", { "-f" });
	argless.set_rule<std::optional<RuleBasicString<char>>>("--help", { "help", "-h", "?" });
	argless.set_rule<RuleVector<std::string>>(nullptr);
	argless.set_rule<std::variant<int, std::tuple<std::array<char, 3>, float, bool, std::monostate, std::optional<std::string>>>>("-v", { "--v" });

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

	if (auto value = result.get<std::tuple<bool, int, ExampleEnum>>("--rule"))
	{
		auto [b, i, e] = *value;

		std::cout << "Rule:" << std::endl;

		std::cout << std::boolalpha;
		std::cout << "\tBool: " << b << std::endl;
		std::cout << "\tInt: " << i << std::endl;

		if (e == ExampleEnum::None)
			std::cout << "\tEnum: None" << std::endl;
		else if (e == ExampleEnum::Active)
			std::cout << "\tEnum: Active" << std::endl;
		else if (e == ExampleEnum::Inactive)
			std::cout << "\tEnum: Inactive" << std::endl;
	}

	if (result.get("--flag"))
	{
		std::cout << "Flag is set" << std::endl;
	}
	else
	{
		std::cout << "Flag is not set" << std::endl;
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

int main()
{
	const char* args[] = { "C:/Windows/System32/Edge.exe", "?", "-v"};
	int argc = sizeof(args) / sizeof(char*);

	_main(argc, reinterpret_cast<const char**>(args));

	return 0;
}
