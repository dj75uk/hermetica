/*
 * ilps22qs.hpp
 *
 *  Created on: Oct 10, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_ILPS22QS_HPP_
#define APP_INC_ILPS22QS_HPP_

#define ILPS22QS_I2C_ADDRESS		0xB8
#define ILPS22QS_DEVICEID			0xB4
#define ILPS22QS_I2C_REG_WHOAMI		0x0F

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
#include "component.hpp"
#include "iic.hpp"
#include "debug.hpp"
#include "alarm.hpp"
#include "temperature.hpp"
#include "movingaverage.hpp"
#include "pressure-barometric.hpp"

class Ilps22qs
	: public HasStatusAndSetupAndCycle
	, public Temperature
	, public BarometricPressure
{
private:
	const char* className = "ilps22qs";
	Iic*    _iic;
	Alarm*  _alarm;
	uint8_t _buffer[8];
public:
	Ilps22qs(Iic* iic)
	{
		_iic     = iic;
		_alarm   = new Alarm(450);
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		HAL_Delay(250);
		memset(_buffer, 0, sizeof(_buffer));

		// check device identifier...
		_buffer[0] = ILPS22QS_I2C_REG_WHOAMI;
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), ILPS22QS_I2C_ADDRESS, _buffer, 1, 10) != HAL_OK)
			return missing(className, __LINE__);
		if (HAL_I2C_Master_Receive(_iic->INSTANCE(), ILPS22QS_I2C_ADDRESS, _buffer, 1, 10) != HAL_OK)
			return missing(className, __LINE__);
		if (_buffer[0] != ILPS22QS_DEVICEID)
			return missing(className, __LINE__);

		_buffer[0] = 0x10; // CTRL_REG1
		_buffer[1] = 0b00011011;
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), ILPS22QS_I2C_ADDRESS, _buffer, 2, 10) != HAL_OK)
			return failed(className, __LINE__);

		_buffer[0] = 0x11; // CTRL_REG2
		_buffer[1] = 0b00010000; // default
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), ILPS22QS_I2C_ADDRESS, _buffer, 2, 10) != HAL_OK)
			return failed(className, __LINE__);

		_buffer[0] = 0x12; // CTRL_REG3
		_buffer[1] = 0b00000001; // default
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), ILPS22QS_I2C_ADDRESS, _buffer, 2, 10) != HAL_OK)
			return failed(className, __LINE__);

		BarometricPressure::accuracy(ACCURACY_HIGH);
		Temperature::accuracy(ACCURACY_HIGHEST);

		return readyWithSet(className, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		if (!_alarm->alarm())
			return status();
		_alarm->reset();

		// get a lovely clean buffer to use...
		memset(_buffer, 0, sizeof(_buffer));

		// get device readings...
		_buffer[0] = 0x28;
		if (HAL_I2C_Master_Transmit(_iic->INSTANCE(), ILPS22QS_I2C_ADDRESS, _buffer, 1, 10) != HAL_OK)
			return Status::Failed;
		if (HAL_I2C_Master_Receive(_iic->INSTANCE(), ILPS22QS_I2C_ADDRESS, _buffer, 5, 10) != HAL_OK)
			return Status::Failed;

		// pressure...
		int32_t p = (((uint32_t)_buffer[2] << 16) | ((uint32_t)_buffer[1] << 8) | (uint32_t)_buffer[0]);
		barometricPressure((float)p / (float)4096.0);

		// temperature...
		int16_t t = (int16_t)(((uint16_t)_buffer[4] << 8) | (uint16_t)_buffer[3]);
		temperature((float)t / (float)100.0);

		return status(Status::Ready);
	}
};

#endif /* APP_INC_ILPS22QS_HPP_ */
