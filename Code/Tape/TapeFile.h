#pragma once

#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <thread>

#include "Tape.h"

#define DEFAULT_OPEN_MODE std::ios_base::in | std::ios_base::out | std::ios_base::binary

#define writeToFileInHex(file, value) file << std::hex << std::setfill('0') << std::setw(blockSize) << value << ' ' //Writes int to file in hex with set length of sizeof(int)*2

class TapeFile : public Tape
{
public:
    TapeFile(const std::string&, bool = true, std::vector<unsigned int> = std::vector<unsigned int>(4, 0), bool = true, std::ios_base::openmode = DEFAULT_OPEN_MODE);
    // Construct a tape class using a file from the device, if a path to an existing file is provided,
    // use it as initial tape value, if not, create a blank tape with one initialized slot
    TapeFile(Tape& source, const std::string&, bool = true, bool = true, std::ios_base::openmode = DEFAULT_OPEN_MODE);
    // Copy constructor using an interface 
    ~TapeFile();
    virtual int read();
    virtual void write(int);
    virtual void rewind();
    virtual bool shift(int, shiftType = shiftType(0)); 

private:

    std::string path;               // Path to input file
    std::string filename;           // Filename without preceding path
    std::ios_base::openmode mode;   // Openmode for the input file
    std::fstream file;              // Filestream for temporary file

    bool saveChanges;               // If == true, the destructor will overwrite the original file with modified values
    bool cleanTmp;                  // If == true, formatted hex files won't be deleted
    int blockSize;                  // Size for a formatted value in temporary file, == sizeof(int)*2, because its stored in hex

};

