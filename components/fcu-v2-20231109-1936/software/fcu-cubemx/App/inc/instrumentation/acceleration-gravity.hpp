/*
 * gravitometer.hpp
 *
 *  Created on: Oct 12, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_GRAVITOMETER_HPP_
#define APP_INC_GRAVITOMETER_HPP_

#include "accuracy.hpp"
#include "vector3.hpp"

class GravityAcceleration : private Vector3, public Accuracy
{
  protected:
    void gravityX(const float value) { Vector3::X(value); }
    void gravityY(const float value) { Vector3::Y(value); }
    void gravityZ(const float value) { Vector3::Z(value); }
  public:
    float gravityX() const { return Vector3::X(); }
    float gravityY() const { return Vector3::Y(); }
    float gravityZ() const { return Vector3::Z(); }
};

#endif /* APP_INC_GRAVITOMETER_HPP_ */
