//
//  main.cpp
//  RCHNanoTimerDemo
//
//  Created by Rob on 28.05.18.
//  Copyright © 2018 Rob Clifton-Harvey. All rights reserved.
//
#include <iostream>

// Include the main NanoTimer header file into the project
#include "../../include/rchnanotimer.h"

// Just a few boring loop variations, adjust the LOOP_LIMIT
// in here if the loops don't run long enough for you. :)
#include "tests.h"


// Mainline
int main (int argc, const char * argv[])
{
// =================================================================================
// PREPARATION
    
    printf("\n\n");
    printf("  A quick n dirty demo program for RCH::NanoTimer\n\n");
    
    printf("  Attempting to answer the age old question:\n");
    printf("  > Which kind of loop executes faster?\n\n\n");
    
    // Set up a nanosecond timer with .xxxxxx precision and ns label
    RCH::NanoTimer nano (6,true);
    nano.timebase.nanoseconds();
    
// =================================================================================
// TESTING
    
    printf("Starting timing sequence...\n\n");
    printf("Stage 1... ");
    
    // Stage 1: clock 4 runs of the first test function
    nano.benchmark( loopForwardIncrementPost, "fwd post 1" );
    nano.benchmark( loopForwardIncrementPost, "fwd post 2" );
    nano.benchmark( loopForwardIncrementPost, "fwd post 3" );
    nano.benchmark( loopForwardIncrementPost, "fwd post 4" );
    
    printf("done @ %s\n",nano.format(nano.interval()).c_str());
    printf("Stage 2... ");
    
    // Stage 2: clock 4 runs of the second test function
    nano.benchmark( loopForwardIncrementPre, "fwd pre 1" );
    nano.benchmark( loopForwardIncrementPre, "fwd pre 2" );
    nano.benchmark( loopForwardIncrementPre, "fwd pre 3" );
    nano.benchmark( loopForwardIncrementPre, "fwd pre 4" );
    
    printf("done @ %s\n",nano.format(nano.interval()).c_str());
    printf("Stage 3... ");
    
    // Stage 3: clock 4 runs of the third test function
    nano.benchmark( loopBackwardDecrementPost, "bwd post 1" );
    nano.benchmark( loopBackwardDecrementPost, "bwd post 2" );
    nano.benchmark( loopBackwardDecrementPost, "bwd post 3" );
    nano.benchmark( loopBackwardDecrementPost, "bwd post 4" );
    
    printf("done @ %s\n",nano.format(nano.interval()).c_str());
    printf("Stage 4... ");
    
    // Stage 4: clock 4 runs of the fourth test function
    nano.benchmark( loopBackwardDecrementPre, "bwd pre 1" );
    nano.benchmark( loopBackwardDecrementPre, "bwd pre 2" );
    nano.benchmark( loopBackwardDecrementPre, "bwd pre 3" );
    nano.benchmark( loopBackwardDecrementPre, "bwd pre 4" );
    
    printf("done @ %s\n",nano.format(nano.interval()).c_str());
    
    // Halt the timer
    nano.stop();
    
    printf("\nTiming sequence completed.\n\n");
    
// =================================================================================
// RESULT PROCESSING
    
    // Array of accumulators to calculate averages of the 4 run times for each stage
    std::vector<double> stageAverages ({0.0, 0.0, 0.0, 0.0});
    
    // Buffer arrays to hold each stage's 4 run results string (formatted)
    std::vector<std::string> stage1;
    std::vector<std::string> stage2;
    std::vector<std::string> stage3;
    std::vector<std::string> stage4;
    
    // Array of formatted average times
    std::vector<std::string> avrg;
    
    // Cycle through 4 runs
    for (int run=1; run<=4; ++run)
    {
        // Put together the stage ID for each stage in this run
        std::string idStage1Run = "fwd post " + std::to_string( run );
        std::string idStage2Run = "fwd pre "  + std::to_string( run );
        std::string idStage3Run = "bwd post " + std::to_string( run );
        std::string idStage4Run = "bwd pre "  + std::to_string( run );
        
        // Array to temporarily buffer the raw and unformatted
        // interval times for this run number of all 4 stages
        std::vector<double> stageIntervals;
        
        // Get the interval times for this run for all 4 stages
        stageIntervals.push_back( nano.interval( idStage1Run ) );
        stageIntervals.push_back( nano.interval( idStage2Run ) );
        stageIntervals.push_back( nano.interval( idStage3Run ) );
        stageIntervals.push_back( nano.interval( idStage4Run ) );
        
        // Add the formatted interval times for this run into the stage result buffers
        stage1.push_back( nano.format( stageIntervals[0] ) );
        stage2.push_back( nano.format( stageIntervals[1] ) );
        stage3.push_back( nano.format( stageIntervals[2] ) );
        stage4.push_back( nano.format( stageIntervals[3] ) );
        
        // Add the current run interval time of each stage
        // to each stage's averaging accumulator
        stageAverages[0] += stageIntervals[0];
        stageAverages[1] += stageIntervals[1];
        stageAverages[2] += stageIntervals[2];
        stageAverages[3] += stageIntervals[3];
    }
    
    // Divide the summed run time for each stage by the number
    // of runs performed ==> gets average run time per stage
    stageAverages[0] /= 4;
    stageAverages[1] /= 4;
    stageAverages[2] /= 4;
    stageAverages[3] /= 4;
    
    // Format the stages' averaged times and store in string array
    avrg.push_back( nano.format( stageAverages[0] ) );
    avrg.push_back( nano.format( stageAverages[1] ) );
    avrg.push_back( nano.format( stageAverages[2] ) );
    avrg.push_back( nano.format( stageAverages[3] ) );
    
// =================================================================================
// EVALUATION
    
    // Now that we have all the data where we need it, "paint" the result table
    printf("\nEvaluation:\n");
    printf("===========\n");
    printf("\n");
    
    printf("Stage:             1              2              3              4\n");
    printf("Direction:         Forward        Forward        Backward       Backward\n");
    printf("Increment:         Post           Pre            Post           Pre\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("Run 1:             %s%s%s%s\n",get(stage1,0).c_str(),get(stage2,0).c_str(),get(stage3,0).c_str(),get(stage4,0).c_str());
    printf("Run 2:             %s%s%s%s\n",get(stage1,1).c_str(),get(stage2,1).c_str(),get(stage3,1).c_str(),get(stage4,1).c_str());
    printf("Run 3:             %s%s%s%s\n",get(stage1,2).c_str(),get(stage2,2).c_str(),get(stage3,2).c_str(),get(stage4,2).c_str());
    printf("Run 4:             %s%s%s%s\n",get(stage1,3).c_str(),get(stage2,3).c_str(),get(stage3,3).c_str(),get(stage4,3).c_str());
    printf("--------------------------------------------------------------------------------\n");
    printf("Average:           %s%s%s%s\n",get(avrg,0).c_str(),get(avrg,1).c_str(),get(avrg,2).c_str(),get(avrg,3).c_str());
    
    
    printf("\n");
    printf("\n");
    printf("Done.\n");
    printf("\n");
    printf("\n");
    
// =================================================================================
// END
    
    return 0;
    
} // end int main
