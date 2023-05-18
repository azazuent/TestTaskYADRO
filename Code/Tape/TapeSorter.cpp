#include "TapeSorter.h"

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

TapeSorter::TapeSorter(Tape* inputTape_, Tape* outputTape_, unsigned int chunkSize_) :
	inputTape(inputTape_), outputTape(outputTape_), chunkSize(chunkSize_ > 0 ? chunkSize_ : max_size) // If chunkSize == 0 -> chunkSize == max int
{}

void TapeSorter::sort()
{
	std::cout << "Sorting...\n";
	inputTape->rewind();
	outputTape->rewind();											// Rewind both tapes
	outputTape->write(inputTape->read());							// Initialize output tape with the first value of input tape

	while (true)
	{
		bool lastIteration = false;									// Flag to break the loop when end of input tape is reached
		std::vector<int> chunk;										// Initialize next chunk
		for (int i = chunkSize; i--;)
		{
			if (!inputTape->shift(1))
			{
				lastIteration = true;								// If end of input tape is reached, set the lastIteration flag
				break;												// Stop reading input tape to avoid duplicates
			}
			chunk.push_back(inputTape->read());						// Fill the chunk
		}
		std::sort(chunk.begin(), chunk.end());						// Sort the chunk
		TapeFile bufTape(*outputTape, "tmpTape.txt", false);		// Create a copy of current output tape to merge chunk and the current output
		outputTape->rewind();										// Return to the start of the output tape before merging

		for (auto it = chunk.begin(); chunk.size() != 0;)			// Create an iterator for the chunk and check that chunk is not empty
		{
			if (*it < bufTape.read())
			{
				outputTape->write(*it);								// If the current value from the chunk is lesser than the current
				it++;												// value on the copy of output tape, write it to output tape
				if (it == chunk.end())
				{
					do
					{
						outputTape->shift(1, Tape::shiftType(1));	// If the whole chunk has been processed, write out remaining
						outputTape->write(bufTape.read());			// values from the copy of the output tape and break the merge cycle
					} while (bufTape.shift(1));
					break;
				}
			}
			else
			{
				outputTape->write(bufTape.read());					// If the current value on the copy of output tape is lesser than
				if (!bufTape.shift(1))								// the current value from the chunk, write it to output tape
				{
					do
					{
						outputTape->shift(1, Tape::shiftType(1));	// If end of output tape's copy was reached, write out remaining
						outputTape->write(*it);						// values from the chunk and break the merge cycle
					} while (++it != chunk.end());
					break;
				}
			}
			outputTape->shift(1, Tape::shiftType(1));				// If there are remaining values to write in the chunk and the copy
		}															// of output tape, prepare for writing by shifting the head of
																	// output tape and initialize a cell if end of tape is reached
		if (lastIteration)											
			break;													// If lastIteration flag is set, break the sort cycle
	}
}
