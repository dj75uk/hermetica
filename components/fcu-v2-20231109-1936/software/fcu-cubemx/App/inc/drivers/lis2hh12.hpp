/*
 * lis2hh12.hpp
 *
 *  Created on: Oct 10, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_DRIVERS_LIS2HH12_HPP_
#define APP_INC_DRIVERS_LIS2HH12_HPP_

#define LIS2HH12_CYCLE_DELAY			100
#define LIS2HH12_I2C_ADDRESS			0x3C
#define LIS2HH12_DEVICEID				0x41
#define LIS2HH12_I2C_REG_WHOAMI			0x0F

#include <cstdio>
#include <cstdint>
#include <cstring>
#include "component.hpp"
#include "iic.hpp"
#include "debug.hpp"
#include "acceleration.hpp"
#include "alarm.hpp"

extern const char* textClassNameLis2hh12;

class Lis2hh12 : public HasStatusAndSetupAndCycle, public Acceleration
{
private:
	Iic*    _iic;
	uint8_t _address;
	Alarm*  _alarm;
public:
	Lis2hh12(Iic* iic)
	{
		_iic     = iic;
		_address = LIS2HH12_I2C_ADDRESS;
		_alarm   = new Alarm(LIS2HH12_CYCLE_DELAY);
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		HAL_Delay(500);

		uint8_t buffer[16];
		memset(buffer, 0, sizeof(buffer));


		// check device identifier...
		if (!_iic->verifySingleByte(_address, LIS2HH12_I2C_REG_WHOAMI, LIS2HH12_DEVICEID))
			return failed(textClassNameLis2hh12, __LINE__);

		// DEVICE FOUND AND IDENTIFIED AS LIS2HH12




		return readyWithSet(textClassNameLis2hh12, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		if (!_alarm->alarm())
			return status();
		_alarm->reset();




		return readyNoSet(textClassNameLis2hh12, __LINE__);
	}
};

#endif /* APP_INC_DRIVERS_LIS2HH12_HPP_ */
