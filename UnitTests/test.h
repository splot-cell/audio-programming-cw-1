#ifndef TEST_H
#define TEST_H
#include <stdbool.h>

void commandLineArgHandler(int argc, const char *argv[]);
void detectHelp(const char *arguments[]);
void sendHelp();
void printWithBorder(char *message[], int rows, int borderWidth);
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

#endif