#include "TestCase.h"

TestCase::TestCase(std::filesystem::path testCasePath_) :
	testCasePath(testCasePath_), expectedResult("")
{
    std::ifstream inputStream(testCasePath);
	if (!inputStream.is_open())
	{
        expectedResult = "-1";
        return;
	}
    std::regex reg("[0-2][0-1][0-4][0-7][0-4][0-8][0-3][0-6][0-4][0-7]|-[0-2][0-1][0-4][0-7][0-4][0-8][0-3][0-6][0-4][0-8]|-?[0-9]{1,9}");
    std::vector<int> input;
    while (!inputStream.eof())
    {
        std::string buf;
        inputStream >> buf;
        if (std::regex_match(buf, reg))
            input.push_back(stoi(buf));
    }
    if (input.empty())
    {
        expectedResult = "-1";
        return;
    }
    std::sort(input.begin(), input.end());
    for (auto it = input.begin(); it != input.end(); it++)
        expectedResult += std::to_string(*it) + " ";
}

bool TestCase::test()
{
    std::string app("TestTaskYADRO.exe");
    std::string resFilename("result" + testCasePath.filename().generic_string());
    std::string arg(testCasePath.generic_string() + " " + resFilename);

    std::cout << "Testing " << testCasePath << '\n';
    std::cout << std::setw(70) << std::setfill('_') << '\n';

    STARTUPINFO si;
    PROCESS_INFORMATION pi; // The function returns this
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    //Prepare CreateProcess args
    std::wstring app_w(app.length(), L' '); // Make room for characters
    std::copy(app.begin(), app.end(), app_w.begin()); // Copy string to wstring.

    std::wstring arg_w(arg.length(), L' '); // Make room for characters
    std::copy(arg.begin(), arg.end(), arg_w.begin()); // Copy string to wstring.

    std::wstring input = app_w + L" " + arg_w;
    wchar_t* arg_concat = const_cast<wchar_t*>(input.c_str());
    const wchar_t* app_const = app_w.c_str();

    // Start the child process.
    if (!CreateProcessW(
        app_const,      // app path
        arg_concat,     // Command line (needs to include app path as first argument. args seperated by whitepace)
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi)            // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        throw std::exception("Could not create child process ");
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    std::cout << std::setw(70) << std::setfill('_') << '\n';
    
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    if (expectedResult == "-1")
    {
        std::cout << "Expected exit code: " << expectedResult << '\n';
        std::cout << "Actual exit code:   " << (int)exitCode << '\n';
        return stoi(expectedResult) == (int)exitCode;
    }
    else if (exitCode == -2)
    {
        throw std::exception("User denied file reconfiguration, testing impossible\n\n");
    }
    std::ifstream resultFilestream(resFilename);
    std::string result;
    std::getline(resultFilestream, result);
    resultFilestream.close();
    std::filesystem::remove(resFilename);
    std::cout << "Expected result: " << expectedResult << '\n';
    std::cout << "Actual result:   " << result << '\n';
    return (expectedResult == result);
}
