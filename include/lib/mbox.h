#pragma once

#include "gpio.h"

/* a properly aligned buffer */
extern volatile unsigned int mBuf[36];
#define ADDR(X) (unsigned int)((unsigned long) X) // ADDR(mBuf) = (unsigned int) &mBuf[0] => cast address of mBuf[0] to unsigned long first the unsigned int

/* Registers */
#define VIDEOCORE_MBOX (MMIO_BASE + 0x0000B880)
#define MBOX0_READ (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x00))
#define MBOX0_PEEK (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x10))
#define MBOX0_SENDER (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x14))
#define MBOX0_STATUS (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x18))
#define MBOX0_CONFIG (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x1C))
#define MBOX1_WRITE (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x20))
#define MBOX1_PEEK (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x30))
#define MBOX1_SENDER (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x34))
#define MBOX1_STATUS (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x38))
#define MBOX1_CONFIG (* (volatile unsigned int*)(VIDEOCORE_MBOX + 0x3C))

//Request/Response code in Buffer content
#define MBOX_RESPONSE 0x80000000
#define MBOX_REQUEST 0

//Status Value (from Status Register)
#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000

//Config register: Enable interrupt
// #define MBOX_CONFIG_IHAVEDATAIRQEN BIT(0)

/* channels */
#define MBOX_CH_POWER 0 //Power management
#define MBOX_CH_FB 1 //Frame buffer
#define MBOX_CH_VUART 2 //Virtual UART
#define MBOX_CH_VCHIQ 3 //VCHIQ
#define MBOX_CH_LEDS 4 //LEDs
#define MBOX_CH_BTNS 5 //Buttons
#define MBOX_CH_TOUCH 6 //Touch screen
#define MBOX_CH_PROP 8 //Property tags (ARM -> VC)

/* tags */
#define MBOX_TAG_GETSERIAL 0x00010004 //Get board serial
#define MBOX_TAG_GETMODEL 0x00010001 //Get board model
#define MBOX_TAG_SETCLKRATE 0x00038002
#define MBOX_TAG_LAST 0
#define MBOX_TAG_SETPHYWH 0x00048003
#define MBOX_TAG_SETVIRTWH 0x00048004
#define MBOX_TAG_SETVIRTOFF 0x00048009
#define MBOX_TAG_SETDEPTH 0x00048005
#define MBOX_TAG_SETPXLORDR 0x00048006
#define MBOX_TAG_GETFB 0x00040001
#define MBOX_TAG_GETPITCH 0x00040008
#define MBOX_TAG_GETMAC 0x00010003
#define MBOX_TAG_GETBOARDREV 0x00010002

/* Function Prototypes */
int mbox_call(unsigned int buffer_addr, unsigned char channel);
int mbox_call_no_print(unsigned int buffer_addr, unsigned char channel);
