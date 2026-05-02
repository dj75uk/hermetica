/*
 * movingaverage.hpp
 *
 *  Created on: Oct 25, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_UTILS_MOVINGAVERAGE_HPP_
#define APP_INC_UTILS_MOVINGAVERAGE_HPP_

#include <cstdio>
#include <cstdint>
#include <string>
#include <cmath>

#define MOVINGAVERAGE_COUNT		15

class MovingAverage
{
private:
	float _values[MOVINGAVERAGE_COUNT];
	float _mean     = 0;
	float _variance = 0;
	float _stdev    = 1;
	float _total    = 0;
	int   _discards = 0;
	bool  _first = true;
public:
	void add(const float value)
	{
		float v = value;
		if (_first)
		{
			_first = false;
			for (uint8_t i = 0; i < MOVINGAVERAGE_COUNT; i++)
				_values[i] = v;
			_total = v * MOVINGAVERAGE_COUNT;
			_mean  = _total / (float)MOVINGAVERAGE_COUNT;
		}
		else
		{
			float absValue = (v > 0) ? v : -1.0F * v;
			if (absValue < 0.05F)
				v = 0;

			if (_discards < 5)
			{
				// how far is this value from the mean?
				if (abs(_mean - v) > (_stdev * 3.0F))
				{
					_discards++;
					return;
				}
				_discards = 0;
			}

			_total   -= _values[0];
			_total   += v;
			_mean     = _total / (float)MOVINGAVERAGE_COUNT;
			_variance = 0;
			for (uint8_t i = 0; i < (MOVINGAVERAGE_COUNT - 1); i++)
			{
				_values[i] = _values[i + 1];
				_variance += (_mean - _values[i]) * (_mean - _values[i]);
			}
			_values[(MOVINGAVERAGE_COUNT - 1)] = v;
			_variance += (_mean - v) * (_mean - v);
			_variance /= MOVINGAVERAGE_COUNT;
			_stdev = sqrt(_variance);
		}
	}
	float average() const
	{
		return _mean;
	}
};

#endif /* APP_INC_UTILS_MOVINGAVERAGE_HPP_ */
