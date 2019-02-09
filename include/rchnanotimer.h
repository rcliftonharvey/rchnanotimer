//
//  RCH::NanoTimer
//
//  An easy to use high precision code stopwatch in pure C++
//  https://github.com/rcliftonharvey/rchnanotimer
//
//  Copyright © 2018 R. Clifton-Harvey
//
//  This library is released under the MIT license, which essentially means
//  you can do anything you want with it, as long as you credit it somewhere.
//
//  A full version of the license file can be found here:
//  https://github.com/rcliftonharvey/rchnanotimer/blob/master/LICENSE
//
#pragma once
#ifndef RCHNANOTIMER_H_INCLUDED
#define RCHNANOTIMER_H_INCLUDED
// ---- MODULE CODE STARTS BELOW ---- //


// Hack to make Visual Studio accept 'and' and 'or' operators in code.
#if _MSC_VER > 1000
    #ifndef and
        #define and &&
    #endif
    #ifndef or
        #define or ||
    #endif
#endif


// Include C++ dependencies
#include <chrono>
#include <cmath>
#include <string>
#include <vector>


// The number format used for timestamps and return values.
//
// The double type has floating point precision and a value range that
// can very easily handle even huge nanosecond precision timestamps.
//
// Non floating point formats are also possible, but it's advidable to
// make them unsigned and as long as possible so they can also handle
// larger nanosecond timestamp values. (e.g. unsigned long int)
#define RCH_NANOTIMER_DATATYPE double

// Default settings when outputting formatted timer values.
#define RCH_NANOTIMER_DEFAULT_PRECISION     6       // format() --> post comma floating point remainder amount
#define RCH_NANOTIMER_DEFAULT_DISPLAYLABEL  true    // format() --> append timebase label to output string

// Text labels to use when outputting formatted timer values.
#define RCH_NANOTIMER_LABELS_NANOSECONDS    " ns"
#define RCH_NANOTIMER_LABELS_MICROSECONDS   " µs"
#define RCH_NANOTIMER_LABELS_MILLISECONDS   " ms"
#define RCH_NANOTIMER_LABELS_SECONDS        " s"
#define RCH_NANOTIMER_LABELS_MINUTES        " min"
#define RCH_NANOTIMER_LABELS_HOURS          " h"

// Factors to divide from nanoseconds to various other time bases.
// Not really user options, but easier to handle in code as defines.
#define RCH_NANOTIMER_NANOSECONDS           1.000000
#define RCH_NANOTIMER_MICROSECONDS          1000.000000
#define RCH_NANOTIMER_MILLISECONDS          1000000.000000
#define RCH_NANOTIMER_SECONDS               1000000000.000000
#define RCH_NANOTIMER_MINUTES               60000000000.000000
#define RCH_NANOTIMER_HOURS                 3600000000000.000000

// Just a convenience define
#define RCH_NANOTIMER_TIMESTAMP std::chrono::high_resolution_clock::time_point

/** RCH classes are in here */
namespace RCH
{
    /** Creates a high precision timer. */
    class NanoTimer
    {
    public:
        
        NanoTimer () {}
        
        /** Constructor that overrides decimal precision and label display formatting */
        NanoTimer (const unsigned int& Precision, const bool& DisplayLabel)
        {
            formatting.precision = Precision;
            formatting.label = DisplayLabel;
        }
        
        ~NanoTimer () {}
        
// ------------------------------------------------------------------------------------------
// TIMEBASE FACTOR
        
        /** Contains methods to switch the timer's time base for retrieved values.
            The timer will always store full precision nanosecond based timestamps,
            but the timebase can be used to scale output values to other timebases
            like milliseconds or full seconds. */
        struct Timebase
        {
        private:
            
            // The currently set time base, nanoseconds by default
            RCH_NANOTIMER_DATATYPE timefactor = RCH_NANOTIMER_NANOSECONDS;
            
        public:
            
            /** Returns the currently set timebase factor by which the original
                full precision timestamps are divided when output. */
            const RCH_NANOTIMER_DATATYPE& factor () const
            {
                return timefactor;
            }
            
            /** Switches the timebase to nanoseconds. */
            void nanoseconds ()
            {
                timefactor = RCH_NANOTIMER_NANOSECONDS;
            }
            
            /** Switches the timebase to microseconds. */
            void microseconds ()
            {
                timefactor = RCH_NANOTIMER_MICROSECONDS;
            }
            
            /** Switches the timebase to milliseconds. */
            void milliseconds ()
            {
                timefactor = RCH_NANOTIMER_MILLISECONDS;
            }
            
            /** Switches the timebase to seconds. */
            void seconds ()
            {
                timefactor = RCH_NANOTIMER_SECONDS;
            }
            
            /** Switches the timebase to minutes. */
            void minutes ()
            {
                timefactor = RCH_NANOTIMER_MINUTES;
            }
            
            /** Switches the timebase to hours. */
            void hours ()
            {
                timefactor = RCH_NANOTIMER_HOURS;
            }
            
        } timebase;
        
// ------------------------------------------------------------------------------------------
// FORMAT / BEAUTIFY OUTPUT
        
        /** Sets up the timer's formatting precision in post-comma floating point steps, and
            whether or not to display a text label at the end of formatted value strings. */
        void format (const unsigned int& Precision, const bool& DisplayLabel)
        {
            formatting.precision = Precision;
            formatting.label = DisplayLabel;
        }
        
        /** Takes a timer value and returns it formatted according to current formatting options. */
        const std::string format (const RCH_NANOTIMER_DATATYPE& Time) const
        {
            // Calculate the factor for rounding
            const RCH_NANOTIMER_DATATYPE roundFactor = std::pow(10,formatting.precision);
            
            // Turn the rounded number value into a string
            std::string value = std::to_string(std::round(Time * roundFactor) / roundFactor);
            
            // To avoid compiler warnings about type mismatch between int / iterator
            const unsigned int valueLength = static_cast<unsigned int>(value.length());
            
            // The position/index of the dot character in the timer value string
            const int dot = static_cast<int>(value.find("."));
            
            if (dot > -1)
            {
                // Evaluate how many post-comma floating point steps will be left
                unsigned int precision = formatting.precision + (formatting.precision > 0);
                
                // Ensure that the maximum precision offset is limited to actually available characters
                if (precision > valueLength - dot)
                {
                    precision = valueLength - dot;
                }
                
                // Remove any trailing numbers from the value string if outside of precision
                value.erase(dot + precision, std::string::npos);
            }
            
            // If a text label should be added to the end of the string
            if (formatting.label == true)
            {
                // Pick the appropriate label for the current timebase and append it
                if (timebase.factor() == RCH_NANOTIMER_NANOSECONDS)
                {
                    value += std::string(RCH_NANOTIMER_LABELS_NANOSECONDS);
                }
                else if (timebase.factor() == RCH_NANOTIMER_MICROSECONDS)
                {
                    value += std::string(RCH_NANOTIMER_LABELS_MICROSECONDS);
                }
                else if (timebase.factor() == RCH_NANOTIMER_MILLISECONDS)
                {
                    value += std::string(RCH_NANOTIMER_LABELS_MILLISECONDS);
                }
                else if (timebase.factor() == RCH_NANOTIMER_SECONDS)
                {
                    value += std::string(RCH_NANOTIMER_LABELS_SECONDS);
                }
                else if (timebase.factor() == RCH_NANOTIMER_MINUTES)
                {
                    value += std::string(RCH_NANOTIMER_LABELS_MINUTES);
                }
                else if (timebase.factor() == RCH_NANOTIMER_HOURS)
                {
                    value += std::string(RCH_NANOTIMER_LABELS_HOURS);
                }
            }
            
            // Output the formatted value as a string
            return value;
        }
        
        /** Takes a timer value along with formatting arguments and returns it formatted as specified.
            This will update the timer's formatting options and affect later calls to format(). */
        const std::string format (const RCH_NANOTIMER_DATATYPE& Time, const int& Precision, const bool& DisplayLabel)
        {
            // Update formatting settings first
            format(Precision,DisplayLabel);
            
            // Then return the formatted time value as a string
            return format(Time);
        }
        
// ------------------------------------------------------------------------------------------
// RESET
        
        /** Hard reset, stops the timer if currently running and deletes all saved split time events. */
        void reset ()
        {
            // Reset the running flag to "not running" state
            running = false;
            
            // Remove all currently saved split time events
            splits.clear();
        }
        
// ------------------------------------------------------------------------------------------
// OPERATION
        
        /** Adds a new split time event to the list without stopping the timer.
            Accepts a string argument ID to help identify this split time event.
            If no string is passed, uses sequential numbers as IDs. */
        const RCH_NANOTIMER_DATATYPE split (std::string ID="")
        {
            // Only add splits
            if (running == true)
            {
                // Make the ID argument lowercase
                std::transform(ID.begin(), ID.end(), ID.begin(), ::tolower);
                
                // Add a new {ID,time} record as the latest split
                splits.push_back(NanoSplit(((ID == "") ? std::to_string(numSplits() + 1) : ID), now()));
                
                // Return the scaled (not formatted) timestamp of the just added split event time
                return scale(splits.back().time);
            }
            
            // Return zero if the timer is currently not running
            return zero();
        }
        
        /** Starts a new timer run.
            Does a full reset beforehand, all previously stored splits will be deleted.
            Accpets a string argument ID to help identify this timer start event.
            If no string is passed, uses "start" as the split identifier. */
        const RCH_NANOTIMER_DATATYPE start (std::string ID="")
        {
            // Erase the slate
            reset();
            
            // Flag the timer as "running" from now on
            running = true;
            
            // Create a new split time event using the passed ID, fall back to "start" if none passed
            split((ID == "") ? "start" : ID);
            
            // Return the scaled (not formatted) timestamp of the just added start event time
            return scale(splits.front().time);
        }
        
        /** Stops the timer completely.
            It will not be possible to add new splits anymore, except after reset() or new start().
            Accepts a string argument ID to help identify this timer stop event.
            If no string is passed, uses "stop" as the split identifier. */
        const RCH_NANOTIMER_DATATYPE stop (std::string ID="")
        {
            // It's only possible to trigger a stop event while the timer is actually running
            if (running == true)
            {
                // Create a new split time event using the passed ID, fall back to "stop" if none passed
                split((ID == "") ? "stop" : ID);
                
                // Flat the timer as "not running" from now on
                running = false;
                
                // Return the scaled (not formatted) timestamp of the just added stop event time
                return scale(splits.back().time);
            }
            
            // Return zero if the timer is currently not running
            return zero();
        }
        
// ------------------------------------------------------------------------------------------
// SPECIFIC POINTS IN TIME
        
        /** Returns the current timestamp in full nanosecond precision. */
        const RCH_NANOTIMER_TIMESTAMP now () const
        {
            return std::chrono::high_resolution_clock::now();
        }
        
        /** Selects and returns (scaled, not formatted) a specific split time event by its numerical index.
            The index Number can be negative to select an index counting from the back. (-1 for last) */
        const RCH_NANOTIMER_DATATYPE time (const int& Number) const
        {
            // If there are no split time events saved anyway, including start or stop
            if (numSplits() == 0)
            {
                // Exit early
                return zero();
            }
            
            // Handle special cases for start and stop splits directly
            if (Number == 0)
            {
                return scale(splits.front().time);
            }
            else if (Number == -1)
            {
                return scale(splits.back().time);
            }
            
            // Since the index Number can be negative, figure out an actual positive array index
            const int splitIndex = Number + (Number < 0) * numSplits();
            
            // If a valid and correctly bounded split index was found
            if ((splitIndex > -1) && (splitIndex < numSplits()))
            {
                // Return the scaled (not formatted) timestamp of the split at the found index
                return scale(splits[splitIndex].time);
            }
            
            // If no split record was found at the queried index, then return zero as default
            return zero();
        }
        
        /** Looks through all saved split time events and searches for a specific string ID.
            Returns the split's scaled (not formatted) timestamp value, if found. */
        const RCH_NANOTIMER_DATATYPE time (std::string SplitID) const
        {
            // If there are no split time events saved anyway, including start or stop
            if (numSplits() == 0)
            {
                // Exit early
                return zero();
            }
            
            // Fetch the split index for this split ID
            const int index = indexFromSplitID(SplitID);
            
            // If a valid index for this split ID was found
            if (index > -1)
            {
                return time(index);
            }
            
            // If no split record was found to match the queried ID, then return zero as default
            return zero();
        }
        
// ------------------------------------------------------------------------------------------
// TIME INTERVAL - START TO STOP
        
        /** Returns the scaled (not formatted) time difference between the start and stop
            split time events. If this is called while the timer is still running, the current
            timestamp will be used as a substitute for the (missing) stop split timestamp. */
        const RCH_NANOTIMER_DATATYPE interval () const
        {
            // Values need to be scaled before summing to avoid floating point precision loss
            const RCH_NANOTIMER_DATATYPE sNow = scale(now());
            const RCH_NANOTIMER_DATATYPE sStart = scale(splits.front().time);
            const RCH_NANOTIMER_DATATYPE sStop = scale(splits.back().time);
            
            // Return the calculated interval in scaled (not formatted) form
            return (running == true) * (sNow - sStart) + (running == false) * (sStop - sStart);
        }
        
// ------------------------------------------------------------------------------------------
// TIME INTERVAL - START TO SPLIT
        
        /** Returns the scaled (not formatted) time difference between the
            start split time event and a passed split time event index number.
            Negative numbers are possible to count from the back of the split time event storage. */
        const RCH_NANOTIMER_DATATYPE interval (const int& EndSplitIndex) const
        {
            // If currently no split records, or queried split event is (theoretical) start split event
            if ((numSplits() == 0) or (EndSplitIndex == 0))
            {
                // Return zero as default
                return zero();
            }
            // If the queried split event is the stop split event
            else if (EndSplitIndex == -1)
            {
                // Take the easy way out and return the full start-to-end interval
                return interval();
            }
            
            // Fetch the already scaled (not formatted) timestamp value for the passed split index
            const RCH_NANOTIMER_DATATYPE splitTime = time(EndSplitIndex);
            
            // Only if the time() call found and returned a valid timestamp for the queried split index
            if (splitTime > 0)
            {
                // Return the scaled (not formatted) time difference between
                // the start split and the split at the queried index.
                return splitTime - scale(splits.front().time);
            }
            
            // If no valid time interval could be generated from the queried index, return zero as default
            return zero();
        }
        
        /** Returns the scaled (not formatted) time difference between the start split
            and another split time event matching the passed split ID string. */
        const RCH_NANOTIMER_DATATYPE interval (std::string SplitID) const
        {
            // If there are no split time events saved anyway, including start or stop
            if (numSplits() == 0)
            {
                // Exit early
                return zero();
            }
            
            // Try to find an index for the passed split ID
            const int index = indexFromSplitID(SplitID);
            
            // If a valid index for the passed split ID was found
            if (index > -1)
            {
                // Return the interval between start and split ID
                return interval(index);
            }
            
            // Since no valid index was found so far, initialize empty variables for split time event indices
            int index1 = -1;
            int index2 = -1;
            
            // If queried ID was not that of start or stop splits, cycle through all splits
            for (unsigned int item=0; item<numSplits(); ++item)
            {
                // If this split time event's ID matches the start ID for the queried string
                if (splits[item].name == "start " + SplitID)
                {
                    index1 = item;
                }
                // If this split time event's ID matches the stop ID for the queried string
                else if (splits[item].name == "stop " + SplitID)
                {
                    index2 = item;
                }
                
                // If two valid indices were found already
                if ((index1 > -1) && (index2 > -1))
                {
                    // Fetch and return the scaled (not formatted) interval between the two indices
                    return interval(index1,index2);
                }
            }
            
            // If no valid time interval could be generated from the queried index, return zero as default
            return zero();
        }
        
// ------------------------------------------------------------------------------------------
// TIME INTERVAL - SPLIT TO SPLIT
        
        /** Returns the scaled (not formatted) time difference between two split time event records.
            The split time events are located by numbered index values.
            Negative numbers are possible to count from the back of the split time event storage. */
        const RCH_NANOTIMER_DATATYPE interval (const int& Split, const int& OtherSplit) const
        {
            // If there are no split time events saved anyway, including start or stop
            if (numSplits() == 0)
            {
                // Exit early
                return zero();
            }
            
            // Filter out the special case where start and stop/last index are passed
            // Irrelevant to check if both are different, (0,0) or (-1,-1) are OK to just call interval()
            if (// If the requested first split time event index points to first or (also past) last index
                ((Split == 0) or (Split == -1) or (Split >= numSplits()))
                and// If the requested other split time event index points to first or (also past) last index
                ((OtherSplit == 0) or (OtherSplit == -1) or (OtherSplit >= numSplits()))
                )
            {   // Since one split index is start split and one index is stop split,
                // take the easy way out and just return the full start-to-stop interval
                return interval();
            }
            
            // Since the Split and OtherSplit indices can be negative, figure out their true positive indexable values
            const int split1 = (Split > -1) * Split + (Split < 0) * (numSplits() + Split);
            const int split2 = (OtherSplit > -1) * OtherSplit + (OtherSplit < 0) * (numSplits() + OtherSplit);
            
            // If both Split and OtherSplit indices are within bounds of available split time events
            if ((split1 > -1) and (split1 < numSplits()) and (split2 > -1) and (split2 < numSplits()))
            {
                // Fetch the scaled (not formatted) timestamps for the two found indices, ordered by index number
                const RCH_NANOTIMER_DATATYPE splitTimeLater = time(std::max(split1,split2));
                const RCH_NANOTIMER_DATATYPE splitTimeEarlier = time(std::min(split1,split2));
                
                // Return the scaled (not formatted) time difference between the
                // larger/later split time event and the smaller/earlier split time event.
                return static_cast<RCH_NANOTIMER_DATATYPE>(splitTimeLater - splitTimeEarlier);
            }
            
            // If no valid time interval could be evaluated from the queried indices, return zero as default
            return zero();
        }
        
        /** Returns the scaled (not formatted) time difference between two split time event records.
            The split time events are located by their string IDs/names. */
        const RCH_NANOTIMER_DATATYPE interval (std::string SplitID, std::string OtherSplitID)
        {
            // If there are no split time events saved anyway, including start or stop
            if (numSplits() == 0)
            {
                // Exit early
                return zero();
            }
            
            // Make the ID arguments lowercase to match the saved ID values
            std::transform(SplitID.begin(), SplitID.end(), SplitID.begin(), ::tolower);
            std::transform(OtherSplitID.begin(), OtherSplitID.end(), OtherSplitID.begin(), ::tolower);
            
            // Try to get the indices for both passed split IDs
            int split1 = indexFromSplitID(SplitID);
            int split2 = indexFromSplitID(OtherSplitID);
            
            // If both evaluated split time event indices are within a legitimate range
            if ((split1 > -1) and (split1 < numSplits()) and (split2 > -1) and (split2 < numSplits()))
            {
                // Fetch the scaled (not formatted) timestamps for the two found indices, ordered by index number
                const RCH_NANOTIMER_DATATYPE splitTimeLater = time(std::max(split1,split2));
                const RCH_NANOTIMER_DATATYPE splitTimeEarlier = time(std::min(split1,split2));
                
                // Return the scaled (not formatted) time difference between the
                // larger/later split time event and the smaller/earlier split time event.
                return static_cast<RCH_NANOTIMER_DATATYPE>(splitTimeLater - splitTimeEarlier);
            }
            
            // If no valid time interval could be evaluated from the queried IDs, return zero as default
            return zero();
        }
        
        const RCH_NANOTIMER_DATATYPE interval (std::string SplitID, const int& OtherSplitIndex)
        {
            // Find the Index for the named split
            int splitIndex1 = indexFromSplitID(SplitID);
            
            // If no valid index for the named split was found
            if (splitIndex1 == -1)
            {
                return zero();
            }
            
            // If a valid index was found
            return interval(splitIndex1, OtherSplitIndex);
        }
        
        const RCH_NANOTIMER_DATATYPE interval (const int& SplitIndex, std::string OtherSplitID)
        {
            // Find the Index for the named split
            int splitIndex2 = indexFromSplitID(OtherSplitID);
            
            // If no valid index for the named split was found
            if (splitIndex2 == -1)
            {
                return zero();
            }
            
            // If a valid index was found
            return interval(SplitIndex, splitIndex2);
        }
        
// ------------------------------------------------------------------------------------------
// BENCHMARKING - ANONYMOUS AND DISCRETE
// Use these methods to do right here, right now testing, start and stop the timer immediately.
        
        /** Runs a function passed via pointer argument and measures its execution time.
            Stop the timer completely after finishing, does not add intermediate split times.
            This method requires an ID string to identify this benchmark stage later on.
            The return value of this function will be the timed interval (start to end),
            i.e. how much time passed between the function's start and its termination.
            The benchmark function will not forward any values that may be returned from
            the timed function. After the benchmark function finishes, the timer will have
            split time entries with the start and stop timestamps. */
        const RCH_NANOTIMER_DATATYPE benchmark (void(* Function)())
        {
            // Reset and start the timer
            start();
            
            // Run the function that should be benchmarked
            Function();
            
            // Stop the timer but don't reset the values
            stop();
            
            // Return the time interval that passed between start and stop
            return interval();
        }
        
        /** Runs a function passed via pointer argument and measures its execution time.
            Stop the timer completely after finishing, does not add intermediate split times.
            The return value from the executed function will be forwarded as the return
            value of this function. After the benchmark function finishes, the timer will
            have split time entries with the start and stop timestamps. */
        template<class RETURNTYPE>
        const RETURNTYPE benchmark (RETURNTYPE(* Function)())
        {
            // Reset and start the timer
            start();
            
            // Run the function that should be benchmarked
            RETURNTYPE result = Function();
            
            // Stop the timer but don't reset the values
            stop();
            
            // Forward the result that was returned from the benchmarked function
            return result;
        }
        
// ------------------------------------------------------------------------------------------
// BENCHMARKING - NAMED AND SEQUENTIAL
// Use these methods to do progressive, staged measurements, adding to a continuously running timer.
        
        /** Runs a function passed via pointer argument and measures its execution time.
            Does NOT stop the timer after finishing, only adds splits for its own start/stop times.
            This method requires an ID string to identify this benchmark stage later on.
            The return value of this function will be the timed interval (start to end),
            i.e. how much time passed between the function's start and its termination.
            The benchmark function will not forward any values that may be returned from
            the timed function. */
        const RCH_NANOTIMER_DATATYPE benchmark (void(* Function)(), std::string SplitStageID)
        {
            // If the timer isn't running yet
            if (running == false)
            {
                // Reset and start the timer
                start("start " + SplitStageID);
            }
            // If the timer was already running
            else
            {
                // Add a new split for the start time of this stage
                split("start " + SplitStageID);
            }
            
            // Run the function that should be benchmarked
            Function();
            
            // Add a new split for the stop time of this stage
            split("stop " + SplitStageID);
            
            // Return the time interval that passed between start and stop of this stage
            return interval("start " + SplitStageID, "stop " + SplitStageID);
        }
        
        /** Runs a function passed via pointer argument and measures its execution time.
            Does NOT stop the timer after finishing, only adds splits for its own start/stop times.
            This method requires an ID string to identify this benchmark stage later on.
            The return value from the executed function will be forwarded as the return
            value of this function. After the benchmark function finishes, the timer will
            have split time entries with the start and stop timestamps. */
        template<class RETURNTYPE>
        const RETURNTYPE benchmark (RETURNTYPE(* Function)(), std::string SplitStageID)
        {
            // If the timer isn't running yet
            if (running == false)
            {
                // Reset and start the timer
                start("start " + SplitStageID);
            }
            // If the timer was already running
            else
            {
                // Add a new split for the start time of this stage
                split("start " + SplitStageID);
            }
            
            // Run the function that should be benchmarked
            RETURNTYPE result = Function();
            
            // Add a new split for the stop time of this stage
            split("stop " + SplitStageID);
            
            // Forward the result that was returned from the benchmarked function
            return result;
        }
        
// ------------------------------------------------------------------------------------------
// EXPORT THE CURRENTLY STORED SPLIT TIMELINE
        
        /** Fills one referenced vector of strings with all IDs/names and fills another vector of numbers
            with timestamps of all split time events that are currently stored for this timer.
            The referenced arrays will be resized to fit. If no split time events are currently in
            storage, then the two referenced vectors will become cleared. */
        void list (std::vector<std::string>& IDs, std::vector<RCH_NANOTIMER_DATATYPE> Timestamps)
        {
            IDs.resize(numSplits());
            Timestamps.resize(numSplits());
            
            // Cycle through all currently stored split time events
            for (unsigned int item=0; item<numSplits(); ++item)
            {
                // Add the current split time ID/name into the IDs array
                IDs.push_back(splits[item].name);
                
                // Add the current split timestamp into the Timestamps array
                Timestamps.push_back(scale(splits[item].time));
            }
        }
        
        /** Returns a vector array that contains the timestamps of all split time events
            currently stored for this timer. If no split time events are in storage, the
            return value will be an empty vector array. */
        const std::vector<RCH_NANOTIMER_DATATYPE> listTimes () const
        {
            // Initialize the empty vector array that will be returned
            std::vector<RCH_NANOTIMER_DATATYPE> data;
            
            // Cycle through all currently stored split time events
            for (unsigned int item=0; item<numSplits(); ++item)
            {
                // Add the current split timestamp into the return vector array
                data.push_back(scale(splits[item].time));
            }
            
            // Return the populated vector array
            // Will be a vector with 0 entries if currently no split time events stored
            return data;
        }
        
        /** Returns a vector array that contains the IDs/names of all split time events
            currently stored for this timer. If no split time events are in storage, the
            return value will be an empty vector array. */
        const std::vector<std::string> listIds () const
        {
            // Initialize the empty vector array that will be returned
            std::vector<std::string> data;
            
            // Cycle through all currently stored split time events
            for (unsigned int item=0; item<numSplits(); ++item)
            {
                // Add the current split time ID/name into the return vector array
                data.push_back(splits[item].name);
            }
            
            // Return the populated vector array
            // Will be a vector with 0 entries if currently no split time events stored
            return data;
        }
        
    private:
        
// ------------------------------------------------------------------------------------------
// INTERNAL SECTION - NOT FOR USE OR ACCESS FROM OUTSIDE
        
        // Internally used data type for name:time records of captured split time events
        struct NanoSplit
        {
            // Convenience constructor for inline instantiation --> NanoSplit variable ("ID",123456);
            NanoSplit (const std::string& Name, const RCH_NANOTIMER_TIMESTAMP& NanoTime) : name(Name), time(NanoTime) {}
            
            // The name of the captured split time event
            const std::string name;
            
            // The full precision timestamp of the captured split time event
            const RCH_NANOTIMER_TIMESTAMP time;
            
        }; // end struct NanoSplit
        
        // Table that holds value output formatting options
        struct NanoFormatting
        {
            // How many floating point values to keep in format() calls
            unsigned int precision = RCH_NANOTIMER_DEFAULT_PRECISION;
            
            // Should a timebase label be added to the value in format() calls
            bool label = RCH_NANOTIMER_DEFAULT_DISPLAYLABEL;
            
        } formatting;
        
        // This is used to keep track if the timer is currently running or stopped.
        bool running = false;
        
        // The "database" that stores all time captured split time events from (and including) start to stop.
        std::vector<NanoSplit> splits;
        
        // Convenience formatter to avoid compiler warnings about type mismatch between int / iterator.
        const int numSplits () const
        {
            // Must be int and not unsigned int, since unsigned int would cause trouble with > -1 conditions
            return static_cast<int>(splits.size());
        }
        
        // Convenience function to always return zero values cast to the correct type
        template <typename RETURNTYPE=RCH_NANOTIMER_DATATYPE>
        const RETURNTYPE zero () const
        {
            return static_cast<RETURNTYPE>(0);
        }
        
        // Convenience macro to scale full precision time stamps into higher timebase timestamps (ns to µs etc.)
        template <typename RETURNTYPE=RCH_NANOTIMER_DATATYPE>
        const RETURNTYPE scale (const RCH_NANOTIMER_TIMESTAMP& Time) const
        {
            return (1e-9 * std::chrono::time_point_cast<std::chrono::nanoseconds>(Time).time_since_epoch().count()) / timebase.factor();
        }
        
        // Returns -1 if no split with this ID found, returns number index if split with this ID found
        const int indexFromSplitID (std::string SplitID) const
        {
            // Default --> no split time with this ID found
            int foundIndex = -1;
            
            // If there are no split time events saved anyway, including start or stop
            if (numSplits() == 0)
            {
                // Exit early
                return foundIndex;
            }
            
            // Make the ID argument lowercase to match the saved ID values
            std::transform(SplitID.begin(), SplitID.end(), SplitID.begin(), ::tolower);
            
            // Handle special case for start split directly
            if ((SplitID == "start") or (SplitID == "begin") or (SplitID == splits.front().name))
            {
                foundIndex = 0;
            }
            // Handle special case for stop split directly
            else if ((SplitID == "stop") or (SplitID == "end") or (SplitID == splits.back().name))
            {
                foundIndex = numSplits() - 1;
            }
            
            // If queried ID was not that of start or stop splits, cycle through all splits
            for (unsigned int item=0; item<numSplits(); ++item)
            {
                // Check if the current split's name matches the ID we're looking for
                if (splits[item].name == SplitID)
                {
                    foundIndex = item;
                }
            }
            
            return foundIndex;
        }
        
    }; // end class RCH::NanoTimer
    
} // end namespace RCH


// Just cleaning up
#undef RCH_NANOTIMER_TIMESTAMP
#undef RCH_NANOTIMER_DATATYPE
#undef RCH_NANOTIMER_NANO
#undef RCH_NANOTIMER_MCRO
#undef RCH_NANOTIMER_MILL
#undef RCH_NANOTIMER_SECS
#undef RCH_NANOTIMER_LABELS_NANOSECONDS
#undef RCH_NANOTIMER_LABELS_MICROSECONDS
#undef RCH_NANOTIMER_LABELS_MILLISECONDS
#undef RCH_NANOTIMER_LABELS_SECONDS
#undef RCH_NANOTIMER_DEFAULT_PRECISION
#undef RCH_NANOTIMER_DEFAULT_DISPLAYLABEL


// ---- MODULE CODE ENDS ABOVE ---- //
#endif // #ifndef RCHNANOTIMER_H_INCLUDED
