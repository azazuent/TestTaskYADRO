#pragma once

#include <fstream>

#include "TapeFile.h"

class TapeSorter
{
public:
	TapeSorter(Tape*, Tape*, unsigned int = 0);

	void sort();	// This method is based on merge sort, but, because the task said to use a "sane"
					// amount of temporary tapes, I decided that a "sane" amount is 1. It reads chunkSize
					// amount of ints from the input tape, sorts them and merges it with the output tape 
					// until the end of input tape is reached.
private:
	Tape* inputTape;
	Tape* outputTape;
	const unsigned int chunkSize;
};

