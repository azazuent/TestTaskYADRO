#include <iostream>

#include "TestCase.h"

//#define DEBUG

#ifdef DEBUG
int main()
{
	try
	{
		TestCase testCase("C:/TestTester/UnitTests/allValid.txt");
		if (testCase.test())
			std::cout << "Test has passed.\n";
		else
			std::cout << "Test did not pass.\n";
		return 0;
	}
	catch (std::exception e)
	{
		std::cout << e.what() << "\n";
		return -1;
	}
}
#else
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: UnitTesterForTestTaskYadro <testCase1Filename> <testCase2Filename>...\n";
		exit(-1);
	}

	for (int i = 1; i < argc; i++)
	{
		try
		{
			TestCase testCase(argv[i]);
			if (testCase.test())
				std::cout << "Test has passed.\n\n";
			else
				std::cout << "Test did not pass.\n\n";
		}
		catch (std::exception e)
		{
			std::cout << e.what();
		}
	}
	return 0;
}
#endif