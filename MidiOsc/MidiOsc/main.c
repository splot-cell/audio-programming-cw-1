//
//  main.c
//  MidiOsc
//
//  Created by Olly Seber on 17/10/2017.
//  Copyright © 2017 Olly Seber. All rights reserved.
//

#include <stdio.h>      // For user inputs and printf.
#include <math.h>       // For sin() and fmod().


// GLOBAL VARIABLES
const int g_sampleRate = 48000;
const double g_pi = 3.14159265359;
const double g_tau = 2 * g_pi;


// FUNCTION PROTOTYPES
double midiToFrequency(int midiNote);
double printSamples(int duration, double frequency);
    // Takes duration in miliseconds and required oscillator frequency.
    // Returns angle in radians for calulating final sample.


int main(int argc, const char * argv[]) {
    
    printSamples(1, 249);
    printf("%.6f\n", sin(printSamples(1, 1020)));
    
    return 0;
}

double printSamples(int duration, double frequency) {
    
    static double lastRadianAngle = 0;
        // Angle is stored between function calls.
    
    for(int sampleIndex = 0; sampleIndex < duration * g_sampleRate / 1000; ++sampleIndex) {
        printf("%.6f\n", sin(fmod((g_tau * frequency  * sampleIndex / g_sampleRate) + lastRadianAngle, g_tau)));
        // Modulus radian value to avoid overflow.
    }
    
    // Set phase by taking the radian value used to calulate NEXT sample as this will be the starting
    // sample of the next oscillation.
    lastRadianAngle = fmod((g_tau * frequency  * (duration * g_sampleRate / 1000) / g_sampleRate) + lastRadianAngle, g_tau);
    
    return lastRadianAngle;
}

/*
    In order to avoid phase issues, must print last sample of previous frequency at beginning of next
    frequency. This means that there will be one un-printed sample after the last frequency has 'finished'
    printing. This must then be printed to ensure the correct number of samples are printed for each
    frequency.
*/
