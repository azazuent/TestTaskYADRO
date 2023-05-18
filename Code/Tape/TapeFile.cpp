#include "TapeFile.h"

TapeFile::TapeFile(const std::string& path_, bool saveChanges_, std::vector<unsigned int> delays_, bool cleanTmp_, std::ios_base::openmode mode_)
    : path(path_), filename(path.substr(path.find_last_of("/\\") + 1)), saveChanges(saveChanges_), mode(mode_), file(path, mode), cleanTmp(cleanTmp_), blockSize(sizeof(int)*2),
    Tape(delays_)
{
    if (!std::filesystem::exists("SorterFiles/tmp"))                // Check if tmp folder for tapes exists
        std::filesystem::create_directory("SorterFiles/tmp");       // Create tmp if not

    if (std::filesystem::exists(path))                              // Check if a non-empty input file exists
    {
        std::fstream formattedFile(std::string("SorterFiles/tmp/") + filename, std::fstream::in | std::fstream::out | std::fstream::trunc); // If exists, use it as origin
        while (!file.eof())
        {
            std::string buf;
            file >> buf;
            if (!buf.empty())
                writeToFileInHex(formattedFile, stoi(buf));         // Fill tmp file with formatted ints
        }
        file.swap(formattedFile);                                   // Swap streams
        formattedFile.close();
    }
    else
    {
        file = std::fstream(std::string("SorterFiles/tmp/") + filename, std::fstream::in | std::fstream::out | std::fstream::trunc);        // If it doesn't exist, create new tmp with one null element
        writeToFileInHex(file, 0);
    }
    rewind();
}

TapeFile::TapeFile(Tape& source, const std::string& filename_, bool saveChanges_, bool cleanTmp_, std::ios_base::openmode mode_) :
    filename(filename_), saveChanges(saveChanges_), mode(mode_), cleanTmp(cleanTmp_), file("SorterFiles/tmp/" + filename, mode | std::fstream::trunc) , 
    blockSize(sizeof(int) * 2), Tape(std::vector<unsigned int> {readDelay, writeDelay, rewindDelay, shiftDelay})
{
    int count = 0; 
    while (source.shift(-1))
        count++;                                // Shift to start of source, remembering initial head position
    do
    {
        writeToFileInHex(file, source.read());  // Copy values from source file
        
    } while (source.shift(1));
    rewind();                                   // Rewind initialized tape

    source.rewind();                            // Return source to initial position
    for (; count--;)
        source.shift(1);
}

TapeFile::~TapeFile()
{
    if (saveChanges)                                                     // If saveChanges == true, overwrite original input file with modified values
    {
        std::fstream outputFile(path, std::fstream::out | std::fstream::trunc);
        rewind();
        do
        {
            outputFile << read() << ' ';
        } while (shift(1)); 
        outputFile.close();
    }
    file.close();
    if (cleanTmp)                                                       // If cleanTmp == true, remove tmp file
        std::filesystem::remove("SorterFiles/tmp/" + filename);
}

int TapeFile::read()
{
    std::this_thread::sleep_for(std::chrono::microseconds(readDelay));  // Delay
    long long savedPos = file.tellg();                                  // Store initial position
    unsigned int value;
    file >> std::hex >> value;                                          // Read value in hex to unsigned int (because fstream's formatted input treats input 
                                                                        // as unsigned and might cause errors if read into signed int

    file.seekg(savedPos);                                               // Return to initial position
    return (int)value;
}

void TapeFile::write(int value)
{
    std::this_thread::sleep_for(std::chrono::microseconds(writeDelay)); // Delay
    long long savedPos = file.tellg();                                  // Store initial position
    file.seekp(savedPos);                                               // Move stream's put pointer to current position
    writeToFileInHex(file, value);                                      // Write formatted value in hex
    file.seekg(savedPos);                                               // Return to initial position
}

void TapeFile::rewind()
{
    std::this_thread::sleep_for(std::chrono::microseconds(rewindDelay));// Delay
    file.seekg(0);                                                      // Return to file's beginning
}

bool TapeFile::shift(int offset, shiftType st)                          // This function has some copypaste because it was adapted to delay
{                                                                       // on each move, and before, movement to the left was done by
                                                                        // seekg(offset * (blockSize+1), std::ios_base::cur)), because if
                                                                        // fstream's get pointer moves past left border, we always know that we
                                                                        // must return to 0, but we can't know where to move if get pointer moves
                                                                        // past right border, so we have to iterate. Now, both directions are iterated to
                                                                        // be able to cause movement delay as said in the task. TODO: Get rid of copypaste

    if (offset > 0)                                                     // Can't treat positive and negative offsets the same way because
    {                                                                   // we can't initialize new slots in the start of the tape
        for (; offset--;)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(shiftDelay)); // Delay for each move
            file.seekg((long long)blockSize + 1, std::ios_base::cur);           // Move fstream get pointer
            if (file.peek() == EOF)                                             
            {                                                                   // If we crossed the right border of the file, clear stream's flags and 
                file.clear();                                                   // start branching on the shiftType option
                if (st == shiftType::stayOnLast)
                {
                    file.seekg(-(blockSize + 1), std::ios_base::cur);           // If option says to stay on the last slot, move stream's get pointer
                    return false;                                               // and return false because we hit the EOF
                }
                else if (st == shiftType::extendByOne)
                {                                                               // If option says to extend the tape by one element, write a 0 and return false because we hit EOF
                    write(0);                                                   // Yes, it's a pretty bad choice for "uninitialized value", which caused some problems
                    return false;                                               // and there probably is a better implementation, maybe I'll fix it someday
                }                                                               // TODO: figure out a better implementation
                else if (st == shiftType::extendAll)
                {
                    write(0);                                                   // If options says to extend the tape for every overflowing value of offset, do it
                    if (offset == 0)                                            // until offset decremented to null, return false because we hit EOF
                        return false;
                }

            }
        }
    }

    else if (offset < 0)
    {
        for (; offset++;)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(shiftDelay)); // Delay for each move
            file.seekg(-((long long)blockSize + 1), std::ios_base::cur);        // Move fstream's get pointer
            if (file.peek() == EOF)
            {
                file.clear();                                                   // If we hit EOF, clear stream's flags, move get pointer to the start
                file.seekg(0);                                                  // and return false
                return false;
            }
        }
    }

    return true;                                                                // Return true if shift didn't hit EOF and everything worked properly
}
