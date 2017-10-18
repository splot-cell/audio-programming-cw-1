#ifndef TEST_H
#define TEST_H
#include <stdbool.h>

double midiToFrequency(int midiNote);
double printSamples(int duration, double frequency);
double calculateAngle(unsigned int sampleIndex, double frequency, double lastRadianAngle);
bool isOnlyInt(const char *string);
bool withinDurationLimit(const long duration);
double midiToFrequency(const int midiNote);
void error(const char *message, int code);

#endif