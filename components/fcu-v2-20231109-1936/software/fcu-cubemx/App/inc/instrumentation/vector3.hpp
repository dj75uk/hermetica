/*
 * vector3.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_VECTOR3_H_
#define APP_INC_VECTOR3_H_

class Vector3
{
  private:
    float _xyz[3] = { 0.0, 0.0, 0.0 };
  protected:
    void X(const float value) { _xyz[0] = value; }
    void Y(const float value) { _xyz[1] = value; }
    void Z(const float value) { _xyz[2] = value; }
  public:
    float X() const { return _xyz[0]; }
    float Y() const { return _xyz[1]; }
    float Z() const { return _xyz[2]; }
};

#endif /* APP_INC_VECTOR3_H_ */
