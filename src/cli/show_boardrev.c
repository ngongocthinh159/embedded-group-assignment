#include "cli/show_boardrev.h"
#include "lib/color.h"
#include "lib/uart.h"
#include "util/tty.h"

void show_boardrev(unsigned int revision) {
    unsigned int revision_bits = revision & 0b111;
    unsigned int type_bits = (revision >> 4) & 0xFF;
    unsigned int processor_bits = (revision >> 12) & 0xF;
    unsigned int manufacturer_bits = (revision >> 15) & 0xF;
    unsigned int memory_size_bits = (revision >> 20) & 0b111;
    unsigned int style_flag_bit = (revision >> 23) & 1;
    unsigned int warranty_bit = (revision >> 25) & 1;
    unsigned int otp_read_bit = (revision >> 29) & 1;
    unsigned int otp_program = (revision >> 30) & 1;
    unsigned int overvoltage = (revision >> 31) & 1;

    println_color("Hardware", CMD_COLOR_BLU);

    print_color("Model", CMD_COLOR_BLU);
    print(": Raspberry Pi ");

    if (type_bits == 0x11) {
        println("4B");
    } else if (type_bits == 0xD) {
        println("3B+");
    } else if (type_bits == 0xE){
        println("3A+");
    } else if (type_bits == 0x8){
        println("3B");
    }

    print_color("Revision", CMD_COLOR_BLU);
    print(": 1.");
    uart_dec(revision_bits);
    println("");

    print_color("Processor", CMD_COLOR_BLU);
    print(": ");
    if (processor_bits == 0x0) {
        println("Broadcom BCM2835");
    } else if (processor_bits == 0x1) {
        println("Broadcom BCM2836");
    } else if (processor_bits == 0x2) {
        println("Broadcom BCM2837");
    } else if (processor_bits == 0x3) {
        println("Broadcom BCM2711");
    } else if (processor_bits == 0x4) {
        println("Broadcom BCM2712");
    }

    print_color("Memory", CMD_COLOR_BLU);
    print(": ");
    if (memory_size_bits == 0x0) {
        println("256MB");
    } else if (memory_size_bits == 0x1) {
        println("512MB");
    } else if (memory_size_bits == 2) {
        println("1GB");
    } else if (memory_size_bits == 3) {
        println("2GB");
    } else if (memory_size_bits == 4) {
        println("4GB");
    } else if (memory_size_bits == 5) {
        println("8GB");
    }

    print_color("Manufacturer", CMD_COLOR_BLU);
    print(": ");
    if (manufacturer_bits == 0x0) {
        println("Sony UK");
    } else if (manufacturer_bits == 0x1) {
        println("Egoman");
    } else if (manufacturer_bits == 0x2) {
        println("Embest");
    } else if (manufacturer_bits == 0x3) {
        println("Sony Japan");
    } else if (manufacturer_bits == 0x4) {
        println("Embest");
    } else if (manufacturer_bits == 0x5) {
        println("Stadium");
    }
    println("");

    println_color("Diagnostic", CMD_COLOR_BLU);

    if (style_flag_bit == 0x0) {
        print_color("Old", CMD_COLOR_CYN);
    } else if (style_flag_bit == 0x1) {
        print_color("New", CMD_COLOR_CYN);
    }
    println(" revision code style");

    print("Warranty ");
    if (warranty_bit == 0x0) {
        println_color("intact", CMD_COLOR_CYN);
    } else if (warranty_bit == 0x1) {
        println_color("voided", CMD_COLOR_CYN);
    }

    print("OTP reading ");
    if (otp_read_bit == 0x0) {
        println_color("allowed", CMD_COLOR_CYN);
    } else if (otp_read_bit == 0x1) {
        println_color("not allowed", CMD_COLOR_CYN);
    }

    print("OTP programming ");
    if (otp_program == 0x0) {
        println_color("allowed", CMD_COLOR_CYN);
    } else if (otp_program == 0x1) {
        println_color("not allowed", CMD_COLOR_CYN);
    }

    print("Over voltage capabilities ");
    if (overvoltage == 0x0) {
        println_color("allowed", CMD_COLOR_CYN);
    } else if (overvoltage == 0x1) {
        println_color("not allowed", CMD_COLOR_CYN);
    }
}
