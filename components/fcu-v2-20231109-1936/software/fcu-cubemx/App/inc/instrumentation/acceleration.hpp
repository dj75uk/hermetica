/*
 * accelerometer.hpp
 *
 *  Created on: Oct 11, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_ACCELEROMETER_HPP_
#define APP_INC_ACCELEROMETER_HPP_

#include "accuracy.hpp"
#include "vector3.hpp"

class Acceleration : private Vector3, public Accuracy
{
  protected:
    void accelerationX(const float value) { Vector3::X(value); }
    void accelerationY(const float value) { Vector3::Y(value); }
    void accelerationZ(const float value) { Vector3::Z(value); }
  public:
    float accelerationX() const { return Vector3::X(); }
    float accelerationY() const { return Vector3::Y(); }
    float accelerationZ() const { return Vector3::Z(); }
};

#endif /* APP_INC_ACCELEROMETER_HPP_ */
