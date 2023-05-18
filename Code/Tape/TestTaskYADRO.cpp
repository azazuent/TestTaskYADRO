#include "TapeSorter.h"
#include "FileHandler.h"

//#define DEBUG

int main(int argc, char* argv[])
{
#ifdef DEBUG
    std::string inputFilename("C:/TestTester/UnitTests/allValid.txt");
    std::string outputFilename("C:/TestTester/output.txt");
#else
    if (argc != 3)
    {
        std::cout << "usage: TestTaskYADRO <inputFilename> <outputFilename>";
        return -1;
    }
    std::string inputFilename(argv[1]);
    std::string outputFilename(argv[2]);
#endif

    FileHandler handler(inputFilename, outputFilename);

    if (!handler.handled())
    {
        std::cout << "File error.\n";
        return -1;
    }
    
    std::vector<unsigned int> delays = handler.getDelays();

    TapeFile inputTape(inputFilename, false, delays, handler.getCleanFiles());
    TapeFile outputTape(outputFilename, true, delays, handler.getCleanFiles(), DEFAULT_OPEN_MODE | std::fstream::trunc);

    TapeSorter tapeSorter(&inputTape, &outputTape, handler.getChunkSize());
    tapeSorter.sort();

    std::cout << "Sorted successfully.\n";
    return 0;
    
}
