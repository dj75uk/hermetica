/*
 * armable.hpp
 *
 *  Created on: Oct 29, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_ARMABLE_HPP_
#define APP_INC_ARMABLE_HPP_

class Armable
{
private:
	bool _armed = false;
public:
	void arm()
	{
		_armed = true;
	}
	bool armed() const
	{
		return _armed;
	}
	void disarm()
	{
		_armed = false;
	}
};

#endif /* APP_INC_ARMABLE_HPP_ */
