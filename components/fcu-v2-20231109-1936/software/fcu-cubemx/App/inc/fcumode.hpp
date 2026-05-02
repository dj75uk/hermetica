/*
 * fcumode.hpp
 *
 *  Created on: Oct 31, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_FCUMODE_HPP_
#define APP_INC_FCUMODE_HPP_

enum class FcuMode : uint8_t
{
	Startup					= 0,
	WaitingForArmStage1		= 1,
	WaitingForArmStage2		= 2,
	WaitingForArmStage3		= 3,
	WaitingForArmStage4		= 4,
	WaitingForArmStage5		= 5,
	Acrobatic				= 6,
	Takeoff					= 7,
	Stabilised				= 8,
	RTB						= 9,
	Waypoint				= 10,
	WaypointPrevious		= 11,
	WaypointNext			= 12,
	Landing					= 13,
	Loiter					= 14,
	Shutdown				= 15
};

class HasFcuMode
{
private:
	FcuMode _fcuMode;
protected:
	void fcuMode(const FcuMode value)
	{
		_fcuMode = value;
	}
public:
	FcuMode fcuMode() const
	{
		return _fcuMode;
	}
};

#endif /* APP_INC_FCUMODE_HPP_ */
