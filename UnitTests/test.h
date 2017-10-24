#ifndef TEST_H
#define TEST_H
#include <stdbool.h>

/*  GLOBAL VARIABLES */
const int g_sampleRate = 48000;
const double g_pi = 3.14159265359;
const double g_tau = 2 * g_pi;
const double g_referenceMidiNote = 69;      //  Midi note 69 is A above middle C.
const double g_referenceFrequency = 440;    //  Desired frequency of g_referenceMidiNote.

/*  STRUCTS */
struct Note {
    int duration;
    int midiNote;
};

/*  ERROR MESSAGES */
enum ERR {
    NO_ERR,
    BAD_COMMAND_LINE,
    BAD_RUNTIME_ARG,
    OUT_OF_BOUNDS_VALUE
};

/*  FUNCTION PROTOTYPES */

/*  For handling command line arguments */
void commandLineArgHandler( int argc, const char *argv[] );
void detectHelp( const char *arguments[] );
void sendHelp();
void printWithBorder( char *message[], int rows, int borderWidth );

/*  For storing user input in an array of notes */
void populateNotes( struct Note *notes, int numberOfLines );
bool getUserInput( char *userInputBuffer, const int inputBufferSize, long *timestamp,
                       long *midiNote );
bool separateArguments( char *userInputBuffer, char *tempTime, char *tempNote );
void parseNewline( char *userInputBuffer );
void flushStdin();
void writeNoteData( struct Note *notes, int noteIndex, long timestamp, long midiNote );
bool timestampToDurationHandler( struct Note *notes, int noteIndex, long timestamp );

/*  User input santising helper functions */
bool isOnlyInt( const char *string );
    //  Input a string from stdin. Each character checked in turn.
    //  First character can be '-' or a numerical digit ('0'-'9'). Subsequent characters can only
    //  be numerical digits.
    //  Provides more robust checking of user input than just scanf or sscanf.
bool withinDurationLimit( int duration, int noteIndex );
    //  Checks that duration will not cause overflow of sample index.


/*  For printing out the notes */
void printNotes( struct Note *notes );
    //  Handles printing of array of notes
double printNote( struct Note note );
    //  Prints single note
double midiToFrequency( const int midiNote );
double calculateAngle( unsigned int sampleIndex, double frequency, double lastRadianAngle );
    //  Helper function for calculating angle to be used in sin()

/*  Other */
void error( const char *message, int code );
    //  Helper function that prints error warning and exits program.
    
#endif
