#ifndef TEST_H
#define TEST_H
#include <stdbool.h>


/*  STRUCTS */
struct Note {
    int duration; // Length of note in milliseconds.
    int midiNote; // Midi note number of note.
};

/*  ERROR MESSAGES */
enum ERR {
    NO_ERR,
    BAD_COMMAND_LINE,
    BAD_RUNTIME_ARG,
    OUT_OF_BOUNDS_VALUE
};

/*  GLOBAL VARIABLES */
const int g_sampleRate = 48000;
const double g_pi = 3.14159265359;
const double g_tau = 2 * g_pi;
const double g_referenceMidiNote = 69;   // Midi note 69 is A above middle C.
const double g_referenceFrequency = 440; // Desired frequency of g_referenceMidiNote.

/*  FUNCTION PROTOTYPES */

/*  For handling command line arguments */

/*      commandLineArgHandler()
 *  Passed command line arguments upon program start.
 *  Checks number of arguments:
 *      - Throws error if more than 2 arguments.
 *      - If exactly 2 calls detectHelp() on second argument.
 *      - If less than 2 returns to main function. */
void commandLineArgHandler( int argc, const char *argv[] );

/*      detectHelp()
 *  Compares <string> to "-help". If equal, calls functions to print help documentation.
 *  Otherwise sends error message. */
void detectHelp( const char *string[] );

/*      sendHelp()
 *  Contains the help documentation, prints this using printWithBorder and exits program. */
void sendHelp();

/*      printWithBorder()
 *  Prints array of strings <message> as lines of text with a border of asterisks.
 *  <rows> = total number of strings in array, i.e. number of lines of text to print.
 *  <borderWidth> defines the size of the border. */
void printWithBorder( char *message[], int rows, int borderWidth );

/*  For storing user input in an array of notes */

/*      populateNotes()
 *  Takes in array of "struct Note" variables as <notes>.
 *  Handles populating array with data of up to <numberOfLines> Notes from user input. */
void populateNotes( struct Note *notes, int numberOfLines );

/*      getUserInput()
 *  Populates <userInputBuffer> of size <inputBufferSize> with runtime input from user.
 *  Handles validating data is in format of <int> <int>.
 *  Two extracted long ints are then written to <timestamp> and <midiNote> respectively. */
bool getUserInput( char *userInputBuffer, const int inputBufferSize, long *timestamp,
                  long *midiNote );

/*      separateArguments()
 *  Separates <userInputBuffer> into two separate strings which are written to <tempTime> and
 *  <tempNote>. Error is thrown if three (or more) strings detected. */
bool separateArguments( char *userInputBuffer, char **tempTime, char **tempNote );

/*      parseNewline()
 *  Searches string <userInputBuffer> for '\n' and replaces it with '\0'. If no newline character
 *  detected user has entered too many characters so error is thrown. */
void parseNewline( char *userInputBuffer );

/*      writeNoteData()
 *  Handles range checking of <timestamp> and <midiNote>, and conversion of <timestamp> to a
 *  duration.
 *  Writes the duration and midi note number into the Note <notes>[ noteIndex ]. */
void writeNoteData( struct Note *notes, int noteIndex, long timestamp, long midiNote );

/*      isOnlyInt()
 *  Converts the <timestamp>s for a series of notes into durations and writes them to the relevent
 *  <noteIndex> within <notes>. */
bool timestampToDurationHandler( struct Note *notes, int noteIndex, long timestamp );

/*  User input santising helper functions */

/*      isOnlyInt()
 *  Input of a <string>. Each character is evaluated in turn. Returns true or false based on
 *  evaluation of the following statements:
 *      - First character can be ascii character '-' or a numerical digit '0' to '9'.
 *      - Subsequent characters can only be numerical digits. */
bool isOnlyInt( const char *string );

/*      withinDurationLimit()
 *  Checks that <duration> will not cause overflow of the sample index when printing samples. Prints
 *  error message based on <noteIndex> if a duration is too long. */
bool withinDurationLimit( int duration, int noteIndex );

/*  For printing out the notes */

/*      printNotes()
 *  Handles printing an array <notes> of "struct Note" variables. */
void printNotes( struct Note *notes );

/*      printNote()
 *  Prints a single stuct Note <note>. */
double printNote( struct Note note );

/*      midiToFrequency()
 *  Converts midi note number <midiNote> to a frequency. */
double midiToFrequency( const int midiNote );

/*      calculateAngle()
 *  Calculates angle in radians required for sin() function based on <sampleIndex>, <frequency>
 *  and <lastRadianAngle> (phase offset) parameters. */
double calculateAngle( unsigned int sampleIndex, double frequency, double lastRadianAngle );

/*  Other */

/*      error()
 *  Helper function for printing error messages and codes */
void error( const char *message, int code );

/*  END OF PROTOTYPES */

    
#endif
