#include "configClass.hpp"
#include <utility>

int main()
{
	configClass test("sample.config", 0);
}

// int main()
// {
// 	std::map<std::string, std::map<std::string, std::string> > above;

// 	std::string	key = "okay";
// 	std::string value = "hello";
// 	std::string letter = "a";

// 	for (int i = 0; i < 2; i++)
// 	{
// 		std::map<std::string, std::string> test;
// 		test.insert(std::make_pair("hi", key));
// 		test.insert(std::make_pair("ho", value));
// 		test.insert(std::make_pair("hi", "not okay"));

// 		above.insert(std::make_pair(letter, test));
// 		key = "okay 2";
// 		value = "hello 2";
// 		letter = "b";
// 	}

// 	std::cout << above["a"]["hi"] << "|" << above["b"]["hi"] << std::endl;
// }