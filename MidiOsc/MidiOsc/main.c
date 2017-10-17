//
//  main.c
//  MidiOsc
//
//  Created by Olly Seber on 17/10/2017.
//  Copyright © 2017 Olly Seber. All rights reserved.
//

#include <stdio.h>      // For user inputs and printf.
#include <math.h>       // For sin() and fmod().
#include <stdbool.h>    // For booleans.
#include <string.h>     // For strlen().
#include <limits.h>     // For overflow checking.


// GLOBAL VARIABLES
const int g_sampleRate = 48000;
const double g_pi = 3.14159265359;
const double g_tau = 2 * g_pi;
const double g_referenceFrequency = 440; // Frequency of A above middle C.
const double g_referenceMidiNote = 69; // Midi note num of A above middle C.

#ifndef TEST
// FUNCTION PROTOTYPES
double midiToFrequency(const int midiNote);
double printSamples(int duration, double frequency);
    // Takes duration in miliseconds and required oscillator frequency.
    // Returns angle in radians for calulating final sample.
double calculateAngle(unsigned int sampleIndex, double frequency, double lastRadianAngle);
    //
bool isOnlyInt(const char *string);
    // Given a string. Will return true if only ascii characters 0-9 are present, otherwise false.
bool withinDurationLimit(const long duration);
    // Checks that duration will not cause overflow of sample index.

int main(int argc, const char * argv[]) {
    
    /*  checkHelp()
            print help, exit
        getInputLine
            correct num args?
        is int?
        is negative?
            yes = printSamples
            no = is less than 128?
                yes = add to array
        Loop
     
     
    */
    
    return 0;
}
#else
#include "../../UnitTests/test.h"
#endif

double printSamples(int duration, double frequency) {
    
    static double lastRadianAngle = 0;
        // Angle is stored between function calls.
    
    for(unsigned int sampleIndex = 0; sampleIndex < duration * g_sampleRate / 1000; ++sampleIndex) {
        printf("%.6f\n", sin(calculateAngle(sampleIndex, frequency, lastRadianAngle)));
    }
    
    // Store the radian value used to calulate NEXT sample as this will be the starting sample of
    // the next oscillation.
    lastRadianAngle = calculateAngle(duration * frequency / g_sampleRate,
                                     frequency, lastRadianAngle);
    return lastRadianAngle;
}

double calculateAngle(unsigned int sampleIndex, double frequency, double lastRadianAngle) {
    return fmod((g_tau * frequency * sampleIndex / g_sampleRate) + lastRadianAngle, g_tau);
}

/*
    In order to avoid phase issues, must print last sample of previous frequency at beginning of
    next frequency. This means that there will be one un-printed sample after the last frequency has
    'finished' printing. This must then be printed to ensure the correct number of samples are
    printed for each frequency.
*/

bool isOnlyInt(const char *string) {
    if(!string)
        return false;
    bool returnValue = true;
    for(int index = 0; index < (strlen(string) / sizeof(char)) && returnValue; ++index) {
        if(string[index] < 48 || 57 < string[index])
            returnValue = false;
    }
    return returnValue;
}

bool withinDurationLimit(const long duration) {
    long numSamples = duration * g_sampleRate / 1000;
    if(numSamples > UINT_MAX)
        return false;
    return true;
}

double midiToFrequency(const int midiNote) {
    return pow(2, (midiNote - g_referenceMidiNote)/12.) * g_referenceFrequency;
}
