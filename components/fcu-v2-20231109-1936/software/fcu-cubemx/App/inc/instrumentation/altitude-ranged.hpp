/*
 * rangedaltitude.hpp
 *
 *  Created on: Oct 20, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_RANGEDALTITUDE_HPP_
#define APP_INC_RANGEDALTITUDE_HPP_

#include <cstdio>
#include <cstdint>
#include "accuracy.hpp"

class RangedAltitude : public Accuracy
{
private:
	float _rangedAltitude = 0.0;
protected:
	void rangedAltitude(const float value) { _rangedAltitude = value; }
public:
	float rangedAltitude() const { return _rangedAltitude; }
};

#endif /* APP_INC_RANGEDALTITUDE_HPP_ */
