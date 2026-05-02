/*
 * magnetometer.hpp
 *
 *  Created on: Oct 11, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_MAGNETOMETER_HPP_
#define APP_INC_MAGNETOMETER_HPP_

#include "accuracy.hpp"
#include "vector3.hpp"

class Magnetometer : private Vector3, public Accuracy
{
  protected:
    void fieldX(const float value) { Vector3::X(value); }
    void fieldY(const float value) { Vector3::Y(value); }
    void fieldZ(const float value) { Vector3::Z(value); }
  public:
    float fieldX() const { return Vector3::X(); }
    float fieldY() const { return Vector3::Y(); }
    float fieldZ() const { return Vector3::Z(); }
};

#endif /* APP_INC_MAGNETOMETER_HPP_ */
