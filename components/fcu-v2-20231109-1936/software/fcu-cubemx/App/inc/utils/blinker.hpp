/*
 * blinker.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Daniel Johnson
 */

#ifndef BLINKER_H_
#define BLINKER_H_

#include "alarm.hpp"
#include "indicator.hpp"

class Blinker
{
private:
	Indicator* _indicator;
	bool       _state;
	Alarm*     _timer;
	uint16_t   _durationMsOn;
	uint16_t   _durationMsOff;
public:
	Blinker(Indicator* indicator, uint16_t durationMsOn, uint16_t durationMsOff)
	{
		_indicator     = indicator;
		_state         = false;
		_timer         = new Alarm(durationMsOff);
		_durationMsOn  = durationMsOn;
		_durationMsOff = durationMsOff;
	}
	void cycle()
	{
		if (_timer->alarm())
		{
			if (_state)
			{
				_timer->timeout(_durationMsOff);
				_indicator->off();
			}
			else
			{
				_timer->timeout(_durationMsOn);
				_indicator->on();
			}
			_state = !_state;
			_timer->reset();
		}
	}
};

#endif /* BLINKER_H_ */
