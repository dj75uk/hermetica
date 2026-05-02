/*
 * location.hpp
 *
 *  Created on: Oct 15, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_LOCATION_HPP_
#define APP_INC_LOCATION_HPP_

#include <cstdio>
#include <cstdint>
#include <cmath>
#include "accuracy.hpp"

class Location : public Accuracy
{
private:
    int32_t _latitude = 0;
    int32_t _longitude = 0;
public:
    int32_t latitude() const  { return _latitude;  }
    void latitude(const int32_t value)  { _latitude = value;  }
    int32_t longitude() const { return _longitude; }
    void longitude(const int32_t value) { _longitude = value; }

    static float bearing(const Location* from, const Location* to)
    {
    	const float x
    		= (float)(cos(from->latitude()) * sin(to->latitude()))
			- (float)(sin(from->latitude()) * cos(to->latitude()) * cos(to->longitude()-from->longitude()));

    	const float y
			= (float)sin(to->longitude() - from->longitude())
			* (float)cos(to->latitude());

    	const float t = atan2(y, x);
    	float result = (t * 180.0F / 3.14159F) + 360.0F;
    	while (result > 360.0F)
    		result -= 360.0f;
    	return result;
    }

    // based upon: https://en.wikipedia.org/wiki/Equirectangular_projection
    static float distance(const Location* from, const Location* to)
    {
    	const float r = 6371.0F;
    	const float x = (float)((to->longitude() - from->longitude()) * cos((from->latitude() + to->latitude()) / 2));
    	const float y = (float)(to->latitude()  - from->latitude());
    	return r * sqrt((x*x) + (y*y));
    }
};

#endif /* APP_INC_LOCATION_HPP_ */
