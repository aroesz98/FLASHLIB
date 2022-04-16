/*
 * spiflash.cpp
 *
 *  Created on: Feb 10, 2022
 *  Updated on: Apr 16, 2022
 *      Author: asz
 */
#include "flashlib.h"

FLASHLIB::FLASHLIB(SPI_HandleTypeDef &bus, GPIO_TypeDef *GPIO_CS_PORT, uint16_t GPIO_CS_PIN){
	__bus = &bus;
	CS_PORT	 = GPIO_CS_PORT;
	CS_PIN	 = GPIO_CS_PIN;
}

FLASHLIB::~FLASHLIB(){};

uint8_t FLASHLIB::txrxSpi(uint8_t Data)
{
	uint8_t ret;
	HAL_SPI_TransmitReceive(__bus, &Data, &ret, 1, 100);
	return ret;
}

FLASH_StatusTypeDef FLASHLIB::readID(void)
{
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	txrxSpi(0x9F);

	__ID = txrxSpi(__dummy) << 16 | txrxSpi(__dummy) << 8 | txrxSpi(__dummy);
	__MFID = (MFID_t)((__ID & 0xFF0000) >> 16);

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

	if(__ID != 0) return FLASH_OK;
	else return FLASH_ERROR;
}

FLASH_StatusTypeDef FLASHLIB::readUniqID(void)
{
	__UniqID = 0;

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	txrxSpi(0x4B);

	for (uint8_t i = 0; i < 4; i++) txrxSpi(__dummy);

	for(int i = 7; i >= 0; i--) {
		__UniqID <<= i*8;
		__UniqID |= (uint64_t)txrxSpi(__dummy);
	}

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

	if(__UniqID != 0) return FLASH_OK;
	else return FLASH_ERROR;
}

void FLASHLIB::writeEnable(void)
{
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	txrxSpi(0x06);
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

void FLASHLIB::writeDisable(void)
{
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	txrxSpi(0x04);
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

uint8_t FLASHLIB::readStatusRegister(uint8_t SelectStatusRegister)
{
	uint8_t status = 0;
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	if (SelectStatusRegister == 1) {
		txrxSpi(0x05);
		status = txrxSpi(__dummy);
		__StatReg1 = status;
	}

	else if (SelectStatusRegister == 2) {
		txrxSpi(0x35);
		status = txrxSpi(__dummy);
		__StatReg2 = status;
	}

	else {
		txrxSpi(0x15);
		status = txrxSpi(__dummy);
		__StatReg3 = status;
	}

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	return status;
}

void FLASHLIB::writeStatusRegister(uint8_t SelectStatusRegister, uint8_t Data)
{
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	if (SelectStatusRegister == 1) {
		txrxSpi(0x01);
		__StatReg1 = Data;
	}

	else if (SelectStatusRegister == 2) {
		txrxSpi(0x31);
		__StatReg2 = Data;
	}

	else {
		txrxSpi(0x11);
		__StatReg3 = Data;
	}

	txrxSpi(Data);
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

void FLASHLIB::waitForWriteEnd(void)
{
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	txrxSpi(0x05);

	do
		__StatReg1 = txrxSpi(__dummy);
	while ((__StatReg1 & 0x01) == 0x01);

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

FLASH_StatusTypeDef FLASHLIB::init(void)
{
	__Lock = 1;
	while (HAL_GetTick() < 100) HAL_Delay(1);
	CS_PORT -> BSRR = CS_PIN;

	readID();

	if(__MFID == Spansion) {
		switch (__ID & 0x000000FF) {
			case 0x19: // 	s25fl256s
				__CHIP_ID = S25FL256S;
				__BlkCnt = 512;
			break;

			case 0x18: // 	s25fl128s
				__CHIP_ID = S25FL128S;
				__BlkCnt = 256;
			break;

			case 0x17: // 	s25fl064l
				__CHIP_ID = S25FL064LS;
				__BlkCnt = 128;
			break;

			case 0x16: // 	s25fl064p
				__CHIP_ID = S25FL064P;
				__BlkCnt = 128;
			break;

			case 0x15: // 	s25fl032p
				__CHIP_ID = S25FL032P;
				__BlkCnt = 64;
			break;

			default:
				__Lock = 0;
			return FLASH_ERROR;
		}
	}

	else if(__MFID == Micron) {
		switch (__ID & 0x000000FF) {
			case 0x22: // 	mt25qx02
				__CHIP_ID = MT25QX02;
				__BlkCnt = 4096;
			break;

			case 0x21: // 	mt25qx01
				__CHIP_ID = MT25QX01;
				__BlkCnt = 2048;
			break;

			case 0x20: // 	mt25qx512
				__CHIP_ID = MT25QX512;
				__BlkCnt = 1024;
			break;

			case 0x19: // 	mt25qx256
				__CHIP_ID = MT25QX256;
				__BlkCnt = 512;
			break;

			case 0x18: // 	mt25qx128
				__CHIP_ID = MT25QX128;
				__BlkCnt = 256;
			break;

			case 0x17: // 	mt25qx64
				__CHIP_ID = MT25QX64;
				__BlkCnt = 128;
			break;

			case 0x16: // 	m25p32
				__CHIP_ID = M25P32;
				__BlkCnt = 64;
			break;

			case 0x15: // 	m54pe16
				__CHIP_ID = M45PE16;
				__BlkCnt = 32;
			break;

			case 0x14: // 	m54pe80
				__CHIP_ID = M45PE80;
				__BlkCnt = 16;
			break;

			case 0x12: // 	m54pe20
				__CHIP_ID = M45PE20;
				__BlkCnt = 4;
			break;

			default:
				__Lock = 0;
			return FLASH_ERROR;
		}
	}

	else if(__MFID == OnSemi) {
		switch (__ID & 0x000000FF) {
			case 0x14: // 	le25x81
				__CHIP_ID = LE25X81;
				__BlkCnt = 16;
			break;

			case 0x13: // 	le25x40
				__CHIP_ID = LE25X40;
				__BlkCnt = 8;
			break;

			case 0x12: // 	le25x20
				__CHIP_ID = LE25X20;
				__BlkCnt = 4;
			break;

			default:
				__Lock = 0;
			return FLASH_ERROR;
		}
	}

	else if(__MFID == ISSI) {
		switch (__ID & 0x000000FF) {
			case 0x13: // 	is25lq040
				__CHIP_ID = IS25LQ040;
				__BlkCnt = 8;
			break;

			case 0x12: // 	is25lq020
				__CHIP_ID = IS25LQ020;
				__BlkCnt = 4;
			break;

			case 0x11: // 	is25lq010
				__CHIP_ID = IS25LQ010;
				__BlkCnt = 2;
			break;

			case 0x10: // 	is25lq512
				__CHIP_ID = IS25LQ512;
				__BlkCnt = 1;
			break;

			case 0x09: // 	is25lq025
				__CHIP_ID = IS25LQ025;
				__BlkCnt = 0;
			break;

			default:
				__Lock = 0;
			return FLASH_ERROR;
		}
	}

	else if(__MFID == Adesto) {
		switch (((__ID & 0x0000FF00) >> 8)) {
			case 0x28: // 	at45db64
				__CHIP_ID = AT45DB64;
				__BlkCnt = 128;
			break;

			case 0x27: // 	at45db32
				__CHIP_ID = AT45DB32;
				__BlkCnt = 64;
			break;

			case 0x26: // 	at45db16
				__CHIP_ID = AT45DB16;
				__BlkCnt = 32;
			break;

			case 0x25: // 	at45db08
				__CHIP_ID = AT45DB08;
				__BlkCnt = 16;
			break;

			case 0x24: // 	at45db04
				__CHIP_ID = AT45DB04;
				__BlkCnt = 8;
			break;

			case 0x23: // 	at45db02
				__CHIP_ID = AT45DB02;
				__BlkCnt = 4;
			break;

			default:
				__Lock = 0;
			return FLASH_ERROR;
		}
	}

	else if(__MFID == GigaDevice) {
		switch (__ID & 0x000000FF) {
			case 0x13: // 	gd25lq40
				__CHIP_ID = GD25LQ40;
				__BlkCnt = 8;
			break;

			case 0x12: // 	gd25lq20
				__CHIP_ID = GD25LQ20;
				__BlkCnt = 4;
			break;

			case 0x11: // 	gd25lq10
				__CHIP_ID = GD25LQ10;
				__BlkCnt = 2;
			break;

			case 0x10: // 	gd25lq05
				__CHIP_ID = GD25LQ05;
				__BlkCnt = 1;
			break;

			default:
				__Lock = 0;
			return FLASH_ERROR;
		}
	}

	else if(__MFID == Winbond) {
		switch (__ID & 0x000000FF) {
			case 0x20: // 	w25q512
				__CHIP_ID = W25Q512;
				__BlkCnt = 1024;
			break;

			case 0x19: // 	w25q256
				__CHIP_ID = W25Q256;
				__BlkCnt = 512;
			break;

			case 0x18: // 	w25q128
				__CHIP_ID = W25Q128;
				__BlkCnt = 256;
			break;

			case 0x17: //	w25q64
				__CHIP_ID = W25Q64;
				__BlkCnt = 128;
			break;

			case 0x16: //	w25q32
				__CHIP_ID = W25Q32;
				__BlkCnt = 64;
			break;

			case 0x15: //	w25q16
				__CHIP_ID = W25Q16;
				__BlkCnt = 32;
			break;

			case 0x14: //	w25q80
				__CHIP_ID = W25Q80;
				__BlkCnt = 16;
			break;

			case 0x13: //	w25q40
				__CHIP_ID = W25Q40;
				__BlkCnt = 8;
			break;

			case 0x12: //	w25q20
				__CHIP_ID = W25Q20;
				__BlkCnt = 4;
			break;

			case 0x11: //	w25q10
				__CHIP_ID = W25Q10;
				__BlkCnt = 2;
			break;

			default:
				__Lock = 0;
			return FLASH_ERROR;
		}
	}

	__PagSiz = 256;
	__SectSiz = 0x1000;
	__SectCnt = __BlkCnt * 16;
	__PagCnt = (__SectCnt * __SectSiz) / __PagSiz;
	__BlkSiz = __SectSiz * 16;
	__SizInKB = (__SectCnt * __SectSiz) / 1024;

	if(((__ID & 0x00FF0000) >> 16) == Winbond) readUniqID();

	readStatusRegister(1);
	readStatusRegister(2);
	readStatusRegister(3);

	__Lock = 0;
	return FLASH_OK;
}

int FLASHLIB::getID(void) {
	return __ID;
}

int FLASHLIB::getChipID(void) {
	return __CHIP_ID;
}

uint64_t FLASHLIB::getUniqID(void) {
	return __UniqID;
}

int FLASHLIB::getSectorCount(void) {
	return __SectCnt;
}

int FLASHLIB::getSectorSize(void) {
	return __SectSiz;
}

int FLASHLIB::getPageCount(void) {
	return __PagCnt;
}

int FLASHLIB::getPageSize(void) {
	return __PagSiz;
}

int FLASHLIB::getBlockCount(void) {
	return __BlkCnt;
}

int FLASHLIB::getBlockSize(void) {
	return __BlkSiz;
}

int FLASHLIB::getFlashSize(void) {
	return __SizInKB;
}

FLASH_StatusTypeDef FLASHLIB::eraseChip(void)
{
	while (__Lock == 1)
		HAL_Delay(1);

	__Lock = 1;

	writeEnable();
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	txrxSpi(0xC7);

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	waitForWriteEnd();
	__Lock = 0;

	return isEmptyPage(0, 0, 16);
}

FLASH_StatusTypeDef FLASHLIB::eraseSector(uint32_t SectorAddr)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;

	waitForWriteEnd();
	SectorAddr = SectorAddr * __SectSiz;
	writeEnable();
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
			__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02)
	{
		txrxSpi(0x21);
		txrxSpi((SectorAddr & 0xFF000000) >> 24);
	}

	else {
		txrxSpi(0x20);
	}

	txrxSpi((SectorAddr & 0xFF0000) >> 16);
	txrxSpi((SectorAddr & 0xFF00) >> 8);
	txrxSpi(SectorAddr & 0xFF);

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	waitForWriteEnd();

	__Lock = 0;

	return isEmptySector(SectorAddr, 0, 16);
}

FLASH_StatusTypeDef FLASHLIB::eraseBlock(uint32_t BlockAddr)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;

	waitForWriteEnd();
	BlockAddr = BlockAddr * __SectSiz * 16;
	writeEnable();
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
			__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
		txrxSpi(0xDC);
		txrxSpi((BlockAddr & 0xFF000000) >> 24);
	}

	else {
		txrxSpi(0xD8);
	}

	txrxSpi((BlockAddr & 0xFF0000) >> 16);
	txrxSpi((BlockAddr & 0xFF00) >> 8);
	txrxSpi(BlockAddr & 0xFF);

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	waitForWriteEnd();

	__Lock = 0;

	return isEmptyBlock(BlockAddr, 0, 16);
}

uint32_t FLASHLIB::PageToSector(uint32_t PageAddress)
{
	return ((PageAddress * __PagSiz) / __SectSiz);
}

uint32_t FLASHLIB::PageToBlock(uint32_t PageAddress)
{
	return ((PageAddress * __PagSiz) / __BlkSiz);
}

uint32_t FLASHLIB::SectorToBlock(uint32_t SectorAddress)
{
	return ((SectorAddress * __SectSiz) / __BlkSiz);
}

uint32_t FLASHLIB::SectorToPage(uint32_t SectorAddress)
{
	return (SectorAddress * __SectSiz) / __PagSiz;
}

uint32_t FLASHLIB::BlockToPage(uint32_t BlockAddress)
{
	return (BlockAddress * __BlkSiz) / __PagSiz;
}

FLASH_StatusTypeDef FLASHLIB::isEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;

	if (((NumByteToCheck_up_to_PageSize + OffsetInByte) > __PagSiz) || (NumByteToCheck_up_to_PageSize == 0))
		NumByteToCheck_up_to_PageSize = __PagSiz - OffsetInByte;

	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;

	for (i = OffsetInByte; i < __PagSiz; i += sizeof(pBuffer)) {
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Page_Address * __PagSiz);
		if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
				__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
			txrxSpi(0x0C);
			txrxSpi((WorkAddress & 0xFF000000) >> 24);
		}

		else {
			txrxSpi(0x0B);
		}

		txrxSpi((WorkAddress & 0xFF0000) >> 16);
		txrxSpi((WorkAddress & 0xFF00) >> 8);
		txrxSpi(WorkAddress & 0xFF);
		txrxSpi(0);

		if(HAL_SPI_Receive(__bus, pBuffer, sizeof(pBuffer), 100) != HAL_OK) return FLASH_ERROR;
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

		for (uint8_t x = 0; x < sizeof(pBuffer); x++) {
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}

	if ((__PagSiz + OffsetInByte) % sizeof(pBuffer) != 0) {
		i -= sizeof(pBuffer);

		for (; i < __PagSiz; i++) {
			HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Page_Address * __PagSiz);
			txrxSpi(0x0B);

			if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
					__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
				txrxSpi(0x0C);
				txrxSpi((WorkAddress & 0xFF000000) >> 24);
			}

			else {
				txrxSpi(0x0B);
			}

			txrxSpi((WorkAddress & 0xFF0000) >> 16);
			txrxSpi((WorkAddress & 0xFF00) >> 8);
			txrxSpi(WorkAddress & 0xFF);
			txrxSpi(0);

			if(HAL_SPI_Receive(__bus, pBuffer, 1, 100) != HAL_OK) return FLASH_ERROR;
			HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}

	__Lock = 0;
	return FLASH_EMPTY;

	NOT_EMPTY:
	__Lock = 0;
	return FLASH_NOTEMPTY;
}
//###################################################################################################################
FLASH_StatusTypeDef FLASHLIB::isEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;

	if ((NumByteToCheck_up_to_SectorSize > __SectSiz) || (NumByteToCheck_up_to_SectorSize == 0))
		NumByteToCheck_up_to_SectorSize = __SectSiz;

	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;

	for (i = OffsetInByte; i < __SectSiz; i += sizeof(pBuffer)) {
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Sector_Address * __SectSiz);

		if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
				__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
			txrxSpi(0x0C);
			txrxSpi((WorkAddress & 0xFF000000) >> 24);
		}

		else {
			txrxSpi(0x0B);
		}

		txrxSpi((WorkAddress & 0xFF0000) >> 16);
		txrxSpi((WorkAddress & 0xFF00) >> 8);
		txrxSpi(WorkAddress & 0xFF);
		txrxSpi(0);

		if(HAL_SPI_Receive(__bus, pBuffer, sizeof(pBuffer), 100) != HAL_OK) return FLASH_ERROR;
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
		for (uint8_t x = 0; x < sizeof(pBuffer); x++) {
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}

	if ((__SectSiz + OffsetInByte) % sizeof(pBuffer) != 0) {
		i -= sizeof(pBuffer);

		for (; i < __SectSiz; i++) {
			HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Sector_Address * __SectSiz);
			if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
					__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
				txrxSpi(0x0C);
				txrxSpi((WorkAddress & 0xFF000000) >> 24);
			}

			else {
				txrxSpi(0x0B);
			}

			txrxSpi((WorkAddress & 0xFF0000) >> 16);
			txrxSpi((WorkAddress & 0xFF00) >> 8);
			txrxSpi(WorkAddress & 0xFF);
			txrxSpi(0);

			if(HAL_SPI_Receive(__bus, pBuffer, 1, 100) != HAL_OK) return FLASH_ERROR;
			HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}

	__Lock = 0;
	return FLASH_EMPTY;

	NOT_EMPTY:
	__Lock = 0;
	return FLASH_NOTEMPTY;
}
//###################################################################################################################
FLASH_StatusTypeDef FLASHLIB::isEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;
	if ((NumByteToCheck_up_to_BlockSize > __BlkSiz) || (NumByteToCheck_up_to_BlockSize == 0))
		NumByteToCheck_up_to_BlockSize = __BlkSiz;

	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;

	for (i = OffsetInByte; i < __BlkSiz; i += sizeof(pBuffer)) {
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Block_Address * __BlkSiz);

		if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
				__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
			txrxSpi(0x0C);
			txrxSpi((WorkAddress & 0xFF000000) >> 24);
		}

		else {
			txrxSpi(0x0B);
		}

		txrxSpi((WorkAddress & 0xFF0000) >> 16);
		txrxSpi((WorkAddress & 0xFF00) >> 8);
		txrxSpi(WorkAddress & 0xFF);
		txrxSpi(0);

		if(HAL_SPI_Receive(__bus, pBuffer, sizeof(pBuffer), 100) != HAL_OK) return FLASH_ERROR;
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

		for (uint8_t x = 0; x < sizeof(pBuffer); x++) {
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((__BlkSiz + OffsetInByte) % sizeof(pBuffer) != 0) {
		i -= sizeof(pBuffer);

		for (; i < __BlkSiz; i++) {
			HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Block_Address * __BlkSiz);

			if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
					__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
				txrxSpi(0x0C);
				txrxSpi((WorkAddress & 0xFF000000) >> 24);
			}

			else {
				txrxSpi(0x0B);
			}

			txrxSpi((WorkAddress & 0xFF0000) >> 16);
			txrxSpi((WorkAddress & 0xFF00) >> 8);
			txrxSpi(WorkAddress & 0xFF);
			txrxSpi(0);

			if(HAL_SPI_Receive(__bus, pBuffer, 1, 100) != HAL_OK) return FLASH_ERROR;
			HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}

	__Lock = 0;
	return FLASH_EMPTY;

	NOT_EMPTY:
	__Lock = 0;
	return FLASH_NOTEMPTY;
}

FLASH_StatusTypeDef FLASHLIB::writeByte(uint8_t pBuffer, uint32_t WriteAddr_inBytes)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;

	waitForWriteEnd();
	writeEnable();
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
			__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
		txrxSpi(0x12);
		txrxSpi((WriteAddr_inBytes & 0xFF000000) >> 24);
	}

	else {
		txrxSpi(0x02);
	}

	txrxSpi((WriteAddr_inBytes & 0xFF0000) >> 16);
	txrxSpi((WriteAddr_inBytes & 0xFF00) >> 8);
	txrxSpi(WriteAddr_inBytes & 0xFF);

	if(HAL_SPI_Transmit(__bus, &pBuffer, 1, 100) != HAL_OK) {
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
		waitForWriteEnd();

		__Lock = 0;

		return FLASH_ERROR;
	}

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	waitForWriteEnd();
	__Lock = 0;

	return FLASH_OK;
}

FLASH_StatusTypeDef FLASHLIB::writePage(
		uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;

	if (((NumByteToWrite_up_to_PageSize + OffsetInByte) > __PagSiz) || (NumByteToWrite_up_to_PageSize == 0))
		NumByteToWrite_up_to_PageSize = __PagSiz - OffsetInByte;

	if ((OffsetInByte + NumByteToWrite_up_to_PageSize) > __PagSiz)
		NumByteToWrite_up_to_PageSize = __PagSiz - OffsetInByte;

	waitForWriteEnd();
	writeEnable();
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	Page_Address = (Page_Address * __PagSiz) + OffsetInByte;

	if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
			__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
		txrxSpi(0x12);
		txrxSpi((Page_Address & 0xFF000000) >> 24);
	}

	else {
		txrxSpi(0x02);
	}

	txrxSpi((Page_Address & 0xFF0000) >> 16);
	txrxSpi((Page_Address & 0xFF00) >> 8);
	txrxSpi(Page_Address & 0xFF);

	if(HAL_SPI_Transmit(__bus, pBuffer, NumByteToWrite_up_to_PageSize, 1000) != HAL_OK) return FLASH_ERROR;

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	waitForWriteEnd();

	__Lock = 0;

	return FLASH_OK;
}

FLASH_StatusTypeDef FLASHLIB::writeSector(
		uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize)
{
	if ((NumByteToWrite_up_to_SectorSize > __SectSiz) || (NumByteToWrite_up_to_SectorSize == 0))
		NumByteToWrite_up_to_SectorSize = __SectSiz;

	if (OffsetInByte >= __SectSiz) return FLASH_ERROR;

	uint32_t StartPage;
	int32_t BytesToWrite;
	uint32_t LocalOffset;

	if ((OffsetInByte + NumByteToWrite_up_to_SectorSize) > __SectSiz)
		BytesToWrite = __SectSiz - OffsetInByte;

	else
		BytesToWrite = NumByteToWrite_up_to_SectorSize;

	StartPage = SectorToPage(Sector_Address) + (OffsetInByte / __PagSiz);
	LocalOffset = OffsetInByte % __PagSiz;

	do
	{
		if(writePage(pBuffer, StartPage, LocalOffset, BytesToWrite) != FLASH_OK) return FLASH_ERROR;

		StartPage++;
		BytesToWrite -= __PagSiz - LocalOffset;
		pBuffer += __PagSiz - LocalOffset;
		LocalOffset = 0;
	} while (BytesToWrite > 0);

	return FLASH_OK;
}

FLASH_StatusTypeDef FLASHLIB::writeBlock(
		uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize)
{
	if ((NumByteToWrite_up_to_BlockSize > __BlkSiz) || (NumByteToWrite_up_to_BlockSize == 0))
		NumByteToWrite_up_to_BlockSize = __BlkSiz;

	if (OffsetInByte >= __BlkSiz) return FLASH_ERROR;

	uint32_t StartPage;
	int32_t BytesToWrite;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToWrite_up_to_BlockSize) > __BlkSiz)
		BytesToWrite = __BlkSiz - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_BlockSize;
	StartPage = BlockToPage(Block_Address) + (OffsetInByte / __PagSiz);
	LocalOffset = OffsetInByte % __PagSiz;
	do
	{
		if(writePage(pBuffer, StartPage, LocalOffset, BytesToWrite != FLASH_OK)) return FLASH_ERROR;
		StartPage++;
		BytesToWrite -= __PagSiz - LocalOffset;
		pBuffer += __PagSiz - LocalOffset;
		LocalOffset = 0;
	} while (BytesToWrite > 0);

	return FLASH_OK;
}
//###################################################################################################################
FLASH_StatusTypeDef FLASHLIB::readByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
			__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
		txrxSpi(0x0C);
		txrxSpi((Bytes_Address & 0xFF000000) >> 24);
	}

	else {
		txrxSpi(0x0B);
	}

	txrxSpi((Bytes_Address & 0xFF0000) >> 16);
	txrxSpi((Bytes_Address & 0xFF00) >> 8);
	txrxSpi(Bytes_Address & 0xFF);
	txrxSpi(0);
	*pBuffer = txrxSpi(__dummy);
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

	__Lock = 0;

	return FLASH_OK;
}
//###################################################################################################################
FLASH_StatusTypeDef FLASHLIB::readBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
	while (__Lock == 1) HAL_Delay(1);
	__Lock = 1;

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
			__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
		txrxSpi(0x0C);
		txrxSpi((ReadAddr & 0xFF000000) >> 24);
	}

	else {
		txrxSpi(0x0B);
	}

	txrxSpi((ReadAddr & 0xFF0000) >> 16);
	txrxSpi((ReadAddr & 0xFF00) >> 8);
	txrxSpi(ReadAddr & 0xFF);
	txrxSpi(0);

	if(HAL_SPI_Receive(__bus, pBuffer, NumByteToRead, 2000) != HAL_OK) {
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
		__Lock = 0;

		return FLASH_ERROR;
	}

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	__Lock = 0;

	return FLASH_OK;
}
//###################################################################################################################
FLASH_StatusTypeDef FLASHLIB::readPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize)
{
	while (__Lock == 1)
		HAL_Delay(1);
	__Lock = 1;
	if ((NumByteToRead_up_to_PageSize > __PagSiz) || (NumByteToRead_up_to_PageSize == 0))
		NumByteToRead_up_to_PageSize = __PagSiz;
	if ((OffsetInByte + NumByteToRead_up_to_PageSize) > __PagSiz)
		NumByteToRead_up_to_PageSize = __PagSiz - OffsetInByte;

	Page_Address = Page_Address * __PagSiz + OffsetInByte;
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	if (	__CHIP_ID == W25Q256   || __CHIP_ID == W25Q512   || __CHIP_ID == S25FL256S || __CHIP_ID == MT25QX256 ||
			__CHIP_ID == MT25QX256 || __CHIP_ID == MT25QX512 || __CHIP_ID == MT25QX01  || __CHIP_ID == MT25QX02) {
		txrxSpi(0x0C);
		txrxSpi((Page_Address & 0xFF000000) >> 24);
	}

	else {
		txrxSpi(0x0B);
	}

	txrxSpi((Page_Address & 0xFF0000) >> 16);
	txrxSpi((Page_Address & 0xFF00) >> 8);
	txrxSpi(Page_Address & 0xFF);
	txrxSpi(0);
	if(HAL_SPI_Receive(__bus, pBuffer, NumByteToRead_up_to_PageSize, 1000) != HAL_OK) {
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
		__Lock = 0;

		return FLASH_ERROR;
	}

	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	__Lock = 0;

	return FLASH_OK;
}
//###################################################################################################################
FLASH_StatusTypeDef FLASHLIB::readSector(
		uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize)
{
	if ((NumByteToRead_up_to_SectorSize > __SectSiz) || (NumByteToRead_up_to_SectorSize == 0))
		NumByteToRead_up_to_SectorSize = __SectSiz;


	if (OffsetInByte >= __SectSiz) return FLASH_ERROR;

	uint32_t StartPage;
	int32_t BytesToRead;
	uint32_t LocalOffset;

	if ((OffsetInByte + NumByteToRead_up_to_SectorSize) > __SectSiz)
		BytesToRead = __SectSiz - OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_SectorSize;

	StartPage = SectorToPage(Sector_Address) + (OffsetInByte / __PagSiz);
	LocalOffset = OffsetInByte % __PagSiz;

	do
	{
		if(readPage(pBuffer, StartPage, LocalOffset, BytesToRead) != FLASH_OK) return FLASH_ERROR;
		StartPage++;
		BytesToRead -= __PagSiz - LocalOffset;
		pBuffer += __PagSiz - LocalOffset;
		LocalOffset = 0;
	} while (BytesToRead > 0);

	return FLASH_OK;
}
//###################################################################################################################
FLASH_StatusTypeDef FLASHLIB::readBlock(
		uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize)
{
	if ((NumByteToRead_up_to_BlockSize > __BlkSiz) || (NumByteToRead_up_to_BlockSize == 0))
		NumByteToRead_up_to_BlockSize = __BlkSiz;

	if (OffsetInByte >= __BlkSiz) return FLASH_ERROR;

	uint32_t StartPage;
	int32_t BytesToRead;
	uint32_t LocalOffset;

	if ((OffsetInByte + NumByteToRead_up_to_BlockSize) > __BlkSiz)
		BytesToRead = __BlkSiz - OffsetInByte;

	else
		BytesToRead = NumByteToRead_up_to_BlockSize;

	StartPage = BlockToPage(Block_Address) + (OffsetInByte / __PagSiz);
	LocalOffset = OffsetInByte % __PagSiz;

	do
	{
		if(readPage(pBuffer, StartPage, LocalOffset, BytesToRead) != FLASH_OK) return FLASH_ERROR;
		StartPage++;
		BytesToRead -= __PagSiz - LocalOffset;
		pBuffer += __PagSiz - LocalOffset;
		LocalOffset = 0;
	} while (BytesToRead > 0);

	return FLASH_OK;
}
