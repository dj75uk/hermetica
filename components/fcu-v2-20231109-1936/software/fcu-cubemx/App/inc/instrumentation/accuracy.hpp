/*
 * accuracy.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_ACCURACY_H_
#define APP_INC_ACCURACY_H_

#define ACCURACY_UNKNOWN      0
#define ACCURACY_LOWEST       1
#define ACCURACY_LOW          2
#define ACCURACY_MEDIUM       3
#define ACCURACY_HIGH         4
#define ACCURACY_HIGHEST      5

#include <cstdint>
#include "macros.hpp"

class Accuracy
{
  private:
    uint8_t _accuracy = ACCURACY_UNKNOWN;
  protected:
    virtual void accuracy(const uint8_t value)
    {
    	_accuracy = LIMIT(value, ACCURACY_UNKNOWN, ACCURACY_HIGHEST);
    }
  public:
    virtual uint8_t accuracy() const
    {
    	return _accuracy;
    }
};

#endif /* APP_INC_ACCURACY_H_ */
