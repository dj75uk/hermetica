/*
 * gyroscope.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_GYROSCOPE_H_
#define APP_INC_GYROSCOPE_H_

#include "accuracy.hpp"
#include "vector3.hpp"

class Gyroscope : private Vector3, public Accuracy
{
  protected:
    void pitchRate(const float value) { Vector3::X(value); }
    void rollRate(const float value) { Vector3::Y(value); }
    void yawRate(const float value) { Vector3::Z(value); }
  public:
    float pitchRate() const { return Vector3::X(); }
    float rollRate() const { return Vector3::Y(); }
    float yawRate() const { return Vector3::Z(); }
};

#endif /* APP_INC_GYROSCOPE_H_ */
