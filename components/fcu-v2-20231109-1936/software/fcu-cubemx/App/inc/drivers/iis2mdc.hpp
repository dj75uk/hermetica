/*
 * iis2mdc.hpp
 *
 *  Created on: Oct 10, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_IIS2MDC_HPP_
#define APP_INC_IIS2MDC_HPP_

#define IIS2MDC_I2C_ADDRESS			0b00111100
#define IIS2MDC_DEVICEID			0b01000000
#define IIS2MDC_I2C_REG_WHOAMI		0x4F

#include <cstdio>
#include <cstdint>
#include <cstring>
#include "component.hpp"
#include "iic.hpp"
#include "debug.hpp"
#include "alarm.hpp"
#include "temperature.hpp"
#include "magnetometer.hpp"

extern const char* textClassNameIis2mdc;

class Iis2mdc
	: public HasStatusAndSetupAndCycle
	, public Temperature
	, public Magnetometer
{
private:

	Iic*    _iic;
	Alarm*  _alarm;
public:
	Iis2mdc(Iic* iic)
	{
		_iic    = iic;
		_alarm  = new Alarm(100);
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		HAL_Delay(50);

		uint8_t buffer[8];
		memset(buffer, 0, sizeof(buffer));

		// check device identifier...
		buffer[0] = 0x4F;
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), IIS2MDC_I2C_ADDRESS, buffer, 1, 10) != HAL_OK)
			return missing(textClassNameIis2mdc, __LINE__);
		if (HAL_I2C_Master_Receive(_iic->INSTANCE(), IIS2MDC_I2C_ADDRESS, buffer, 1, 10) != HAL_OK)
			return missing(textClassNameIis2mdc, __LINE__);
		//if (buffer[0] != IIS2MDC_DEVICEID)
		//	return status(Status::Missing);

		buffer[0] = 0x60; // CTRL_REG1
		buffer[1] = 0b10001100;
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), IIS2MDC_I2C_ADDRESS, buffer, 2, 10) != HAL_OK)
			return failed(textClassNameIis2mdc, __LINE__);

		buffer[0] = 0x61; // CTRL_REG2
		buffer[1] = 0b00000000;
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), IIS2MDC_I2C_ADDRESS, buffer, 2, 10) != HAL_OK)
			return failed(textClassNameIis2mdc, __LINE__);

		buffer[0] = 0x62; // CTRL_REG3
		buffer[1] = 0b00010000;
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), IIS2MDC_I2C_ADDRESS, buffer, 2, 10) != HAL_OK)
			return failed(textClassNameIis2mdc, __LINE__);

		buffer[0] = 0x63; // INT_CTRL_REG
		buffer[1] = 0b00000000;
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), IIS2MDC_I2C_ADDRESS, buffer, 2, 10) != HAL_OK)
			return failed(textClassNameIis2mdc, __LINE__);

		return readyWithSet(textClassNameIis2mdc, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		if (!_alarm->alarm())
			return status();
		_alarm->reset();

		// get a lovely clean buffer to use...
		uint8_t buffer[8];
		memset(buffer, 0, sizeof(buffer));

		// get device readings...
		for (uint8_t i = 0; i < 8; i++)
		{
			uint8_t r = 0x68 + i;
			uint8_t v = 0;
			if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), IIS2MDC_I2C_ADDRESS, &r , 1, 10) != HAL_OK)
				return failedNoSet(textClassNameIis2mdc, __LINE__);
			if (HAL_I2C_Master_Receive(_iic->INSTANCE(), IIS2MDC_I2C_ADDRESS, &v, 1, 10) != HAL_OK)
				return failedNoSet(textClassNameIis2mdc, __LINE__);

			buffer[i] = v;
		}

		// magnetometer...


		// temperature...
		int16_t t = (((uint16_t)buffer[7] << 8) | (uint16_t)buffer[6]);
		temperature((float)t / (float)8.0);



		return readyNoSet(textClassNameIis2mdc, __LINE__);
	}
};

#endif /* APP_INC_IIS2MDC_HPP_ */
