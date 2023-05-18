#pragma once

class Tape {
public:

    enum class shiftType {
        stayOnLast = 0,                                     // Don't move past last slot
        extendByOne = 1,                                    // Move past last slot by one and initialize it
        extendAll = 2                                       // Move past last slot and initialize all new slots
    };

    Tape(std::vector<unsigned int> delays_ = std::vector<unsigned int>(4, 0) ) :
        readDelay(delays_[0]), writeDelay(delays_[1]), rewindDelay(delays_[2]), shiftDelay(delays_[3]) {}
    virtual ~Tape() {}
    virtual int read() = 0;                                 // Read from current slot
    virtual void write(int) = 0;                            // Write into current slot
    virtual void rewind() = 0;                              // Move to first slot
    virtual bool shift(int, shiftType = shiftType(0)) = 0;  // Move n slots, do something according to shiftType, return true if tape borders weren't crossed, false otherwise

    const unsigned int readDelay;                           // Delay to read from slot in microseconds
    const unsigned int writeDelay;                          // Delay to write to slot in microseconds
    const unsigned int rewindDelay;                         // Delay to rewind in microseconds
    const unsigned int shiftDelay;                          // Delay to shift for one cell in microseconds
}; 
