/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/
//*----------------------------------------------------------------------------
//* File Name           : NandECC_Soft.c
//*----------------------------------------------------------------------------
#include "main.h"

#ifdef NAND_SOFT_ECC

#include "NandFlash.h"

typedef struct nand_oobinfo {
	unsigned char eccbytes;
	unsigned char eccpos[32];
} SNand_oobinfo, *PSNand_oobinfo;

static const unsigned char nand_ecc_precalc_table[] = {
	0x00, 0x55, 0x56, 0x03, 0x59, 0x0c, 0x0f, 0x5a, 0x5a, 0x0f, 0x0c, 0x59, 0x03, 0x56, 0x55, 0x00,
	0x65, 0x30, 0x33, 0x66, 0x3c, 0x69, 0x6a, 0x3f, 0x3f, 0x6a, 0x69, 0x3c, 0x66, 0x33, 0x30, 0x65,
	0x66, 0x33, 0x30, 0x65, 0x3f, 0x6a, 0x69, 0x3c, 0x3c, 0x69, 0x6a, 0x3f, 0x65, 0x30, 0x33, 0x66,
	0x03, 0x56, 0x55, 0x00, 0x5a, 0x0f, 0x0c, 0x59, 0x59, 0x0c, 0x0f, 0x5a, 0x00, 0x55, 0x56, 0x03,
	0x69, 0x3c, 0x3f, 0x6a, 0x30, 0x65, 0x66, 0x33, 0x33, 0x66, 0x65, 0x30, 0x6a, 0x3f, 0x3c, 0x69,
	0x0c, 0x59, 0x5a, 0x0f, 0x55, 0x00, 0x03, 0x56, 0x56, 0x03, 0x00, 0x55, 0x0f, 0x5a, 0x59, 0x0c,
	0x0f, 0x5a, 0x59, 0x0c, 0x56, 0x03, 0x00, 0x55, 0x55, 0x00, 0x03, 0x56, 0x0c, 0x59, 0x5a, 0x0f,
	0x6a, 0x3f, 0x3c, 0x69, 0x33, 0x66, 0x65, 0x30, 0x30, 0x65, 0x66, 0x33, 0x69, 0x3c, 0x3f, 0x6a,
	0x6a, 0x3f, 0x3c, 0x69, 0x33, 0x66, 0x65, 0x30, 0x30, 0x65, 0x66, 0x33, 0x69, 0x3c, 0x3f, 0x6a,
	0x0f, 0x5a, 0x59, 0x0c, 0x56, 0x03, 0x00, 0x55, 0x55, 0x00, 0x03, 0x56, 0x0c, 0x59, 0x5a, 0x0f,
	0x0c, 0x59, 0x5a, 0x0f, 0x55, 0x00, 0x03, 0x56, 0x56, 0x03, 0x00, 0x55, 0x0f, 0x5a, 0x59, 0x0c,
	0x69, 0x3c, 0x3f, 0x6a, 0x30, 0x65, 0x66, 0x33, 0x33, 0x66, 0x65, 0x30, 0x6a, 0x3f, 0x3c, 0x69,
	0x03, 0x56, 0x55, 0x00, 0x5a, 0x0f, 0x0c, 0x59, 0x59, 0x0c, 0x0f, 0x5a, 0x00, 0x55, 0x56, 0x03,
	0x66, 0x33, 0x30, 0x65, 0x3f, 0x6a, 0x69, 0x3c, 0x3c, 0x69, 0x6a, 0x3f, 0x65, 0x30, 0x33, 0x66,
	0x65, 0x30, 0x33, 0x66, 0x3c, 0x69, 0x6a, 0x3f, 0x3f, 0x6a, 0x69, 0x3c, 0x66, 0x33, 0x30, 0x65,
	0x00, 0x55, 0x56, 0x03, 0x59, 0x0c, 0x0f, 0x5a, 0x5a, 0x0f, 0x0c, 0x59, 0x03, 0x56, 0x55, 0x00
};

/* Define default oob placement schemes for large and small page devices */
static struct nand_oobinfo nand_oob_8 = {
	3, 					// eccbytes
	{0, 1, 2} 			// eccpos
};

static struct nand_oobinfo nand_oob_16 = {
	6,					// eccbytes
	{0, 1, 2, 3, 6, 7}	// eccpos
};

static struct nand_oobinfo nand_oob_64 = {
	24,					// eccbytes
	{ 40, 41, 42, 43, 44, 45, 46, 47,
	  48, 49, 50, 51, 52, 53, 54, 55,
	  56, 57, 58, 59, 60, 61, 62, 63 }	// eccpos
};

//*----------------------------------------------------------------------------
//* \fn    nand_trans_result
//* \brief Creates non-inverted ECC code from line parity
//* \input:  reg2:	line parity reg 2
//* 		 reg3:	line parity reg 3
//* 		 ecc_code:	ecc
//*----------------------------------------------------------------------------
static void nand_trans_result(unsigned char reg2, unsigned char reg3, unsigned char *ecc_code)
{
	unsigned char a, b, i, tmp1, tmp2;

	/* Initialize variables */
	a = b = 0x80;
	tmp1 = tmp2 = 0;

	/* Calculate first ECC byte */
	for (i = 0; i < 4; i++)
	{
		if (reg3 & a)		/* LP15,13,11,9 --> ecc_code[0] */
			tmp1 |= b;
		b >>= 1;
		if (reg2 & a)		/* LP14,12,10,8 --> ecc_code[0] */
			tmp1 |= b;
		b >>= 1;
		a >>= 1;
	}

	/* Calculate second ECC byte */
	b = 0x80;
	for (i = 0; i < 4; i++)
	{
		if (reg3 & a)		/* LP7,5,3,1 --> ecc_code[1] */
			tmp2 |= b;
		b >>= 1;
		if (reg2 & a)		/* LP6,4,2,0 --> ecc_code[1] */
			tmp2 |= b;
		b >>= 1;
		a >>= 1;
	}

	/* Store two of the ECC bytes */
	ecc_code[0] = tmp1;
	ecc_code[1] = tmp2;
}

//*----------------------------------------------------------------------------
//* \fn    nand_calculate_ecc
//* \brief Calculate 3 byte ECC code for 256 byte block
//* \input: dat:		raw data
//* 		ecc_code:	buffer for ECC
//*----------------------------------------------------------------------------
int nand_calculate_ecc(const unsigned char *dat, unsigned char *ecc_code)
{
	int j;
	unsigned char idx, reg1, reg2, reg3;

	/* Initialize variables */
	reg1 = reg2 = reg3 = 0;
	ecc_code[0] = ecc_code[1] = ecc_code[2] = 0;

	/* Build up column parity */
	for(j = 0; j < 256; j++)
	{
		/* Get CP0 - CP5 from table */
		idx = nand_ecc_precalc_table[dat[j]];
		reg1 ^= (idx & 0x3f);

		/* All bit XOR = 1 ? */
		if (idx & 0x40)
		{
			reg3 ^= (unsigned char) j;
			reg2 ^= ~((unsigned char) j);
		}
	}

	/* Create non-inverted ECC code from line parity */
	nand_trans_result(reg2, reg3, ecc_code);

	/* Calculate final ECC code */
	ecc_code[0] = ~ecc_code[0];
	ecc_code[1] = ~ecc_code[1];
	ecc_code[2] = ((~reg1) << 2) | 0x03;

	return 0;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandWriteSectorECC
//* \brief Write a Sector + ECC bytes (256 bytes ECC)
//*----------------------------------------------------------------------------
BOOL AT91F_NandWriteSectorECC(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pInBuffer)
{
	BOOL			bRet = TRUE;

	PSNand_oobinfo 	pNand_oobinfo;
	unsigned int	datidx = 0, eccidx = 0, eccsteps = pNandInfo->uDataNbBytes / 256;
	unsigned int	uBytesToWrite, i;
	unsigned char 	spare_bytes[64];
	unsigned char 	*pECCbuff = spare_bytes;
	unsigned char 	ecc_code[3];
	
	// Init Spare Bytes buffer
	for (i=0; i<64; i++)
		spare_bytes[i]=0xFF;

	// Choose OOB structure
	switch (pNandInfo->uSpareNbBytes)
	{
		case 8: pNand_oobinfo = &nand_oob_8;
				break;
		case 16: pNand_oobinfo = &nand_oob_16;
				break;
		case 64: pNand_oobinfo = &nand_oob_64;
				break;
		default:
				break;
	}

	// Compute ECC and fill in oob buffer
	for (; eccsteps; eccsteps--)
	{
		nand_calculate_ecc((unsigned char *)pInBuffer + datidx, ecc_code);
		
		for (i = 0; i < 3; i++, eccidx++)
			spare_bytes[(pNand_oobinfo->eccpos)[eccidx]] = ecc_code[i];
		
		datidx += 256;
	}

	// Enable the chip
	NAND_ENABLE_CE();

#ifdef NAND_SMALL_BLOCKS
	WRITE_NAND_COMMAND(CMD_WRITE_A);
#endif

	// Write from start
	WRITE_NAND_COMMAND(CMD_WRITE_1);

	uBytesToWrite = pNandInfo->uDataNbBytes;	
#ifdef NAND_LARGE_BLOCKS
	WRITE_NAND_ADDRESS(0x00);
#endif
	WRITE_NAND_ADDRESS(0x00);

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

	// Write oob loop
	if (pNandInfo->uDataBusWidth)
	{	// 16 bits
		for(i=0; i<(pNandInfo->uSpareNbBytes)/2; i++) // Div2 because of 16bits
		{
			WRITE_NAND16(*((short*)pECCbuff));
			pECCbuff+=2;
		}
	}
	else
	{	// 8 bits
		for(i=0; i<pNandInfo->uSpareNbBytes; i++)
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

#endif /* END NAND_SOFT_ECC */
