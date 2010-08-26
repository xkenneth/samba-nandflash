/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/
//*----------------------------------------------------------------------------
//* File Name           : NandFlash.h
//*----------------------------------------------------------------------------

#ifndef _NANDFLASH_H
#define _NANDFLASH_H

//! \brief	Group all usefull sizes for the nandflash chip
typedef struct _NandInfo
{
	unsigned int uDataNbBytes	;	//!< Nb of bytes in data section
	unsigned int uSpareNbBytes	;	//!< Nb of bytes in spare section
	unsigned int uSectorNbBytes	;	//!< Total nb of bytes in a sector

	unsigned int uBlockNbSectors;	//!< Nb of sector in a block
	unsigned int uBlockNbData	;	//!< Nb of DataBytes in a block
	unsigned int uBlockNbSpares	;	//!< Nb of SpareBytes in a block
	unsigned int uBlockNbBytes	;	//!< Total nb of bytes in a block

	unsigned int uNbBlocks		;	//!< Nb of blocks in device
	unsigned int uNbSectors		;	//!< Total nb of sectors in device
	unsigned int uNbData		;	//!< Nb of DataBytes in device
	unsigned int uNbSpares		;	//!< Nb of SpareBytes in device
	unsigned int uNbBytes		;	//!< Total nb of bytes in device
	unsigned int uOffset		;

	unsigned int uDataBusWidth	;	//!< Data Bus Width (8/16 bits)

	unsigned int uBadBlockInfoOffset;       //!< Bad block info offset in spare zone (in bytes) 

} SNandInfo, *PSNandInfo;

// Sector Info Structure
//typedef __packed struct _SectorInfo
//{
//    unsigned int dwReserved1;       // Reserved - used by FAL
//    char  bOEMReserved;             // For use by OEM
//    char  bBadBlock;	            // Indicates if block is BAD
//    short wReserved2;               // Reserved - used by FAL
//} SSectorInfoi, *PSSectorInfo;

typedef __packed struct _SectorInfo
{
    unsigned char spare[16];
} SSectorInfoi, *PSSectorInfo;

#ifdef NAND_LARGE_BLOCKS
#define BAD_BLOCK_INFO_OFFSET 0
#else
#define BAD_BLOCK_INFO_OFFSET 5
#endif

#define BAD_BLOCK_TAG   0xFF


// Wait mode
#define WAIT_POOL					POOL_ON_READYBUSY			//!< Default pool mode
#define WAIT_INTERRUPT				INTERRUPT_ON_READYBUSY		//!< Default interrupt mode
#define	ERASE_TIMEOUT				10							//!< erase maximum time in ms
#define RESET_TIMEOUT				10							//!< reset maximum time in ms
#define READ_TIMEOUT				10							//!< read maximum time in ms
#define WRITE_TIMEOUT				10							//!< write maximum time in ms

#define POOL_ON_READYBUSY			0x01						//!< Pool on ReadyBusy PIO
#define INTERRUPT_ON_READYBUSY		0x02						//!< Interrupt on ReadyBusy PIO
#define POOL_ON_STATUS				0x04						//!< Pool on Status byte

// Sector zones
#define SPARE_VALUE					0xFF

#define ZONE_DATA					0x01	//!< Sector data zone
#define ZONE_INFO					0x02	//!< Sector info zone

// Nand flash chip status codes
#define STATUS_READY                (0x01<<6)					//!< Status code for Ready
#define STATUS_ERROR                (0x01<<0)					//!< Status code for Error

// Nand flash commands
#define CMD_READ_1					0x00
#define CMD_READ_2					0x30

#define CMD_READID					0x90

#define CMD_WRITE_1					0x80
#define CMD_WRITE_2					0x10

#define CMD_ERASE_1					0x60
#define CMD_ERASE_2					0xD0

#define CMD_STATUS					0x70
#define CMD_RESET					0xFF


// Nand flash commands (small blocks)
#define CMD_READ_A					0x00
#define CMD_READ_C					0x50

#define CMD_WRITE_A					0x00
#define CMD_WRITE_C					0x50


#define NAND_BUS_WIDTH_8BITS		0x0
#define NAND_BUS_WIDTH_16BITS		0x1

// NandFlash functions
extern void AT91F_Nand_HardwareInit (void);

extern void AT91F_NandInit(PSNandInfo pNandInfo);
extern BOOL AT91F_NandEraseBlock(PSNandInfo pNandInfo, unsigned int uPhyBlckNb, unsigned int noCheck);
extern BOOL AT91F_NandWrite(PSNandInfo pNandInfo, unsigned int uBlockNb, unsigned int uSectorNb, unsigned int uSpareValue, char *pInBuffer);
extern BOOL AT91F_NandRead(PSNandInfo pNandInfo, unsigned int uBlockNb, unsigned int uSectorNb, unsigned int uSpareValue, char *pOutBuffer);
extern BOOL AT91F_NandReadID(void);
extern BOOL AT91F_NandWriteBlock(PSNandInfo pNandInfo, unsigned int uBlockNb, char *pInBuffer);
extern BOOL AT91F_NandWriteSectorECC(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pInBuffer);
extern BOOL AT91F_CheckBlock(PSNandInfo pNandInfo, unsigned int uBlockNb, char *pOutBuffer);

#endif
