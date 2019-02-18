# RCH::NanoTimer
#### Nanosecond precision benchmarking timer in pure C++

-------------------------------------------------------------------------------------------------------

## Index

1. [Introduction](https://github.com/rcliftonharvey/rchnanotimer/blob/master/README.md#introduction)
2. [How to use](https://github.com/rcliftonharvey/rchnanotimer/blob/master/README.md#how-to-use)
3. [Benchmarking](https://github.com/rcliftonharvey/rchnanotimer/blob/master/README.md#benchmarking)
4. [Example project](https://github.com/rcliftonharvey/rchnanotimer/blob/master/README.md#example-project)
5. [License](https://github.com/rcliftonharvey/rchnanotimer/blob/master/README.md#license)

-------------------------------------------------------------------------------------------------------

## Introduction 

This class implements a high-precision timer in pure C++, intended for benchmarking code sections or entire functions.

The behaviour of the NanoTimer is based on that of a real-world stopwatch: besides **starting**, **stopping** and **resetting** the timer, it's also possible to save **split time events** while the timer is running. This makes it possible to store, poll and compare any number of time intervals on the way,  not just the start and stop times.

The timebase for any retrieved values is *nanoseconds* by default, but can be switched to *microseconds*, *milliseconds*, *seconds*, *minutes* and even *hours* on the fly.

The NanoTimer provides special benchmarking methods to run and time external functions.

This class was written with no dependencies, which means you can use it by itself and in any application, without being bound to any 3rd party frameworks.

I have done my best to comment the sources as much as I could (or made sense to me), so by reading through the headers you should get a good enough idea of what things can do or what you need to do with them.

The original repository for RCH::NanoTimer is right here:<br>
[https://github.com/rcliftonharvey/rchnanotimer]

-------------------------------------------------------------------------------------------------------

## How to use

Include the [header file](https://github.com/rcliftonharvey/rchnanotimer/tree/master/include/rchnanotimer.h) folder in your project.

Depending on your compiler and workspace settings, you will probably have to adjust the include path for this file. But once the include path is sorted out, this is the only line you should need:
```c++
#include "rchnanotimer.h"
```

To avoid possible collisions with other libraries you may be using in your projects, the NanoTimer class resides in the **RCH::** namespace by default. You're of course welcome to change the namespace in the [rchnanotimer.h](https://github.com/rcliftonharvey/rchnanotimer/tree/master/include/rchnanotimer.h) main include.

Since the NanoTimer is not stateful, you can instantiate it anywhere you want: in your header or implementation files, on the fly, when and where you need it. To get started, go ahead and instantiate a NanoTimer class anywhere in your code.
```c++
RCH::NanoTimer nano;
```

That's it. Now for the interesting part... measuring things!
```c++
nano.start();           // Discards previous values and re/starts the timer
/* Do processing here */
nano.stop();            // Halts the timer, but holds collected events in memory
/* Do evaluating here */
nano.reset();           // Halts the timer and removes any events from its memory 
```

To measure a sequence of events and evaluate the intervals later, without losing previously measured intervals by restarting the timer over and over again, the NanoTimer offers what is called **splits**. A split will log a timer event along the way, but the timer will continue running until it is explicitly stopped, reset or restarted.
```c++
nano.start();           // Discards previous values and re/starts the timer
/* Do processing step 1 here */
nano.split();           // Logs the first split time without stopping the timer
/* Any amount of time of irrelevant processing passes */
nano.split();           // Logs the second split time without stopping the timer
/* Do processing step 2 here */
nano.split();           // Logs the third split time without stopping the timer
/* Any amount of time of irrelevant processing passes */
nano.split();           // Logs the fourth split time without stopping the timer
/* Do processing step 3 here */
nano.stop();            // Halts the timer, but holds the measured times in memory 
```

> Note that despite me mentioning a **running** timer, there is **no** background thread chewing up CPU or anything. The term **running** quite simply refers to an internal *state* of which various timer functionality needs to be aware. The NanoTimer only picks timestamp samples from the timer that's running on your system anyway, but there is nothing like a counter variable that would keep continuously incrementing, only to eventually run your machine out of memory if you wait long enough. None of that. It's just a *state* variable.

Theoretically, after stopping the timer, it would now be possible to fetch individual split times already, as well as intervals between any two individual split times. Individual split times (including start and stop events) are addressed **per index**, so **the first event is the 0-th** event, *not* the 1st. Split items can also be addressed with **negative indices**, which selects the events counting from the back of the event list.
```c++
// Fetch unformatted timestamps (scaled to timebase)
nano.time(0);           // Timestamp of start event
nano.time(2);           // Timestamp of 2nd split
nano.time(-1);          // Timestamp of stop event

// Fetch unformatted intervals (scaled to timebase)
nano.interval();        // Full timer interval, time it took from start to end
nano.interval(1);       // Time passed between start event and first logged split
nano.interval(3);       // Time passed between start event and third logged split
nano.interval(2,3);     // Time passed between second and third split events
nano.interval(-2,-1);   // Time passed between fourth event split and stop event
```

Working with numbered index addressing is fine when evaluating timings with automated loops, but it can grow cumbersome and abstract fairly quickly. For this reason, any split event (including start and stop events) can be named with an **ID** of type **std::string** at the time they're stored, making it possible to later address and select them with their IDs again. The *start* and *stop* events will automatically be addressable with the IDs *"start"* and *"stop"*.
```c++
nano.start("demo start"); // Selectable with "start" even if ID given    
/* Do processing step 1 here */
nano.split("stop process 1");
/* Any amount of time of irrelevant processing passes */
nano.split("start process 2");
/* Do processing step 2 here */
nano.split("stop process 2");
/* Any amount of time of irrelevant processing passes */
nano.split("broken yet?");
/* Do processing step 3 here */
nano.stop();              // Selectable with "stop" even if no ID given 
```

Already makes more sense, eh? Now let's see how to fetch those times and intervals with IDs...
```c++
// Fetch unformatted timestamps (scaled to timebase)
nano.time("start");     // Works in addition to ID "demo start" that was assigned
nano.time("demo start");
nano.time("stop process 2");
nano.time("stop");

// Fetch unformatted intervals (scaled to timebase)
nano.interval("stop process 1");  // From start event to named split event
nano.interval("start process 2"); // From start event to named split event
nano.interval("start process 2","stop process 2"); // Pretty evident already 
nano.interval("broken yet?","stop");   // From last split to stop event 
```

> It's also possible to select split intervals by combining **ID** and **index** in one call.

Now... timestamps like *1796640.000000* are cool and all, but they don't look like much. So the NanoTimer class brings its own little timestamp formatting feature that allows to **define how many post comma decimal numbers** should be displayed, and whether or not the **appropriate time symbol** (*ns*, *µs*, *ms*, *s*, *min*, *h*) should be added after the actual value. It doesn't matter if you pass in an external variable holding a timestamp, or if you apply the formatting to the result of a **time()** or **interval()** call directly.
```c++
// Fetch a timer interval and store it in a variable
double timestampInterval = nano.interval();
// ==> 1796640.000000

// Format the fetched numerical timestamp into a readable string
std::string formattedInterval = nano.format( timestampInterval );
// ==> 1796640.00 ns

// Fetch and format a timestamp in one go
std::string formattedTimestamp = nano.format( nano.time("stop") );
// ==> 1796640.00 ns
```

If you're not happy with the default format settings (*nanoseconds*, *6 decimal numbers*, *added value label*), then you can adjust them to your liking by calling the NanoTimer method **format()**. The first argument for this method is the **amount of decimal numbers** to keep, note that the actual maximum precision might vary depending on your platform. The second argument for this method will set whether or not the **appropriate time symbol** is added to the end of the formatted string.
```c++
// Timestamp formatting
nano.format(4,false);    // format(1796640.000000) will output --> 1796640.0000
nano.format(0,true);     // format(1796640.000000) will output --> 1796640 ns

// Timestamp timebase
nano.timebase.nanoseconds();    // time() and interval() now return  ns timestamps
nano.timebase.microseconds();   // time() and interval() now return  µs timestamps
nano.timebase.milliseconds();   // time() and interval() now return  ms timestamps
nano.timebase.seconds();        // time() and interval() now return   s timestamps
nano.timebase.minutes();        // time() and interval() now return min timestamps
nano.timebase.hours();          // time() and interval() now return   h timestamps

// Switch to milliseconds, 3 decimals, added symbol label
nano.timebase.milliseconds();
nano.format(3,true);
nano.format(1796640.000000);
// ==> 1.797 ms   (rounded up)
nano.format(1796640.000000,5,false);
// ==> 1.79664
```

Formatted timestamp values will always be of type **std::string**, while raw and unformatted timestamps will be in the NanoTimer's specified data type, which is set to **double** by default.

The timestamps will always be stored and kept in their **full nanosecond precision**, so you can switch back and forth between coarser and finer timebases at any point in the benchmarking process *without altering the precision* of the timed events.
```c++
nano.timebase.nanoseconds();
nano.start();                   // 1894086.000000 ns
nano.split();                   // 1899765.000000 ns
nano.timebase.seconds();
nano.split();                   // 0.001903 s           @ time(2)
nano.timebase.microseconds();
nano.stop();                    // 1912.687000 µs
nano.timebase.milliseonds();
nano.time(2);                   // 1.903675 ms          @ time(2)
nano.timebase.nanoseconds();
nano.time(2);                   // 1903675.000000 ns    @ time(2)
```

Instead of making you pick out individual times and intervals to get the full picture, the NanoTimer can also **export its entire content** in one go, in various ways. This allows fast access to all available data from outside the timer instance at any stage of the timing cycle.
```c++
std::vector<std::string> splitIds;      // External string array that will be filled later
std::vector<double>      timestamps;    // External double array that will be filled later

// Exports IDs and timestamps of all stored events to the external arrays
nano.list(splitIds, timestamps);

// Exports only the timestamps of all stored events to the external double array
timestamps = nano.listTimes();

// Exports only the IDs of all stored events to the external string array
splitIds = nano.listIds();
```

And that's just about all there's to it.
Just about, because...

-------------------------------------------------------------------------------------------------------

## Benchmarking

Since I'm such a lazy guy, I built in functionality to take most of the grunt work out of my hands, like managing timer starts and stops and split events between them.  I call this the **benchmarking** workflow, because it makes the process of timing and comparing functions conveniently easy. The **benchmark()** methods in NanoTimer allow conducting individual anonymous and discrete timing runs, as well as continuous timing runs with several named stages.

The mechanics of the benchmarking methods are easily explained: 
- call the *benchmark()* method
- pass a function pointer into it
- get back the (raw) interval timestamp of how long it took to process the passed function

Here's an example of a **discrete timing run** for an external function.
```c++
// Exemple external function we'll be benchmarking, return type VOID and NO arguments
void doSomething ()
{
    for (unsigned int counter=0; counter<1000; /* void */)
    {
        ++counter;
    }
}

// Create a timer instance with 1 decimal and added timebase symbol, switch to microseconds.
RCH::NanoTimer nano (1, true);
nano.timebase.microseconds();

// Here we pass the function name (without parentheses) into the benchmark call, this
// will start the timer, run the function, stop the timer and save all event timestamps.
// Since a *void* function without return value is timed, the return benchmark() method
// will automatically return the interval between its start and stop times.
double interval = nano.benchmark( doSomething ); 
// ==> 1820.855988

// Turn the raw timestamp into a formatted string
std::string formatted = nano.format( interval );
// ==> 1820.9 µs   (rounded up)

// Or do it all in one line :)
std::string formatted = nano.format ( nano.benchmark( doSomething ) );
// ==> 1820.9 µs   (rounded up)
```

It's possible to discretely benchmark external functions that **do** have a **return value**, the benchmarking function should adapt to the return type automatically. When benchmarking an external function with a return value, the benchmarking function will **not** return the interval timestamp of the timer run, but it will forward out the value returned from the benchmarked function. However, the timer instance will still have the start and stop time events stored for later access. 
```c++
// The external function we'll be benchmarking, DOES have a return type but NO arguments
unsigned int doSomething1 ()
{
    unsigned int counter;

    for (counter=0; counter<1000; /* void */)
    {
        ++counter;
    }
    
    return counter;
}

// Create the timer instance
RCH::NanoTimer nano (1, true);
nano.timebase.microseconds();

// Run the benchmark, this will output the timed function's return value
unsigned int returnValue = nano.benchmark( doSomething1 ); 
// ==> 1000 ... this is what the tested function returned, it's NOT a timestamp

std::string formatted = nano.format( nano.interval() );
// ==> 1820.9 µs   (rounded up)
```

Up until now, the benchmarking methods *started a timer, ran a function and stopped the timer* again. Doing two runs like that with the same timer instance would mean *the second timing run clears and overwrites the time events of the first run*. Keeping track and managing the results would have to happen externally in your code in between timing tuns. And to keep several results available, until all timing runs are done, would require several timer instances and evaluating the results of those several different instances... way too much work.

So to the rescue comes **named and sequential** benchmarking. This means timing runs are no longer anonymous and discrete, but they have IDs that help identify their timing results in an ongoing list of split time events. What that means is...

The **first call** to a named benchmarking method will *start the timer instance*, any **consecutive calls** to a named benchmarking method will *add split time events* for their start and stop timestamps, but **not** restart or stop the timer. It's necessary to **explicitly stop** the timer after all timing stages are done. At that point, all timestamps and their IDs will be available to poll or list/export as desired for further evaluation.

Here's an example run where *three different external functions* are all benchmarked using the *same timer instance*. This will accumulate and store the results of all timed stages between *calling benchmark() for the first time* and finally *stopping the timer*. This can obviously be extended by just calling more named benchmarking methods.

> **Note** that the example below focuses on the timing aspect, not on handling and formatting return values.
```c++
// Example external function to benchmark
void wasteResources () { for (int i=0; i<10000; ++i) {/* some calculation */} }

// Set up the timer instance
RCH::NanoTimer nano (2,true);
nano.timebase.microseconds();

// Run a sequence of benchmarks on the same timer
nano.benchmark(wasteResources,"waster");    // Starts the timer
nano.benchmark(std::getchar,  "getchar");   // Adds split times, doesn't restart
nano.benchmark(std::rand,     "random");    // Adds split times, doesn't restart
nano.stop();                                // Testing done, so halt timer

// Export the data for external processing
std::vector<std::string> benchIds ( nano.listIds() );
// ==> {"start waster","stop waster","start getchar", ... ,"stop random","stop"}

std::vector<double>    benchTimes ( nano.listTimes() );
// ==> {123456.000000,234567.000000,345678.000000,456789.000000, ... }

// Accessing individual stage intervals
double stage1 = nano.interval("start waster","stop waster");
double stage2 = nano.interval("getchar");   // looks for "start getchar" and "stop getchar"
std::string stage3 = nano.format( nano.interval( "start rand","stop rand" ) );
std::string overall = nano.format( nano.interval() );

// Of course, all timestamps and intervals can also be accessed through numbered indices
std::vector<double> timestamps;

for (int step=0; step<timestamps.end(); ++step)
{
    timestamps.push_back( nano.time(step) );
}
```

> Note that while a named sequence of benchmarks is being conducted, calling a regular non-named benchmark function only once will already totally reset the timer and clear all of the split time events collected thus far.

This should make it very easy to conduct, evaluate and even (externally) graph extensive timing tests with several modules and stages, but without managing a lot of timer instances or intermediate results. See the included demo project for an example of how to maybe do that.

-------------------------------------------------------------------------------------------------------

## Example project

For a real world example, check out the included [RCHNanoTimerDemo](https://github.com/rcliftonharvey/rchnanotimer/tree/master/demo) project. This demo project not only shows how to conduct and evaluate a full named and sequential benchmark, but it also attempts to settle an age-old argument: **Which kind of loop is faster?** Have a look into the [tests.h](https://github.com/rcliftonharvey/rchnanotimer/tree/master/demo/RCHNanoTimerDemo/tests.h) file to see what I mean. :)

Yes, the demo project is for XCode, I know, I know. Us evil Mac guys. But the C++ part is so simple that you should have no problem creating a non-XCode workspace from the (two...) files yourself and building it. 

-------------------------------------------------------------------------------------------------------

## License

This source code is provided under the [MIT License](https://github.com/rcliftonharvey/rchnanotimer/tree/master/LICENSE).<br>
A copy of the MIT license in written form comes in the download of this library.

-------------------------------------------------------------------------------------------------------

Enjoy!

Rob Clifton-Harvey
