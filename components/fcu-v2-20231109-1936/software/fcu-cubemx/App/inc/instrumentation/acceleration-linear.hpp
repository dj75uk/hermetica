/*
* linearaccelerometer.hpp
*
*  Created on: Oct 12, 2023
*      Author: Daniel Johnson
*/

#ifndef APP_INC_LINEARACCELEROMETER_HPP_
#define APP_INC_LINEARACCELEROMETER_HPP_

#include "accuracy.hpp"
#include "vector3.hpp"

class LinearAcceleration : private Vector3, public Accuracy
{
private:
	bool  _dirty    = false;
	float _fpiPitch = 0.0F;
	float _fpiYaw   = 0.0F;
	void calculateFpi()
	{
		if (_dirty)
		{
			_dirty = false;
			_fpiPitch = atan(linearAccelerationZ() / linearAccelerationX());
			_fpiYaw   = atan(linearAccelerationY() / linearAccelerationX());
		}
	}
protected:
	void linearAccelerationX(const float value) { if (X() != value) { _dirty = true; X(value); } }
	void linearAccelerationY(const float value) { if (Y() != value) { _dirty = true; Y(value); } }
	void linearAccelerationZ(const float value) { if (Z() != value) { _dirty = true; Z(value); } }
public:
	float linearAccelerationX() const { return X(); }
	float linearAccelerationY() const { return Y(); }
	float linearAccelerationZ() const { return Z(); }
	float fpiPitch() { calculateFpi(); return _fpiPitch; }
	float fpiYaw()   { calculateFpi(); return _fpiYaw;   }
};

#endif /* APP_INC_LINEARACCELEROMETER_HPP_ */
