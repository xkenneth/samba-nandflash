/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/
//*----------------------------------------------------------------------------
//* File Name           : main.c
//*----------------------------------------------------------------------------

#include "main.h"
#include "NandFlash.h"

// Define TCL_PAGE_BUF which is the address of the page content to write (written by the SAMBA TCL)
// TCL_PAGE_BUF[0] = Command

unsigned int *TCL_PAGE_BUF = (unsigned int *)PAGE_BUF;

static SNandInfo sNandInfo;

#ifdef DEBUG
extern void AT91F_SMC_Init (void);
#endif

#define R_BIT	1
#define W_BIT	2

//*----------------------------------------------------------------------------
//* Function Name       : read_jtag_control
//* Object              : read debug communication control register
//* Input Parameters    :
//* Output Parameters   : R0
//*----------------------------------------------------------------------------
__inline unsigned int read_jtag_control(void)
{
    /* read debug communication channel control register in R0*/
    __asm("MRC p14, 0, r0, c0, c0");
    /*return value = (R0)*/
}

//*----------------------------------------------------------------------------
//* Function Name       : write_jtag
//* Object              : write data on jtag
//*----------------------------------------------------------------------------
__inline void write_jtag_data(unsigned int val)
{
    /* write debug communication channel control register*/
    __asm("MCR p14, 0, val, c1, c0");
}

//*----------------------------------------------------------------------------
//* \fn    main
//* \brief
//*----------------------------------------------------------------------------
int main()
{
#ifdef DBG_ACK
	unsigned int dCom = *(TCL_PAGE_BUF - 1);
#endif

#ifdef DEBUG
	AT91F_SMC_Init();
#endif

	AT91F_Nand_HardwareInit ();

	switch (TCL_PAGE_BUF[0])
	{
		case AT91C_NAND_ID:
			AT91F_NandReadID();
			break;

		case AT91C_NAND_INIT:
			AT91F_NandInit(&sNandInfo);
			break;

//*----------------------------------------------------------------------------
//* AT91C_NAND_ERASE:
//*
//* TCL_PAGE_BUF[1] = NF_BlockNb
//*----------------------------------------------------------------------------
		case AT91C_NAND_ERASE:
			TCL_PAGE_BUF[0] = (AT91F_NandEraseBlock(&sNandInfo, TCL_PAGE_BUF[1], 0) == TRUE) ? 0x0 : 0xBAD;
			break;

//*----------------------------------------------------------------------------
//* AT91C_NAND_ERASE_FULL:
//*
//* TCL_PAGE_BUF[1] = NF_BlockNb
//*----------------------------------------------------------------------------
		case AT91C_NAND_ERASE_FULL:
			TCL_PAGE_BUF[0] = (AT91F_NandEraseBlock(&sNandInfo, TCL_PAGE_BUF[1], 1) == TRUE) ? 0x0 : 0xBAD;
			break;

//*----------------------------------------------------------------------------
//* AT91C_NAND_READ:
//*
//* TCL_PAGE_BUF[1] = NF_BlockNb    // Block Number to write
//* TCL_PAGE_BUF[2] = NF_SectorNb   // Sector Number in the Block
//* TCL_PAGE_BUF[3] = NF_SpareValue // Block State value (OK or Bad) -> determine DATA_ZONE or INFO_ZONE
//* TCL_PAGE_BUF[4] = NF_pBuffer    // Data Buffer (SectorSize)
//*----------------------------------------------------------------------------
		case AT91C_NAND_READ:
			TCL_PAGE_BUF[0] = (AT91F_NandRead(&sNandInfo,
											   TCL_PAGE_BUF[1],
											   TCL_PAGE_BUF[2],
											   TCL_PAGE_BUF[3],
											   (char *)&(TCL_PAGE_BUF[4])) == TRUE) ? 0x0 : 0xBAD;
			break;

//*----------------------------------------------------------------------------
//* AT91C_NAND_WRITE:
//*
//* TCL_PAGE_BUF[1] = NF_BlockNb    // Block Number to write
//* TCL_PAGE_BUF[2] = NF_SectorNb   // Sector Number in the Block
//* TCL_PAGE_BUF[3] = NF_SpareValue // Block State value (OK or Bad) -> determine DATA_ZONE or INFO_ZONE
//* TCL_PAGE_BUF[4] = NF_pBuffer    // Data Buffer (SectorSize)
//*----------------------------------------------------------------------------
		case AT91C_NAND_WRITE:
			TCL_PAGE_BUF[0] = (AT91F_NandWrite(&sNandInfo,
											   TCL_PAGE_BUF[1],
											   TCL_PAGE_BUF[2],
											   TCL_PAGE_BUF[3],
											   (char *)&(TCL_PAGE_BUF[4])) == TRUE) ? 0x0 : 0xBAD;
			break;

//*----------------------------------------------------------------------------
//* AT91C_NAND_WRITE_BLOCK:
//*
//* TCL_PAGE_BUF[1] = NF_BlockNb    // Block Number to write
//* TCL_PAGE_BUF[4] = NF_pBuffer    // Data Buffer (BlockSize)
//*----------------------------------------------------------------------------
		case AT91C_NAND_WRITE_BLOCK:
			TCL_PAGE_BUF[0] = (AT91F_NandWriteBlock(&sNandInfo,
											   TCL_PAGE_BUF[1],
											   (char *)&(TCL_PAGE_BUF[4])) == TRUE) ? 0x0 : 0xBAD;
			break;

//*----------------------------------------------------------------------------
//* AT91C_NAND_CHECK_BLOCK:
//*
//* TCL_PAGE_BUF[1] = NF_BlockNb    // Block Number to check
//* TCL_PAGE_BUF[4] = NF_pBuffer    // Data Buffer (BlockSize)
//*----------------------------------------------------------------------------
		case AT91C_NAND_CHECK_BLOCK:
			TCL_PAGE_BUF[0] = (AT91F_CheckBlock(&sNandInfo,
											   TCL_PAGE_BUF[1],
											   (char *)&(TCL_PAGE_BUF[4])) == TRUE) ? 0x0 : 0xBAD;
			break;

		default: break;
	}

#ifdef DBG_ACK
	// Send ACK char for serial link
	if (dCom == 1)
		AT91C_BASE_DBGU->DBGU_THR = 0x6;
	// Send ACK char for JTAG link
	if (dCom == 2)
	{
		while((read_jtag_control() & W_BIT)); // Wait a new data to write
		write_jtag_data(0x6);
	}
#endif

	return 1;
}

