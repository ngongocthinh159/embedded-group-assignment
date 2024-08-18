#include "gpio.h"

/**
 * Properly aligned buffer
 */
extern volatile unsigned int mBuf[36];
#define ADDR(X) (unsigned int)((unsigned long)X)

/**
 * Mailbox Registers
 */
#define VIDEOCORE_MBOX (MMIO_BASE + 0x0000B880)
#define MBOX0_READ (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x00))
#define MBOX0_PEEK (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x10))
#define MBOX0_SENDER (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x14))
#define MBOX0_STATUS (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x18))
#define MBOX0_CONFIG (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x1C))
#define MBOX1_WRITE (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x20))
#define MBOX1_PEEK (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x30))
#define MBOX1_SENDER (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x34))
#define MBOX1_STATUS (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x38))
#define MBOX1_CONFIG (*(volatile unsigned int *)(VIDEOCORE_MBOX + 0x3C))

/**
 * Mailbox Request/Response code in Buffer
 */
#define MBOX_RESPONSE 0x80000000
#define MBOX_REQUEST 0

/**
 * Mailbox Status Values
 */
#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000

/**
 * Mailbox Channels
 */
// Power management
#define MBOX_CH_POWER 0
// Frame buffer
#define MBOX_CH_FB 1
// Virtual UART
#define MBOX_CH_VUART 2
// VCHIQ
#define MBOX_CH_VCHIQ 3
// LEDs
#define MBOX_CH_LEDS 4
// Buttons
#define MBOX_CH_BTNS 5
// Touch screen
#define MBOX_CH_TOUCH 6
// Property tags (ARM -> VC)
#define MBOX_CH_PROP 8

/**
 * Mailbox Tags
 */
// Board serial
#define MBOX_TAG_GETSERIAL 0x00010004
// Board model
#define MBOX_TAG_GETMODEL 0x00010001
#define MBOX_TAG_SETCLKRATE 0x00038002
#define MBOX_TAG_LAST 0

/**
 * Function Prototypes
 */
int mbox_call(unsigned int buffer_addr, unsigned char channel);
