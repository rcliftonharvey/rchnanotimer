#pragma once
#ifndef TESTS_H
#define TESTS_H


// Make this higher or lower to control how often the
// FOR loops inside the test functions will iterate.
#define LOOP_LIMIT 100000000


// Steps through a loop from 0 to LOOP_LIMIT in steps of +1.
// The iterator is incremented via post-increment. (step++)
void loopForwardIncrementPost ()
{
    unsigned int accumulator = 0;
    
    for (unsigned int step=0; step<LOOP_LIMIT; step++)
    {
        accumulator++;
    }
}

// Steps through a loop from 0 to LOOP_LIMIT in steps of +1.
// The iterator is incremented via pre-increment. (++step)
void loopForwardIncrementPre ()
{
    unsigned int accumulator = 0;
    
    for (unsigned int step=0; step<LOOP_LIMIT; ++step)
    {
        accumulator++;
    }
}

// Steps through a loop from LOOP_LIMIT to 1 in steps of -1.
// The iterator is decremented via post-decrement. (step--)
void loopBackwardDecrementPost ()
{
    unsigned int accumulator = 0;
    
    for (unsigned int step=LOOP_LIMIT; step>0; step--)
    {
        accumulator++;
    }
}

// Steps through a loop from LOOP_LIMIT to 1 in steps of -1.
// The iterator is decremented via pre-decrement. (--step)
void loopBackwardDecrementPre ()
{
    unsigned int accumulator = 0;
    
    for (unsigned int step=LOOP_LIMIT; step>0; --step)
    {
        accumulator++;
    }
}


// Helper function that will pad a result string with whitespaces
// so it fits into the result column width (16) nicely.
const std::string get (std::vector<std::string>& Test, const unsigned int& Run)
{
    std::string padded = Test[Run];
    
    const int remainder = 16 - static_cast<int>(padded.length()) - 1;
    
    for (unsigned int pos=0; pos<remainder; ++pos)
    {
        padded.append(" ");
    }
    
    return padded;
}


#endif // TESTS_H
