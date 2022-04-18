# FLASHLIB
SPI flash memory library for STM32

Hello. This is my first library for SPI Flash memories. Usage is pretty simple:

Create a FLASHLIB object by:

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
	  Macronix:
		MX25UVR20,
		MX25UVR40,
		MX25UVR80,
		MX25UVR16,
		MX25UVR32,
		MX25UVR64,
		MX25UVR128,
	  Fudan Microelectroncis:
		FM25Q02,
		FM25Q04,
		FM25Q08,
		FM25Q16,
		FM25Q32,
		FM25Q64,
		FM25Q128,
		
All functions of library:

		FLASH_StatusTypeDef init(void);		//Initialize flash memory

		int getID(void);			//get ID based on flash size
		int getChipID(void);			//get chip ID - returns ID based on structure inside library
		uint64_t getUniqID(void);		//returns unique ID of flash memory
		int getSectorCount(void);		//returns number of sectors
		int getSectorSize(void);		//returns sector size
		int getPageCount(void);			//returns number of pages
		int getPageSize(void);			//returns page size
		int getBlockCount(void);		//returns number of blocks
		int getBlockSize(void);			//returns block size
		int getFlashSize(void);			//returns memory size in bytes

		FLASH_StatusTypeDef eraseChip(void);				//Full Chip Erase
		FLASH_StatusTypeDef eraseSector(uint32_t SectorAddr);		//Erase 1 Sector
		FLASH_StatusTypeDef eraseBlock(uint32_t BlockAddr);		//Erase 1 Block

		uint32_t PageToSector(uint32_t PageAddress);
		uint32_t PageToBlock(uint32_t PageAddress);
		uint32_t SectorToBlock(uint32_t SectorAddress);
		uint32_t SectorToPage(uint32_t SectorAddress);
		uint32_t BlockToPage(uint32_t BlockAddress);

		//Check if page is empty - returns FLASH_EMPTY, FLASH_NOTEMPTY or FLASH_ERROR when error occurs.
		FLASH_StatusTypeDef isEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize);
		
		//Check if sector is empty - returns FLASH_EMPTY, FLASH_NOTEMPTY or FLASH_ERROR when error occurs.
		FLASH_StatusTypeDef isEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize);
		
		//Check if block is empty - returns FLASH_EMPTY, FLASH_NOTEMPTY or FLASH_ERROR when error occurs.
		FLASH_StatusTypeDef isEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize);

		//Write single byte
		FLASH_StatusTypeDef writeByte(uint8_t pBuffer, uint32_t Bytes_Address);
		
		//Write page from buffer at selected address. You can choose offset & number of bytes to write.
		FLASH_StatusTypeDef writePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize);
		
		//Write sector from buffer at selected address. You can choose offset & number of bytes to write.
		FLASH_StatusTypeDef writeSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
		
		//Write block from buffer at selected address. You can choose offset & number of bytes to write.
		FLASH_StatusTypeDef writeBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize);

		//Read 1 byte from memory.
		FLASH_StatusTypeDef readByte(uint8_t *pBuffer, uint32_t Bytes_Address);
		
		//Read block of bytes to buffer. Can be more than block size.
		FLASH_StatusTypeDef readBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
		
		//Read page to buffer at selected address. You can choose offset & number of bytes to write.
		FLASH_StatusTypeDef readPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize);
		
		//Read sector to buffer at selected address. You can choose offset & number of bytes to write.
		FLASH_StatusTypeDef readSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);
		
		//Read block to buffer at selected address. You can choose offset & number of bytes to write.
		FLASH_StatusTypeDef readBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize);

They returns value or status. All possible returned statuses:

    FLASH_OK
    FLASH_ERROR
    FLASH_LOCKED
    FLASH_UNLOCKED
    FLASH_EMPTY
    FLASH_NOTEMPTY
    
Please comment if you found any errors.
