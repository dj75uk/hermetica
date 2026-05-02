/*
 * airspeed.hpp
 *
 *  Created on: Oct 15, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_AIRSPEED_HPP_
#define APP_INC_AIRSPEED_HPP_

#include <cstdio>
#include <cstdint>
#include "accuracy.hpp"

class Airspeed : public Accuracy
{
private:
	float _airspeed = 0.0;
protected:
	void airspeed(const float value) { _airspeed = value; }
public:
	float airspeed() const { return _airspeed; }
};

#endif /* APP_INC_AIRSPEED_HPP_ */
