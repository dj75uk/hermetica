/*
 * func.hpp
 *
 *  Created on: Oct 27, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_FUNC_HPP_
#define APP_INC_FUNC_HPP_

#include <cstdint>
#include "pwm.hpp"

int16_t normalise(uint16_t in)														{ return ((int16_t)in) - PWM_CENTRE; }
uint16_t unnormalise(int16_t in)													{ return (uint16_t)(in + PWM_CENTRE); }

template <typename T> T multiply(const T & in, const T & n)							{ return in * n; }
template <typename T> T multiply(const T & in, const T & n, const bool condition)	{ return condition ? in * n : in; }
template <typename T> T absolute(const T & in)										{ return (in < 0) ? -1 * in : in; }
template <typename T> T lowpass(const T & in, const T & n)							{ return (in < n) ? in : n; }
template <typename T> T highpass(const T & in, const T & n)							{ return (in > n) ? in : n; }
template <typename T> T bandpass(const T & in, const T & n1, const T & n2)			{ return highpass<T>(lowpass<T>(in, n2), n1); }

template <typename T> T rescale(const T & in, const T & in_min, const T & in_max, const T & out_min, const T & out_max)
{
	return (int16_t)((float)((in - in_min) * (out_max - out_min)) / (float)(in_max - in_min) + out_min);
}

#endif /* APP_INC_FUNC_HPP_ */
