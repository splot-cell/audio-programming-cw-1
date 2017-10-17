#ifndef TEST_H
#define TEST_H
#include <stdbool.h>

double midiToFrequency(int midiNote);
double printSamples(int duration, double frequency);
double calculateAngle(long sampleIndex, double frequency, double lastRadianAngle);
bool isOnlyInt(const char *string);

#endif