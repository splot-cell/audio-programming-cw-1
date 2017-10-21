#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
extern "C" {
#include "test.h"
#include <stdbool.h>
}

TEST_GROUP(Samples) {};
TEST_GROUP(StringTesting) {};
TEST_GROUP(DurationTests) {};
TEST_GROUP(MidiTests) {};
TEST_GROUP(HelperFunctions) {};

TEST(Samples, initialSampleAccurate) {
   double result = calculateAngle(0, 1376.42, 0);
   CHECK_EQUAL(0, result);
}

TEST(Samples, secondSampleAccurate) {
   double result = calculateAngle(1, 1376.42, 0);
   DOUBLES_EQUAL(0.1801729567, result, 0.000000001);
}

TEST(StringTesting, isOnlyInt_identifiesZero) {
	const char *input = "0";
	bool result = isOnlyInt(input);
	CHECK(result);
}

TEST(StringTesting, isOnlyInt_identifiesNegative) {
	const char *input = "-0";
	bool result = isOnlyInt(input);
	CHECK(result);
}

TEST(StringTesting, isOnlyInt_identifiesNegativeInMiddle) {
	const char *input = "0-0";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(StringTesting, isOnlyInt_identifies9) {
	const char *input = "9";
	bool result = isOnlyInt(input);
	CHECK(result);
}

TEST(StringTesting, isOnlyInt_identifiesColon) {
	const char *input = ":";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(StringTesting, isOnlyInt_identifiesSlash) {
	const char *input = "/";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(StringTesting, isOnlyInt_iteratesStringFalse) {
	const char *input = "0123456789p";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(StringTesting, isOnlyInt_iteratesStringTrue) {
	const char *input = "0123456789";
	bool result = isOnlyInt(input);
	CHECK(result);
}

TEST(StringTesting, isOnlyInt_identifiesSpace) {
	const char *input = "01234 56789";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(StringTesting, isOnlyInt_identifiesNULL) {
	const char *input = "01234\056789";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(StringTesting, isOnlyInt_identifiesOnlyNULL) {
	const char *input = NULL;
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(DurationTests, withinDuration_isTrue) {
	bool result = withinDurationLimit(0);
	CHECK(result);
}

TEST(DurationTests, withinDuration_justTrue) {
	bool result = withinDurationLimit(89478485);
	CHECK(result);
}

TEST(DurationTests, withinDuration_justFalse) {
	bool result = withinDurationLimit(89478486);
	CHECK(!result);
}

TEST(MidiTests, midiToFrequency_noteC1) {
	double result = midiToFrequency(12);
	DOUBLES_EQUAL(16.3516 ,result, 0.0001);
}

TEST(MidiTests, midiToFrequency_note103) {
	double result = midiToFrequency(103);
	DOUBLES_EQUAL(3135.96 ,result, 0.01);
}

TEST(HelperFunctions, timeStampHandler_ignores_first_note) {
	struct Note notes[10];
	notes[0].duration = 10;
	timestampToDurationHandler(notes, 0, 50);
	DOUBLES_EQUAL(10, notes[0].duration, 0.0000001);
}

TEST(HelperFunctions, timeStampHandler_overrides_previous_note) {
	struct Note notes[10];
	notes[0].duration = 10;
	notes[1].duration = 15;
	bool result = timestampToDurationHandler(notes, 0, 50);
	DOUBLES_EQUAL(10, notes[0].duration, 0.0000001);
	CHECK(result);
	result = timestampToDurationHandler(notes, 1, 70);
	CHECK(result);
	DOUBLES_EQUAL(20, notes[0].duration, 0.0000001);
	DOUBLES_EQUAL(15, notes[1].duration, 0.0000001);
}

TEST(HelperFunctions, timeStampHandler_identifies_invalid_first_run) {
	struct Note notes[10];
	notes[0].duration = 10;
	notes[1].duration = 15;
	bool result = timestampToDurationHandler(notes, 0, -1);
	DOUBLES_EQUAL(10, notes[0].duration, 0.0000001);
	CHECK(!result);
}

TEST(HelperFunctions, timeStampHandler_identifies_invalid_subsequent_runs) {
	struct Note notes[10];
	notes[0].duration = 10;
	notes[1].duration = 15;
	bool result = timestampToDurationHandler(notes, 0, 0);
	DOUBLES_EQUAL(10, notes[0].duration, 0.0000001);
	CHECK(result);
	result = timestampToDurationHandler(notes, 1, -2);
	CHECK(!result);
	result = timestampToDurationHandler(notes, 1, -1);
	CHECK(!result);
}
