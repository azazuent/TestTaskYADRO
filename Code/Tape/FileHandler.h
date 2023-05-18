#pragma once

#include <string>
#include <filesystem>
#include <regex>
#include <iostream>
#include <fstream>

class FileHandler
{
public:
	FileHandler(std::string& inputFilePath_, std::string& outputFilePath_);
	~FileHandler();
	bool verifyForInput(std::string&);										// This method checks if the input file exists, doesn't have non-numeric and
																			// incorrect values, and creates it's formatted copy if the user allows to
																			// Returns false if the file doesn't exist, or the user didn't allow to format it, or
																			// the file doesn't have any valid(within the int range) numeric values, returns true otherwise

	bool formatForInput(std::string&);										// This method creates a copy of the input file without non-numeric and
																			// incorrect values and redirects inputFilePath to this copy.
																			// Returns false if the input file doesn't have any valid numeric files, returns true otherwise

	bool verifyForOutput();													// This method checks if the path to output directory exists and creates the output file.
																			// Returns true if the file is created successfully, returns false otherwise

	void verifyFilesDiffer();												// This method checks if the filepaths refer to the same file, returns false if they do
																			// or they couldn't be accessed, true otherwise

	bool handleCfg();														// This method checks if a configuration file exists in program's files, initializes
																			// it if not, checks if it's not corrupted, asks user for permission to reinitialize if it is.
																			// Returns false if user denied permission to reinitialize the file, true otherwise

	bool handled() { return isHandled; }									// Getter for isHandled flag

	std::vector<unsigned int>& getDelays() { return delays; }
	unsigned int getChunkSize() { return chunkSize; }
	bool getCleanFiles() { return cleanFiles; }
	
private:

	std::string& inputFilePath;
	std::string& outputFilePath;
	bool isHandled;

	std::vector<unsigned int> delays;
	unsigned int chunkSize;
	bool cleanFiles;

	bool reinitializeCfgFile();												// This method reinitializes the configuration file, returns true if successful, false otherwise
	bool getUserDecision();													// Utility method to get user input, loops until a y | Y | n | N value is read
};

