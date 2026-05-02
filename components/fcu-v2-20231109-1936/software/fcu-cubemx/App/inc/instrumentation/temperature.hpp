/*
 * thermometer.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_TEMPERATURE_H_
#define APP_INC_TEMPERATURE_H_

#include "accuracy.hpp"

class Temperature : public Accuracy
{
private:
	float _temperature;
protected:
	void temperature(const float value)
	{
		_temperature = value;
	}
public:
	float temperature() const
	{
		return _temperature;
	}
};

#endif /* APP_INC_TEMPERATURE_H_ */
