/*
 * iic.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_IIC_H_
#define APP_INC_IIC_H_

#define IIC_RX_QUEUE_MAXIMUM_SIZE		64
#define IIC_DEFAULT_TIMEOUT             0xFFFF

#include <cstdio>
#include <cstdint>
#include <cstring>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_i2c.h"
#include "debug.hpp"

extern const char* textFormatIicTrace;
extern const char* textIicTraceWrite;
extern const char* textIicTraceRead;
extern const char* textOk;
extern const char* textTimeout;
extern const char* textBusy;
extern const char* textError;


class Iic
{
private:
	char className[5];
	I2C_HandleTypeDef* _hal = NULL;
	uint8_t            _instanceId;

	void trace(const uint8_t device, const uint8_t reg, const uint8_t data, const uint8_t dir, const uint8_t result)
	{
		char s[128];
		char r[12];
		r[0] = 0;

		switch (result)
		{
		case HAL_OK:
			strcat(r, textOk);
			break;
		case HAL_BUSY:
			strcat(r, textBusy);
			break;
		case HAL_TIMEOUT:
			strcat(r, textTimeout);
			break;
		case HAL_ERROR:
		default:
			strcat(r, textError);
			break;
		}

		char bin[10];
		sprintf_binary8(bin, data);

		sprintf(s, textFormatIicTrace, device, reg, (dir == 0) ? textIicTraceWrite : textIicTraceRead, bin, data, data, r);
		debugWriteTrace(className, __LINE__, s);
	}

protected:
	uint8_t instanceId() const { return _instanceId; }
public:
	Iic(I2C_HandleTypeDef* hal, uint8_t instanceId)
	{
		assert(hal != NULL);
		_hal        = hal;
		_instanceId = instanceId;
		className[0] = 'i';
		className[1] = 'i';
		className[2] = 'c';
		switch (instanceId)
		{
		case 1:
			className[3] = '1';
			break;
		case 2:
			className[3] = '2';
			break;
		case 3:
			className[3] = '3';
			break;
		case 4:
			className[3] = '4';
			break;
		case 5:
			className[3] = '5';
			break;
		default:
			className[3] = '?';
			break;
		}
		className[4] = '\0';
	}
	I2C_HandleTypeDef* INSTANCE() const
	{
		return _hal;
	}
	bool readSingleByte(const uint8_t deviceAddress, const uint8_t registerAddress, uint8_t* buffer)
	{
		auto result = HAL_I2C_Mem_Read(_hal, deviceAddress, registerAddress, 1, buffer, 1, IIC_DEFAULT_TIMEOUT);
		trace(deviceAddress, registerAddress, buffer[0], 1, result);
		return result == HAL_OK;
	}
	bool writeSingleByte(const uint8_t deviceAddress, const uint8_t registerAddress, uint8_t* buffer)
	{
		auto result = HAL_I2C_Mem_Write(_hal, deviceAddress, registerAddress, 1, buffer, 1, IIC_DEFAULT_TIMEOUT);
		trace(deviceAddress, registerAddress, buffer[0], 0, result);
		return result == HAL_OK;
	}
	bool writeSingleByte(const uint8_t deviceAddress, const uint8_t registerAddress, const uint8_t value)
	{
		uint8_t buffer = value;
		auto result = HAL_I2C_Mem_Write(_hal, deviceAddress, registerAddress, 1, &buffer, 1, IIC_DEFAULT_TIMEOUT);
		trace(deviceAddress, registerAddress, buffer, 0, result);
		return result == HAL_OK;
	}
	bool bitSet(uint8_t deviceAddress, uint8_t registerAddress, uint8_t bitNumber)
	{
		// read the existing value...
		uint8_t existing;
		if (!readSingleByte(deviceAddress, registerAddress, &existing))
			return false;

		// determine if the bit needs setting...
		uint8_t bitmask = 1 << bitNumber;
		if (existing & bitmask)
			// bit is already set...
			return true;

		// modify and write the existing byte...
		uint8_t desired = existing | bitmask;
		if (!writeSingleByte(deviceAddress, registerAddress, &desired))
			return false;

		// read the byte again for verification...
		uint8_t verified;
		if (!readSingleByte(deviceAddress, registerAddress, &verified))
			return false;

		// return the result...
		return (verified & bitmask);
	}
	bool bitSetNoVerify(uint8_t deviceAddress, uint8_t registerAddress, uint8_t bitNumber)
	{
		// read the existing value...
		uint8_t existing;
		if (!readSingleByte(deviceAddress, registerAddress, &existing))
			return false;

		// determine if the bit needs setting...
		uint8_t bitmask = 1 << bitNumber;
		if (existing & bitmask)
			// bit is already set...
			return true;

		// modify and write the existing byte...
		uint8_t desired = existing | bitmask;
		if (!writeSingleByte(deviceAddress, registerAddress, &desired))
			return false;

		// return the result...
		return true;
	}
	bool bitClr(uint8_t deviceAddress, uint8_t registerAddress, uint8_t bitNumber)
	{
		// read the existing value...
		uint8_t existing;
		if (!readSingleByte(deviceAddress, registerAddress, &existing))
			return false;

		// determine if the bit needs clearing...
		uint8_t bitmask = 1 << bitNumber;
		if (!(existing & bitmask))
			// bit is already clear...
			return true;

		// modify and write the existing byte...
		uint8_t desired = existing & ~bitmask;
		if (!writeSingleByte(deviceAddress, registerAddress, &desired))
			return false;

		// read the byte again for verification...
		uint8_t verified;
		if (!readSingleByte(deviceAddress, registerAddress, &verified))
			return false;

		// return the result...
		return (!(verified & bitmask));
	}
	bool verifySingleByte(const uint8_t deviceAddress, const uint8_t registerAddress, const uint8_t expected)
	{
		uint8_t temp = 0;
		if (!readSingleByte(deviceAddress, registerAddress, &temp))
			return false;
		return (temp == expected);
	}
	bool writeSingleByteWithVerify(const uint8_t deviceAddress, const uint8_t registerAddress, uint8_t* buffer)
	{
		uint8_t temp = 0;

		// read the existing value...
		if (!readSingleByte(deviceAddress, registerAddress, &temp))
			return false;

		// check if the value needs writing at all...
		if (temp == buffer[0])
			return true;

		// write the value...
		temp = buffer[0];
		if (!writeSingleByte(deviceAddress, registerAddress, &temp))
			return false;

		// verify the value...
		return verifySingleByte(deviceAddress, registerAddress, buffer[0]);
	}
	bool writeSingleByteWithVerify(const uint8_t deviceAddress, const uint8_t registerAddress, const uint8_t value)
	{
		uint8_t temp = value;
		return writeSingleByteWithVerify(deviceAddress, registerAddress, &temp);
	}
	bool readByteRange(const uint8_t deviceAddress, const uint8_t firstRegister, const uint8_t lastRegister, uint8_t* buffer)
	{
		uint8_t temp;
		uint8_t bufferIndex = 0;
		for (uint8_t r = firstRegister; r <= lastRegister; r++)
		{
			if (!readSingleByte(deviceAddress, r, &temp))
				return false;
			buffer[bufferIndex++] = temp;
		}
		return true;
	}
};

#endif /* APP_INC_IIC_H_ */
