/*
 * mb85rc256v.h
 *
 *  Created on: Oct 9, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_MB85RC256V_H_
#define APP_INC_MB85RC256V_H_

#include <cstdio>
#include <cstdint>
#include <cstring>
#include "component.hpp"
#include "storage.hpp"
#include "iic.hpp"
#include "debug.hpp"

#define MB85RC256V_I2C_ADDRESS					0b10100000
#define MB85RC256V_STORAGE_ADDRESS_LOW			0x0000
#define MB85RC256V_STORAGE_ADDRESS_HIGH			0x7FFF
#define MB85RC256V_STORAGE_ADDRESS_TEST			MB85RC256V_STORAGE_ADDRESS_HIGH

class Mb85rc256v : public HasStatusAndSetup, public Storage
{
private:
	const char* className = "mb85rc256v";
	Iic*    _iic;
	uint8_t _address;
protected:
	bool addressValid(const uint32_t address) const
	{
		return (address <= MB85RC256V_STORAGE_ADDRESS_HIGH);
	}
public:
	Mb85rc256v(Iic* iic)
	{
		_iic     = iic;
		_address = MB85RC256V_I2C_ADDRESS;
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		HAL_Delay(500);

		if (!write(MB85RC256V_STORAGE_ADDRESS_TEST, 0b10101010))
			return failed(className, __LINE__);
		HAL_Delay(50);
		if (!verify(MB85RC256V_STORAGE_ADDRESS_TEST, 0b10101010))
			return failed(className, __LINE__);
		HAL_Delay(50);

		uint8_t writeBuffer[4];
		writeBuffer[0] = 1;
		writeBuffer[1] = 2;
		writeBuffer[2] = 3;
		writeBuffer[3] = 4;
		if (!write(MB85RC256V_STORAGE_ADDRESS_TEST - 16, writeBuffer, sizeof(writeBuffer)))
			return failed(className, __LINE__);
		HAL_Delay(50);

		uint8_t readBuffer[4];
		if (!read(MB85RC256V_STORAGE_ADDRESS_TEST - 16, readBuffer, sizeof(readBuffer)))
			return failed(className, __LINE__);
		HAL_Delay(50);

		for (uint8_t i = 0; i < sizeof(readBuffer); i++)
		{
			if (readBuffer[i] != writeBuffer[i])
				return failed(className, __LINE__);
		}

		return readyWithSet(className, __LINE__);
	}
	virtual bool read(const uint32_t address, const uint8_t* data) override
	{
		return read(address, data, 1);
	}
	virtual bool read(const uint32_t address, const uint8_t* data, const uint16_t length) override
	{
		if (!addressValid(address))
			return false;

		uint8_t transmit[2];
		transmit[0] = (0xFF00 & address) >> 8; // msb
		transmit[1] = (0x00FF & address);      // lsb

		// send the 16-bit address...
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), _address, transmit, 2, 10) != HAL_OK)
			return false;
		// read the 8-bit contents...
		if (HAL_I2C_Master_Receive(_iic->INSTANCE(), _address, (uint8_t*)data, length, 10) != HAL_OK)
			return false;

		return true;
	}
	virtual bool write(const uint32_t address, const uint8_t data) override
	{
		// check the address...
		if (!addressValid(address))
			return false;

		uint8_t transmit[3];
		transmit[0] = (0xFF00 & address) >> 8; // address msb
		transmit[1] = (0x00FF & address);      // address lsb
		transmit[2] = data;                    // data

		// send 16-bit memory address and the data...
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), _address, transmit, 3, 10) != HAL_OK)
			return false;

		return true;
	}
	virtual bool write(const uint32_t address, const uint8_t* data, const uint16_t length) override
	{
		// check the address...
		if (!addressValid(address))
			return false;
		if (length == 0)
			return false;
		if (length == 1)
			return write(address, data[0]);

		uint8_t transmit[2 + length];
		transmit[0] = (0xFF00 & address) >> 8; // msb
		transmit[1] = (0x00FF & address);      // lsb
		memcpy(transmit + 2, data, length);

		// send 16-bit memory address and the data...
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), _address, transmit, 2 + length, 10) != HAL_OK)
			return false;

		return true;
	}
	virtual bool verify(const uint32_t address, const uint8_t expected) override
	{
		uint8_t actual = ~expected;
		if (!read(address, &actual))
			return false;
		return (actual == expected);
	}
};

#endif /* APP_INC_MB85RC256V_H_ */
