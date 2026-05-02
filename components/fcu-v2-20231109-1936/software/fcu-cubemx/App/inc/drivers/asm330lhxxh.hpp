/*
 * asm330lhxxh.hpp
 *
 *  Created on: Oct 10, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_ASM330LHXXH_HPP_
#define APP_INC_ASM330LHXXH_HPP_

#define ASM330LHXXH_DEVICEID				0b01101011
#define ASM330LHXXH_I2C_ADDRESS				0xD4
#define ASM330LHXXH_I2C_REG_WHOAMI			0x0F

#include <cstdio>
#include <cstdint>
#include <string>
#include "component.hpp"
#include "iic.hpp"
#include "debug.hpp"
#include "alarm.hpp"

extern const char* textClassNameAsm330lhxxh;

class Asm330lhxxh
	: public HasStatusAndSetupAndCycle
{
private:
	Iic*   _iic;
	Alarm* _alarm;
public:
	Asm330lhxxh(Iic* iic)
	{
		_iic    = iic;
		_alarm  = new Alarm(50);
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();


		HAL_Delay(500);


		if (!_iic->verifySingleByte(ASM330LHXXH_I2C_ADDRESS, 0x0F, 0b01101011))
			return missing(textClassNameAsm330lhxxh, __LINE__);

		// restart the device...
		if (!_iic->bitSetNoVerify(ASM330LHXXH_I2C_ADDRESS, 0x12, 0)) // SW_RESET
			return failed(textClassNameAsm330lhxxh, __LINE__);
		HAL_Delay(10);
		uint8_t buffer[8];
		uint32_t exitDue = HAL_GetTick() + 5000;
		while (1)
		{
			if (!_iic->readSingleByte(ASM330LHXXH_I2C_ADDRESS, 0x12, buffer))
				return failed(textClassNameAsm330lhxxh, __LINE__);
			uint8_t data = buffer[0] & 0b00000001;
			if (data == 0)
				break;
			HAL_Delay(10);
			if (HAL_GetTick() > exitDue)
				return failed(textClassNameAsm330lhxxh, __LINE__);
		}
		HAL_Delay(100);

		// set register FUNC_CFG_ACCESS...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x01, 0);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x01, 1);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x01, 2);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x01, 3);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x01, 4);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x01, 5);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x01, 6); // SHUB_REG_ACCESS
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x01, 7); // FUNC_CFG_ACCESS

		// set register PIN_CTRL...
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x02, 0);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x02, 1);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x02, 2);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x02, 3);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x02, 4);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x02, 5);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x02, 6); // SDO_PU_EN
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x02, 7);

		// set register FIFO_CTRL1...
		_iic->writeSingleByteWithVerify(ASM330LHXXH_I2C_ADDRESS, 0x07, (uint8_t)0x00); // TODO: investigate

		// set register FIFO_CTRL2...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x08, 0); // WTM8
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x08, 1);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x08, 2);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x08, 3);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x08, 4); // ODRCHG_EN
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x08, 5);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x08, 6);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x08, 7); // STOP_ON_WTM

		// set register FIFO_CTRL3...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x09, 0); // BDR_XL_0
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x09, 1); // BDR_XL_1
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x09, 2); // BDR_XL_2
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x09, 3); // BDR_XL_3
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x09, 4); // BDR_GY_0
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x09, 5); // BDR_GY_1
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x09, 6); // BDR_GY_2
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x09, 7); // BDR_GY_3

		// set register FIFO_CTRL4...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x0A, 0); // FIFO_MODE0
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x0A, 1); // FIFO_MODE1
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x0A, 2); // FIFO_MODE2
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x0A, 3);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x0A, 4); // ODR_T_BATCH_0
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x0A, 5); // ODR_T_BATCH_1
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x0A, 6); // DEC_TS_BATCH_0
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x0A, 7); // DEC_TS_BATCH_1

		// set register CTRL1_XL...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x10, 0);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x10, 1); // LPF2_XL_EN
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x10, 2); // FS0_XL
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x10, 3); // FS1_XL
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x10, 4); // ODR_XL0
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x10, 5); // ODR_XL1
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x10, 6); // ODR_XL2
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x10, 7); // ODR_XL3

		// set register CTRL2_G...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x11, 0); // FS_4000
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x11, 1); // FS_125
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x11, 2); // FS0_G
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x11, 3); // FS1_G
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x11, 4); // ODR_G0
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x11, 5); // ODR_G1
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x11, 6); // ODR_G2
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x11, 7); // ODR_G3

		// set register CTRL3_C...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x12, 0); // SW_RESET
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x12, 1);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x12, 2); // IF_INC
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x12, 3); // SIM
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x12, 4); // PP_OD
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x12, 5); // H_LACTIVE
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x12, 6); // BDU
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x12, 7); // BOOT

		// set register CTRL4_C...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x13, 0);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x13, 1); // LPF1_SEL_G
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x13, 2); // I2C_DISABLE
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x13, 3); // DRDY_MASK
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x13, 4);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x13, 5); // INT2_ON_INT1
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x13, 6); // SLEEP_G
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x13, 7);

		// set register CTRL5_C...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x14, 0); // ST0_XL
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x14, 1); // ST1_XL
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x14, 2); // ST0_G
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x14, 3); // ST1_G
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x14, 4);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x14, 5); // ROUNDING0
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x14, 6); // ROUNDING1
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x14, 7);

		// set register CTRL6_C...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x15, 0); // FTYPE_0
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x15, 1); // FTYPE_1
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x15, 2); // FTYPE_2
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x15, 3); // USR_OFF_W
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x15, 4); // XL_HM_MODE
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x15, 5); // LVL2_EN
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x15, 6); // LVL1_EN
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x15, 7); // TRIG_EN

		// set register CTRL7_C...
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x16, 0);
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x16, 1); // USR_OFF_ON_OUT
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x16, 2);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x16, 3);
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x16, 4); // HPM0_G
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x16, 5); // HPM1_G
		_iic->bitSet(ASM330LHXXH_I2C_ADDRESS, 0x16, 6); // HP_EN_G
		_iic->bitClr(ASM330LHXXH_I2C_ADDRESS, 0x16, 7); // G_HM_MODE


		return readyWithSet(textClassNameAsm330lhxxh, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		if (!_alarm->alarm())
			return status();
		_alarm->reset();





		return readyNoSet(textClassNameAsm330lhxxh, __LINE__);
	}
};

#endif /* APP_INC_ASM330LHXXH_HPP_ */
