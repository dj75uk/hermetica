
/*
 * osd_waitingforarm.cpp
 *
 * Created: 01/11/2023 20:39:01
 *  Author: Daniel Johnson
 */ 
#include "osd.hpp"

void OSD::drawWaitingForArm(uint8_t* screen, FcuMode fcuMode, VtxMode submode)
{
	uint8_t secondsRemainingUntilArm = 5;
	uint8_t w = 40;
	uint8_t h = 40;
	uint8_t x0 = CX - (w + 15);
	uint8_t y0 = 40;
	uint8_t y1 = y0 + h;
	uint8_t x1 = x0 + w;
	uint8_t x2 = CX + 15;
	uint8_t x3 = x2 + w;
	
	switch (fcuMode)
	{
		case FcuMode::WaitingForArmStage1:
		case FcuMode::WaitingForArmStage2:
		case FcuMode::WaitingForArmStage3:
		sprintf_P(_sz, PSTR("WAITING FOR ARMING INPUT"));
		_g.print(screen, CX - (strlen(_sz) * 6) / 2, 10, _sz);
		sprintf_P(_sz, PSTR("MOVE STICKS AS SHOWN BELOW"));
		_g.print(screen, CX - (strlen(_sz) * 6) / 2, 20, _sz);
		_g.line(screen, x0, y0, x0, y1, 1);
		_g.line(screen, x0, y0, x1, y0, 1);
		_g.line(screen, x1, y1, x0, y1, 1);
		_g.line(screen, x1, y1, x1, y0, 1);
		_g.line(screen, x2, y0, x2, y1, 1);
		_g.line(screen, x2, y0, x3, y0, 1);
		_g.line(screen, x3, y1, x2, y1, 1);
		_g.line(screen, x3, y1, x3, y0, 1);
		_g.circle(screen, x0 + (w / 2), y0 + (h / 2), 4, 1);
		_g.circle(screen, x2 + (w / 2), y0 + (h / 2), 4, 1);
		break;
	}
	switch (fcuMode)
	{
		case FcuMode::WaitingForArmStage1:
			sprintf_P(_sz, PSTR("STEP 1 OF 3"));
			_g.print(screen, CX - (strlen(_sz) * 6) / 2, 90, _sz);
			_g.line(screen, x0 + (w / 2), y0 + (h / 2), x0 + 4, y0 + 4, 1);
			_g.line(screen, x2 + (w / 2), y0 + (h / 2), x3 - 4, y0 + 4, 1);
			_g.line(screen, x0 + 4, y0 + 4, x0 + 4, y0 + 8, 1);
			_g.line(screen, x0 + 4, y0 + 4, x0 + 9, y0 + 4, 1);
			_g.line(screen, x3 - 4, y0 + 4, x3 - 4, y0 + 8, 1);
			_g.line(screen, x3 - 4, y0 + 4, x3 - 9, y0 + 4, 1);
			break;
		case FcuMode::WaitingForArmStage2:
			sprintf_P(_sz, PSTR("STEP 2 OF 3"));
			_g.print(screen, CX - (strlen(_sz) * 6) / 2, 90, _sz);
			_g.line(screen, x0 + (w / 2), y0 + (h / 2), x1 - 4, y1 - 4, 1);
			_g.line(screen, x2 + (w / 2), y0 + (h / 2), x2 + 4, y1 - 4, 1);
			_g.line(screen, x1 - 4, y1 - 4, x1 - 9, y1 - 4, 1);
			_g.line(screen, x1 - 4, y1 - 4, x1 - 4, y1 - 8, 1);
			_g.line(screen, x2 + 4, y1 - 4, x2 + 9, y1 - 4, 1);
			_g.line(screen, x2 + 4, y1 - 4, x2 + 4, y1 - 8, 1);
			break;
		case FcuMode::WaitingForArmStage3:
			sprintf_P(_sz, PSTR("STEP 3 OF 3"));
			_g.print(screen, CX - (strlen(_sz) * 6) / 2, 90, _sz);
			_g.line(screen, x0 + (w / 2), y0 + (h / 2), x0 + (w / 2), y1 - 4, 1);
			_g.line(screen, x0 + (w / 2), y1 - 4, x0 + (w / 2) - 3, y1 - 7, 1);
			_g.line(screen, x0 + (w / 2), y1 - 4, x0 + (w / 2) + 3, y1 - 7, 1);
			break;
		case FcuMode::WaitingForArmStage4:
			switch (submode)
			{
				case VtxMode::WaitingForArmTimer1:
					secondsRemainingUntilArm = 0;
					break;
				case VtxMode::WaitingForArmTimer2:
					secondsRemainingUntilArm = 1;
					break;
				case VtxMode::WaitingForArmTimer3:
					secondsRemainingUntilArm = 2;
					break;
				case VtxMode::WaitingForArmTimer4:
					secondsRemainingUntilArm = 3;
					break;
				case VtxMode::WaitingForArmTimer5:
				default:
					secondsRemainingUntilArm = 4;
					break;
			}		
			sprintf_P(_sz, PSTR("ARMING IN %u..."), secondsRemainingUntilArm);
			_g.print(screen, CX - (strlen(_sz) * 6) / 2, CY, _sz);
			break;
	}
}
