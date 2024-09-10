#pragma once

#define PI 3.14159265358979323846

// Define constants for the LCG. These values are common in many implementations.
// https://en.wikipedia.org/wiki/Linear_congruential_generator
#define LCG_A 1103515245
#define LCG_C 12345
#define LCG_M 2147483648  // 2^31

unsigned int RGB_to_hex(int r, int g, int b);
int abs(int x);
double fabs(double x);
double sqrt(double n);
double sin(double x);
double cos(double x);
int rand(int from, int to);
unsigned int lcg_rand();
