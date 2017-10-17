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


// GLOBAL VARIABLES
const int g_sampleRate = 48000;
const double g_pi = 3.14159265359;
const double g_tau = 2 * g_pi;

#ifndef TEST
// FUNCTION PROTOTYPES
double midiToFrequency(int midiNote);
double printSamples(int duration, double frequency);
    // Takes duration in miliseconds and required oscillator frequency.
    // Returns angle in radians for calulating final sample.
double calculateAngle(long sampleIndex, double frequency, double lastRadianAngle);
    //
bool isOnlyInt(const char *string);
    // Given a string. Will return true if only ascii characters 0-9 are present, otherwise false.

int main(int argc, const char * argv[]) {
    
    printSamples(1, 249);
    printf("%.6f\n", sin(printSamples(1, 1020)));
    
    return 0;
}
#else
#include "../../UnitTests/test.h"
#endif

double printSamples(int duration, double frequency) {
    
    static double lastRadianAngle = 0;
        // Angle is stored between function calls.
    
    for(long sampleIndex = 0; sampleIndex < duration * g_sampleRate / 1000; ++sampleIndex) {
        printf("%.6f\n", sin(calculateAngle(sampleIndex, frequency, lastRadianAngle)));
    }
    
    // Store the radian value used to calulate NEXT sample as this will be the starting sample of
    // the next oscillation.
    lastRadianAngle = calculateAngle(duration * frequency / g_sampleRate,
                                     frequency, lastRadianAngle);
    return lastRadianAngle;
}

double calculateAngle(long sampleIndex, double frequency, double lastRadianAngle) {
    return fmod((g_tau * frequency * sampleIndex / g_sampleRate) + lastRadianAngle, g_tau);
}

/*
    In order to avoid phase issues, must print last sample of previous frequency at beginning of
    next frequency. This means that there will be one un-printed sample after the last frequency has
    'finished' printing. This must then be printed to ensure the correct number of samples are
    printed for each frequency.
*/

bool isOnlyInt(const char *string) {
    bool returnValue = true;
    for(int index = 0; index < (strlen(string) / sizeof(char)) && returnValue; ++index) {
        if(string[index] < 48 || 57 < string[index])
            returnValue = false;
    }
    return returnValue;
}
