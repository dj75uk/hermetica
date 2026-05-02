/*
 * alarm.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Daniel Johnson
 */
#ifndef ALARM_H_
#define ALARM_H_

#include <cstdint>
#include "stm32h7xx_hal.h"
#include "application.h"

class Alarm
{
private:
	uint16_t _timeout = 0;
	uint32_t _due     = 0;
public:
	Alarm(const uint16_t timeout)
	{
		_timeout = timeout;
		reset();
	}
	uint16_t timeout() const
	{
		return _timeout;
	}
	void timeout(const uint16_t value)
	{
		_timeout = value;
	}
	void reset()
	{
		_due = HAL_GetTick() + _timeout;
	}
	bool alarm() const
	{
		return HAL_GetTick() >= _due;
	}
};

#endif /* ALARM_H_ */
