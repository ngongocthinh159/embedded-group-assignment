/*
#include "cli/get_boardrev.h"
unsigned int revision;
void set_revision(unsigned int revision_input){
    revision = revision_input;
}
unsigned int get_revision_bits(){
    unsigned int revision_bits = revision & 0b111;
    return revision_bits;
}

unsigned int get_type_bits(){
    unsigned int type_bits = (revision >> 4) & 0xFF;
    return type_bits;
}

unsigned int get_processor_bits(){
    unsigned int processor_bits = (revision >> 12) & 0xF;
    return processor_bits;
}

unsigned int get_manufacturer_bits() {
    unsigned int manufacturer_bits = (revision >> 15) & 0xF;
    return manufacturer_bits;
}

unsigned int get_memory_size_bits() {
    unsigned int memory_size_bits = (revision >> 20) & 0b111;
    return memory_size_bits;
}

unsigned int get_style_flag_bit() {
    unsigned int style_flag_bit = (revision >> 23) & 1;
    return style_flag_bit;
}

unsigned int get_warranty_bit() {
    unsigned int warranty_bit = (revision >> 25) & 1;
    return warranty_bit;
}

unsigned int get_otp_read_bit() {
    unsigned int otp_read_bit = (revision >> 29) & 1;
    return otp_read_bit;
}

unsigned int get_otp_program_bit() {
    unsigned int otp_program_bit = (revision >> 30) & 1;
    return otp_program_bit;
}

unsigned int get_overvoltage_bit() {
    unsigned int overvoltage_bit = (revision >> 31) & 1;
    return overvoltage_bit;
}*/
