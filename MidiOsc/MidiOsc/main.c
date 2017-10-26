/*
*   main.c
*   MidiOsc
*
*   Created by Olly Seber on 17/10/2017.
*   Copyright © 2017 Olly Seber. All rights reserved.
*/

#include <stdio.h>      //  For user inputs, printf(), sprintf().
#include <math.h>       //  For sin() and fmod().
#include <stdbool.h>    //  For booleans.
#include <string.h>     //  For strlen(), strcmp(), strtok().
#include <limits.h>     //  For overflow checking.
#include <stdlib.h>     //  For exit().

/*  For unit testing */
#ifdef TEST
#include "../../UnitTests/test.h"
#else

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

int main( int argc, const char * argv[] ) {
    commandLineArgHandler( argc, argv );
    
    int numberOfLines = 100;
    struct Note notes[ numberOfLines ];
    
    populateNotes( notes, numberOfLines );
    
    printNotes( notes );
    
    return NO_ERR;
}
#endif


void commandLineArgHandler( int argc, const char *argv[] ) {
    if ( argc > 2 ) {
        error( "Maximum of one runtime argument accepted.", BAD_COMMAND_LINE );
    }
    else if ( argc == 2 ) {
        detectHelp( argv );
    }
    return;
}


void detectHelp( const char *string[] ) {
    if ( strcmp( string[1], "-help" ) == 0 ) {
        sendHelp();
        exit( NO_ERR );
    }
    else {
        error( "Format not recognised! Type \"-help\" for formatting specification.",
              BAD_COMMAND_LINE );
    }
}


void sendHelp() {
    char *helpTitle[] = {
        "OLLY'S WONDEROUS COURSEWORK SUBMISSION",
        "-- Help Documentation --"
    };
    printWithBorder( helpTitle, ( sizeof( helpTitle ) / sizeof( helpTitle[ 0 ] ) ), 3 );
    printf( "%c",'\n' );
    char *helpText[] = {//----------------------------single line character limit---|
        "This program will print the samples of a sine wave as floating point numbers",
        "to six decimal places. Simply enter two integers and watch it go! The format",
        "required is:",
        "",
        "<duration> <midi note number>",
        "",
        "The program will read up to 30 characters on each line, and will ignore any",
        "extra whitespace added before, between or after the integers (though this",
        "does still count towards the 30 characters).",
        "",
        "The program accepts up to 100 pairs of integers, so you can play fun tunes",
        "such as the Family Guy theme song.",
        "",
        "Output will begin once the <midi note number> is set to a value less than 0."
    };
    printWithBorder( helpText, ( sizeof( helpText ) / sizeof( helpText[ 0 ] ) ), 1 );
    return;
}


void printWithBorder( char *message[], int rows, int borderWidth ) {
    
    /* Set up border parameters */
    int pad = 1, numColumns = 80,
    numRows = rows  + ( 2 * ( pad + borderWidth ) );
    
    for ( int r = 0; r < numRows; ++r ) { // Cycle through each row
        for ( int c = 0; c < numColumns; ++c ) { // Cycle through each character in the row
            
            /* If we're in the border rows or characters */
            if ( r < borderWidth || r >= numRows - borderWidth ||
                c < borderWidth || c >= numColumns - borderWidth ) {
                printf( "%c", '*' );
            }
            
            /* If we're wthin the padding rows or characters */
            else if ( r < pad + borderWidth || r >= numRows - borderWidth - pad ||
                     c < pad + borderWidth || c >= numColumns - borderWidth - pad ) {
                printf( "%c", ' ' );
            }
            
            
            else { // We must now be in the rows and character 'columns' with potential text
                
                /* Integer divide remainig whitespace by 2 */
                int centreOffset = (int) ( ( numColumns -
                                            strlen( message[ r - borderWidth - pad ] ) ) / 2 ) - pad - borderWidth;
                
                /* Fill in whitespace before printing message */
                if ( c - borderWidth - pad < centreOffset ) {
                    printf( "%c", ' ' );
                }
                
                /* Printing message */
                else if ( strlen( message[ r - borderWidth - pad ] ) >
                         c - centreOffset - borderWidth - pad ) {
                    printf( "%c",
                           message[ r-borderWidth-pad ][ c - centreOffset - borderWidth - pad ] );
                }
                
                /* Finishing whitespace */
                else {
                    printf( "%c", ' ' );
                }
            }
        }
        printf( "%c", '\n' );
    }
    return;
}


void populateNotes( struct Note *notes, int numberOfLines ) {
    
    const int inputBufferSize = 32; // 32 characters required by fgets for 30 user characters + '\n'
    char userInputBuffer[ inputBufferSize ] = { 0 };
    int noteIndex = 0;
    long tempTimestamp = 0, tempMidiNote = 0;
    
    do {
        if ( !getUserInput( userInputBuffer, inputBufferSize, &tempTimestamp, &tempMidiNote ) ) {
            error( "User input not in a recognised format.", BAD_RUNTIME_ARG );
        }
        
        writeNoteData( notes, noteIndex, tempTimestamp, tempMidiNote );
        
        if ( ( noteIndex == 0 ) && ( notes[ noteIndex ].midiNote < 0 ) ) {
            error( "No valid midi note values entered. Cannot print samples.", BAD_RUNTIME_ARG );
        }
        
    } while ( ( notes[ noteIndex++ ].midiNote >= 0 ) && ( noteIndex < numberOfLines ) );
    
    notes[ numberOfLines - 1 ].midiNote = -1; // Ensures 100th note value is negative
}


bool getUserInput( char *userInputBuffer, const int inputBufferSize,
                  long *timestamp, long *midiNote ) {
    if ( fgets( userInputBuffer, inputBufferSize, stdin ) == NULL ) {
        return false;
    }
    parseNewline( userInputBuffer );
    
    /* Check only two arguments provided, separated by ' ' or '\t' */
    char *tempTime = NULL, *tempNote = NULL;
    if ( !separateArguments( userInputBuffer, &tempTime, &tempNote ) ) {
        return false;
    }
    
    /* Check the two arguments contain only integers then convert strings to longs */
    if ( !isOnlyInt( tempTime ) || !isOnlyInt( tempNote ) ) {
        return false;
    }
    *timestamp = strtol( tempTime, NULL, 10 );
    *midiNote = strtol( tempNote, NULL, 10 );
    
    return true;
}


bool separateArguments( char *userInputBuffer, char **tempTime, char **tempNote ) {
    /* Uses pointers to pointers in order to modify pointers passed in. */
    
    char *thirdArgument = NULL;
    
    *tempTime = strtok( userInputBuffer, " \t" );
    *tempNote = strtok( NULL, " \t" );
    thirdArgument = strtok( NULL, " \t" );
    
    if ( *tempTime == NULL || *tempNote == NULL || thirdArgument != NULL ) {
        return false;
    }
    return true;
}


void parseNewline( char *userInputBuffer ) {
    char *p;
    if ( ( p = strchr( userInputBuffer, '\n' ) ) ) { // Extra parenthesis required as assignment
        *p = 0;
    }
    else { // User has entered more than 30 characters
        error( "Too many characters entered on one line!" , BAD_RUNTIME_ARG );
    }
    return;
}


void writeNoteData( struct Note *notes, int noteIndex, long timestamp, long midiNote ) {
    
    if ( midiNote > 127 || midiNote < INT_MIN ) {
        error( "The MIDI ‘note on’ message contains data out of bounds.",
                OUT_OF_BOUNDS_VALUE );
    }
    
    if ( !timestampToDurationHandler( notes, noteIndex, timestamp ) ) {
        error( "The time values need to be non-negative and increasing in value.",
                OUT_OF_BOUNDS_VALUE );
    }
    
    notes[ noteIndex ].midiNote = (int) midiNote;
}


bool timestampToDurationHandler( struct Note *notes, int noteIndex, long timestamp ) {
    
    /* Check range */
    if ( timestamp > INT_MAX || timestamp < INT_MIN ) {
        error( "The timestamp you have entered will cause overflow. Please choose a smaller value.",
                OUT_OF_BOUNDS_VALUE );
    }
    
    if ( timestamp < 0 ) {
        return false;
    }

    static int previousTimestamp = 0;
    if ( noteIndex < 1 ) {} // Do nothing on first note
    else if ( timestamp - previousTimestamp <= 0 ) {
        return false;
    }
    else { /* Write duration to the previous note */
        notes[ noteIndex - 1 ].duration = (int) timestamp - previousTimestamp;
        withinDurationLimit( notes[ noteIndex - 1 ].duration, noteIndex );
    }
    
    /* Store the timestamp between function calls */
    previousTimestamp = (int) timestamp;
    return true;
}


bool isOnlyInt( const char *string ) {
    
    if ( !string ) {
        return false;
    }
    
    bool retValue = true;
    
    for ( int index = 0; index < ( strlen( string ) / sizeof( char ) ); ++index ) {
        /* If character is outside numerical digit range */
        if ( ( ( string[ index ] < 48 || 57 < string[ index ] ) &&
              /* AND character is not '-' in first caracter of string */
              !( index == 0 && string[ index ] == 45 ) ) &&
            /* AND the expression has not already been evaulated to false */
            retValue ) {
            retValue = false; // Then there are some 'shady characters' in <string>.
        }
    }
    return retValue;
}


bool withinDurationLimit( int duration, int noteIndex ) {
    
    /* Calculate maximum numnber of samples per milisecond for the duration of the note before
     * overflow occurs. */
    double overflowCheck = INT_MAX / duration; // By this point duration will be larger than 0
    
    /* Checks that this is within the limits imposed by the sample rate */
    if ( overflowCheck < g_sampleRate / 1000 ) {
        char errorMessage[ 50 ];
        sprintf( errorMessage, "The duration of note number %d is too long!", noteIndex );
        error( errorMessage, OUT_OF_BOUNDS_VALUE );
    }
    return true;
}


void printNotes( struct Note *notes ) {
    
    int noteIndex = 0;
    double finalRadianAngle = 0;
    
    while ( notes[ noteIndex ].midiNote >= 0 ) {
            finalRadianAngle = printNote( notes[ noteIndex++ ] );
    }
    
    /* In order to avoid phase issues, must print last sample of previous note at beginning of
     * next note. This means that there will be one un-printed sample after the last note has
     * 'finished' printing. This must then be printed to ensure the correct number of samples are
     * printed for each note. */
    printf( "%.6f\n", sin( finalRadianAngle ) );
     
    return;
}


double printNote( struct Note note ) {
    
    /* Phase offset angle is stored between function calls. */
    static double lastRadianAngle = 0;

    double frequency = midiToFrequency(note.midiNote);
    
    for ( unsigned int sampleIndex = 0;
         sampleIndex < note.duration * g_sampleRate / 1000; ++sampleIndex ) {
        printf( "%.6f\n", sin( calculateAngle( sampleIndex, frequency, lastRadianAngle ) ) );
    }
    
    /* Store the radian value used to calulate NEXT sample as this will be the starting sample of
       the next oscillation. */
    lastRadianAngle = calculateAngle( note.duration * g_sampleRate / 1000,
                                     frequency, lastRadianAngle );
    return lastRadianAngle; // Return this so it can be printed after final note generated.
}


double midiToFrequency( const int midiNote ) {
    return ( pow( 2, ( midiNote - g_referenceMidiNote ) / 12. ) ) * g_referenceFrequency;
}


double calculateAngle( unsigned int sampleIndex, double frequency, double lastRadianAngle ) {
    return fmod( ( g_tau * frequency * sampleIndex / g_sampleRate ) + lastRadianAngle, g_tau );
}


void error( const char *message, int errorCode ) {
    printf( "%s\n", message );
    exit( errorCode );
}
