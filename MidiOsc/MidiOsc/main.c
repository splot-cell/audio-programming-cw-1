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
    char *helpText[] = {//----------------------------single line character limit---|
        "OLLY'S WONDEROUS COURSEWORK SUBMISSION",
        "-- Help Documentation --",
        "",
        "This program will print the samples of a sine wave as floating point numbers",
        "to six decimal places. Simply enter two integers and watch it go! The format",
        "required is:",
        "",
        "<duration> <midi note number>",
        "",
        "The program accepts up to 100 pairs of integers, so you can play fun tunes",
        "such as the Family Guy theme song.",
        "",
        "Output will begin once the <midi note number> is set to a value less than 0."
    };
    int numLines = sizeof(helpText) / sizeof(helpText[0]);
    printWithBorder(helpText, numLines);
    exit(0);
}

void printWithBorder(char *message[], int rows) {
    int pad = 1, borderWidth = 3, numColumns = 80, numRows = rows  + (2*(pad+borderWidth));
        // Set up border parameters
    int r = 0, c = 0;
        // Set up indexes for rows and columns
    
    for(r = 0; r < numRows; ++r) {
        for(c = 0; c < numColumns; ++c) {
            if(r < borderWidth || r >= numRows - borderWidth ||
               c < borderWidth || c >= numColumns - borderWidth)
                // If we're in the border rows or columns
                printf("%c", '*');
            else if(r < pad + borderWidth || r >= numRows - borderWidth - pad ||
                    c < pad + borderWidth || c >= numColumns - borderWidth - pad)
                // If we're wthin the padding rows or columns
                printf("%c", ' ');
            else {  // We're in the rows and columns with potential text
                int centreOffset = (int) ((numColumns -
                                strlen(message[r - borderWidth - pad])) / 2) - pad - borderWidth;
                    // Integer divide remainig whitespace by 2
                
                if(c - borderWidth - pad < centreOffset)
                    printf("%c", ' ');
                else if(strlen(message[r-borderWidth-pad]) > c - centreOffset - borderWidth - pad)
                    // Checking for out-of-bounds index
                    printf("%c", message[r-borderWidth-pad][c - centreOffset - borderWidth - pad]);
                else // Fill in whitespace
                    printf("%c", ' ');
            }
        }
        printf("%c", '\n');
    }
}
