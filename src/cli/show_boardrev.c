#include "cli/show_boardrev.h"
#include "lib/uart.h"
#include "util/tty.h"

void show_boardrev(unsigned int revision) {
    unsigned int revision_bits = revision & 0b111;
    unsigned int type_bits = (revision >> 4) & 0xFF;
    unsigned int processor_bits = (revision >> 12) & 0xF;
    unsigned int manufracturer_bits = (revision >> 15) & 0xF;
    unsigned int memory_size_bits = (revision >> 20) & 0b111;
    unsigned int style_flag_bit = (revision >> 23) & 1;
    unsigned int warranty_bit = (revision >> 25) & 1;
    unsigned int otp_read_bit = (revision >> 29) & 1;
    unsigned int otp_program = (revision >> 30) & 1;
    unsigned int overvoltage = (revision >> 31) & 1;

    for (int i = 0; i <= 5; i++) {
        if (revision_bits == i) {
            print("Revisions: 1.");
            uart_dec(i);
            print(" \n");
        }
    }
    print("Revision bits value: ");
    uart_hex_no_base(revision_bits, 1);
    print("\n\n");

    if (type_bits == 0x11) {
        print("Model: 4B");
    } else if (type_bits == 0xD) {
        print("Model: 3B+");
    }else if(type_bits ==0xE){
        print("Model: 3A+");
    }else if(type_bits == 0x8){
        print("Model: 3B");
    }
    // cover 4 types of board
    print("\nModel bits value: ");
    uart_hex_no_base(type_bits, 2);
    print("\n\n");

    print("Processor:");
    if (processor_bits == 0x0) {
        print(" BCM2835");
    } else if (processor_bits == 0x1) {
        print(" BCM2836");
    } else if (processor_bits == 0x2) {
        print(" BCM2837");
    } else if (processor_bits == 0x3) {
        print(" BCM2711");
    } else if (processor_bits == 0x4) {
        print(" BCM2712");
    }
    print("\nprocessor bits value: ");
    uart_hex_no_base(processor_bits, 1);
    print("\n\n");

    print("Manufracturer:");
    if (manufracturer_bits == 0x0) {
        print(" Sony UK");
    } else if (manufracturer_bits == 0x1) {
        print(" Egoman");
    } else if (manufracturer_bits == 0x2) {
        print(" Embest");
    } else if (manufracturer_bits == 0x3) {
        print(" Sony Japan");
    } else if (manufracturer_bits == 0x4) {
        print(" Embest");
    } else if (manufracturer_bits == 0x5) {
        print(" Stadium");
    }
    print("\nManufracture bit value: ");
    uart_hex_no_base(manufracturer_bits, 1);
    print("\n\n");

    print("Memory size: ");
    if (memory_size_bits == 0x0) {
        print(" 256MB");
    } else if (memory_size_bits == 0x1) {
        print(" 512MB");
    } else if (memory_size_bits == 2) {
        print(" 1GB");
    } else if (memory_size_bits == 3) {
        print(" 2GB");
    } else if (memory_size_bits == 4) {
        print(" 4GB");
    } else if (memory_size_bits == 5) {
        print(" 8GB");
    }
    print("\n");
    print("Memory bit value: ");
    uart_hex_no_base(memory_size_bits, 1);
    print("\n\n");

    if (style_flag_bit == 0x0) {
        print("Revision code style: old\n");
    } else if (style_flag_bit == 0x1) {
        print("Revision code style: new\n");
    }
    print("style bit value: ");
    uart_hex_no_base(style_flag_bit, 1);
    print("\n\n");

    if (warranty_bit == 0x0) {
        print("Warranty intact");
    } else if (warranty_bit == 0x1) {
        print("Warranty voided");
    }
    print("\nWarranty bit value: ");
    uart_hex_no_base(warranty_bit, 1);
    print("\n\n");

    if (otp_read_bit == 0x0) {
        print("OTP reading allowed");
    } else if (otp_read_bit == 0x1) {
        print("OTP reading not allowed");
    }
    print("\nOTP read bit value: ");
    uart_hex_no_base(otp_read_bit, 1);
    print("\n\n");

    if (otp_program == 0x0) {
        print("OTP programming allow");
    } else if (otp_program == 0x1) {
        print("OTP programming not allow");
    }
    print("\n");
    print("OTP programming bit value: ");
    uart_hex_no_base(otp_program, 1);
    print("\n\n");
    if (overvoltage == 0x0) {
        print("Overvoltage allowed\nOvervoltage value: ");
    } else if (overvoltage == 0x1) {
        print("Overvoltage not allowed\nOvervoltage value: ");
    }
    uart_hex_no_base(overvoltage, 1);
    print("\n");
}
