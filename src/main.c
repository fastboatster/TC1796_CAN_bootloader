/**************************************************************************
** 																		  *
**  INFINEON TriCore Bootloader, CANLoader, Version 1.0                   *
**																		  *
**  Id: main.c 2008-11-21 15:02:32Z winterstein                     	  *
**                                                                        *
**  DESCRIPTION :                                                         *
**      -Communication with PC via BL protocol                            *
**      -Implementation of flash functions:         					  *
**		  -Erase flash                                                    *
**		  -Program flash												  *
**		  -Verify flash								    				  *
**		  -Protect flash												  *
**  	  -Program SPRAM												  *
**  	  -Run programmed flash user code from address 0xA0000000         *
**  	  -Run programmed SPRAM user code from address 0xD4001400      	  *
**  							                                          *
**  REMARKS :                                                             *
**    -Stack and CSA initialized        								  *
**    -Interrupt and Trap table not initialized                            *
**  							                                          *
**************************************************************************/


typedef unsigned char   BYTE;
typedef unsigned int    UINT;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef unsigned long long QWORD;

#include "reg176x.h"

//FLASH CONSTANTS

#define PAGE_SIZE        	256   // program FLASH page size
#define DFLASH_PAGE_SIZE	128	  // data FLASH page size

#define PAGE_START_MASK			0x0FF
#define SECTOR_START_MASK		0x0FFF
#define PFLASH_BASE_MASK		(~0x1FFFFF)

#define DFLASH_PAGE_START_MASK		0x07F
#define DFLASH_SECTOR_START_MASK	0x0FFFF
#define DFLASH_BASE_MASK         	(~0x0FFFF)

#define FLASHPBUSY       0x00000001      /* Program Flash Busy */
#define FLASHD0BUSY      0x00000004      /* Data Flash Bank 0 Busy */
#define FLASHD1BUSY      0x00000008      /* Data Flash Bank 1 Busy */

#define FLASH_MASK_BUSY (FLASHPBUSY | FLASHD0BUSY | FLASHD1BUSY)

#define FLASHPFOPER      0x00000100      /* Program Flash Operation Error */
#define FLASHDFOPER      0x00000200      /* Data Flash Operation Error */
#define FLASHSQER        0x00000400      /* Command Sequence Error */
#define FLASH_MASK_ERROR (FLASHPFOPER | FLASHDFOPER | FLASHSQER)

#define FLASH_PROTECTION_ERROR_MASK 0x00000800 /* Protection Error */

#define PFLASH0_FSR 0xF8002010
#define PFLASH1_FSR 0xF8004010
#define DFLASH_FSR  0xF8002010

//Masks used in Erase procedure
#define PFLASH_MASK1 0x3FFF
#define PFLASH_MASK2 0x1FFFF
#define PFLASH_MASK3 0x3FFFF
#define DFLASH_MASK  0x7FFF

//BSL CONSTANTS

#define HEADER_BLOCK_SIZE  	   16
#define DATA_BLOCK_SIZE  	   PAGE_SIZE+8

#define HEADER_BLOCK 		   0x00
#define DATA_BLOCK 	 		   0x01
#define EOT_BLOCK			   0x02

#define BSL_PROGRAM_FLASH      0x00
#define BSL_RUN_FROM_FLASH     0x01
#define BSL_PROGRAM_SPRAM      0x02
#define BSL_RUN_FROM_SPRAM     0x03
#define BSL_ERASE_FLASH        0x04
#define BSL_PROTECT_FLASH      0x06
#define BSL_READ_MEM32         0x08
#define BSL_SEND_PSSWD         0x10

#define BSL_BLOCK_TYPE_ERROR   0xFF
#define BSL_MODE_ERROR 		   0xFE
#define BSL_CHKSUM_ERROR 	   0xFD
#define BSL_ADDRESS_ERROR 	   0xFC
#define BSL_ERASE_ERROR		   0xFB
#define BSL_PROGRAM_ERROR	   0xFA
#define BSL_VERIFICATION_ERROR 0xF9
#define BSL_PROTECTION_ERROR   0xF8
#define BSL_SUCCESS 		   0x55

#pragma noclear
BYTE HeaderBlock[HEADER_BLOCK_SIZE];
BYTE DataBlock[PAGE_SIZE+16];

//global variable indicating whether the device is
//a TC1766B or TC1796B device. The CAN register addresses of this
//of this are different from those of TC1766, TC1796
_Bool TC1766_B;


void SendCANMessage(DWORD data)
{
	if (TC1766_B) {
		CAN_MODATAL1_BB = data;
		CAN_MOFCR1_BB   = 0x04000000;
		CAN_MOCTR1_BB   = 0x0F200000;
	} else {
		CAN_MODATAL1 = data;
		CAN_MOFCR1   = 0x04000000;
		CAN_MOCTR1   = 0x0F200000;
	}
}


void EraseSector(DWORD dwSectorAddr,DWORD dwSize)
{

	_Bool SectorAddressValid = 0;
	_Bool PFlash = 1;

	//check if address is valid
	if((dwSectorAddr >= 0xA0000000) && (dwSectorAddr < 0xA0200000)) { //PFlash 0
		if (dwSectorAddr<0xA0020000) {
			if (!(dwSectorAddr & PFLASH_MASK1)) {
				SectorAddressValid = 1;
			}
		} else if ((dwSectorAddr>=0xA0020000) && (dwSectorAddr<0xA0040000)) {
			if (!(dwSectorAddr & PFLASH_MASK2))
				SectorAddressValid = 1;
		} else {
			if (!(dwSectorAddr & PFLASH_MASK3))
				SectorAddressValid = 1;
		}

	} else if ((dwSectorAddr >= 0xA0200000) && (dwSectorAddr < 0xA0400000)) { //PFlash 1
		if (dwSectorAddr<0xA0220000) {
			if (!(dwSectorAddr & PFLASH_MASK1))
				SectorAddressValid = 1;
		} else if ((dwSectorAddr>=0xA0220000) && (dwSectorAddr<0xA0240000)) {
			if (!(dwSectorAddr & PFLASH_MASK2))
				SectorAddressValid = 1;
		} else {
			if (!(dwSectorAddr & PFLASH_MASK3))
				SectorAddressValid = 1;
		}
	} else if ((dwSectorAddr == 0xAFE00000) || (dwSectorAddr == 0xAFE10000)) { //DFlash
		PFlash = 0;
		SectorAddressValid = 1;
	}

	if 	(!SectorAddressValid) {

		SendCANMessage(BSL_ADDRESS_ERROR);
		return;
	}

	//***************** Check if sector is already empty ************
	DWORD dwAddr = dwSectorAddr;
	DWORD size = dwSize;

	if((dwSize & 0x03) || (dwSize==0)) {
		SendCANMessage(BSL_ERASE_ERROR);
		return;
	}

	while(size>0)
	{
		DWORD* p=(DWORD*)(dwAddr);
		volatile DWORD dw=*p;
		if(0!=dw)
			break;

		dwAddr+=4;
		size-=4;
	}
	//Sector is empty
	if (size == 0) {
		SendCANMessage(BSL_SUCCESS);
		return;
	}
	//**************************************************************


	if (PFlash) {
		dwAddr=dwSectorAddr & PFLASH_BASE_MASK;
	} else {
		dwAddr=dwSectorAddr & DFLASH_BASE_MASK;
	}

	// prepare magic pointers
	volatile DWORD* pdwAddr5554=(DWORD*)(dwAddr+0x5554);
	volatile DWORD* pdwAddrAAA8=(DWORD*)(dwAddr+0xAAA8);

	// clear status register
	*pdwAddr5554=0x000000F5; //	ClearStatus(dwFlashBaseAddr);

	// erase physical sector
	*pdwAddr5554=0x000000AA;
	*pdwAddrAAA8=0x00000055;
	*pdwAddr5554=0x00000080;
	*pdwAddr5554=0x000000AA;
	*pdwAddrAAA8=0x00000055;

	if (PFlash)
		*((DWORD*)dwSectorAddr)=0x00000030;   // erase logical sector
	else
		*((DWORD*)dwSectorAddr)=0x00000040;   // erase physical sector

	//poll status register
	volatile DWORD* pdwFsr;

	if((dwSectorAddr>=0xA0200000) && (PFlash)) {
		pdwFsr=(DWORD*)PFLASH1_FSR;
	} else {
		pdwFsr=(DWORD*)PFLASH0_FSR;
	}

	while(*pdwFsr & FLASH_MASK_BUSY) {};

	if(*pdwFsr & FLASH_MASK_ERROR)
	{
		SendCANMessage(BSL_ERASE_ERROR);


	} else if (*pdwFsr & FLASH_PROTECTION_ERROR_MASK) {
		SendCANMessage(BSL_PROTECTION_ERROR);

	} else {
		SendCANMessage(BSL_SUCCESS);
	}

}

_Bool ProgramFlashPage(DWORD dwPageAddr,_Bool PFlash)
{
	DWORD dwFlashBaseAddr;
	if (PFlash)
		dwFlashBaseAddr=dwPageAddr & PFLASH_BASE_MASK;
	else
		dwFlashBaseAddr=dwPageAddr & DFLASH_BASE_MASK;

	// check if it is a valid page start address
	if (PFlash) {
		if(dwPageAddr & PAGE_START_MASK)
		{
			SendCANMessage(BSL_ADDRESS_ERROR);
			return 0;
		}
	} else {
		if(dwPageAddr & DFLASH_PAGE_START_MASK)
		{
			SendCANMessage(BSL_ADDRESS_ERROR);
			return 0;
		}
	}

	// prepare magic pointers
	volatile DWORD* pdwAddr5554=(DWORD*)(dwFlashBaseAddr+0x5554);
	volatile DWORD* pdwAddrAAA8=(DWORD*)(dwFlashBaseAddr+0xAAA8);
	volatile QWORD* pqwAddr55F0=(QWORD*)(dwFlashBaseAddr+0x55F0);

	// clear status register
	*pdwAddr5554=0x000000F5;


	// enter page mode
	if (PFlash)
		*pdwAddr5554=0x00000050;  // program FLASH
	else
		*pdwAddr5554=0x0000005D;  // data FLASH

	// load assembly buffer
	UINT ui;
	for(ui=0;ui<PAGE_SIZE;ui+=8)
	{
		if ((!PFlash) && (ui==DFLASH_PAGE_SIZE))
			break;

		QWORD qw = (QWORD)(DataBlock[2+ui] & 0xFF);
		qw      += ((QWORD)(DataBlock[3+ui] & 0xFF) << 8);
		qw      += ((QWORD)(DataBlock[4+ui] & 0xFF) << 16);
		qw      += ((QWORD)(DataBlock[5+ui] & 0xFF) << 24);

		qw      += ((QWORD)(DataBlock[6+ui] & 0xFF) << 32);
		qw      += ((QWORD)(DataBlock[7+ui] & 0xFF) << 40);
		qw      += ((QWORD)(DataBlock[8+ui] & 0xFF) << 48);
		qw      += ((QWORD)(DataBlock[9+ui] & 0xFF) << 56);

		*pqwAddr55F0=qw;

	}

	// write page
	*pdwAddr5554=0x000000AA;
	*pdwAddrAAA8=0x00000055;
	*pdwAddr5554=0x000000A0;
	*((DWORD*)dwPageAddr)=0x000000AA;

	//poll status register
	volatile DWORD* pdwFsr;

	if((dwPageAddr>=0xA0200000) && (PFlash)) {
		pdwFsr=(DWORD*)PFLASH1_FSR;
	} else {
		pdwFsr=(DWORD*)PFLASH0_FSR;
	}

	while(*pdwFsr & FLASH_MASK_BUSY) {};

	if(*pdwFsr & FLASH_MASK_ERROR)
	{
		SendCANMessage(BSL_PROGRAM_ERROR);
		return 0;
	} else if (*pdwFsr & FLASH_PROTECTION_ERROR_MASK) {
		SendCANMessage(BSL_PROTECTION_ERROR);
		return 0;
	} else {
		if (DataBlock[1] == 0x00) //no verification mode, send ackn here
			SendCANMessage(BSL_SUCCESS);
		return 1;
	}

}


void VerifyPage(DWORD PageAddr, _Bool PFlash)
{
	UINT ui;
	_Bool error=0;
	DWORD dwPageAddr=PageAddr;

	//check if address is valid doesn't need to be done anymore


	for(ui=0;ui<PAGE_SIZE;ui+=4)
	{
		if ((!PFlash) && (ui==DFLASH_PAGE_SIZE))
			break;

		volatile DWORD* p=(DWORD*)(dwPageAddr);
		DWORD FlashDW=*p;

		DWORD SentDW  =  (DataBlock[2+ui] & 0xFF);
			  SentDW += ((DataBlock[3+ui] & 0xFF) << 8);
			  SentDW += ((DataBlock[4+ui] & 0xFF) << 16);
			  SentDW += ((DataBlock[5+ui] & 0xFF) << 24);


		if(SentDW != FlashDW)
		{
			error = 1;
		}

		dwPageAddr+=4;
	}

	if (error) {
		SendCANMessage(BSL_VERIFICATION_ERROR);
	} else {
		SendCANMessage(BSL_SUCCESS);
	}
}


void ProgramSPRAMPage(DWORD dwPageAddr)
{
	//4 byte aligned address?
	if (dwPageAddr & 0x03) {
		SendCANMessage(BSL_ADDRESS_ERROR);
		return;
	}

	volatile DWORD *dest_ptr = (DWORD *) (dwPageAddr);
	UINT ui;

	for (ui=0; ui<PAGE_SIZE; ui+=4) {

		DWORD dw = (DWORD)(DataBlock[2+ui] & 0xFF);
		dw      += ((DWORD)(DataBlock[3+ui] & 0xFF) << 8);
		dw      += ((DWORD)(DataBlock[4+ui] & 0xFF) << 16);
		dw      += ((DWORD)(DataBlock[5+ui] & 0xFF) << 24);

		*dest_ptr++ = dw;

	}

	SendCANMessage(BSL_SUCCESS);
}


BYTE CheckProtection()
{

	volatile DWORD* pdwFsr;

	if (HeaderBlock[10] == 1)
        pdwFsr=(DWORD*)PFLASH1_FSR;
    else
        pdwFsr=(DWORD*)PFLASH0_FSR;


	if (!(*pdwFsr & (1<<16)))
		return 0xFF; //protection disabled

	if ((*pdwFsr & (1<<18)) && (!(*pdwFsr & (1<<19))) )
		return 0xFE; //read protection enabled

	if ((*pdwFsr & (1<<21)) && (!(*pdwFsr & (1<<25))) )
		return 0x00; //write protection USER 0 enabled

	if ((*pdwFsr & (1<<22)) && (!(*pdwFsr & (1<<26))) )
		return 0x01; //write protection USER 1 enabled

	if (*pdwFsr & (1<<23))
		return 0x02; //write protection USER 0 enabled

	return 0xFF;
}

void InstallWriteProtection(void)
{

	DWORD dwFlashBaseAddr;
	if (HeaderBlock[10] == 0x01)
        dwFlashBaseAddr = 0xA0200000;
    else
        dwFlashBaseAddr = 0xA0000000;

	// prepare magic pointers
	volatile DWORD* pdwAddr5554=(DWORD*)(dwFlashBaseAddr+0x5554);
	volatile DWORD* pdwAddrAAA8=(DWORD*)(dwFlashBaseAddr+0xAAA8);
	volatile QWORD* pqwAddr55F0=(QWORD*)(dwFlashBaseAddr+0x55F0);

	// clear status register
	*pdwAddr5554=0x000000F5;

	// enter page mode
	*pdwAddr5554=0x00000050;


	// load assembly buffer
	DWORD User0Config = 0x00;
	//Sectors of PFlash to be write protected
    User0Config |= ((HeaderBlock[11] & 0xFF) << 8);
    User0Config |= ( HeaderBlock[12] & 0xFF);

    User0Config |= 0x4000; //Data Flash Excluded from Read Protection
    User0Config &= 0x7FFF; //No Read Protection


	DWORD User0Password1;
	User0Password1  = ((HeaderBlock[2] & 0xFF) << 24);
    User0Password1 |= ((HeaderBlock[3] & 0xFF) << 16);
    User0Password1 |= ((HeaderBlock[4] & 0xFF) << 8);
    User0Password1 |= ( HeaderBlock[5] & 0xFF);

	DWORD User0Password2;
	User0Password2  = ((HeaderBlock[6] & 0xFF) << 24);
    User0Password2 |= ((HeaderBlock[7] & 0xFF) << 16);
    User0Password2 |= ((HeaderBlock[8] & 0xFF) << 8);
    User0Password2 |= ( HeaderBlock[9] & 0xFF);

	//Bytes 0-7
	QWORD qw;
	qw=(QWORD)(User0Config);
	*pqwAddr55F0=qw;

	//Bytes 8-15
	qw=(QWORD)(User0Config);
	*pqwAddr55F0=qw;

	//Bytes 16-23
	qw=(QWORD)(User0Password1);
	qw+=((QWORD)(User0Password2)<<32);
	*pqwAddr55F0=qw;

	//Bytes 24-31
	qw=(QWORD)(User0Password1);
	qw+=((QWORD)(User0Password2)<<32);
	*pqwAddr55F0=qw;

	//Bytes 32-255 = 0
	UINT ui;
	for(ui=32;ui<PAGE_SIZE;ui+=8)
		*pqwAddr55F0 = 0;

	// write page 0
	*pdwAddr5554=0x000000AA;
	*pdwAddrAAA8=0x00000055;
	*pdwAddr5554=0x000000C0;
	*((DWORD*)dwFlashBaseAddr)=0x000000AA;

	//poll status register
	volatile DWORD* pdwFsr;

    if (HeaderBlock[10] == 0x01)
        pdwFsr=(DWORD*)PFLASH1_FSR;
    else
        pdwFsr=(DWORD*)PFLASH0_FSR;

	while(*pdwFsr & FLASH_MASK_BUSY) {};

	if((*pdwFsr & FLASH_MASK_ERROR) || (*pdwFsr & FLASH_PROTECTION_ERROR_MASK))
	{
		SendCANMessage(BSL_PROGRAM_ERROR);
		return;
	}

	//Confirm Protection

	// enter page mode
	*pdwAddr5554=0x00000050;

	//load assembly buffer
	//Bytes 0-7
	qw=(QWORD)(0x8AFE15C3); //Confirmation code
	*pqwAddr55F0=qw;

	//Bytes 8-15
	qw=(QWORD)(0x8AFE15C3); //Confirmation code
	*pqwAddr55F0=qw;

	//Bytes 16-255 = 0
	for(ui=16;ui<PAGE_SIZE;ui+=8)
		*pqwAddr55F0 = 0;

	//write page 0
	*pdwAddr5554=0x000000AA;
	*pdwAddrAAA8=0x00000055;
	*pdwAddr5554=0x000000C0;

	dwFlashBaseAddr = 0xA0000200; //Page 2
	*((DWORD*)dwFlashBaseAddr)=0x000000AA;

	//poll status register
    if (HeaderBlock[10] == 0x01)
        pdwFsr=(DWORD*)PFLASH1_FSR;
    else
        pdwFsr=(DWORD*)PFLASH0_FSR;

	while(*pdwFsr & FLASH_MASK_BUSY) {};

	if(*pdwFsr & FLASH_MASK_ERROR)
	{
		SendCANMessage(BSL_PROGRAM_ERROR);

	} else if (*pdwFsr & FLASH_PROTECTION_ERROR_MASK) {
		SendCANMessage(BSL_PROTECTION_ERROR);

	} else {
		SendCANMessage(BSL_SUCCESS);
	}
}

_Bool DisableWriteProtection()
{
	DWORD dwFlashBaseAddr;
	if (HeaderBlock[10] == 0x01)
        dwFlashBaseAddr = 0xA0200000;
    else
        dwFlashBaseAddr = 0xA0000000;

	// prepare magic pointers
	volatile DWORD* pdwAddr5554=(DWORD*)(dwFlashBaseAddr+0x5554);
	volatile DWORD* pdwAddrAAA8=(DWORD*)(dwFlashBaseAddr+0xAAA8);
	volatile DWORD* pdwAddr553C=(DWORD*)(dwFlashBaseAddr+0x553C);
	volatile DWORD* pdwAddr5558=(DWORD*)(dwFlashBaseAddr+0x5558);

	// clear status register
	*pdwAddr5554=0x000000F5;

	DWORD User0Password1;
	User0Password1  = ((HeaderBlock[2] & 0xFF) << 24);
    User0Password1 |= ((HeaderBlock[3] & 0xFF) << 16);
    User0Password1 |= ((HeaderBlock[4] & 0xFF) << 8);
    User0Password1 |= ( HeaderBlock[5] & 0xFF);

	DWORD User0Password2;
	User0Password2  = ((HeaderBlock[6] & 0xFF) << 24);
    User0Password2 |= ((HeaderBlock[7] & 0xFF) << 16);
    User0Password2 |= ((HeaderBlock[8] & 0xFF) << 8);
    User0Password2 |= ( HeaderBlock[9] & 0xFF);

	// write page 0
	*pdwAddr5554=0x000000AA;
	*pdwAddrAAA8=0x00000055;
	*pdwAddr553C=0x00000000;

	*pdwAddrAAA8=User0Password1;
	*pdwAddrAAA8=User0Password2;

	*pdwAddr5558=0x00000005; //disable write protection

	//poll status register
	volatile DWORD* pdwFsr;
    if (HeaderBlock[10] == 0x01)
        pdwFsr=(DWORD*)PFLASH1_FSR;
    else
        pdwFsr=(DWORD*)PFLASH0_FSR;

	if((*pdwFsr & FLASH_MASK_ERROR) || (*pdwFsr & FLASH_PROTECTION_ERROR_MASK))
	{
		SendCANMessage(BSL_PROTECTION_ERROR);
		return 0;
	}

	return 1;
}

void RemoveProtection(void)
{
	DWORD dwFlashBaseAddr;
	if (HeaderBlock[10] == 0x01)
        dwFlashBaseAddr = 0xA0200000;
    else
        dwFlashBaseAddr = 0xA0000000;


	// prepare magic pointers
	volatile DWORD* pdwAddr5554=(DWORD*)(dwFlashBaseAddr+0x5554);
	volatile DWORD* pdwAddrAAA8=(DWORD*)(dwFlashBaseAddr+0xAAA8);
	volatile DWORD* pdwAddr0000=(DWORD*)(dwFlashBaseAddr);

	// clear status register
	*pdwAddr5554=0x000000F5;

	// erase page 0
	*pdwAddr5554=0x000000AA;
	*pdwAddrAAA8=0x00000055;
	*pdwAddr5554=0x00000080;
	*pdwAddr5554=0x000000AA;
	*pdwAddrAAA8=0x00000055;
	*pdwAddr0000=0x000000C0;

	//poll status register
	volatile DWORD* pdwFsr;
    if (HeaderBlock[10] == 0x01)
        pdwFsr=(DWORD*)PFLASH1_FSR;
    else
        pdwFsr=(DWORD*)PFLASH0_FSR;


	while(*pdwFsr & FLASH_MASK_BUSY) {};

	if(*pdwFsr & FLASH_MASK_ERROR)
	{
		SendCANMessage(BSL_PROGRAM_ERROR);

	} else if (*pdwFsr & FLASH_PROTECTION_ERROR_MASK) {
		SendCANMessage(BSL_PROTECTION_ERROR);

	} else {
		SendCANMessage(BSL_SUCCESS);
	}
}


void RunUserCode(void)
{
	SendCANMessage(BSL_SUCCESS);

    __asm("movh.a %a15,0xa000\n"			//load flash
    	"\tlea %a15,[%a15]0x0000\n"			//base address
    	"\tji %a15");						//au revoir!

}

void RunUserCodeSPRAM(void)
{
	SendCANMessage(BSL_SUCCESS);

    __asm("movh.a %a15,0xd400\n"			//load SPRAM
    	"\tlea %a15,[%a15]0x1400\n"			//base address
    	"\tji %a15");						//au revoir!

}


_Bool WaitForDataBlock(void)
{
	UINT i;
	BYTE chksum = 0;
	DWORD dataL;
	DWORD dataH;

	//Receive the first two CAN messages (8 bytes) subsequently to check for an EOT block of 16 bytes
	//if HEADER_BLOCK_SIZE is changed, it must be a multiple of 8!
	//if HEADER_BLOCK_SIZE is equal to the size of the EOT block
	for (i=0; i<HEADER_BLOCK_SIZE; i+=8) {

		while ((!(CAN_MOCTR0 & 0x08)) &&
			   (!(CAN_MOCTR0_BB & 0x08)));
		if (CAN_MOCTR0_BB & 0x08)
			TC1766_B = 1;
		else
			TC1766_B = 0;

		if (TC1766_B) {
			dataL = CAN_MODATAL0_BB;
			dataH = CAN_MODATAH0_BB;
		} else {
			dataL = CAN_MODATAL0;
			dataH = CAN_MODATAH0;
		}

		DataBlock[i+0] = (dataL & 0xFF);
		DataBlock[i+1] = ((dataL>>8) & 0xFF);
		DataBlock[i+2] = ((dataL>>16) & 0xFF);
		DataBlock[i+3] = ((dataL>>24) & 0xFF);
		DataBlock[i+4] = (dataH & 0xFF);
		DataBlock[i+5] = ((dataH>>8) & 0xFF);
		DataBlock[i+6] = ((dataH>>16) & 0xFF);
		DataBlock[i+7] = ((dataH>>24) & 0xFF);
		if (TC1766_B) {
			CAN_MOCTR0_BB   = 0x00A00008;  //Clear NewDat
		} else {
			CAN_MOCTR0   = 0x00A00008;  //Clear NewDat		}
		}
	}

	//check for EOT block and return if found and correct
	if (DataBlock[0] == EOT_BLOCK) {
		//calculate checksum
		for (i=1; i<HEADER_BLOCK_SIZE-1; i++) {
			chksum = chksum ^ DataBlock[i];
		}
		//compare checksums
		if (chksum != DataBlock[HEADER_BLOCK_SIZE-1]) {

			SendCANMessage(BSL_CHKSUM_ERROR);
			DataBlock[0] = 0xFF; //make block type invalid
			return 0;
		}

		return 0;
	}

	if (DataBlock[0] != DATA_BLOCK) {
		SendCANMessage(BSL_BLOCK_TYPE_ERROR);
		return 0;
	}

	//remaining 248 bytes
	for (i=16; i<PAGE_SIZE+8; i+=8) {
		while ((!(CAN_MOCTR0 & 0x08)) &&
			   (!(CAN_MOCTR0_BB & 0x08)));
		if (CAN_MOCTR0_BB & 0x08)
			TC1766_B = 1;
		else
			TC1766_B = 0;

		if (TC1766_B) {
			dataL = CAN_MODATAL0_BB;
			dataH = CAN_MODATAH0_BB;
		} else {
			dataL = CAN_MODATAL0;
			dataH = CAN_MODATAH0;
		}
		DataBlock[i+0] = (dataL & 0xFF);
		DataBlock[i+1] = ((dataL>>8) & 0xFF);
		DataBlock[i+2] = ((dataL>>16) & 0xFF);
		DataBlock[i+3] = ((dataL>>24) & 0xFF);
		DataBlock[i+4] = (dataH & 0xFF);
		DataBlock[i+5] = ((dataH>>8) & 0xFF);
		DataBlock[i+6] = ((dataH>>16) & 0xFF);
		DataBlock[i+7] = ((dataH>>24) & 0xFF);
		if (TC1766_B) {
			CAN_MOCTR0_BB   = 0x00A00008;  //Clear NewDat
		} else {
			CAN_MOCTR0   = 0x00A00008;  //Clear NewDat		}
		}
	}

	for (i=1; i<PAGE_SIZE+7; i++)
		chksum = chksum ^ DataBlock[i];

	if (chksum != DataBlock[PAGE_SIZE+7]) {
		SendCANMessage(BSL_CHKSUM_ERROR);
		return 0;
	}

	return 1;

}


_Bool WaitForHeader(void)
{
	// does this need to be 16 bytes?
	UINT i;
	BYTE chksum = 0;
	DWORD dataL;
	DWORD dataH;

	//Receive two CAN messages (8 bytes) subsequently to form a Header block of 16 bytes
	//if HEADER_BLOCK_SIZE is changed, it must be a multiple of 8!
	for (i=0; i<HEADER_BLOCK_SIZE; i+=8) {
		while ((!(CAN_MOCTR0 & 0x08)) &&
			   (!(CAN_MOCTR0_BB & 0x08)));

		if (CAN_MOCTR0_BB & 0x08)
			TC1766_B = 1;
		else
			TC1766_B = 0;

		if (TC1766_B) {
			dataL = CAN_MODATAL0_BB;
			dataH = CAN_MODATAH0_BB;
		} else {
			dataL = CAN_MODATAL0;
			dataH = CAN_MODATAH0;
		}

		HeaderBlock[i+0] = (dataL & 0xFF);
		HeaderBlock[i+1] = ((dataL>>8) & 0xFF);
		HeaderBlock[i+2] = ((dataL>>16) & 0xFF);
		HeaderBlock[i+3] = ((dataL>>24) & 0xFF);
		HeaderBlock[i+4] = (dataH & 0xFF);
		HeaderBlock[i+5] = ((dataH>>8) & 0xFF);
		HeaderBlock[i+6] = ((dataH>>16) & 0xFF);
		HeaderBlock[i+7] = ((dataH>>24) & 0xFF);
		if (TC1766_B) {
			CAN_MOCTR0_BB   = 0x00A00008;  //Clear NewDat
		} else {
			CAN_MOCTR0   = 0x00A00008;  //Clear NewDat
		}
	}

	if (HeaderBlock[0] != HEADER_BLOCK) {
		SendCANMessage(BSL_BLOCK_TYPE_ERROR);
		return 0;
	}

	if ((HeaderBlock[1]!=BSL_PROGRAM_FLASH) &&
		(HeaderBlock[1]!=BSL_PROGRAM_SPRAM) &&
		(HeaderBlock[1]!=BSL_ERASE_FLASH) &&
		(HeaderBlock[1]!=BSL_RUN_FROM_FLASH) &&
		(HeaderBlock[1]!=BSL_RUN_FROM_SPRAM) &&
		(HeaderBlock[1]!=BSL_PROTECT_FLASH) &&
		(HeaderBlock[1]!=BSL_READ_MEM32)) {
			SendCANMessage(BSL_MODE_ERROR);

		return 0;
	}
	for (i=1; i<HEADER_BLOCK_SIZE-1; i++) {
		chksum = chksum ^ HeaderBlock[i];
	}
	// need to send checksum as last byte in the second CAN message
	if (chksum != HeaderBlock[HEADER_BLOCK_SIZE-1]) {

		SendCANMessage(BSL_CHKSUM_ERROR);
		return 0;
	}

	return 1;
}


/*
 * 0x08: Read32.
 * Uses bytes 1-4 to set the address to read from.
 * Returns the value from performing a direct Tricore bus read to the read address. This can be RAM, a register, or an address in Flash if it is unlocked and in read mode.
 */
void Read32(DWORD dwaddress) {

	DWORD address_value = *(DWORD *) dwaddress;
	BYTE canData[8];
	//	canData[0] = 0x2;
	canData[0] = BSL_READ_MEM32;
	canData[4] = (address_value >> 24) & 0xFF;
	canData[3] = (address_value >> 16) & 0xFF;
	canData[2] = (address_value >> 8) & 0xFF;
	canData[1] = address_value & 0xFF;
	canData[5] = 0xFF;
	canData[6] = 0xFF;
	canData[7] = 0xFF;
	SendCANMessage(canData); // send data
}

int main(void)
{
	//assuming a TC1766B device
	//this value will be changed after the first CAN message
	//is received from HOST
	TC1766_B = 1;

	for (;;) {
		if (WaitForHeader()) {

            DWORD dwSize;
			DWORD dwAddress;

			dwAddress  = ((HeaderBlock[2] & 0xFF) << 24);
		    dwAddress |= ((HeaderBlock[3] & 0xFF) << 16);
		    dwAddress |= ((HeaderBlock[4] & 0xFF) << 8);
		    dwAddress |= ( HeaderBlock[5] & 0xFF);

			switch (HeaderBlock[1]) {
			// read contents of the address in memory
			case BSL_READ_MEM32:
				Read32(dwAddress);
				break;
			case BSL_SEND_PSSWD:
				// will get a header
				break;
			case BSL_PROGRAM_FLASH:
				SendCANMessage(BSL_SUCCESS); // send ackn for header

				_Bool validRange=0;
				_Bool PFlash=1;
				//check if user start address is in boundary
				if ((dwAddress >= 0xA0000000) && (dwAddress <= 0xA03FFFFF)) {

					validRange=1;


				} else if (((dwAddress >= 0xAFE00000) && (dwAddress <= 0xAFE07FFF)) ||
					((dwAddress >= 0xAFE10000) && (dwAddress <= 0xAFE17FFF))) {

					validRange=1;
					PFlash = 0;
				}
				if (validRange) {
					DWORD addr=dwAddress;
					while (WaitForDataBlock()) {
						if ((ProgramFlashPage(addr,PFlash)) &&
							(DataBlock[1]==0x01)) {
							VerifyPage(addr,PFlash);
						}
						if (PFlash)
							addr += PAGE_SIZE;
						else
							addr += DFLASH_PAGE_SIZE;

					}

					if (DataBlock[0] == EOT_BLOCK) {
						SendCANMessage(BSL_SUCCESS);
					}
				} else {
					SendCANMessage(BSL_ADDRESS_ERROR);
				}
				break;

			case BSL_PROGRAM_SPRAM:
				SendCANMessage(BSL_SUCCESS); // send ackn for header


				if ((dwAddress >= 0xD4001400) && (dwAddress < 0xD400C000)) {
					DWORD addr=dwAddress;

					while (WaitForDataBlock()) {

						ProgramSPRAMPage(addr);
						addr += PAGE_SIZE;
					}
					if (DataBlock[0] == EOT_BLOCK) {
						SendCANMessage(BSL_SUCCESS);
					}

				} else {
					SendCANMessage(BSL_ADDRESS_ERROR);
				}
				break;

			case BSL_ERASE_FLASH:

				//Read the sector size additionally to the sector address
				dwSize  = ((HeaderBlock[6] & 0xFF) << 24);
				dwSize |= ((HeaderBlock[7] & 0xFF) << 16);
				dwSize |= ((HeaderBlock[8] & 0xFF) << 8);
				dwSize |= ( HeaderBlock[9] & 0xFF);

				EraseSector(dwAddress, dwSize);
				break;

			case BSL_RUN_FROM_FLASH:
				RunUserCode();
				break;

			case BSL_RUN_FROM_SPRAM:
				RunUserCodeSPRAM();
				break;

			case BSL_PROTECT_FLASH:

				if (CheckProtection() == 0xFF) { //unprotected

					InstallWriteProtection();
				} else {				  		 //protected

					if(DisableWriteProtection())
						RemoveProtection();

				}

				for (;;); //infinite loop, device shall be reset!

			}
		}
    }

    return 0;
}


