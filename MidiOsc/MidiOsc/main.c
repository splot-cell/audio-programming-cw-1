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


/* GLOBAL VARIABLES */
const int g_sampleRate = 48000;
const double g_pi = 3.14159265359;
const double g_tau = 2 * g_pi;
const double g_referenceFrequency = 440; // Frequency of A above middle C.
const double g_referenceMidiNote = 69; // Midi note num of A above middle C.

/* STRUCTS */
struct Note {
    int duration;
    int midiNote;
};

/* ERROR MESSAGES */
enum ERR {NO_ERR, BAD_COMMAND_LINE, BAD_RUNTIME_ARG, OUT_OF_BOUNDS_VALUE};

#ifndef TEST
/* FUNCTION PROTOTYPES */
void commandLineArgHandler(int argc, const char *argv[]);
void detectHelp(const char *arguments[]);
void sendHelp();
void printWithBorder(char *message[], int rows);
/*
 *  Prints message with a border.
 *
 *  <message> is array of strings. Each string is a line to be printed.
 *  <rows> parameter is total number of lines.
 *
 *  Appearance can be set from within the funciton body.
 *  If the a string input is too long for the row it will be cut short.
 */
void populateNotes(struct Note *notes, int numberOfLines);
bool validateUserInput(char *userInputBuffer, int *timeStamp, int *midiNote);
void writeNoteData(struct Note *notes, int noteIndex, int timeStamp, int midiNote);
void timestampToDurationHandler(struct Note *notes, int noteIndex, int timeStamp);
double midiToFrequency(const int midiNote);
void printNotes(struct Note *notes);
double printNote(struct Note note);
    /*  IIMPROVE THESE COMMENTS
     *
     *  Takes duration in miliseconds and required oscillator frequency.
     *  Returns angle in radians for calulating final sample.
     */
double calculateAngle(unsigned int sampleIndex, double frequency, double lastRadianAngle);
    //
bool isOnlyInt(const char *string);
    /*
     *  Input a string from stdin.
     *  Will check that only ascii characters '0'-'9' are present.
     *  Provides more robust checking of user input than just scanf or sscanf.
     */
bool withinDurationLimit(const long duration);
    //  Checks that duration will not cause overflow of sample index.
void error(const char *message, int code);
    //  Helper function, prints error warning and exits program.

int main(int argc, const char * argv[]) {
    commandLineArgHandler(argc, argv);
    
    int numberOfLines = 100;
    struct Note notes[numberOfLines];
    populateNotes(notes, numberOfLines);
    
    // Incorporate strtok()
    // Check whether first note is <0
    

    return NO_ERR;
}
#else
#include "../../UnitTests/test.h"
#endif

void commandLineArgHandler(int argc, const char *argv[]) {
    if(argc > 2)
        error("Maximum of one runtime argument accepted.", BAD_COMMAND_LINE);
    else if(argc == 2)
        detectHelp(argv);
    return;
}

void detectHelp(const char *arguments[]) {
    if(strcmp(arguments[1], "-help") == 0) {
        sendHelp();
        exit(NO_ERR);
    }
    error("Format not recognised! Type \"-help\" for formatting specification.",
          BAD_COMMAND_LINE);
}

void sendHelp() {
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
    return;
}

void populateNotes(struct Note *notes, int numberOfLines) {
    char userInputBuffer[30] = {0};
    int noteIndex = 0, tempTimeStamp = 0, tempMidiNote = 0;
    
    do {
        if(!validateUserInput(userInputBuffer, &tempTimeStamp, &tempMidiNote))
            error("User input not in a recognised format.", BAD_RUNTIME_ARG);
        
        writeNoteData(notes, noteIndex, tempTimeStamp, tempMidiNote);
        
        ++noteIndex;
    } while(notes[noteIndex].midiNote >= 0 && noteIndex < numberOfLines);
    notes[numberOfLines - 1].midiNote = -1; // Ensures 100th midiNote will end printing loop
    
    printNotes(notes);
    
    //Print the samples
    
}

bool validateUserInput(char *userInputBuffer, int *timeStamp, int *midiNote) {
    if(fgets(userInputBuffer, sizeof(userInputBuffer), stdin) == NULL)
        return false;
    char *tokenSeparater = " ", *tempTime = NULL, *tempNote = NULL;
    tempTime = strtok(userInputBuffer, tokenSeparater);
    tempNote = strtok(NULL, tokenSeparater);
    if(tempTime == NULL || tempNote == NULL)
        return false;
    if(!(isOnlyInt(tempTime) || isOnlyInt(tempNote)))
        return false;
    *timeStamp = atoi(tempTime);
    *midiNote = atoi(tempNote);
    //sscanf(userInputBuffer, "%d %d", timeStamp, midiNote);
    return true;
}

void writeNoteData(struct Note *notes, int noteIndex, int timeStamp, int midiNote) {
    if(midiNote > 127) {
        error("The MIDI ‘note on’ message contains data out of bounds.",
              OUT_OF_BOUNDS_VALUE);
    }
    
    timestampToDurationHandler(notes, noteIndex, timeStamp);
    
    notes[noteIndex].midiNote = midiNote;
}

void timestampToDurationHandler(struct Note *notes, int noteIndex, int timeStamp) {
    if(noteIndex < 1)
        return;
    static int previousTimestamp = 0;
    if(timeStamp - previousTimestamp <= 0) {
        error("The time values need to be non-negative and increasing in value.",
            OUT_OF_BOUNDS_VALUE);
    }
    
    notes[noteIndex - 1].duration = timeStamp - previousTimestamp;
    previousTimestamp = timeStamp;
    return;
}

void printNotes(struct Note *notes) {
    int noteIndex = 0;
    double lastSample = 0;
    while(notes[noteIndex].midiNote >= 0) {
        lastSample = printNote(notes[noteIndex]);
    }
    printf("%.6f\n", sin(lastSample));
    /*
     *  In order to avoid phase issues, must print last sample of previous frequency at beginning of
     *  next frequency. This means that there will be one un-printed sample after the last frequency
     *  has 'finished' printing. This must then be printed to ensure the correct number of samples
     *  are printed for each frequency.
     */
    return;
}

double printNote(struct Note note) {
    
    static double lastRadianAngle = 0;
        // Angle is stored between function calls.
    double frequency = midiToFrequency(note.midiNote);
    
    for(unsigned int sampleIndex = 0;
        sampleIndex < note.duration * g_sampleRate / 1000; ++sampleIndex) {
        printf("%.6f\n", sin(calculateAngle(sampleIndex, frequency, lastRadianAngle)));
    }
    
    // Store the radian value used to calulate NEXT sample as this will be the starting sample of
    // the next oscillation.
    lastRadianAngle = calculateAngle(note.duration * frequency / g_sampleRate,
                                     frequency, lastRadianAngle);
    return lastRadianAngle;
}

double calculateAngle(unsigned int sampleIndex, double frequency, double lastRadianAngle) {
    return fmod((g_tau * frequency * sampleIndex / g_sampleRate) + lastRadianAngle, g_tau);
}


bool isOnlyInt(const char *string) {
    if(!string)
        return false;
    for(int index = 0; index < (strlen(string) / sizeof(char)); ++index) {
        if(string[index] < 48 || 57 < string[index])
            return true;
    }
    return false;
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

void printWithBorder(char *message[], int rows) {
    /* Set up border parameters */
    int pad = 1, borderWidth = 1, numColumns = 80, numRows = rows  + (2*(pad+borderWidth));
    
    for(int r = 0; r < numRows; ++r) {
        for(int c = 0; c < numColumns; ++c) {
            /* If we're in the border rows or columns */
            if(r < borderWidth || r >= numRows - borderWidth ||
               c < borderWidth || c >= numColumns - borderWidth)
                printf("%c", '*');
            
            /* If we're wthin the padding rows or columns */
            else if(r < pad + borderWidth || r >= numRows - borderWidth - pad ||
                    c < pad + borderWidth || c >= numColumns - borderWidth - pad)
                printf("%c", ' ');
            
            
            else {
                /* We must now be in the rows and columns with potential text */
                
                /* Integer divide remainig whitespace by 2 */
                int centreOffset = (int) ((numColumns -
                                strlen(message[r - borderWidth - pad])) / 2) - pad - borderWidth;
                
                /* Fill in whitespace before printing message */
                if(c - borderWidth - pad < centreOffset)
                    printf("%c", ' ');
                
                /* Printing message */
                else if(strlen(message[r-borderWidth-pad]) > c - centreOffset - borderWidth - pad)
                    printf("%c", message[r-borderWidth-pad][c - centreOffset - borderWidth - pad]);
                
                /* Finishing whitespace */
                else
                    printf("%c", ' ');
            }
        }
        printf("%c", '\n');
    }
    return;
}
