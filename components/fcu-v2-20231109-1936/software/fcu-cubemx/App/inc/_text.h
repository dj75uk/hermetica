/*
 * _text.h
 *
 *  Created on: Oct 22, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC__TEXT_H_
#define APP_INC__TEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

const char* textClassNameCInterrupts		= "stm32h7xx_it";
const char* textClassNameApplication		= "app";
const char* textClassNameFcu				= "fcu";
const char* textClassNamePca9685pw			= "pca9685pw";
const char* textClassNameBoard				= "board";
const char* textClassNameBoardV2R1			= "boardv2r1";
const char* textClassNameLis2hh12			= "lis2hh12";
const char* textClassNameStm32h723vgt6 		= "stm32h723vgt6";
const char* textClassNameAsm330lhxxh 		= "asm330lhxxh";
const char* textClassNameBno055 			= "bno055";
const char* textClassNameIis2mdc 			= "iis2mdc";
const char* textClassNameHcsr04p            = "hcsr04p";

const char* textFaultHard					= "hard fault";
const char* textFaultMemory					= "memory fault";
const char* textFaultBus					= "bus fault";
const char* textFaultUsage					= "usage fault";

const char* textFormatIicTrace				= "device=0x%02X register=0x%02X %s 0b%s %03u 0x%02X => %s";
const char* textIicTraceWrite				= "[write]";
const char* textIicTraceRead				= "[read] ";

const char* textOk							= "ok";
const char* textError						= "error";
const char* textTimeout						= "timeout";
const char* textBusy						= "busy";
const char* textRun							= "run";
const char* textStop						= "stop";




#ifdef __cplusplus
}
#endif

#endif /* APP_INC__TEXT_H_ */
