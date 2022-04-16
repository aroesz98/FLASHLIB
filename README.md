# FLASHLIB
SPI flash memory library for STM32

Hello. This is my first library for SPI Flash memories. Usage is pretty simple:

Create a SPIFLASH object by:
	FLASHLIB flash(flash_spi, MEM_CS_GPIO_Port, MEM_CS_Pin);

Memories are identified by manufacturer id. This is actual list of supported memories:
  Winbond:
    W25Q10,
    W25Q20,
    W25Q40,
    W25Q80,
    W25Q16,
    W25Q32,
    W25Q64,
    W25Q128,
    W25Q256,
    W25Q512,

  Spansion:
    S25FL032P,
    S25FL064P,
    S25FL064LS,
    S25FL128S,
    S25FL256S,

  Micron:
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

  OnSemicoductor:
    LE25X20,
    LE25X40,
    LE25X81,

  Adesto:
    AT45DB02,
    AT45DB04,
    AT45DB08,
    AT45DB16,
    AT45DB32,
    AT45DB64,

  ISSI:
    IS25LQ025,
    IS25LQ512,
    IS25LQ010,
    IS25LQ020,
    IS25LQ040,

  GigaDevice:
    GD25LQ05,
    GD25LQ10,
    GD25LQ20,
    GD25LQ40,
    
There are all functions of library:

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

They returns value or status. All possible returned statuses:

    FLASH_OK			= 0x00,
    FLASH_ERROR		= 0x01,

    FLASH_LOCKED		= 0x02,
    FLASH_UNLOCKED	= 0x03,

    FLASH_EMPTY		= 0x04,
    FLASH_NOTEMPTY	= 0x05,
    
Please comment if you found any errors.
