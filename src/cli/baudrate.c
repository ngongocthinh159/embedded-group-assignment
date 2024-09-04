#include "cli/baudrate.h"
#include "util/string.h"

// Only correct with uart0_clock = 48Mhz, uart1_clock (sys_clock) = 250Mhz (both are the default values)
Baudrate valid_baudrates[] = {
    {"477", 477, 65513, 6289, 20},
    {"600", 600, 52082, 5000, 1},
    {"1200", 1200, 26041, 2500, 1},
    {"1800", 1800, 17360, 1667, 43},
    {"2400", 2400, 13020, 1250, 1},
    {"4800", 4800, 6509, 625, 1},
    {"9600", 9600, 3254, 313, 33},
    {"19200", 19200, 1627, 156, 17},
    {"28800", 28800, 1084, 104, 11},
    {"38400", 38400, 813, 78, 9},
    {"57600", 57600, 542, 52, 6},
    {"76800", 76800, 406, 39, 5},
    {"115200", 115200, 270, 26, 3},
    {"230400", 230400, 135, 13, 2},
    {"460800", 460800, 67, 7, 33},
    {"576000", 576000, 53, 5, 14},
    {"921600", 921600, 33, 3, 17}
};

Baudrate* current_baudrate = &valid_baudrates[12]; // initial baudrate 115200
unsigned int current_stopbits = 1; // initial stopbit

Baudrate invalid_baudrate = {"", -1, -1, -1, -1};

Baudrate* get_baudrate(const char* inputString) {
    for (int i = 0; i < number_of_valid_baudrates(); i++) {
        if (strcmp(valid_baudrates[i].toString, inputString) == 0) {
            return &valid_baudrates[i];
        }
    }
    return &invalid_baudrate; // No match found
}

int is_valid_baudrate(Baudrate* baudrate) {
  return baudrate->value != -1;
}

int number_of_valid_baudrates() {
  return sizeof(valid_baudrates) / sizeof(invalid_baudrate);
}
