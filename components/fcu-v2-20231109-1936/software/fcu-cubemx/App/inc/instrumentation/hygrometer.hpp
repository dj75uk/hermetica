/*
 * hygrometer.hpp
 *
 *  Created on: Oct 12, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_HUMIDITY_HPP_
#define APP_INC_HUMIDITY_HPP_

class Humidity : public Accuracy
{
private:
	float _humidity;
protected:
	void humidity(const float value)
	{
		_humidity = value;
	}
public:
	float humidity() const
	{
		return _humidity;
	}
};

#endif /* APP_INC_HUMIDITY_HPP_ */
