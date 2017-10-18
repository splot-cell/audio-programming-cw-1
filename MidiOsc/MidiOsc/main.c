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
#include <string.h>     // For strlen() and strcmp().
#include <limits.h>     // For overflow checking.
#include <stdlib.h>     // For exit().


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
void error(const char *message, int code);
void detectHelp(const char *arguments[]);
void printHelp();
void printWithBorder(char *message[], int rows);

// STRUCT
struct Note {int duration; double frequency;};

int main(int argc, const char * argv[]) {
    if(argc == 2)
        detectHelp(argv);
    else if(argc > 2)
        error("Maximum of one command line argument accepted.\n", 1);
    
     
     
    
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
 *  In order to avoid phase issues, must print last sample of previous frequency at beginning of
 *  next frequency. This means that there will be one un-printed sample after the last frequency has
 *  'finished' printing. This must then be printed to ensure the correct number of samples are
 *  printed for each frequency.
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

void error(const char *message, int code) {
    printf("%s\n", message);
    exit(code);
}

void detectHelp(const char *arguments[]) {
    if(strcmp(arguments[1], "-help") == 0)
        printHelp();
    error("Format not recognised! Type \"-help\" for formatting specification.\n", 2);
}

void printHelp() {
    char *helpText[] = {
        "This is the first line of the help text",
        "and this is the second.",
        "This is a much longer line to see if I got the maths right, I certainly ho"
    };
    int numLines = sizeof(helpText) / sizeof(helpText[0]);
    printWithBorder(helpText, numLines);
    exit(0);
}

void printWithBorder(char *message[], int rows) {
    int numColumns = 80, pad = 1, numRows = rows + 2 + (2*pad); // Set up border parameters
    int r = 0, c = 0; // Set up indexes for rows and columns
    
    for(r = 0; r < numRows; ++r) {
        for(c = 0; c < numColumns; ++c) {
            if(r == 0 || r == numRows - 1 || c == 0 || c == numColumns - 1)
                // If we're in the first or last row, or first or last column
                printf("%c", '*');
            else if(r <= pad || r >= numRows - 1 - pad || c <= pad || c >= numColumns - 1 - pad)
                // If we're wthin the padding rows or columns
                printf("%c", ' ');
            else { // We're in the rows with text
                int centreOffset = (int) ((numColumns - strlen(message[r-1-pad])) / 2) - pad -1;
                    // Integer divide remainig whitespace by 2
                if(c - 1 - pad < centreOffset)
                    printf("%c", ' ');
                else if(strlen(message[r-1-pad]) > c - centreOffset - 1 - pad)
                    // Checking for out-of-bounds index
                    printf("%c", message[r-1-pad][c - centreOffset - 1 - pad]);
                else
                    printf("%c", ' ');
            }
        }
        printf("%c", '\n');
    }
}
