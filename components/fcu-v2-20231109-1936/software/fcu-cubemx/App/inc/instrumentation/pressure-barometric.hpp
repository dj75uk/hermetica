/*
 * barometer.hpp
 *
 *  Created on: Oct 10, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_BAROMETER_HPP_
#define APP_INC_BAROMETER_HPP_

#include "movingaverage.hpp"

class BarometricPressure : public Accuracy
{
private:
	MovingAverage _barometricPressure;
	float _barometricVsi = 0.0;
	float _barometricAltitude        = 0.0;
	float _barometricInitialPressure = 0.0;
	uint32_t _lastTimestamp = 0;
	float    _lastAltitude = 0;

protected:
	void barometricPressure(const float value)
	{
		uint32_t timestamp = HAL_GetTick();

		if ((value > 800.0F) && (value < 1200.0F))
		{
			_barometricPressure.add(value);
			_barometricAltitude = pressureToAltitude(_barometricPressure.average());

			if (_lastTimestamp > 0)
			{
				_barometricVsi = (_barometricAltitude - _lastAltitude) * (1000.0F / (float)(timestamp - _lastTimestamp));
				if (abs(_barometricVsi < 0.001F))
					_barometricVsi = 0.0F;
			}
			_lastTimestamp = timestamp;
			_lastAltitude  = _barometricAltitude;
		}
	}
	static float pressureToAltitude(const float pressure)
	{
		return 44307.694F * (1.0F - pow(pressure / 1013.25F, 0.190284F));
	}
public:
	float barometricPressure() const
	{
		return _barometricPressure.average();
	}
	float barometricAltitude() const
	{
		return _barometricAltitude;
	}
	float barometricVsi() const
	{
		return _barometricVsi;
	}
	void barometricInitialPressure(const float value)
	{
		_barometricInitialPressure = value;
	}
};

#endif /* APP_INC_BAROMETER_HPP_ */
