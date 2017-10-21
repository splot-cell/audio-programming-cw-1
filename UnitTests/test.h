#ifndef TEST_H
#define TEST_H
#include <stdbool.h>

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
bool validateUserInput(char *userInputBuffer, const int inputBufferSize, int *timeStamp,
                       int *midiNote);
void parseNewline(char *userInputBuffer);
void flushStdin();
void writeNoteData(struct Note *notes, int noteIndex, int timeStamp, int midiNote);
bool timestampToDurationHandler(struct Note *notes, int noteIndex, int timeStamp);
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
