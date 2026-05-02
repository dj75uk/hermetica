/*
 * fcu.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_FCU_HPP_
#define APP_INC_FCU_HPP_

#define CONFIG_FCU_HOLD_PITCH_TRIGGER						0.25F
#define CONFIG_FCU_HOLD_PITCH_TRIM							8.00F
#define CONFIG_FCU_HOLD_ROLL_TRIGGER						0.50F
#define CONFIG_FCU_HOLD_ROLL_TRIM							15.00F
#define CONFIG_FCU_HOLD_HEADING_TRIGGER						0.25F
#define CONFIG_FCU_HOLD_HEADING_YAWRATE						8.00F
#define CONFIG_FCU_HOLD_YAWRATE_TRIGGER     				0.25F
#define CONFIG_FCU_HOLD_YAWRATE_TRIM						0.25F
#define CONFIG_FCU_HOLD_ALTITUDE_TRIGGER					5.00F
#define CONFIG_FCU_HOLD_ALTITUDE_PITCH						3.00F

#define CONFIG_FCU_TAKEOFF_PITCH							15.0F
#define CONFIG_FCU_TAKEOFF_FLAPPERON						80.0F
#define CONFIG_FCU_LANDING_PITCH							1.50F
#define CONFIG_FCU_LANDING_FLAPPERON						130.0F
#define CONFIG_FCU_LOITER_ROLL								20.0F
#define CONFIG_FCU_LOITER_YAWRATE							20.0F

#define CONFIG_FCU_WAYPOINT_TARGET_DISTANCE					10.0F

#define CONFIG_FCU_RTB_TARGET_DISTANCE						10.0F
#define CONFIG_FCU_WAYPOINTS								32

#define CONFIG_FCU_FACTOR_THROTTLEDIFFERENTIAL				0.25F

#define CONFIG_FCU_MIX_PITCH_VS_YAW							0.50F
#define CONFIG_FCU_MIX_THROTTLEDIFFERENTIAL_VS_YAW			0.06F


#include <cstdio>
#include <cstdint>
#include <string>
#include "pwm.hpp"
#include "board.hpp"
#include "alarm.hpp"
#include "servo.hpp"
#include "pid.hpp"
#include "rcrx.hpp"
#include "vtxclient.hpp"
#include "location.hpp"
#include "input.hpp"
#include "inputprocessor.hpp"
#include "armable.hpp"
#include "fcumode.hpp"




class Waypoint : public Location
{
public:
	float    barometricAltitude  = 0;
	float    gpsAltitude = 0;
	float    airspeed  = 0;
	FcuMode  modeUponReaching = FcuMode::Waypoint;
	void reset()
	{
		latitude(0);
		longitude(0);
		barometricAltitude = 0;
		gpsAltitude        = 0;
		airspeed           = 0;
		modeUponReaching   = FcuMode::Loiter;
	}
};

class FcuInput
{
public:
	FcuMode fcuMode;

	float actualPitch     = 0;
	float actualRoll      = 0;
	float actualHeading   = 0;
	float actualPitchRate = 0;
	float actualRollRate  = 0;
	float actualYawRate   = 0;

	float inputPitch  = 0;
	InputState inputPitchState;
	float inputRoll   = 0;
	InputState inputRollState;
	float inputYaw    = 0;
	InputState inputYawState;
	float inputThrottle = 0;
	InputState inputThrottleState;
	float inputThrottleDifferential = 0;
	InputState inputThrottleDifferentialState;

	float gpsAltitude = 0;
	float barometricAltitude = 0;
	float rangedAltitude = 0;
	float airspeed = 0;

	Location location;
	Waypoint waypoint;
	Waypoint home;
};

class FcuOutput
{
private:
	bool 	_holdPitch 			= false;
	float 	_holdingPitch 		= 0;
	bool 	_holdRoll 			= false;
	float 	_holdingRoll 		= 0;
	bool 	_holdYawRate 		= false;
	float 	_holdingYawRate		= 0;
	bool 	_holdHeading		= false;
	float 	_holdingHeading		= 0;
	bool 	_holdAltitude		= false;
	float 	_holdingAltitude	= 0;
public:
	bool shouldTriggerArming = false;
	bool shouldTriggerDisarming = false;
	bool shouldIncrementWaypoint = false;
	bool shouldDecrementWaypoint = false;
	FcuMode fcuMode = FcuMode::Startup;
	VtxMode vtxMode;

	float throttleR = 0;
	float throttleL = 0;
	float throttleDifferential = 0;
	float pitchRate  = 0;
	float rollRate  = 0;
	float yawRate   = 0;
	float flapperon = 0;





	void holdPitch(float value)
	{
		_holdPitch    = true;
		_holdingPitch = value;
	}
	bool holdPitch() const
	{
		return _holdPitch;
	}
	float holdingPitch() const
	{
		return _holdingPitch;
	}
	void holdRoll(float value)
	{
		_holdRoll    = true;
		_holdingRoll = value;
	}
	bool holdRoll() const
	{
		return _holdRoll;
	}
	float holdingRoll() const
	{
		return _holdingRoll;
	}
	void holdYawRate(float value)
	{
		_holdYawRate    = true;
		_holdingYawRate = value;
	}
	bool holdYawRate() const
	{
		return _holdYawRate;
	}
	float holdingYawRate() const
	{
		return _holdingYawRate;
	}

	void holdHeading(float value)
	{
		_holdHeading    = true;
		_holdingHeading = value;
	}
	bool holdHeading() const
	{
		return _holdHeading;
	}
	float holdingHeading() const
	{
		return _holdingHeading;
	}

	void holdAltitude(float value)
	{
		_holdAltitude    = true;
		_holdingAltitude = value;
	}
	bool holdAltitude() const
	{
		return _holdAltitude;
	}
	float holdingAltitude() const
	{
		return _holdingAltitude;
	}

};

extern const char* textClassNameFcu;
static uint32_t _armingTimer = 0;

class Fcu
	: public HasFcuMode
{
private:
	Board*   	    	_board						= NULL;
	InputProcessor* 	_inputProcessor				= NULL;
	VtxClient*      	_vtxClient					= NULL;
	Alarm* 				_cycleTimer					= NULL;
	bool     			_setupComplete				= false;

	Pid*	 			_pidPitch					= NULL;
	Pid*     			_pidRoll					= NULL;
	Pid*     			_pidPitchRate				= NULL;
	Pid*     			_pidRollRate				= NULL;
	Pid*     			_pidYawRate					= NULL;

	Waypoint 			_loiter;
	Waypoint            _home;
	Waypoint 			_waypoints[CONFIG_FCU_WAYPOINTS];
	uint8_t  			_currentWaypoint			= 0;

	Input* 				_inputPitch    				= NULL;
	Input* 				_inputRoll     				= NULL;
	Input* 				_inputYaw      				= NULL;
	Input* 				_inputThrottle 				= NULL;
	Input* 				_inputThrottleDifferential 	= NULL;

protected:
	virtual Board* board() const
	{
		return _board;
	}
	virtual VtxClient* vtx() const
	{
		return _vtxClient;
	}
	virtual InputProcessor* inputProcessor() const
	{
		return _inputProcessor;
	}
public:
	Fcu(Board* board, InputProcessor* inputProcessor, VtxClient* vtxClient)
	{
		_board			= board;
		_inputProcessor	= inputProcessor;
		_vtxClient		= vtxClient;
		//
		_setupComplete	= false;
		_cycleTimer		= new Alarm(50);
		_pidPitch		= new Pid();
		_pidRoll		= new Pid();
		_pidPitchRate	= new Pid();
		_pidRollRate	= new Pid();
		_pidYawRate		= new Pid();
		//
		fcuMode(FcuMode::Startup);
	}
	virtual ~Fcu()
	{
	}
	Input* inputPitch()
	{
		if (_inputPitch == NULL)
			_inputPitch = _inputProcessor->input(InputUsage::Pitch);
		return _inputPitch;
	}
	Input* inputRoll()
	{
		if (_inputRoll == NULL)
			_inputRoll = _inputProcessor->input(InputUsage::Roll);
		return _inputRoll;
	}
	Input* inputYaw()
	{
		if (_inputYaw == NULL)
			_inputYaw = _inputProcessor->input(InputUsage::Yaw);
		return _inputYaw;
	}
	Input* inputThrottle()
	{
		if (_inputThrottle == NULL)
			_inputThrottle = _inputProcessor->input(InputUsage::Throttle);
		return _inputThrottle;
	}
	Input* inputThrottleDifferential()
	{
		if (_inputThrottleDifferential == NULL)
			_inputThrottleDifferential = _inputProcessor->input(InputUsage::ThrottleDifferential);
		return _inputThrottleDifferential;
	}
	Input* inputByUsage(InputUsage inputUsage)
	{
		return _inputProcessor->input(inputUsage);
	}
	static bool bearing(const float heading, const float target)
	{
		float result = (360.0F - heading) + target;
		while (result >=360.0F)
			result -= 360.0F;
		return result;
	}
	static bool bearingIsClockwise(const float heading, const float target)
	{
		return bearing(heading, target) <= 180.0F;
	}
	void setup()
	{
		// if we have already run setup, skip this...
		if (_setupComplete)
			return;
		_setupComplete = true;

		// setup the board (it calls setup on its components)...
		if (board()->setup() != Status::Ready)
		{
			debugWriteError(textClassNameFcu, __LINE__, "board setup failed");
			board()->fail(255);
		}

		// setup the video transmitter...
		vtx()->setup();
		vtx()->mode(FcuMode::Startup);

		// set default state...
		fcuMode(FcuMode::WaitingForArmStage1);
		board()->disarm();

		// indicate that all setup is now complete...
		board()->indicatorError()->off();
		board()->indicatorWarning()->off();
	}
	void cycle()
	{
		if (!_setupComplete)
			return;

		/// DEBUGGING!!!! ///////////////////////
		char s[200];
		char t[64];
		s[0] = 0;
		t[0] = 0;
		//fcuMode(FcuMode::Acrobatic);
		//arm();
		/////////////////////////////////////////

		// cycle the board...
		board()->cycle();

		// cycle the input-group...
		_inputProcessor->cycle();

		// cycle the vtx client...
		vtx()->cycle();

		// only cycle the pid control loops at fixed intervals...
		if (!_cycleTimer->alarm())
			return;
		_cycleTimer->reset();

		// limit the mode depending on our state...
		switch (fcuMode())
		{
		case FcuMode::Startup:
		case FcuMode::WaitingForArmStage1:
		case FcuMode::WaitingForArmStage2:
		case FcuMode::WaitingForArmStage3:
		case FcuMode::WaitingForArmStage4:
		case FcuMode::WaitingForArmStage5:
			// continuously update take-off location...
			_home.latitude(board()->gps()->latitude());
			_home.longitude(board()->gps()->longitude());
			_home.barometricAltitude = board()->barometricAltitude();
			_home.gpsAltitude = board()->gpsAltitude();
			break;
		case FcuMode::Shutdown:
			break;
		case FcuMode::Acrobatic:
		case FcuMode::Takeoff:
		case FcuMode::Stabilised:
		case FcuMode::RTB:
		case FcuMode::Waypoint:
		case FcuMode::WaypointPrevious:
		case FcuMode::WaypointNext:
		case FcuMode::Landing:
		case FcuMode::Loiter:
		default:
			fcuMode(desiredFcuMode());
			break;
		}

		// update the loiter point if we're not using it...
		if (fcuMode() != FcuMode::Loiter)
		{
			// set the loiter point...
			_loiter.latitude(board()->latitude());
			_loiter.longitude(board()->longitude());
			_loiter.barometricAltitude = board()->barometricAltitude();
		}


		/// DEBUGGING!!!! ///////////////////////
		//fcuMode(FcuMode::Stabilised);
		/////////////////////////////////////////


		// snapshot our current state...
		FcuInput current;
		//
		current.fcuMode 						= fcuMode();
		current.actualPitch     				= board()->pitch();
		current.actualRoll      				= board()->roll();
		current.actualHeading    				= board()->heading();
		current.actualPitchRate 				= board()->pitchRate();
		current.actualRollRate  				= board()->rollRate();
		current.actualYawRate   				= board()->yawRate();
		//
		current.inputThrottle             		= inputThrottle()->normalisedValue;
		current.inputThrottleState				= (InputState)inputThrottle()->inputState;
		current.inputThrottleDifferential 		= inputThrottleDifferential()->normalisedValue * CONFIG_FCU_FACTOR_THROTTLEDIFFERENTIAL;
		current.inputThrottleDifferentialState 	= (InputState)inputThrottleDifferential()->inputState;
		current.inputPitch                		= inputPitch()->normalisedValue;
		current.inputPitchState					= (InputState)inputPitch()->inputState;
		current.inputRoll					  	= inputRoll()->normalisedValue;
		current.inputRollState					= (InputState)inputRoll()->inputState;
		current.inputYaw               		   	= inputYaw()->normalisedValue;
		current.inputYawState					= (InputState)inputYaw()->inputState;
		//
		current.location.latitude(board()->latitude());
		current.location.longitude(board()->longitude());
		current.home                            = _home;
		current.waypoint						= _waypoints[_currentWaypoint];
		//
		current.barometricAltitude              = board()->barometricAltitude();
		current.gpsAltitude                     = board()->gpsAltitude();


		// default mix values..,
		float mixPitchVsYaw 					= CONFIG_FCU_MIX_PITCH_VS_YAW;
		float mixThrottleDifferentialVsYaw 		= CONFIG_FCU_MIX_THROTTLEDIFFERENTIAL_VS_YAW;

		// run the autopilot...
		FcuOutput output = handleFcuMode(current);

		// handle arming and disarming...
		if (output.shouldTriggerDisarming)
		{
			board()->disarm();
		}
		else if (output.shouldTriggerArming)
		{
			board()->arm();
		}

		if (output.shouldIncrementWaypoint)
		{
			if (_currentWaypoint < (CONFIG_FCU_WAYPOINTS - 1))
				_currentWaypoint++;
		}
		else if (output.shouldDecrementWaypoint)
		{
			if (_currentWaypoint > 0)
				_currentWaypoint--;
		}

		// switch the fcu mode...
		fcuMode(output.fcuMode);

		vtx()->mode(output.fcuMode);
		vtx()->submode(output.vtxMode);

		/************************************************************************************************************/
		/* SET DEFAULT OUTPUTS BASED UPON MANUAL INPUT                                                              */
		/************************************************************************************************************/

		output.throttleR = (current.inputThrottle + current.inputThrottleDifferential)
			- (current.inputYaw * mixThrottleDifferentialVsYaw);

		output.throttleL = (current.inputThrottle - current.inputThrottleDifferential)
			+ (current.inputYaw * mixThrottleDifferentialVsYaw);

		output.rollRate  = current.inputRoll;
		output.pitchRate = current.inputPitch;
		output.yawRate   = current.inputYaw;
		output.flapperon = 0;

		/************************************************************************************************************/
		/* OVERRIDE ALTITUDE IF DESIRED                                                                             */
		/************************************************************************************************************/

		if (output.holdAltitude())
		{
			float error = output.holdingAltitude() - current.barometricAltitude;
			if (absolute(error) > CONFIG_FCU_HOLD_ALTITUDE_TRIGGER)
			{
				if (error > 0)
					output.holdPitch(CONFIG_FCU_HOLD_ALTITUDE_PITCH);
				else
					output.holdPitch(-CONFIG_FCU_HOLD_ALTITUDE_PITCH);
			}
		}

		/************************************************************************************************************/
		/* OVERRIDE PITCH IF DESIRED                                                                                */
		/************************************************************************************************************/

		if (output.holdPitch())
		{
			float error = output.holdingPitch() - current.actualPitch;
			if (absolute(error) > CONFIG_FCU_HOLD_PITCH_TRIGGER)
			{
				output.pitchRate = -1.0F *
					bandpass<float>(
						rescale<float>(
							bandpass<float>(error, -CONFIG_FCU_HOLD_PITCH_TRIM, CONFIG_FCU_HOLD_PITCH_TRIM),
							-CONFIG_FCU_HOLD_PITCH_TRIM,
							CONFIG_FCU_HOLD_PITCH_TRIM,
							-500.0F,
							500.0F
						),
						-500.0F,
						500.0F
					);
			}
		}

		/************************************************************************************************************/
		/* OVERRIDE ROLL IF DESIRED                                                                                 */
		/************************************************************************************************************/

		if (output.holdRoll())
		{
			float error = output.holdingRoll() - current.actualRoll;

			if (absolute(error) > CONFIG_FCU_HOLD_ROLL_TRIGGER)
			{
				output.rollRate =
					bandpass<float>(
						rescale<float>(
							bandpass<float>(error, -CONFIG_FCU_HOLD_ROLL_TRIM, CONFIG_FCU_HOLD_ROLL_TRIM),
							-CONFIG_FCU_HOLD_ROLL_TRIM,
							CONFIG_FCU_HOLD_ROLL_TRIM,
							-500.0F,
							500.0F
						),
						-500.0F,
						500.0F
					);
			}
		}

		/************************************************************************************************************/
		/* OVERRIDE HEADING IF DESIRED (WAYPOINT, RTB)                                                              */
		/************************************************************************************************************/

		if (output.holdHeading())
		{
			float error = output.holdingHeading() - current.actualHeading;
			while (error > 360.0F)
				error -= 360.0F;
			while (error < -360.0F)
				error += 360.0F;

			if (absolute(error) > CONFIG_FCU_HOLD_HEADING_TRIGGER)
			{
				if (error > 0)
					output.holdYawRate(CONFIG_FCU_HOLD_HEADING_YAWRATE);
				else
					output.holdYawRate(-CONFIG_FCU_HOLD_HEADING_YAWRATE);
			}
		}

		/************************************************************************************************************/
		/* OVERRIDE YAW-RATE IF DESIRED (STABILISED, LOITER)                                                        */
		/************************************************************************************************************/

		if (output.holdYawRate())
		{
			float error = output.holdingYawRate() - current.actualYawRate;
			if (absolute(error) > CONFIG_FCU_HOLD_YAWRATE_TRIGGER)
			{
				output.yawRate =
					bandpass<float>(
						rescale<float>(
							bandpass<float>(error, -CONFIG_FCU_HOLD_YAWRATE_TRIM, CONFIG_FCU_HOLD_YAWRATE_TRIM),
							-CONFIG_FCU_HOLD_YAWRATE_TRIM,
							CONFIG_FCU_HOLD_YAWRATE_TRIM,
							-500.0F,
							500.0F
						),
						-500.0F,
						500.0F
					);
			}
		}

		/************************************************************************************************************/
		/* FINAL PWM HANDLING                                                                                       */
		/************************************************************************************************************/

		uint16_t pwmREsc     = PWM_MINIMUM;
		uint16_t pwmLEsc     = PWM_MINIMUM;
		uint16_t pwmRAileron = PWM_CENTRE;
		uint16_t pwmLAileron = PWM_CENTRE;
		uint16_t pwmRVTail   = PWM_CENTRE;
		uint16_t pwmLVTail   = PWM_CENTRE;
board()->arm();
		if (board()->armed())
		{
			// calculate final pwm values...
			//
			// remember to mix in:
			//		throttle -> differential thrust
			//		throttle -> manual yaw helper
			//		vtail    -> pitch vs yaw
			//		aileron  -> flapperons
			//
			pwmREsc     = unnormalise((int16_t)output.throttleR);

			pwmLEsc     = unnormalise((int16_t)output.throttleL);

			pwmRAileron = unnormalise((int16_t)output.rollRate + (int16_t)output.flapperon);

			pwmLAileron = unnormalise((int16_t)output.rollRate - (int16_t)output.flapperon);

			pwmRVTail   = unnormalise((int16_t)(output.pitchRate * mixPitchVsYaw)
				+ (int16_t)(output.yawRate * (1.0F - mixPitchVsYaw)));

			pwmLVTail   = unnormalise((int16_t)(output.pitchRate * mixPitchVsYaw)
				- (int16_t)(output.yawRate * (1.0F - mixPitchVsYaw)));
		}

		// write pwm outputs...
		board()->servoREsc()->pwm(pwmREsc);
		board()->servoLEsc()->pwm(pwmLEsc);
		board()->servoRAileron()->pwm(pwmRAileron);
		board()->servoLAileron()->pwm(pwmLAileron);
		board()->servoRVTail()->pwm(pwmRVTail);
		board()->servoLVTail()->pwm(pwmLVTail);

		board()->throttlePercentageR((uint8_t)(100.0F * ((float)pwmREsc - 1000.0F) / 1000.0F));
		board()->throttlePercentageL((uint8_t)(100.0F * ((float)pwmLEsc - 1000.0F) / 1000.0F));

		/************************************************************************************************************/
		/* DONE                                                                                                     */
		/************************************************************************************************************/

		sprintf(s, "m:%u  ibu:", (uint8_t)fcuMode());
		sprintf(t, " %+04d", inputThrottle()->normalisedValue);
		strcat(s, t);
		sprintf(t, " %+04d", inputYaw()->normalisedValue);
		strcat(s, t);
		sprintf(t, " %+04d", inputPitch()->normalisedValue);
		strcat(s, t);
		sprintf(t, " %+04d", inputRoll()->normalisedValue);
		strcat(s, t);
		sprintf(t, " %+04d", inputByUsage(InputUsage::Override)->normalisedValue);
		strcat(s, t);
		sprintf(t, " %+04d", inputByUsage(InputUsage::FcuMode)->normalisedValue);
		strcat(s, t);
		sprintf(t, " %+04d", inputByUsage(InputUsage::ThrottleDifferential)->normalisedValue);
		strcat(s, t);
		sprintf(t, " %+04d", inputByUsage(InputUsage::Aux01)->normalisedValue);
		strcat(s, t);
		sprintf(t, " %s", _inputProcessor->overriding() ? "or" : "--");
		strcat(s, t);

		sprintf(t, "  pwm: %04u", pwmREsc); strcat(s, t);
		sprintf(t, " %04u", pwmLEsc);      strcat(s, t);
		sprintf(t, " %04u", pwmRAileron);  strcat(s, t);
		sprintf(t, " %04u", pwmLAileron);  strcat(s, t);
		sprintf(t, " %04u", pwmRVTail);    strcat(s, t);
		sprintf(t, " %04u", pwmLVTail);    strcat(s, t);

		sprintf(t, " rc: %c", (board()->rcrxA() == NULL) ? '-' : (board()->rcrxA()->link() ? '1' : '0')); strcat(s, t);
		sprintf(t, "%c",      (board()->rcrxB() == NULL) ? '-' : (board()->rcrxB()->link() ? '1' : '0')); strcat(s, t);
		sprintf(t, "%c",      (board()->rcrxC() == NULL) ? '-' : (board()->rcrxC()->link() ? '1' : '0')); strcat(s, t);


		sprintf(s, "xa:%+.4f ya:%+.4f za:%+.4f x2:%+.4f y2:%+.4f z2:%+.4f lo:-%u hi:%u\n",
				board()->accelerationX(), board()->accelerationY(), board()->accelerationZ(),
				board()->linearAccelerationX(), board()->linearAccelerationY(), board()->linearAccelerationZ(),
				30,30
		);

		sprintf(s, "%.2f\n", board()->rangedAltitude());

		s[0]=0;


		if (s[0] != 0)
			//debugWriteInfo(textClassNameFcu, __LINE__, s);
			board()->mcu()->uart4()->write((uint8_t*)s, strlen(s));
	}
private:
	FcuMode desiredFcuMode()
	{
		FcuMode result;

		// determine mode based upon user input (if possible)...
		switch (_inputProcessor->state(InputUsage::FcuMode))
		{
		case InputState::Low:
			result = FcuMode::Takeoff;
			break;
		case InputState::Centre:
			result = FcuMode::Stabilised;
			break;
		case InputState::High:
			result = FcuMode::Acrobatic;
			break;
		case InputState::Unknown:
		default:
			result = FcuMode::Acrobatic;
			break;
		}
		return result;
	}
	FcuOutput handleFcuMode(const FcuInput current)
	{
		switch (current.fcuMode)
		{
		case FcuMode::Takeoff:
			return handleFcuModeTakeoff(current);
		case FcuMode::Landing:
			return handleFcuModeLanding(current);

		case FcuMode::Acrobatic:
			return handleFcuModeAcrobatic(current);
		case FcuMode::Stabilised:
			return handleFcuModeStabilised(current);

		case FcuMode::RTB:
			return handleFcuModeRTB(current);
		case FcuMode::Waypoint:
		case FcuMode::WaypointPrevious:
		case FcuMode::WaypointNext:
			return handleFcuModeWaypoint(current);
		case FcuMode::Loiter:
			return handleFcuModeLoiter(current);

		case FcuMode::Shutdown:
			return handleFcuModeShutdown();
		case FcuMode::WaitingForArmStage1:
		case FcuMode::WaitingForArmStage2:
		case FcuMode::WaitingForArmStage3:
		case FcuMode::WaitingForArmStage4:
		case FcuMode::WaitingForArmStage5:
			return handleFcuModeWaitingForArmStageN(current);
		case FcuMode::Startup:
		default:
			// something has gone severely wrong (check setup function)...
			return handleFcuModeStartup();
		}
	}
	static FcuOutput handleFcuModeStartup()
	{
		FcuOutput output;
		debugWriteError(textClassNameFcu, __LINE__, "failed");
		output.fcuMode = FcuMode::Shutdown;
		output.vtxMode = VtxMode::Decluttered;
		output.shouldTriggerDisarming = true;
		return output;
	}
	static FcuOutput handleFcuModeShutdown()
	{
		FcuOutput output;
		output.fcuMode = FcuMode::Shutdown;
		output.vtxMode = VtxMode::Decluttered;
		output.shouldTriggerDisarming = true;
		output.throttleR = 0;
		output.throttleL = 0;
		output.rollRate  = 0;
		output.pitchRate = 0;
		output.yawRate   = 0;
		return output;
	}
	static FcuOutput handleFcuModeTakeoff(const FcuInput current)
	{
		FcuOutput output;
		output.fcuMode = FcuMode::Takeoff;
		output.vtxMode = VtxMode::Decluttered;

		// takeoff flaps...
		output.flapperon = CONFIG_FCU_TAKEOFF_FLAPPERON;

		// if no manual input, attempt to hold pitch angle of zero...
		if (current.inputPitch == 0)
			output.holdPitch(CONFIG_FCU_TAKEOFF_PITCH);

		// if no manual input, attempt to hold roll angle of zero...
		if (current.inputRoll == 0)
			output.holdRoll(0);


		return output;
	}
	static FcuOutput handleFcuModeLanding(const FcuInput current)
	{
		FcuOutput output;
		output.fcuMode = FcuMode::Landing;
		output.vtxMode = VtxMode::Decluttered;

		// fly to take off point
		// then loiter with altitude reduction (slow spiral down)...
		// then align for landing
		// fly straight and level
		// reduce speed to stall+x%
		// reduce altitude
		// land
		// throttle to zero
		// mode to shutdown
		//_mode = FcuMode::Shutdown;
		//vtx()->mode(VtxMode::Landing);

		// takeoff flaps...
		output.flapperon = CONFIG_FCU_LANDING_FLAPPERON;

		// if no manual input, attempt to hold pitch angle of zero...
		if (current.inputPitch == 0)
			output.holdPitch(CONFIG_FCU_LANDING_PITCH);

		// if no manual input, attempt to hold roll angle of zero...
		if (current.inputRoll == 0)
			output.holdRoll(0);

		return output;
	}
	static FcuOutput handleFcuModeAcrobatic(const FcuInput current)
	{
		UNUSED(current);
		FcuOutput output;
		output.fcuMode    = FcuMode::Acrobatic;
		output.vtxMode    = VtxMode::Decluttered;
		return output;
	}
	static FcuOutput handleFcuModeStabilised(const FcuInput current)
	{
		FcuOutput output;
		output.fcuMode = FcuMode::Stabilised;
		output.vtxMode = VtxMode::Decluttered;

		// if no manual input, attempt to hold pitch angle of zero...
		if (current.inputPitch == 0)
			output.holdPitch(0);

		// if no manual input, attempt to hold roll angle of zero...
		if (current.inputRoll == 0)
			output.holdRoll(0);

		return output;
	}
	static FcuOutput handleFcuModeLoiter(const FcuInput current)
	{
		FcuOutput output;
		output.fcuMode = FcuMode::Loiter;
		output.vtxMode = VtxMode::Decluttered;

		// circle around current gps location...
		// * altitude hold
		// * throttle limited
		// * throttle differential automatic
		// * roll automatic
		// * yaw automatic

		// if no manual input, attempt to hold pitch angle of zero...
		if (current.inputPitch == 0)
			output.holdPitch(0);

		// if no manual input, attempt to hold a fixed roll angle...
		if (current.inputRoll == 0)
			output.holdRoll(CONFIG_FCU_LOITER_ROLL);

		// if no manual input, attempt to hold a fixed yaw rate...
		if (current.inputYaw == 0)
			output.holdYawRate(CONFIG_FCU_LOITER_YAWRATE);

		return output;
	}
	static FcuOutput handleFcuModeRTB(const FcuInput current)
	{
		FcuOutput output;
		output.fcuMode = FcuMode::RTB;
		output.vtxMode = VtxMode::Decluttered;

		// fly to take off point then change mode to loiter...
		if (Location::distance(&current.location, &current.home) > CONFIG_FCU_RTB_TARGET_DISTANCE)
		{
			output.holdHeading(Location::bearing(&current.location, &current.home));
			output.holdAltitude(current.barometricAltitude);
			output.holdRoll(0);
			output.holdPitch(0);
		}
		else
		{
			output.fcuMode = FcuMode::Loiter;
		}
		return output;
	}
	static FcuOutput handleFcuModeWaypoint(const FcuInput current)
	{
		FcuOutput output;
		output.fcuMode = FcuMode::Waypoint;
		output.vtxMode = VtxMode::Decluttered;

		// fly to current waypoint then increment waypoint...
		if (Location::distance(&current.location, &current.waypoint) > CONFIG_FCU_WAYPOINT_TARGET_DISTANCE)
		{
			output.holdHeading(Location::bearing(&current.location, &current.waypoint));
			output.holdAltitude(current.waypoint.barometricAltitude);
			output.holdRoll(0);
			output.holdPitch(0);
		}
		else
		{
			output.shouldIncrementWaypoint = true;
			output.fcuMode = current.waypoint.modeUponReaching;
		}
		return output;
	}
	static FcuOutput handleFcuModeWaitingForArmStageN(const FcuInput current)
	{
		FcuOutput output;
		output.fcuMode = current.fcuMode;
		//
		if (current.fcuMode == FcuMode::WaitingForArmStage1)
		{
			// display message on vtx...
			output.vtxMode = VtxMode::WaitingForArmTimer1;

			if ((current.inputPitchState == InputState::High) && (current.inputRollState == InputState::High))
			{
				if ((current.inputThrottleState == InputState::High) && (current.inputYawState == InputState::Low))
				{
					output.fcuMode = FcuMode::WaitingForArmStage2;
				}
			}
			return output;
		}
		if (current.fcuMode == FcuMode::WaitingForArmStage2)
		{
			// display message on vtx...
			output.vtxMode = VtxMode::WaitingForArmTimer2;

			if ((current.inputPitchState == InputState::Low) && (current.inputRollState == InputState::Low))
			{
				if ((current.inputThrottleState == InputState::Low) && (current.inputYawState == InputState::High))
				{
					output.fcuMode = FcuMode::WaitingForArmStage3;
				}
			}
			return output;
		}
		if (current.fcuMode == FcuMode::WaitingForArmStage3)
		{
			// display message on vtx...
			output.vtxMode = VtxMode::WaitingForArmTimer3;

			if ((current.inputPitchState == InputState::Centre) && (current.inputRollState == InputState::Centre))
			{
				if ((current.inputThrottleState == InputState::Low) && (current.inputYawState == InputState::Centre))
				{
					_armingTimer = HAL_GetTick();
					output.fcuMode = FcuMode::WaitingForArmStage4;
				}
			}
			return output;
		}
		if (current.fcuMode == FcuMode::WaitingForArmStage4)
		{
			// this is the arming countdown...

			uint32_t elapsed = HAL_GetTick() - _armingTimer;

			if (elapsed > 5000)
			{
				output.fcuMode = FcuMode::Takeoff;
				output.shouldTriggerArming = true;
			}
			else
			{
				if (elapsed > 4000)
				{
					output.vtxMode = VtxMode::WaitingForArmTimer1;
				}
				else
				{
					if (elapsed > 3000)
					{
						output.vtxMode = VtxMode::WaitingForArmTimer2;
					}
					else
					{
						if (elapsed > 2000)
						{
							output.vtxMode = VtxMode::WaitingForArmTimer3;
						}
						else
						{
							if (elapsed > 1000)
							{
								output.vtxMode = VtxMode::WaitingForArmTimer4;
							}
							else
							{
								output.vtxMode = VtxMode::WaitingForArmTimer5;
							}
						}
					}
				}
			}
		}
		return output;
	}
};

#endif /* APP_INC_FCU_HPP_ */
