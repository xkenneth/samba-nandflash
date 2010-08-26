/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/
//*----------------------------------------------------------------------------
//* File Name           : main.h
//*----------------------------------------------------------------------------
#ifndef _MAIN_H
#define _MAIN_H

//*----------------------------------------------------------------------------
//* GLOBAL DECLARATION
//*----------------------------------------------------------------------------
#include <stdlib.h>

#define	NULL	0

typedef enum _BOOL {TRUE, FALSE} BOOL;

typedef char 	BYTE;
typedef char* 	PBYTE;
typedef unsigned char 	UCHAR;
typedef short 	WORD;
typedef short* 	PWORD;
typedef int 	DWORD;
typedef int* 	PDWORD;
typedef unsigned int ULONG;
typedef void *  PVOID;

//*----------------------------------------------------------------------------
//* SAM-BA Specific NAND Commands
//*----------------------------------------------------------------------------
#define AT91C_NAND_INIT         0
#define AT91C_NAND_ERASE        1
#define AT91C_NAND_READ         2
#define AT91C_NAND_WRITE        3
#define AT91C_NAND_ID           4
#define AT91C_NAND_WRITE_BLOCK  5
#define AT91C_NAND_CHECK_BLOCK  6
#define AT91C_NAND_ERASE_FULL   7

//*----------------------------------------------------------------------------
//* NAND Commands
//*----------------------------------------------------------------------------
// 8 bits devices
#define WRITE_NAND_COMMAND(d) do{ *(volatile unsigned char *)((unsigned long)AT91C_SMARTMEDIA_BASE | AT91_SMART_MEDIA_CLE) = (unsigned char)(d); } while(0)
#define WRITE_NAND_ADDRESS(d) do{ *(volatile unsigned char *)((unsigned long)AT91C_SMARTMEDIA_BASE | AT91_SMART_MEDIA_ALE) = (unsigned char)(d); } while(0)
#define WRITE_NAND(d) do{ *(volatile unsigned char *)((unsigned long)AT91C_SMARTMEDIA_BASE) = (unsigned char)d; } while(0)
#define READ_NAND() ((volatile unsigned char)(*(volatile unsigned char *)(unsigned long)AT91C_SMARTMEDIA_BASE))

// 16 bits devices
#define WRITE_NAND16(d) do{ *(volatile unsigned short *)((unsigned long)AT91C_SMARTMEDIA_BASE) = (unsigned short)d; } while(0)
#define READ_NAND16() ((volatile unsigned short)(*(volatile unsigned short *)(unsigned long)AT91C_SMARTMEDIA_BASE))

//*----------------------------------------------------------------------------
//* CPU Specific Declarations
//*----------------------------------------------------------------------------
#define AT91C_SMARTMEDIA_BASE	0x40000000


#ifdef AT91SAM7SE32
#include "AT91SAM7SE32.h"
#include "lib_AT91SAM7SE32.h"

#define PAGE_BUF           0x200013F0
#define VERIF_BUF          0x20030000
#define VERIF_BUF_SDRAM    0x20060000 // Not Tested

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOB_SODR = AT91C_PIO_PB18;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOB_CODR = AT91C_PIO_PB18;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOB_PDSR & AT91C_PIO_PB19))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOB->PIO_ODR = AT91C_PIO_PB19; \
	AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB18 | AT91C_PIO_PB19; \
	AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB18; \
}
#endif


#ifdef AT91SAM7SE256
#include "AT91SAM7SE256.h"
#include "lib_AT91SAM7SE256.h"

#define PAGE_BUF  	0x2013F0
#define VERIF_BUF 	0x206000
#define VERIF_BUF_SDRAM 	0x20060000 // Not Tested

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOB_SODR = AT91C_PIO_PB18;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOB_CODR = AT91C_PIO_PB18;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOB_PDSR & AT91C_PIO_PB19))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOB->PIO_ODR = AT91C_PIO_PB19; \
	AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB18 | AT91C_PIO_PB19; \
	AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB18; \
}
#endif


#ifdef AT91SAM7SE512
#include "AT91SAM7SE512.h"
#include "lib_AT91SAM7SE512.h"

#define PAGE_BUF  	0x2013F0
#define VERIF_BUF 	0x206000
#define VERIF_BUF_SDRAM 	0x20060000 // Not Tested

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOB_SODR = AT91C_PIO_PB18;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOB_CODR = AT91C_PIO_PB18;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOB_PDSR & AT91C_PIO_PB19))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOB->PIO_ODR = AT91C_PIO_PB19; \
	AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB18 | AT91C_PIO_PB19; \
	AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB18; \
}
#endif


#ifdef AT91SAM9260
#include "AT91SAM9260.h"
#include "lib_AT91SAM9260.h"

#define NAND_SOFT_ECC			1

#define PAGE_BUF  			0x200013F0
#define VERIF_BUF 			0x20030000
#define VERIF_BUF_SDRAM 	0x20060000

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOC_SODR = AT91C_PIO_PC14;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOC_CODR = AT91C_PIO_PC14;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOC_PDSR & AT91C_PIO_PC13))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOC->PIO_ODR = AT91C_PIO_PC13; \
	AT91C_BASE_PIOC->PIO_PER = AT91C_PIO_PC13 | AT91C_PIO_PC14; \
	AT91C_BASE_PIOC->PIO_OER = AT91C_PIO_PC14; \
}
#endif


#ifdef AT91SAM9261
#include "AT91SAM9261.h"
#include "lib_AT91SAM9261.h"

#define NAND_SOFT_ECC				1

#define PAGE_BUF  			  0x3013F0
#define VERIF_BUF 			  0x310000
#define VERIF_BUF_SDRAM 	0x20060000

#define AT91_SMART_MEDIA_ALE    (1 << 22)	//* our ALE is AD22
#define AT91_SMART_MEDIA_CLE    (1 << 21)	//* our CLE is AD21

#define NAND_DISABLE_CE() do { *AT91C_PIOC_SODR = AT91C_PIO_PC14;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOC_CODR = AT91C_PIO_PC14;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOC_PDSR & AT91C_PIO_PC15))
#endif


#ifdef AT91SAM9262
#include "AT91SAM9262.h"
#include "lib_AT91SAM9262.h"

#define PAGE_BUF  	0x3013F0
#define VERIF_BUF 	0x310000
#define VERIF_BUF_SDRAM 	0x20060000 // Not Tested

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOD_SODR = AT91C_PIO_PD15;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOD_CODR = AT91C_PIO_PD15;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOA_PDSR & AT91C_PIO_PA22))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOD->PIO_PER = AT91C_PIO_PD15; \
	AT91C_BASE_PIOD->PIO_OER = AT91C_PIO_PD15; \
	AT91C_BASE_PIOA->PIO_ODR = AT91C_PIO_PA22; \
	AT91C_BASE_PIOA->PIO_PER = AT91C_PIO_PA22; \
}
#endif


#ifdef AT91SAM9263
#include "AT91SAM9263.h"
#include "lib_AT91SAM9263.h"

#define NAND_SOFT_ECC			1

#define PAGE_BUF  	0x200013F0
#define VERIF_BUF 	0x20030000
#define VERIF_BUF_SDRAM 	0x20060000 // Not Tested

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define AT91C_BASE_HECC AT91C_BASE_HECC0  /* redefine ECC base address */

#define NAND_DISABLE_CE() do { *AT91C_PIOD_SODR = AT91C_PIO_PD15;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOD_CODR = AT91C_PIO_PD15;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOA_PDSR & AT91C_PIO_PA22))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOD->PIO_PER = AT91C_PIO_PD15; \
	AT91C_BASE_PIOD->PIO_OER = AT91C_PIO_PD15; \
	AT91C_BASE_PIOA->PIO_ODR = AT91C_PIO_PA22; \
	AT91C_BASE_PIOA->PIO_PER = AT91C_PIO_PA22; \
}
#endif


#ifdef AT91SAM9XE128
#include "AT91SAM9XE128.h"
#include "lib_AT91SAM9XE128.h"

#define NAND_SOFT_ECC			1

#define PAGE_BUF  			0x200013F0
#define VERIF_BUF 			0x20030000
#define VERIF_BUF_SDRAM 	0x20060000

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOC_SODR = AT91C_PIO_PC14;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOC_CODR = AT91C_PIO_PC14;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOC_PDSR & AT91C_PIO_PC13))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOC->PIO_ODR = AT91C_PIO_PC13; \
	AT91C_BASE_PIOC->PIO_PER = AT91C_PIO_PC13 | AT91C_PIO_PC14; \
	AT91C_BASE_PIOC->PIO_OER = AT91C_PIO_PC14; \
}
#endif


#ifdef AT91SAM9XE256
#include "AT91SAM9XE256.h"
#include "lib_AT91SAM9XE256.h"

#define NAND_SOFT_ECC			1

#define PAGE_BUF  			0x200013F0
#define VERIF_BUF 			0x20030000
#define VERIF_BUF_SDRAM 	0x20060000

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOC_SODR = AT91C_PIO_PC14;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOC_CODR = AT91C_PIO_PC14;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOC_PDSR & AT91C_PIO_PC13))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOC->PIO_ODR = AT91C_PIO_PC13; \
	AT91C_BASE_PIOC->PIO_PER = AT91C_PIO_PC13 | AT91C_PIO_PC14; \
	AT91C_BASE_PIOC->PIO_OER = AT91C_PIO_PC14; \
}
#endif


#ifdef AT91SAM9XE512
#include "AT91SAM9XE512.h"
#include "lib_AT91SAM9XE512.h"

#define NAND_SOFT_ECC			1

#define PAGE_BUF  			0x200013F0
#define VERIF_BUF 			0x20030000
#define VERIF_BUF_SDRAM 	0x20060000

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOC_SODR = AT91C_PIO_PC14;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOC_CODR = AT91C_PIO_PC14;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOC_PDSR & AT91C_PIO_PC13))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOC->PIO_ODR = AT91C_PIO_PC13; \
	AT91C_BASE_PIOC->PIO_PER = AT91C_PIO_PC13 | AT91C_PIO_PC14; \
	AT91C_BASE_PIOC->PIO_OER = AT91C_PIO_PC14; \
}
#endif


#ifdef AT91SAM9RL64
#include "AT91SAM9RL64.h"
#include "lib_AT91SAM9RL64.h"

#define NAND_SOFT_ECC			1

#define PAGE_BUF  			0x200013F0
#define VERIF_BUF 			0x20030000
#define VERIF_BUF_SDRAM 	0x20060000

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOB_SODR = AT91C_PIO_PB6;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOB_CODR = AT91C_PIO_PB6;} while(0)

#define NAND_WAIT_READY() while (!(*AT91C_PIOD_PDSR & AT91C_PIO_PD17))

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOB->PIO_PER = AT91C_PB6_NCS3_NANDCS; \
	AT91C_BASE_PIOB->PIO_OER = AT91C_PB6_NCS3_NANDCS; \
    AT91C_BASE_PIOB->PIO_ASR = AT91C_PB4_NANDOE | AT91C_PB5_NANDWE; \
    AT91C_BASE_PIOB->PIO_BSR = 0; \
	AT91C_BASE_PIOB->PIO_PDR = AT91C_PB4_NANDOE | AT91C_PB5_NANDWE; \
	AT91C_BASE_PIOD->PIO_ODR = AT91C_PIO_PD17; \
	AT91C_BASE_PIOD->PIO_PER = AT91C_PIO_PD17; \
}
#endif


#ifdef AT91CAP9
#include "AT91CAP9.h"
#include "lib_AT91CAP9.h"

#define NAND_SOFT_ECC			1

#define PAGE_BUF  			0x700013F0
#define VERIF_BUF 			0x70030000
#define VERIF_BUF_SDRAM 	0x70060000

#define AT91_SMART_MEDIA_ALE    (1 << 21)	//* our ALE is AD21
#define AT91_SMART_MEDIA_CLE    (1 << 22)	//* our CLE is AD22

#define NAND_DISABLE_CE() do { *AT91C_PIOD_SODR = AT91C_PIO_PD15;} while(0)
#define NAND_ENABLE_CE() do { *AT91C_PIOD_CODR = AT91C_PIO_PD15;} while(0)

#define NAND_WAIT_READY() {unsigned int i; for (i=0; i < 20000; i++);}

#define AT91f_OpenNandFlashPIO() { \
	AT91C_BASE_PIOD->PIO_PER = AT91C_PIO_PD15; \
	AT91C_BASE_PIOD->PIO_OER = AT91C_PIO_PD15; \
}
#endif

extern unsigned int *TCL_PAGE_BUF;
#endif
