/*
 * heading.hpp
 *
 *  Created on: Oct 15, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_HEADING_HPP_
#define APP_INC_HEADING_HPP_

#include "accuracy.hpp"

class Heading : public Accuracy
{
private:
	float _heading = 0;
public:
	float heading() const { return _heading; }
	void heading(const float value) { _heading = value; }
};

#endif /* APP_INC_HEADING_HPP_ */
