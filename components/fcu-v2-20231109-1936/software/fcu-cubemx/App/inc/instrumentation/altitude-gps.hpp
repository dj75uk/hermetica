/*
 * GpsAltitude.hpp
 *
 *  Created on: Oct 15, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_GPSALTITUDE_HPP_
#define APP_INC_GPSALTITUDE_HPP_

#include <cstdio>
#include <cstdint>
#include "accuracy.hpp"

class GpsAltitude : public Accuracy
{
private:
	float _gpsAltitude = 0.0;
protected:
	void gpsAltitude(const float value) { _gpsAltitude = value; }
public:
	float gpsAltitude() const { return _gpsAltitude; }
};

#endif /* APP_INC_GPSALTITUDE_HPP_ */
