#include "CppUTest/TestHarness.h"
extern "C" {
#include "test.h"
#include <stdbool.h>
}

TEST_GROUP(FirstTestGroup) {};

TEST(FirstTestGroup, initialSampleAccurate) {
   double result = calculateAngle(0, 1376.42, 0);
   CHECK_EQUAL(0, result);
}

TEST(FirstTestGroup, secondSampleAccurate) {
   double result = calculateAngle(1, 1376.42, 0);
   DOUBLES_EQUAL(0.1801729567, result, 0.000000001);
}

TEST(FirstTestGroup, isOnlyInt_identifiesZero) {
	const char *input = "0";
	bool result = isOnlyInt(input);
	CHECK(result);
}

TEST(FirstTestGroup, isOnlyInt_identifies9) {
	const char *input = "9";
	bool result = isOnlyInt(input);
	CHECK(result);
}

TEST(FirstTestGroup, isOnlyInt_identifiesColon) {
	const char *input = ":";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(FirstTestGroup, isOnlyInt_identifiesSlash) {
	const char *input = "/";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(FirstTestGroup, isOnlyInt_iteratesStringFalse) {
	const char *input = "0123456789p";
	bool result = isOnlyInt(input);
	CHECK(!result);
}

TEST(FirstTestGroup, isOnlyInt_iteratesStringTrue) {
	const char *input = "0123456789";
	bool result = isOnlyInt(input);
	CHECK(result);
}