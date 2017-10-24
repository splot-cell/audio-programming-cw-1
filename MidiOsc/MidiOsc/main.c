//
//  main.c
//  MidiOsc
//
//  Created by Olly Seber on 17/10/2017.
//  Copyright © 2017 Olly Seber. All rights reserved.
//

#include <stdio.h>      // For user inputs, printf(), sprintf().
#include <math.h>       // For sin() and fmod().
#include <stdbool.h>    // For booleans.
#include <string.h>     // For strlen(), strcmp(), strtok().
#include <limits.h>     // For overflow checking.
#include <stdlib.h>     // For exit().

#ifdef TEST
#include "../../UnitTests/test.h"
#else
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

/* FUNCTION PROTOTYPES */
void commandLineArgHandler(int argc, const char *argv[]);
void detectHelp(const char *arguments[]);
void sendHelp();
void printWithBorder(char *message[], int rows, int borderWidth);
void populateNotes(struct Note *notes, int numberOfLines);
bool validateUserInput(char *userInputBuffer, const int inputBufferSize, long *timestamp,
                       long *midiNote);
void parseNewline(char *userInputBuffer);
void flushStdin();
void writeNoteData(struct Note *notes, int noteIndex, long timestamp, long midiNote);
bool timestampToDurationHandler(struct Note *notes, int noteIndex, long timestamp);
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
     *  Input a string from stdin. Each character checked in turn.
     *  First character can be '-' or a numerical digit ('0'-'9'). Subsequent characters can only
     *  be numerical digits.
     *  Provides more robust checking of user input than just scanf or sscanf.
     */
bool withinDurationLimit(int duration, int noteIndex);
    //  Checks that duration will not cause overflow of sample index.
void error(const char *message, int code);
    //  Helper function, prints error warning and exits program.

int main(int argc, const char * argv[]) {
    commandLineArgHandler(argc, argv);
    
    int numberOfLines = 100;
    struct Note notes[numberOfLines];
    
    populateNotes(notes, numberOfLines);
    
    printNotes(notes);
    
    return NO_ERR;
}
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
    char *helpTitle[] = {
        "OLLY'S WONDEROUS COURSEWORK SUBMISSION",
        "-- Help Documentation --"
    };
    printWithBorder(helpTitle, sizeof(helpTitle) / sizeof(helpTitle[0]), 3);
    printf("%c",'\n');
    char *helpText[] = {//----------------------------single line character limit---|
        "This program will print the samples of a sine wave as floating point numbers",
        "to six decimal places. Simply enter two integers and watch it go! The format",
        "required is:",
        "",
        "<duration> <midi note number>",
        "",
        "The program will read up to 30 characters on each line, and will ignore any",
        "extra whitespace. If more than 30 characters are entered, the additional",
        "ones will be discarded!",
        "",
        "The program accepts up to 100 pairs of integers, so you can play fun tunes",
        "such as the Family Guy theme song.",
        "",
        "Output will begin once the <midi note number> is set to a value less than 0."
    };
    printWithBorder(helpText, sizeof(helpText) / sizeof(helpText[0]),1);
    return;
}

void populateNotes(struct Note *notes, int numberOfLines) {
    const int inputBufferSize = 31;
    char userInputBuffer[inputBufferSize] = {0};
    int noteIndex = 0;
    long tempTimestamp = 0, tempMidiNote = 0;
    
    do {
        if(!validateUserInput(userInputBuffer, inputBufferSize, &tempTimestamp, &tempMidiNote))
            error("User input not in a recognised format.", BAD_RUNTIME_ARG);
        
        writeNoteData(notes, noteIndex, tempTimestamp, tempMidiNote);
        
        if(noteIndex == 0 && notes[noteIndex].midiNote < 0)
            error("No valid midi note values entered. Cannot print samples.", BAD_RUNTIME_ARG);
        
    } while(notes[noteIndex++].midiNote >= 0 && noteIndex < numberOfLines);
    notes[numberOfLines - 1].midiNote = -1; // Ensures 100th note value is negative
}

bool validateUserInput(char *userInputBuffer, const int inputBufferSize, long *timestamp,
                       long *midiNote) {
    if(fgets(userInputBuffer, inputBufferSize, stdin) == NULL)
        return false;
    parseNewline(userInputBuffer);
    
    /* Check only two arguments provided, separated by ' ' or '\t' */
    char *tempTime = NULL, *tempNote = NULL, *thirdArgument = NULL;
    tempTime = strtok(userInputBuffer, " \t");
    tempNote = strtok(NULL, " \t");
    thirdArgument = strtok(NULL, " \t");
    if(tempTime == NULL || tempNote == NULL || thirdArgument != NULL)
        return false;
    
    /* Check the two arguments contain only integers then convert strings to longs */
    if(!isOnlyInt(tempTime) || !isOnlyInt(tempNote))
        return false;
    *timestamp = strtol(tempTime, NULL, 10);
    *midiNote = strtol(tempNote, NULL, 10);
    return true;
}

void parseNewline(char *userInputBuffer) {
    char *p;
    if((p = strchr(userInputBuffer, '\n'))) /* Extra parenthesis required as assignment */
        *p = 0;
    else /* User has entered more than 30 characters */
        flushStdin();
    return;
}

void flushStdin() {
    scanf("%*[^\n]");   /* Scan and discard up to next newline character */
    scanf("%*c");       /* Discard next character (the newline) */
    return;
}

void writeNoteData(struct Note *notes, int noteIndex, long timestamp, long midiNote) {
    if(midiNote > 127 || midiNote < INT_MIN) {
        error("The MIDI ‘note on’ message contains data out of bounds.",
              OUT_OF_BOUNDS_VALUE);
    }
    
    if(!timestampToDurationHandler(notes, noteIndex, timestamp))
        error("The time values need to be non-negative and increasing in value.",
              OUT_OF_BOUNDS_VALUE);
    
    notes[noteIndex].midiNote = (int) midiNote;
}

bool timestampToDurationHandler(struct Note *notes, int noteIndex, long timestamp) {
    if(timestamp > INT_MAX || timestamp < INT_MIN)
        error("The timestamp you have entered will cause overflow. Please choose a smaller value.",
              OUT_OF_BOUNDS_VALUE);
    
    static int previousTimestamp = 0;
    if(timestamp < 0)
        return false;
    
    if(noteIndex < 1) {}            /*  Do nothing  */
    else if(timestamp - previousTimestamp <= 0)
        return false;
    else {
        notes[noteIndex - 1].duration = (int) timestamp - previousTimestamp;
        withinDurationLimit(notes[noteIndex - 1].duration, noteIndex);
    }
    
    previousTimestamp = (int) timestamp;  /*  Executes every call */
    return true;
}

void printNotes(struct Note *notes) {
    int noteIndex = 0;
    double finalRadianAngle = 0;
    while(notes[noteIndex].midiNote >= 0) {
        finalRadianAngle = printNote(notes[noteIndex++]);
    }
    printf("%.6f\n", sin(finalRadianAngle));
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
    lastRadianAngle = calculateAngle(note.duration * g_sampleRate / 1000,
                                     frequency, lastRadianAngle);
    return lastRadianAngle;
}

double calculateAngle(unsigned int sampleIndex, double frequency, double lastRadianAngle) {
    return fmod((g_tau * frequency * sampleIndex / g_sampleRate) + lastRadianAngle, g_tau);
}


bool isOnlyInt(const char *string) {
    if(!string)
        return false;
    bool retValue = true;
    for(int index = 0; index < (strlen(string) / sizeof(char)); ++index) {
        if((string[index] < 48 || 57 < string[index]) && retValue)
            retValue = false;
        if(index == 0 && string[index] == 45)   /*  If the first character is '-', override the  */
            retValue = true;                    /*  retValue as this is still valid              */
    }
    return retValue;
}

bool withinDurationLimit(int duration, int noteIndex) {
    /*
     *  Creates limit for maximum duration note accepted. Ensures that a loop isn't created when
     *  printing the samples due to the sampleIndex overflowing.
     */
    double overflowCheck = INT_MAX / duration;
    if(overflowCheck < g_sampleRate / 1000) {
        char errorMessage[50];
        sprintf(errorMessage, "The duration of note number %d is too long!", noteIndex);
        error(errorMessage, OUT_OF_BOUNDS_VALUE);
    }
    return true;
}

double midiToFrequency(const int midiNote) {
    return pow(2, (midiNote - g_referenceMidiNote)/12.) * g_referenceFrequency;
}

void error(const char *message, int code) {
    printf("%s\n", message);
    exit(code);
}

void printWithBorder(char *message[], int rows, int borderWidth) {
    /*
     *  Prints message with a border.
     *
     *  <message> is array of strings. Each string is a line to be printed.
     *  <rows> parameter is total number of lines.
     *
     *  Appearance can be set from within the funciton body.
     *  If the a string input is too long for the row it will be cut short.
     */
    
    /*  Set up border parameters */
    int pad = 1, numColumns = 80, numRows = rows  + (2*(pad+borderWidth));
    
    for(int r = 0; r < numRows; ++r) {
        for(int c = 0; c < numColumns; ++c) {
            /*  If we're in the border rows or columns */
            if(r < borderWidth || r >= numRows - borderWidth ||
               c < borderWidth || c >= numColumns - borderWidth)
                printf("%c", '*');
            
            /*  If we're wthin the padding rows or columns */
            else if(r < pad + borderWidth || r >= numRows - borderWidth - pad ||
                    c < pad + borderWidth || c >= numColumns - borderWidth - pad)
                printf("%c", ' ');
            
            
            else {
                /*  We must now be in the rows and columns with potential text */
                
                /*  Integer divide remainig whitespace by 2 */
                int centreOffset = (int) ((numColumns -
                                strlen(message[r - borderWidth - pad])) / 2) - pad - borderWidth;
                
                /*  Fill in whitespace before printing message */
                if(c - borderWidth - pad < centreOffset)
                    printf("%c", ' ');
                
                /*  Printing message */
                else if(strlen(message[r-borderWidth-pad]) > c - centreOffset - borderWidth - pad)
                    printf("%c", message[r-borderWidth-pad][c - centreOffset - borderWidth - pad]);
                
                /*  Finishing whitespace */
                else
                    printf("%c", ' ');
            }
        }
        printf("%c", '\n');
    }
    return;
}
