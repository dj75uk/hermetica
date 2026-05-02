/*
 * macros.h
 *
 *  Created on: Oct 4, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_MACROS_H_
#define APP_INC_MACROS_H_

#define LIMIT(value, minimum, maximum)               (((value) < (minimum)) ? (minimum) : (((value)>(maximum)) ? (maximum) : (value)))
#define map2(t, x, in_min, in_max, out_min, out_max)  ((t)((((t)x - (t)in_min) * ((t)out_max - (t)out_min)) / (((t)in_max - (t)in_min)) + (t)out_min))

#define SCALE(t, x, in_min, in_max, out_min, out_max)  ((t)((((t)x - (t)in_min) * ((t)out_max - (t)out_min)) / (((t)in_max - (t)in_min)) + (t)out_min))

// ardunio clones...
#define millis() (HAL_GetTick())






#endif /* APP_INC_MACROS_H_ */
