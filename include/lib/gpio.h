#pragma once

/**
 * Peripheral physical address:
 * - 0xFE000000 for RPI4 (Low Peripheral mode)
 * - 0x3F000000 for EPI3
 */
#ifdef RPI3
#define MMIO_BASE 0x3F000000
#else
#define MMIO_BASE 0xFE000000
#endif

/**
 * GPIO Registers
 * https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf#%5B%7B%22num%22%3A69%2C%22gen%22%3A0%7D%2C%7B%22name%22%3A%22XYZ%22%7D%2C115%2C841.89%2Cnull%5D
 */
#define GPFSEL0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200000))
#define GPFSEL1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200004))
#define GPFSEL2 (*(volatile unsigned int *)(MMIO_BASE + 0x00200008))
#define GPFSEL3 (*(volatile unsigned int *)(MMIO_BASE + 0x0020000C))
#define GPFSEL4 (*(volatile unsigned int *)(MMIO_BASE + 0x00200010))
#define GPFSEL5 (*(volatile unsigned int *)(MMIO_BASE + 0x00200014))
#define GPSET0 (*(volatile unsigned int *)(MMIO_BASE + 0x0020001C))
#define GPSET1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200020))
#define GPCLR0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200028))
#define GPLEV0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200034))
#define GPLEV1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200038))
#define GPEDS0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200040))
#define GPEDS1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200044))
#define GPHEN0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200064))
#define GPHEN1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200068))

//RBP3 only
#define GPPUD           (* (volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       (* (volatile unsigned int*)(MMIO_BASE+0x00200098))
#define GPPUDCLK1       (* (volatile unsigned int*)(MMIO_BASE+0x0020009C))

//RBP4 only
#define GPIO_PUP_PDN_CNTRL_REG0    (* (volatile unsigned int*)(MMIO_BASE+0x002000E4))
#define GPIO_PUP_PDN_CNTRL_REG1    (* (volatile unsigned int*)(MMIO_BASE+0x002000E8))
#define GPIO_PUP_PDN_CNTRL_REG2    (* (volatile unsigned int*)(MMIO_BASE+0x002000EC))
#define GPIO_PUP_PDN_CNTRL_REG3    (* (volatile unsigned int*)(MMIO_BASE+0x002000F0))

/**
 * Unsigned integer type definitions
 */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;
