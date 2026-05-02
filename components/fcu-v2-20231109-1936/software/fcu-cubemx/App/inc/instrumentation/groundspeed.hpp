/*
 * airspeed.hpp
 *
 *  Created on: Oct 15, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_GROUNDSPEED_HPP_
#define APP_INC_GROUNDSPEED_HPP_

#include <cstdio>
#include <cstdint>
#include "accuracy.hpp"

class Groundspeed : public Accuracy
{
private:
	float _groundspeed = 0.0;
protected:
	void groundspeed(const float value) { _groundspeed = value; }
public:
	float groundspeed() const { return _groundspeed; }
};

#endif /* APP_INC_GROUNDSPEED_HPP_ */
