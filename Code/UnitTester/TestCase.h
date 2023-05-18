#pragma once

#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <Windows.h>
#include <iostream>

class TestCase
{
public:
	TestCase(std::filesystem::path);
	bool test();
private:

	std::filesystem::path testCasePath;
	std::string expectedResult;
};

