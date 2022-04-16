/*
 * spiflash.h
 *
 *  Created on: Feb 10, 2022
 *  Updated on: Apr 16, 2022
 *      Author: asz
 */
#ifndef INC_SPIFLASH_H_
#define INC_SPIFLASH_H_

#include "main.h"
#include "stdio.h"
#include <stdbool.h>

typedef enum
{
  FLASH_OK			= 0x00,
  FLASH_ERROR		= 0x01,

  FLASH_LOCKED		= 0x02,
  FLASH_UNLOCKED	= 0x03,

  FLASH_EMPTY		= 0x04,
  FLASH_NOTEMPTY	= 0x05,
} FLASH_StatusTypeDef;

typedef enum
{
	Winbond		= 0xEF,
	Spansion	= 0x01,
	Micron		= 0x20,
	OnSemi		= 0x62,
	Adesto		= 0x1F,
	ISSI		= 0x9D,
	GigaDevice	= 0xC8,
} MFID_t;

typedef enum
{
	W25Q10 = 1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,

	S25FL032P,
	S25FL064P,
	S25FL064LS,
	S25FL128S,
	S25FL256S,

	M45PE20,
	M45PE80,
	M45PE16,
	M25P32,
	MT25QX64,
	MT25QX128,
	MT25QX256,
	MT25QX512,
	MT25QX01,
	MT25QX02,

	LE25X20,
	LE25X40,
	LE25X81,

	AT45DB02,
	AT45DB04,
	AT45DB08,
	AT45DB16,
	AT45DB32,
	AT45DB64,

	IS25LQ025,
	IS25LQ512,
	IS25LQ010,
	IS25LQ020,
	IS25LQ040,

	GD25LQ05,
	GD25LQ10,
	GD25LQ20,
	GD25LQ40,
} Chip_ID_t;

class FLASHLIB {
	private:
		Chip_ID_t __CHIP_ID;
		MFID_t	  __MFID;
		uint32_t  __ID;
		uint64_t  __UniqID;

		uint16_t  __PagSiz;
		uint32_t  __PagCnt;
		uint32_t  __SectSiz;
		uint32_t  __SectCnt;
		uint32_t  __BlkSiz;
		uint32_t  __BlkCnt;
		uint32_t  __SizInKB;

		uint8_t   __StatReg1;
		uint8_t  __StatReg2;
		uint8_t   __StatReg3;
		uint8_t   __Lock;
		uint8_t   __dummy = 0xA5;

		SPI_HandleTypeDef* __bus;
		GPIO_TypeDef* CS_PORT;
		uint16_t CS_PIN;

		uint8_t txrxSpi(uint8_t Data);
		FLASH_StatusTypeDef readUniqID(void);
		FLASH_StatusTypeDef readID(void);

		void writeEnable(void);
		void writeDisable(void);
		uint8_t readStatusRegister(uint8_t SelectStatusRegister_1_2_3);
		void writeStatusRegister(uint8_t SelectStatusRegister_1_2_3, uint8_t Data);
		void waitForWriteEnd(void);
	public:
		FLASHLIB(SPI_HandleTypeDef &bus, GPIO_TypeDef *GPIO_CS_PORT, uint16_t GPIO_CS_PIN);
		~FLASHLIB();

		FLASH_StatusTypeDef init(void);

		int getID(void);
		int getChipID(void);
		uint64_t getUniqID(void);
		int getSectorCount(void);
		int getSectorSize(void);
		int getPageCount(void);
		int getPageSize(void);
		int getBlockCount(void);
		int getBlockSize(void);
		int getFlashSize(void);

		FLASH_StatusTypeDef eraseChip(void);
		FLASH_StatusTypeDef eraseSector(uint32_t SectorAddr);
		FLASH_StatusTypeDef eraseBlock(uint32_t BlockAddr);

		uint32_t PageToSector(uint32_t PageAddress);
		uint32_t PageToBlock(uint32_t PageAddress);
		uint32_t SectorToBlock(uint32_t SectorAddress);
		uint32_t SectorToPage(uint32_t SectorAddress);
		uint32_t BlockToPage(uint32_t BlockAddress);

		FLASH_StatusTypeDef isEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize);
		FLASH_StatusTypeDef isEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize);
		FLASH_StatusTypeDef isEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize);

		FLASH_StatusTypeDef writeByte(uint8_t pBuffer, uint32_t Bytes_Address);
		FLASH_StatusTypeDef writePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize);
		FLASH_StatusTypeDef writeSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
		FLASH_StatusTypeDef writeBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize);

		FLASH_StatusTypeDef readByte(uint8_t *pBuffer, uint32_t Bytes_Address);
		FLASH_StatusTypeDef readBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
		FLASH_StatusTypeDef readPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize);
		FLASH_StatusTypeDef readSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);
		FLASH_StatusTypeDef readBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize);
};

#endif
