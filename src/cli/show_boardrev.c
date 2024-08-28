#include "cli/show_info.h"
#include "cli/get_boardrev.h"
#include "util/tty.h"
#include "lib/uart.h"
void show_boardrev(unsigned int revision){
    set_revision(revision);
    unsigned int revision_bits = get_revision_bits();
    unsigned int type_bits = get_type_bits();
    unsigned int processor_bits = get_processor_bits();
    unsigned int manufracturer_bits = get_manufacturer_bits();
    unsigned int memory_size_bits = get_memory_size_bits();
    unsigned int style_flag_bit = get_style_flag_bit();
    unsigned int warranty_bit = get_warranty_bit();
    unsigned int otp_read_bit = get_otp_read_bit();
    unsigned int otp_program = get_otp_program_bit();
    unsigned int overvoltage = get_overvoltage_bit();

    for(int i=0;i<=5;i++){
        if(revision_bits == i){
          print("Revisions: 1.");
          uart_dec(i);
          print(" \n");
        }
    }
    print("Revision bits value: ");
    uart_hex_no_base(revision_bits,1);
    print("\n\n");
    

    if(type_bits == 0x11){
        print("Type: 4B");
    }else if(type_bits == 0xD){
        print("Type: 3B+");
    }
    // the two boards used in this course
    print("\nType bits value: ");
    uart_hex_no_base(type_bits,2);
    print("\n\n");
      

    print("Processor:");
    if(processor_bits ==0x0){
        print(" BCM2835");
    }else if(processor_bits == 0x1){
        print(" BCM2836");
    }else if(processor_bits == 0x2){
        print(" BCM2837");
    }else if(processor_bits == 0x3){
        print(" BCM2711");
    }else if (processor_bits == 0x4){
        print(" BCM2712");
    }
    print("\nprocessor bits value: ");
    uart_hex_no_base(processor_bits,1);
    print("\n\n");


    print("Manufracturer:");
    if(manufracturer_bits ==0x0){
        print(" Sony UK");
    }else if(manufracturer_bits == 0x1){
        print(" Egoman");
    }else if(manufracturer_bits == 0x2){
        print(" Embest");
    }else if(manufracturer_bits == 0x3){
        print(" Sony Japan");
    }else if (manufracturer_bits == 0x4){
        print(" Embest");
    }else if (manufracturer_bits == 0x5){
        print(" Stadium");
    }
    print("\nManufracture bit value: ");
    uart_hex_no_base(manufracturer_bits,1);
    print("\n\n");      
    
    print("Memory size: ");
    if(memory_size_bits ==0x0){
        print(" 256MB");
    }else if(memory_size_bits == 0x1){
        print(" 512MB");
    }else if(memory_size_bits == 2){
        print(" 1GB");
    }else if(memory_size_bits == 3){
        print(" 2GB");
    }else if (memory_size_bits == 4){
        print(" 4GB");
    }else if (memory_size_bits == 5){
        print(" 8GB");
    }
    print("\n");
    print("Memory bit value: ");
    uart_hex_no_base(memory_size_bits,1);
    print("\n\n");

      
    if(style_flag_bit == 0x0){
        print("Revision code style: old\n");
    }else if(style_flag_bit == 0x1){
    print("Revision code style: new\n");}
    print("style bit value: ");
    uart_hex_no_base(style_flag_bit,1);
    print("\n\n");


    if(warranty_bit == 0x0){
        print("Warranty intact");
    }else if (warranty_bit ==0x1){
        print("warranty voided");
    }
    print("\nwarranty bit value: ");
    uart_hex_no_base(warranty_bit,1);
    print("\n\n");


    if(otp_read_bit == 0x0){
        print("OTP reading allowed");
    }else if(otp_read_bit ==0x1){
        print("OTP reading not allowed");
    }
    print("\nOTP read bit value: ");
    uart_hex_no_base(otp_read_bit,1);
    print("\n\n");


    if(otp_program == 0x0){
        print("OTP programming allow");
    }else if(otp_program ==0x1){
        print("OTP programming not allow");
    }
    print("\n");
    print("OTP programming bit value: ");
    uart_hex_no_base(otp_program,1);
    print("\n\n");
    if(overvoltage == 0x0){
        print("Overvoltage allowed\nvalue: ");
    }else if(overvoltage = 0x1){
        print("Overvoltage not allowed\nvalue: ");
    }
    uart_hex_no_base(overvoltage,1);
    print("\n");
      
}