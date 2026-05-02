/*
 * ypr.hpp
 *
 *  Created on: Oct 11, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_HEADING_PITCH_ROLL_HPP_
#define APP_INC_HEADING_PITCH_ROLL_HPP_

#include "accuracy.hpp"
#include "heading.hpp"
#include "vector3.hpp"

class HeadingPitchRoll : private Vector3, public Heading
{
  protected:
    void pitch(const float value) { Vector3::Y(value); }
    void roll(const float value) { Vector3::Z(value); }
  public:
    float pitch() const { return Vector3::Y(); }
    float roll() const { return Vector3::Z(); }
};

#endif /* APP_INC_HEADING_PITCH_ROLL_HPP_ */
