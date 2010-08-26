/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/
//*----------------------------------------------------------------------------
//* File Name           : NandFlash.c
//*----------------------------------------------------------------------------
#include "main.h"
#include "NandFlash.h"


BOOL AT91F_CheckBlock(PSNandInfo pNandInfo, unsigned int uBlockNb, char *pOutBuffer);
BOOL AT91F_TagBadBlock(PSNandInfo pNandInfo, unsigned int uBlockNb);


#ifdef DEBUG

#define AT91C_SM_NWE_SETUP		(0 << 0)
#define AT91C_SM_NCS_WR_SETUP	(0 << 8)
#define AT91C_SM_NRD_SETUP		(0 << 16)
#define AT91C_SM_NCS_RD_SETUP	(0 << 24)

#define AT91C_SM_NWE_PULSE 		(2 << 0)  //2   4
#define AT91C_SM_NCS_WR_PULSE	(3 << 8)  //3   6
#define AT91C_SM_NRD_PULSE		(2 << 16) //2   3
#define AT91C_SM_NCS_RD_PULSE	(4 << 24) //4   5

#define AT91C_SM_NWE_CYCLE 		(3 << 0)  //3   6
#define AT91C_SM_NRD_CYCLE		(5 << 16) //5   5

#define AT91C_SM_TDF	        (1 << 16)

//*----------------------------------------------------------------------------
//* \fn    AT91F_SMC_Init
//* \brief NandFlash init
//*----------------------------------------------------------------------------
void AT91F_SMC_Init (void)
{
	// Setup Smart Media, first enable the address range of CS3 in HMATRIX user interface
	AT91C_BASE_MATRIX->MATRIX_EBICSA |= AT91C_MATRIX_CS3A_SM;

	// Configure SMC CS3
	AT91C_BASE_SMC->SMC_SETUP3 = (AT91C_SM_NWE_SETUP | AT91C_SM_NCS_WR_SETUP |
								  AT91C_SM_NRD_SETUP | AT91C_SM_NCS_RD_SETUP);

	AT91C_BASE_SMC->SMC_PULSE3 = (AT91C_SM_NWE_PULSE | AT91C_SM_NCS_WR_PULSE |
								  AT91C_SM_NRD_PULSE | AT91C_SM_NCS_RD_PULSE);

	AT91C_BASE_SMC->SMC_CYCLE3 = (AT91C_SM_NWE_CYCLE | AT91C_SM_NRD_CYCLE);

  	AT91C_BASE_SMC->SMC_CTRL3 = (AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE |
  								 AT91C_SMC_NWAITM_NWAIT_DISABLE |
  								 AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS | AT91C_SM_TDF);	// AT91C_SMC_DBW_WIDTH_EIGTH_BITS

 	AT91F_PIOC_CfgPMC ();

  	// Configure Ready/Busy signal
  	AT91F_PIO_CfgInput(AT91C_BASE_PIOC, AT91C_PIO_PC15);

  	// Enable PC0=SMOE line PC1=SMWE, A21=CLE, A22=ALE
  	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOC, (AT91C_PC0_SMOE | AT91C_PC1_SMWE), 0);

  	// Enable NandFlash
  	AT91F_PIO_CfgOutput(AT91C_BASE_PIOC, AT91C_PIO_PC14);


}
#endif

//*----------------------------------------------------------------------------
//* \fn    AT91F_Nand_HardwareInit
//* \brief NandFlash init
//*----------------------------------------------------------------------------
void AT91F_Nand_HardwareInit (void)
{

#if defined (AT91SAM9260) || defined (AT91SAM9XE128) || defined (AT91SAM9XE256) || defined (AT91SAM9XE512)
	// Bug: Always re-configure Nand PIOs (removed by ROM code)
	AT91f_OpenNandFlashPIO();
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOC);

	// Bug: Always re-enable Nand on NCS3 (removed by ROM code)
	AT91C_BASE_CCFG->CCFG_EBICSA |= AT91C_EBI_CS3A_SM;
#endif

#ifdef AT91SAM9262
	// Bug: Always re-configure Nand PIOs (removed by ROM code)
	AT91f_OpenNandFlashPIO();
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOA);

	// Bug: Always re-enable Nand on NCS3 (removed by ROM code)
	AT91C_BASE_MATRIX->MATRIX_EBI0 |= AT91C_MATRIX_CS3A_SM;
#endif

#ifdef AT91SAM9263
	// Bug: Always re-configure Nand PIOs (removed by ROM code)
	AT91f_OpenNandFlashPIO();
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOA);

	// Bug: Always re-enable Nand on NCS3 (removed by ROM code)
	AT91C_BASE_CCFG->CCFG_EBI0CSA |= AT91C_EBI_CS3A_SM;
#endif

#if (defined (AT91SAM7SE512) || defined (AT91SAM7SE256) || defined (AT91SAM7SE32))
	// Bug: Always re-configure Nand PIOs (removed by ROM code)
	AT91f_OpenNandFlashPIO();
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOB);

	// Bug: Always re-enable Nand on NCS3 (removed by ROM code)
  	AT91C_BASE_EBI->EBI_CSA |= AT91C_EBI_CS3A_SMC_NandFlash;
#endif

#ifdef AT91SAM9RL64
	// Bug: Always re-configure Nand PIOs (removed by ROM code)
	AT91f_OpenNandFlashPIO();
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOD);
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOB);

	// Bug: Always re-enable Nand on NCS3 (removed by ROM code)
	AT91C_BASE_CCFG->CCFG_EBICSA |= AT91C_EBI_CS3A_SM;
#endif

#ifdef AT91CAP9
	// Bug: Always re-configure Nand PIOs (removed by ROM code)
	AT91f_OpenNandFlashPIO();
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOABCDE);

	// Bug: Always re-enable Nand on NCS3 (removed by ROM code)
	AT91C_BASE_CCFG->CCFG_EBICSA |= AT91C_EBI_CS3A_SM;
#endif

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandInit
//* \brief Initialize NandFlash informations
//*
//* TCL_PAGE_BUF[1] = NF_NbBlocks
//* TCL_PAGE_BUF[2] = NF_BlockSize
//* TCL_PAGE_BUF[3] = NF_SectorSize
//* TCL_PAGE_BUF[4] = NF_SpareSize
//* TCL_PAGE_BUF[5] = NF_DataBusWidth
//*----------------------------------------------------------------------------
void AT91F_NandInit(PSNandInfo pNandInfo)
{
	unsigned int uSectorSize, i=0;

	pNandInfo->uNbBlocks 	  = TCL_PAGE_BUF[1];	//!< Nb of blocks in device
	pNandInfo->uBlockNbData	  = TCL_PAGE_BUF[2];	//!< Nb of DataBytes in a block
	pNandInfo->uDataNbBytes	  = TCL_PAGE_BUF[3];	//!< Nb of bytes in data section
	pNandInfo->uSpareNbBytes  = TCL_PAGE_BUF[4];	//!< Nb of bytes in spare section
	pNandInfo->uSectorNbBytes = pNandInfo->uDataNbBytes +
								pNandInfo->uSpareNbBytes;	//!< Total nb of bytes in a sector

	pNandInfo->uBlockNbSectors = pNandInfo->uBlockNbData / pNandInfo->uDataNbBytes;		//!< Nb of sector in a block
	pNandInfo->uBlockNbSpares = pNandInfo->uSpareNbBytes * pNandInfo->uBlockNbSectors;	//!< Nb of SpareBytes in a block
	pNandInfo->uBlockNbBytes = pNandInfo->uSectorNbBytes * pNandInfo->uBlockNbSectors;	//!< Total nb of bytes in a block

	pNandInfo->uNbSectors = pNandInfo->uBlockNbSectors * pNandInfo->uNbBlocks;	//!< Total nb of sectors in device
	pNandInfo->uNbData = pNandInfo->uBlockNbBytes * pNandInfo->uNbBlocks;		//!< Nb of DataBytes in device
	pNandInfo->uNbSpares = pNandInfo->uBlockNbSpares * pNandInfo->uNbBlocks;	//!< Nb of SpareBytes in device
	pNandInfo->uNbBytes	= pNandInfo->uNbData + pNandInfo->uNbSpares;			//!< Total nb of bytes in device

	pNandInfo->uDataBusWidth = TCL_PAGE_BUF[5];		//!< Data Bus Width (8/16 bits)

	uSectorSize = pNandInfo->uDataNbBytes - 1;
	pNandInfo->uOffset = 0;

	while (uSectorSize >> i)
	{
		pNandInfo->uOffset++;
		i++;
	}

    if (pNandInfo->uDataBusWidth)
    {
        pNandInfo->uBadBlockInfoOffset = 2 * BAD_BLOCK_INFO_OFFSET;
    }
    else
    {
        pNandInfo->uBadBlockInfoOffset = BAD_BLOCK_INFO_OFFSET;
    }
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandEraseBlock
//* \brief Erase a Block
//*----------------------------------------------------------------------------
BOOL AT91F_NandEraseBlock(PSNandInfo pNandInfo, unsigned int uPhyBlckNb, unsigned int noCheck)
{
	unsigned int uPhySecNb;
	BOOL bRet = TRUE;
	char *pOutBuffer = (char *)&(TCL_PAGE_BUF[4]); // can be checked by calling application !

	if (noCheck == 0)
	{
        if (AT91F_CheckBlock(pNandInfo, uPhyBlckNb, pOutBuffer) == FALSE)
	    {
		    return FALSE;
	    }
	}

	// Compute the sector number of the first sector of the block
	uPhySecNb = uPhyBlckNb * pNandInfo->uBlockNbSectors;

	// Chip enable
	NAND_ENABLE_CE();

	// Push Erase_1 command
	WRITE_NAND_COMMAND(CMD_ERASE_1);

	// Push sector address in three cycles
	WRITE_NAND_ADDRESS((uPhySecNb >>  0) & 0xFF);
	WRITE_NAND_ADDRESS((uPhySecNb >>  8) & 0xFF);
	WRITE_NAND_ADDRESS((uPhySecNb >> 16) & 0xFF);

	// Push Erase_2 command
	WRITE_NAND_COMMAND(CMD_ERASE_2);

	//  Wait for nand to be ready
	NAND_WAIT_READY();
	NAND_WAIT_READY();

	// Check status bit for error notification
	WRITE_NAND_COMMAND(CMD_STATUS);
	NAND_WAIT_READY();
	if (READ_NAND() & STATUS_ERROR)
	{
		// Error during block erasing
		bRet = FALSE;
		goto exit;
	}

exit:
	// Chip disable
	NAND_DISABLE_CE();

	return bRet;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandReadID
//* \brief Read Nand ID
//*----------------------------------------------------------------------------
BOOL AT91F_NandReadID(void)
{
	BOOL bRet = TRUE;
	unsigned char  bManufacturerID, bDeviceID, byte2, byte3;

	// Enable chipset
	NAND_ENABLE_CE();

	// Ask the Nand its IDs
	WRITE_NAND_COMMAND(CMD_READID);
	WRITE_NAND_ADDRESS(0x00);

	// Read answer
	bManufacturerID	= READ_NAND();
    bDeviceID		= READ_NAND();
	byte2	        = READ_NAND(); // don't care
    byte3		    = READ_NAND(); // extended informations (memory organisation)

	// Disable chipset before returning
	NAND_DISABLE_CE();

	TCL_PAGE_BUF[0] = (byte3 << 24) | (byte2 << 16) | (bManufacturerID << 8) | bDeviceID;

	return bRet;
}

#ifdef NAND_SMALL_BLOCKS
//*----------------------------------------------------------------------------
//* \fn    AT91F_NandWriteSector
//* \brief Write a Sector
//*----------------------------------------------------------------------------
BOOL AT91F_NandWriteSector(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pInBuffer, unsigned int fZone)
{
	BOOL		bRet = TRUE;
	unsigned int	uBytesToWrite;
	unsigned int	i;
	unsigned char   Cmd;

	// Push offset address
	switch(fZone)
	{
		case ZONE_DATA:
			uBytesToWrite = pNandInfo->uDataNbBytes;
			Cmd = CMD_WRITE_A;
			break;
		case ZONE_INFO:
			uBytesToWrite = pNandInfo->uSpareNbBytes;
			pInBuffer+=pNandInfo->uDataNbBytes;
			Cmd = CMD_WRITE_C;
			break;
		case ZONE_DATA | ZONE_INFO:
			uBytesToWrite = pNandInfo->uSectorNbBytes;
			Cmd = CMD_WRITE_A;
			break;
		default:
			bRet = FALSE;
			goto exit;
	}

	// Enable the chip
	NAND_ENABLE_CE();

	// Write from start
	WRITE_NAND_COMMAND(Cmd);

	// Write start
	WRITE_NAND_COMMAND(CMD_WRITE_1);

	// Push sector address
	uSectorAddr >>= pNandInfo->uOffset;

	WRITE_NAND_ADDRESS(0x00);
	WRITE_NAND_ADDRESS((uSectorAddr >>  0) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >>  8) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >> 16) & 0xFF);

	// Write data loop
	if (pNandInfo->uDataBusWidth)
	{	// 16 bits
		for(i=0; i<uBytesToWrite/2; i++) // Div2 because of 16bits
		{
			WRITE_NAND16(*((short*)pInBuffer));
			pInBuffer+=2;
		}
	}
	else
	{	// 8 bits
		for(i=0; i<uBytesToWrite; i++)
		{
			WRITE_NAND(*((char*)pInBuffer));
			pInBuffer++;
		}
	}

	// Write end
	WRITE_NAND_COMMAND(CMD_WRITE_2);

	// Wait for the Nand to be ready
	NAND_WAIT_READY();
	NAND_WAIT_READY();

	// Check status bit for error notification
	WRITE_NAND_COMMAND(CMD_STATUS);
	if (READ_NAND() & STATUS_ERROR)
	{
		// Error during block erasing
		bRet = FALSE;
		goto exit;
	}

exit:

	// Disable the chip
	NAND_DISABLE_CE();

	return bRet;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandReadSector
//* \brief Read a Sector
//*----------------------------------------------------------------------------
BOOL AT91F_NandReadSector(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pOutBuffer, unsigned int fZone)
{
	BOOL		bRet = TRUE;
	unsigned int	uBytesToRead, i;
	unsigned char   Cmd;

	// WARNING : During a read procedure you can't call the ReadStatus flash cmd
	// The ReadStatus fill the read register with 0xC0 and then corrupt the read.

	// Push offset address
	switch(fZone)
	{
		case ZONE_DATA:
			uBytesToRead = pNandInfo->uDataNbBytes;
			Cmd = CMD_READ_A;
			break;
		case ZONE_INFO:
			uBytesToRead = pNandInfo->uSpareNbBytes;
			pOutBuffer += pNandInfo->uDataNbBytes;
			Cmd = CMD_READ_C;
			break;
		case ZONE_DATA | ZONE_INFO:
			uBytesToRead = pNandInfo->uSectorNbBytes;
			Cmd = CMD_READ_A;
			break;
		default:
			bRet = FALSE;
			goto exit;
	}

	// Enable the chip
	NAND_ENABLE_CE();

	// Write specific command, Read from start
	WRITE_NAND_COMMAND(Cmd);

	// Push sector address
	uSectorAddr >>= pNandInfo->uOffset;

	WRITE_NAND_ADDRESS(0x00);
	WRITE_NAND_ADDRESS((uSectorAddr >>  0) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >>  8) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >> 16) & 0xFF);

	// Wait for flash to be ready (can't pool on status, read upper WARNING)
	NAND_WAIT_READY();
	NAND_WAIT_READY();	// Need to be done twice, READY detected too early the first time?

	// Read loop
	if (pNandInfo->uDataBusWidth)
	{	// 16 bits
		for(i=0; i<uBytesToRead/2; i++) // Div2 because of 16bits
		{
			*((short*)pOutBuffer) = READ_NAND16();
			pOutBuffer+=2;
		}
	} else {
		for(i=0; i<uBytesToRead; i++)
		{
			*pOutBuffer = READ_NAND();
			pOutBuffer++;
		}
	}

exit:
	// Disable the chip
	NAND_DISABLE_CE();

	return bRet;
}

#else /* NAND_LARGE_BLOCKS */

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandWriteSector
//* \brief Write a Sector
//*----------------------------------------------------------------------------
BOOL AT91F_NandWriteSector(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pInBuffer, unsigned int fZone)
{
	BOOL			bRet = TRUE;
	unsigned int	uBytesToWrite;
	unsigned int	i;

	// Enable the chip
	NAND_ENABLE_CE();

	// Write from start
	WRITE_NAND_COMMAND(CMD_WRITE_1);

	// Push offset address
	switch(fZone)
	{
		case ZONE_DATA:
			uBytesToWrite = pNandInfo->uDataNbBytes;
			WRITE_NAND_ADDRESS(0x00);
			WRITE_NAND_ADDRESS(0x00);
			break;
		case ZONE_INFO:
			uBytesToWrite = pNandInfo->uSpareNbBytes;
			pInBuffer+=pNandInfo->uDataNbBytes;

			// Write data loop
			if (pNandInfo->uDataBusWidth){	// 16 bits
				WRITE_NAND_ADDRESS(((pNandInfo->uDataNbBytes/2) >>  0) & 0xFF); // Div 2 is because we address in word and not in byte
				WRITE_NAND_ADDRESS(((pNandInfo->uDataNbBytes/2) >>  8) & 0xFF);
			} else {						// 8 bits
				WRITE_NAND_ADDRESS((pNandInfo->uDataNbBytes >>  0) & 0xFF);
				WRITE_NAND_ADDRESS((pNandInfo->uDataNbBytes >>  8) & 0xFF);
			}
			break;
		case ZONE_DATA | ZONE_INFO:
			uBytesToWrite = pNandInfo->uSectorNbBytes;
			WRITE_NAND_ADDRESS(0x00);
			WRITE_NAND_ADDRESS(0x00);
			break;
		default:
			bRet = FALSE;
			goto exit;
	}

	// Push sector address
	uSectorAddr >>= pNandInfo->uOffset;

	WRITE_NAND_ADDRESS((uSectorAddr >>  0) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >>  8) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >> 16) & 0xFF);

	// Write data loop
	if (pNandInfo->uDataBusWidth)
	{	// 16 bits
		for(i=0; i<uBytesToWrite/2; i++) // Div2 because of 16bits
		{
			WRITE_NAND16(*((short*)pInBuffer));
			pInBuffer+=2;
		}
	}
	else
	{	// 8 bits
		for(i=0; i<uBytesToWrite; i++)
		{
			WRITE_NAND(*((char*)pInBuffer));
			pInBuffer++;
		}
	}

	// Write end
	WRITE_NAND_COMMAND(CMD_WRITE_2);

	//  Wait for the Nand to be ready
	NAND_WAIT_READY();
	NAND_WAIT_READY();

	// Check status bit for error notification
	WRITE_NAND_COMMAND(CMD_STATUS);
	if (READ_NAND() & STATUS_ERROR)
	{
		// Error during block erasing
		bRet = FALSE;
		goto exit;
	}

exit:

	// Disable the chip
	NAND_DISABLE_CE();

	return bRet;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandReadSector
//* \brief Read a Sector
//*----------------------------------------------------------------------------
BOOL AT91F_NandReadSector(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pOutBuffer, unsigned int fZone)
{
	BOOL			bRet = TRUE;
	unsigned int	uBytesToRead, i;

	// WARNING : During a read procedure you can't call the ReadStatus flash cmd
	// The ReadStatus fill the read register with 0xC0 and then corrupt the read.

	// Enable the chip
	NAND_ENABLE_CE();

	// Write specific command, Read from start
	WRITE_NAND_COMMAND(CMD_READ_1);

	// Push offset address
	switch(fZone)
	{
		case ZONE_DATA:
			uBytesToRead = pNandInfo->uDataNbBytes;
			WRITE_NAND_ADDRESS(0x00);
			WRITE_NAND_ADDRESS(0x00);
			break;
		case ZONE_INFO:
			uBytesToRead = pNandInfo->uSpareNbBytes;
			pOutBuffer += pNandInfo->uDataNbBytes;
			if (pNandInfo->uDataBusWidth)
			{		// 16 bits
				WRITE_NAND_ADDRESS(((pNandInfo->uDataNbBytes/2) >>  0) & 0xFF); // Div 2 is because we address in word and not in byte
				WRITE_NAND_ADDRESS(((pNandInfo->uDataNbBytes/2) >>  8) & 0xFF);
			} else { // 8 bits
				WRITE_NAND_ADDRESS((pNandInfo->uDataNbBytes >>  0) & 0xFF);
				WRITE_NAND_ADDRESS((pNandInfo->uDataNbBytes >>  8) & 0xFF);
			}
			break;
		case ZONE_DATA | ZONE_INFO:
			uBytesToRead = pNandInfo->uSectorNbBytes;
			WRITE_NAND_ADDRESS(0x00);
			WRITE_NAND_ADDRESS(0x00);
			break;
		default:
			bRet = FALSE;
			goto exit;
	}

	// Push sector address
	uSectorAddr >>= pNandInfo->uOffset;

	WRITE_NAND_ADDRESS((uSectorAddr >>  0) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >>  8) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >> 16) & 0xFF);

	WRITE_NAND_COMMAND(CMD_READ_2);

	// Wait for flash to be ready (can't pool on status, read upper WARNING)
	NAND_WAIT_READY();
	NAND_WAIT_READY();	// Need to be done twice, READY detected too early the first time?

	// Read loop
	if (pNandInfo->uDataBusWidth)
	{	// 16 bits
		for(i=0; i<uBytesToRead/2; i++) // Div2 because of 16bits
		{
			*((short*)pOutBuffer) = READ_NAND16();
			pOutBuffer+=2;
		}
	} else {
		for(i=0; i<uBytesToRead; i++)
		{
			*pOutBuffer = READ_NAND();
			pOutBuffer++;
		}
	}

exit:
	// Disable the chip
	NAND_DISABLE_CE();

	return bRet;
}
#endif /* NAND_SMALL_BLOCKS */

#ifdef NAND_HW_ECC
//*----------------------------------------------------------------------------
//* \fn    AT91F_NandWriteSector
//* \brief Write a Sector + ECC bytes
//*----------------------------------------------------------------------------
BOOL AT91F_NandWriteSectorECC(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pInBuffer)
{
	BOOL			bRet = TRUE;

	AT91PS_ECC pECC = (AT91PS_ECC)AT91C_BASE_HECC;

	unsigned int	uBytesToWrite;
	unsigned int	i;
	unsigned int 	ecc_value;
	unsigned char 	spare_bytes[64];
	unsigned char  *pECCbuff = spare_bytes;

	// Init Spare Bytes buffer
	for (i=0; i<64; i++)
		spare_bytes[i]=0xFF;

	// Enable the chip
	NAND_ENABLE_CE();

	// Reset ECC
	pECC->ECC_CR = AT91C_ECC_RST;

	// Write Data
	uBytesToWrite = pNandInfo->uDataNbBytes;

#ifdef NAND_SMALL_BLOCKS
	WRITE_NAND_COMMAND(CMD_WRITE_A);
#endif

	// Write from start
	WRITE_NAND_COMMAND(CMD_WRITE_1);

	// Push sector address
	uSectorAddr >>= pNandInfo->uOffset;

#ifdef NAND_LARGE_BLOCKS
	WRITE_NAND_ADDRESS(0x00);
#endif
        WRITE_NAND_ADDRESS(0x00);
	WRITE_NAND_ADDRESS((uSectorAddr >>  0) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >>  8) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >> 16) & 0xFF);

	// Write data loop
	if (pNandInfo->uDataBusWidth)
	{	// 16 bits
		for(i=0; i<uBytesToWrite/2; i++) // Div2 because of 16bits
		{
			WRITE_NAND16(*((short*)pInBuffer));
			pInBuffer+=2;
		}
	}
	else
	{	// 8 bits
		for(i=0; i<uBytesToWrite; i++)
		{
			WRITE_NAND(*((char*)pInBuffer));
			pInBuffer++;
		}
	}

	// Write ECC
	ecc_value = pECC->ECC_PR & 0xFFFF;

	spare_bytes[0] = (unsigned char)(ecc_value & 0xFF);
	spare_bytes[1] = (unsigned char)((ecc_value >> 8) & 0xFF);

	ecc_value = pECC->ECC_NPR & 0xFFFF;

	spare_bytes[2] = (unsigned char)(ecc_value & 0xFF);
	spare_bytes[3] = (unsigned char)((ecc_value >> 8) & 0xFF);

	uBytesToWrite = pNandInfo->uSpareNbBytes;

	// Write ECC
	if (pNandInfo->uDataBusWidth)
	{	// 16 bits
		for(i=0; i<uBytesToWrite/2; i++) // Div2 because of 16bits
		{
			WRITE_NAND16(*((short*)pECCbuff));
			pECCbuff+=2;
		}
	}
	else
	{	// 8 bits
		for(i=0; i<uBytesToWrite; i++)
		{
			WRITE_NAND(*((char*)pECCbuff));
			pECCbuff++;
		}
	}

	// Write end
	WRITE_NAND_COMMAND(CMD_WRITE_2);

	//  Wait for the Nand to be ready
	NAND_WAIT_READY();
	NAND_WAIT_READY();

	// Check status bit for error notification
	WRITE_NAND_COMMAND(CMD_STATUS);
	if (READ_NAND() & STATUS_ERROR)
	{
		// Error during block erasing
		bRet = FALSE;
		goto exit;
	}

exit:

	// Disable the chip
	NAND_DISABLE_CE();

	return bRet;
}
#endif /* END NAND_HW_ECC */

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandWrite
//* \brief Write Sector Algorithm
//*----------------------------------------------------------------------------
BOOL AT91F_NandWrite(PSNandInfo pNandInfo, unsigned int uBlockNb, unsigned int uSectorNb, unsigned int uSpareValue, char *pInBuffer)
{
	BOOL bRet = TRUE;
	unsigned int uSectorAddr = uBlockNb * pNandInfo->uBlockNbData + uSectorNb * pNandInfo->uDataNbBytes;
	unsigned int i;
	char *pOutBuffer = (char *)VERIF_BUF;

	if (uSpareValue != SPARE_VALUE)
	{	// Mark Block as Bad
		AT91F_TagBadBlock(pNandInfo, uBlockNb);
		return bRet;
	}

	if (AT91F_CheckBlock(pNandInfo, uBlockNb, pOutBuffer) == FALSE)
	{
		return FALSE;
	}

	AT91F_NandWriteSector(pNandInfo, uSectorAddr, pInBuffer, ZONE_DATA);

	// Verify if writing happened correctly
	AT91F_NandReadSector(pNandInfo, uSectorAddr, pOutBuffer, ZONE_DATA);

	for (i=0; i<pNandInfo->uDataNbBytes/4; i++)
	{
		if (((unsigned int *)pOutBuffer)[i] != ((unsigned int *)pInBuffer)[i])
		{
			bRet = FALSE;
			goto exit;
		}
	}

exit:
	return bRet;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandWriteBlock
//* \brief Write Sector Algorithm
//*----------------------------------------------------------------------------
BOOL AT91F_NandWriteBlock(PSNandInfo pNandInfo, unsigned int uBlockNb, char *pInBuffer)
{
	BOOL		 bRet = TRUE;
	unsigned int uSectorAddr;
	unsigned int uSectorNb;
	unsigned int i;
	char *pOutBuffer = (char *)VERIF_BUF_SDRAM;
	char *pBuffer;

	for (uSectorNb=0; uSectorNb < pNandInfo->uBlockNbSectors; uSectorNb++)
	{
		pBuffer = (char *)(pInBuffer + uSectorNb * pNandInfo->uDataNbBytes);
		uSectorAddr = uBlockNb * pNandInfo->uBlockNbData + uSectorNb * pNandInfo->uDataNbBytes;

		// Write Data + ECC
#if defined (NAND_HW_ECC) || defined (NAND_SOFT_ECC)
		AT91F_NandWriteSectorECC(pNandInfo, uSectorAddr, pBuffer);
#else
		AT91F_NandWriteSector(pNandInfo, uSectorAddr, pBuffer, ZONE_DATA);
#endif
		// Verify if writing happened correctly
		AT91F_NandReadSector(pNandInfo, uSectorAddr, pOutBuffer, ZONE_DATA);

		for (i=0; i<pNandInfo->uDataNbBytes/4; i++)
		{
			if (((unsigned int *)pOutBuffer)[i] != ((unsigned int *)pBuffer)[i])
			{
				bRet = FALSE;
				goto exit;
			}
		}

	}
exit:
	return bRet;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandRead
//* \brief Read Sector Algorithm
//*----------------------------------------------------------------------------
BOOL AT91F_NandRead(PSNandInfo pNandInfo, unsigned int uBlockNb, unsigned int uSectorNb, unsigned int uSpareValue, char *pOutBuffer)
{
	unsigned int uSectorAddr = uBlockNb * pNandInfo->uBlockNbData;

	uSectorAddr = uBlockNb * pNandInfo->uBlockNbData + uSectorNb * pNandInfo->uDataNbBytes;

	if (AT91F_CheckBlock(pNandInfo, uBlockNb, pOutBuffer) == FALSE)
	{
		return FALSE;
	}

	return AT91F_NandReadSector(pNandInfo, uSectorAddr, pOutBuffer, ZONE_DATA);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CheckBlock
//* \brief Check if block is marked Bad
//*----------------------------------------------------------------------------
BOOL AT91F_CheckBlock(PSNandInfo pNandInfo, unsigned int uBlockNb, char *pOutBuffer)
{
	unsigned int i = 0;
	PSSectorInfo pSectorInfo;
	unsigned int uSectorAddr = uBlockNb * pNandInfo->uBlockNbData;

	// Read first page and second page spare zone to detect if block is bad
	for (i = 0; i < 2; i++)
    {
		AT91F_NandReadSector(pNandInfo, (uSectorAddr + i * pNandInfo->uDataNbBytes), pOutBuffer, ZONE_INFO);
		pSectorInfo = (PSSectorInfo)&pOutBuffer[pNandInfo->uDataNbBytes];
		if (pSectorInfo->spare[pNandInfo->uBadBlockInfoOffset] != BAD_BLOCK_TAG)
		{
			return FALSE;
		}
    }

	return TRUE;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TagBadBlock
//* \brief Tag a block as Bad
//*----------------------------------------------------------------------------
BOOL AT91F_TagBadBlock(PSNandInfo pNandInfo, unsigned int uBlockNb)
{
	unsigned int i = 0;
	char *pInBuffer = (char *)VERIF_BUF;
	PSSectorInfo pSectorInfo = (PSSectorInfo)&pInBuffer[pNandInfo->uDataNbBytes];
	unsigned int uSectorAddr = uBlockNb * pNandInfo->uBlockNbData;

	for (i = 0; i < pNandInfo->uBlockNbSpares; i++)
	{
		pSectorInfo->spare[i] = (char)0xFF;
	}
	pSectorInfo->spare[pNandInfo->uBadBlockInfoOffset] = (char)0x00;

	// Mark first and second page spare zone of block as Bad
	for (i = 0; i < 2; i++)
       	{
		AT91F_NandWriteSector(pNandInfo, uSectorAddr + (i * pNandInfo->uDataNbBytes), pInBuffer, ZONE_INFO);
	}

	return TRUE;
}



