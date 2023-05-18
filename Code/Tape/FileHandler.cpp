#include "FileHandler.h"

FileHandler::FileHandler(std::string& inputFilePath_, std::string& outputFilePath_) :
    inputFilePath(inputFilePath_), outputFilePath(outputFilePath_)
{
    verifyFilesDiffer();
    isHandled = verifyForInput(inputFilePath)                                           // Handles and checks all files, isHandled is true only if
        && verifyForOutput()                                                            // all files were processed without unresolved errors
        && handleCfg();                                                           
}

FileHandler::~FileHandler()
{
    if (cleanFiles)
        std::filesystem::remove(inputFilePath);
}

bool FileHandler::verifyForInput(std::string& inputFilePath)
{
    if (!std::filesystem::exists("SorterFiles"))                                        // Create work directory, where all temporary files will be stored
        std::filesystem::create_directory("SorterFiles");
    if (!std::filesystem::exists(inputFilePath))                                        // Check if input file exists
    {
        std::cout << inputFilePath << " not found.\n";
        return false;
    }

    std::ifstream file(inputFilePath);
    if (!file.is_open())                                                                // Check if it's possible to open
    {
        std::cout << "Can't open " << inputFilePath << '\n';                        
        return false;
    }
    return formatForInput(inputFilePath);                                               // Format input file
}

bool FileHandler::formatForInput(std::string& inputFilePath)
{
    std::cout << "Formatting input...\n";
    std::regex reg("[0-2][0-1][0-4][0-7][0-4][0-8][0-3][0-6][0-4][0-7]|-[0-2][0-1][0-4][0-7][0-4][0-8][0-3][0-6][0-4][0-8]|-?[0-9]{1,9}");
    std::ifstream inputFile(inputFilePath);                                             // Open input file
    inputFilePath =                                                                     
        "SorterFiles/formatted"                                                         // Redirecting input path to formatted file
        + inputFilePath.substr(inputFilePath.find_last_of("/\\") + 1); 
    std::ofstream formattedInputFile(inputFilePath, std::ofstream::out | std::ofstream::trunc); // Create file to be filled with valid values
    bool hasValidValues = false;                                                        // Flag to check if input has valid values
    while (!inputFile.eof())
    {
        std::string buf;
        inputFile >> buf;
        if (std::regex_match(buf, reg))
        {
            formattedInputFile << buf << ' ';                                           // Fill formatted input file only with valid values
            hasValidValues = true;                                                      // If at least one valid value is found, set hasValiValues flag
        }
    }
    inputFile.close();
    formattedInputFile.close();
    if (hasValidValues)
        return true;                                                                    // Return true if formatted successfully
    std::cout << "File doesn't have any valid values\n";
    std::filesystem::remove(inputFilePath);
    return false;                                                                       // Return false otherwise
}

bool FileHandler::verifyForOutput()
{
    std::string outputDirectory = outputFilePath.substr(0, outputFilePath.find_last_of("/\\"));
    std::ofstream file(outputFilePath, std::ofstream::out | std::ofstream::trunc);      // Try creating an output file
    if (!file.is_open())
    {
        std::cout << "Can't create a file in directory " << outputDirectory << '\n';    // If unsuccessful, return false
        return false;
    }
    file.close();
    return true;                                                                        // Return true otherwise
}

void FileHandler::verifyFilesDiffer()
{
    try
    {                                                                                   // Should have used filesystem::path all along,
                                                                                        // only used it here despite it's very convenient
                                                                                        // TODO: rewrite code using path instead of string for file paths
        if (std::filesystem::equivalent(std::filesystem::path(inputFilePath), std::filesystem::path(outputFilePath)))
        {
            std::cout << "Can't read and write into same file.\n";                      // If paths refer to the same file, exit the program
            exit(-1);
        }
    }
    catch (std::exception&)
    {}                                                                                  // If files couldn't be opened, delegate the check to other methods
}

bool FileHandler::handleCfg()
{
    std::ifstream cfgFile("SorterFiles/cfg.txt", std::ifstream::in);                    // Initialize fstream for cfg file
    if (!cfgFile.is_open())
    {
        std::cout << "Creating configuration file...\n";                                // If cfg file doesn't exist, initialize default cfg
        if (!reinitializeCfgFile())                                                     // If cfg file couldn't be created, return false   
            return false;
        cfgFile.open("SorterFiles/cfg.txt", std::ifstream::in);                         // Reinitialaze fstream for cfg file
    }
    std::string buf;
    std::vector<unsigned int> input;
    for (int i = 0; i < 6; i++)
    {
        std::getline(cfgFile, buf);                                                     // Read line into buffer
        try 
        {
            int numValue;
            numValue = stoi(buf.substr(0, buf.find_first_of(' ')));                     // If int is found at the start of buffer, add it to input vector
            if (numValue < 0)
                throw std::exception("Parameter is negative\n");
            input.push_back((unsigned int)numValue);
        }
        catch (std::exception) 
        {
            std::cout << "Configuration file corrupted, reinitialize? [y/n]: ";         // If not, ask user's permission to reinitialize cfg file
            if (getUserDecision())
                return reinitializeCfgFile();
            else exit(-2);
        }
    }
    delays = std::vector<unsigned int>(&input[0], &input[4]);
    chunkSize = input[4];
    cleanFiles = input[5];
    return true;
}

bool FileHandler::reinitializeCfgFile()
{
    std::ofstream newCfgFile("SorterFiles/cfg.txt", std::ofstream::out | std::ofstream::trunc); // If creating filestream for new cfg file
    if (!newCfgFile.is_open())                                                          
    {
        std::cout << "Can't create configuration file\n";                               // Return false if cfg file couldn't be created
        return false;
    }
    newCfgFile << "0 read delay in microseconds\n";
    newCfgFile << "0 write delay in microseconds\n";
    newCfgFile << "0 rewind delay in microseconds\n";
    newCfgFile << "0 shift delay in microseconds\n";
    newCfgFile << "0 chunk size\n";
    newCfgFile << "1 clean files?";
    newCfgFile.close();
    delays = std::vector<unsigned int>({ 0,0,0,0 });
    chunkSize = 0;
    cleanFiles = true;
    return true;                                                                        // Return true if everything worked properly
}

bool FileHandler::getUserDecision()
{
    std::string userInput;
    do
    {
        std::cin >> userInput;
    } while (!std::regex_match(userInput, std::regex("^[yYnN]$")));
    if (userInput == "y" || userInput == "Y")
        return true;
    return false;
}
