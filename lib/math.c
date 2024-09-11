#include "lib/math.h"

unsigned int RGB_to_hex(int r, int g, int b) {
  return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

int abs(int x) {
  if (x >= 0) return x;
  return -x;
}

double fabs(double x) { return (x < 0) ? -x : x; }

double sqrt(double n) {
  if (n < 0) {
    return -1;  // Square root of negative numbers is not supported
  }
  if (n == 0 || n == 1) {
    return n;  // The square root of 0 or 1 is the number itself
  }

  double low = 0, high = n, mid = -1;
  double precision = 0.000001;  // Set the desired precision

  while ((high - low) > precision) {
    mid = (low + high) / 2;
    double square = mid * mid;

    if (square == n) {
      return mid;  // Found the exact square root
    } else if (square < n) {
      low = mid;  // Narrow down to the upper half
    } else {
      high = mid;  // Narrow down to the lower half
    }
  }

  return mid;  // Return the approximated square root
}

double pow(double base, int exponent) {
  double result = 1.0;
  int positive_exponent = exponent >= 0 ? exponent : -exponent;

  // Multiply base by itself exponent times
  for (int i = 0; i < positive_exponent; i++) {
    result *= base;
  }

  // If the exponent is negative, return the reciprocal
  if (exponent < 0) {
    result = 1.0 / result;
  }

  return result;
}

double factorial(int n) {
  double result = 1.0;

  // Multiply result by each integer from 1 to n
  for (int i = 1; i <= n; i++) {
    result *= i;
  }

  return result;
}

// Approximation of sine using Taylor series
double sin(double x) {
  double result = 0.0;
  double term;
  int sign = 1;

  // Convert x to the range [-PI, PI] for better accuracy
  while (x > PI) x -= 2 * PI;
  while (x < -PI) x += 2 * PI;

  // Calculate the first few terms of the series
  for (int i = 0; i < 10; i++) {
    int term_index = 2 * i + 1;
    term = sign * (pow(x, term_index) / factorial(term_index));
    result += term;
    sign = -sign;  // alternate sign for each term
  }
  return result;
}

// Approximation of cosine using Taylor series
double cos(double x) {
  double result = 0.0;
  double term;
  int sign = 1;

  // Convert x to the range [-PI, PI] for better accuracy
  while (x > PI) x -= 2 * PI;
  while (x < -PI) x += 2 * PI;

  // Calculate the first few terms of the series
  for (int i = 0; i < 10; i++) {
    int term_index = 2 * i;
    term = sign * (pow(x, term_index) / factorial(term_index));
    result += term;
    sign = -sign;  // alternate sign for each term
  }
  return result;
}

// Global variable to hold the current seed/state.
unsigned long seed = 1234;

unsigned int lcg_rand() {
  seed = (LCG_A * seed + LCG_C) % LCG_M;
  return (unsigned int)seed;
}

// Function to generate a random number within a specified range [min, max]
int rand(int min, int max) {
  max++;
  if (min >= max) {
    return min;  // Return min if range is invalid
  }
  int range = max - min;
  int num = lcg_rand();
  return num % range + min;
}

int min(int a, int b) { return (a < b) ? a : b; }

int max(int a, int b) { return (a > b) ? a : b; }
