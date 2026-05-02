/*
 * crc.cpp
 *
 * Created: 01/05/2023 21:49:16
 *  Author: Daniel.Johnson
 */ 

#include "crc.h"
#include <stdint-gcc.h>

uint8_t crc8(const uint8_t *data, uint8_t len)
{

	uint8_t crc = 0;
	uint8_t currByte;

	for (int i = 0 ; i < len ; i++)
	{
		currByte = data[i];
		crc ^= currByte;
		for (int i = 0; i < 8; i++)
		{
			if ((crc & 0x80) != 0)
			{
				crc = (uint8_t)((crc << 1) ^ CRC_POLYGEN);
			}
			else
			{
				crc <<= 1;
			}
		}
	}
	return crc;
}

uint8_t crcQ(const uint8_t *data, const uint8_t len)
{
	uint8_t crc = 0;
	uint8_t i   = 0;
	uint8_t j   = len;
	//
	while (j--)
	{
		crc ^= *data++;
		for (i = 0; i < 8; i++)
		{
			if ((crc & 0x80) != 0)
			{
				crc = (uint8_t)((crc << 1) ^ 0x07);
			}
			else
			{
				crc <<= 1;
			}
		}
	}
	return crc;
}