/*
 * main2.cpp
 *
 *  Created on: Oct 5, 2023
 *      Author: Daniel Johnson
 */
#include <cstdio>
#include <cstdint>
#include <cstring>
//

#include "main.h"
//
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_uart.h"
//
#include "input.hpp"
#include "component.hpp"
//
//#include "mcu.hpp"
#include "stm32h723vgt6.hpp"

#include "servo.hpp"
#include "esc.hpp"

//#include "rcrx.hpp"
#include "rcrx-sbus.hpp"
#include "rcrx-dsmx.hpp"

#include "mb85rc256v.hpp"
#include "bno055.hpp"
#include "bme280.hpp"
#include "uartgps.hpp"
#include "hcsr04p.hpp"

//#include "board.hpp"
#include "board-v2r1.hpp"

//#include "vtxclient.hpp"

#include "inputprocessor.hpp"

#include "fcu.hpp"

#include "application.h"


//
#ifdef __cplusplus
extern "C"
{
#endif

extern const char* textClassNameApplication;
extern const char* textRun;

//extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
//extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
//extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart7;
//extern UART_HandleTypeDef huart9;
extern UART_HandleTypeDef huart10;
extern I2C_HandleTypeDef  hi2c4;
extern I2C_HandleTypeDef  hi2c5;

// data-points directly from input devices (receivers etc)...
InputDatum inputData[DATUMS];

// data-points by usage...
Input      inputs[USAGEDATUMS];

// primary mcu...
Stm32h723vgt6 mcu(
	&huart2, &huart4, &huart5, &huart7, &huart10,
	&hi2c4,  &hi2c5
);

// radiolink at10-2 sbus controller...
RcRxSbus sbusRcRx(mcu.uart10(), inputData, InputSource::Source1, 11);

// spektrum dx6e dsmx controller...
RcRxDsmx dsmxRcRx(mcu.uart5(), inputData, InputSource::Source2);

// external components...
Mb85rc256v mb85rc256v(mcu.iic5());
Bno055     bno055(mcu.iic5());
Bme280     bme280(mcu.iic5());
UartGps    uartgps(mcu.uart7());
Hcsr04p    hcsr04p(GPIOE, GPIO_PIN_10, 9);

// create a list of components to inject into the board...
HasStatusAndSetup* externalComponents[] = {
	&sbusRcRx,
	&dsmxRcRx,
	&mb85rc256v,
	&bno055,
	&bme280,
	&uartgps,
	&hcsr04p
};
uint8_t externalComponentsCount =
	sizeof(externalComponents) / sizeof(HasStatusAndSetup*);

// create the servos...
Esc   servoLEsc(	  6);
Servo servoLAileron(  7, PWM_CENTRE,  1.00F,  true, 1100, 2000);
Servo servoLVTail(	  8, PWM_CENTRE,  1.00F,  true, 1100, 1900);
Servo servoRVTail(	  9, PWM_CENTRE,  1.00F, false, 1100, 1900);
Servo servoRAileron( 10, PWM_CENTRE,  1.00F,  true, 1100, 1900);
Esc   servoREsc(	 11);

// create the board...
BoardV2R1 board(&mcu,
	&servoREsc,
	&servoLEsc,
	&servoRAileron,
	&servoLAileron,
	&servoRVTail,
	&servoLVTail,
	externalComponents, externalComponentsCount
);

// create the vtx client...
VtxClient vtxClient(mcu.uart2(), &board);

// create the input processor...
InputProcessor inputProcessor(inputData, inputs);

// create the fcu...
Fcu fcu(&board, &inputProcessor, &vtxClient);

void applicationClearDatumArray()
{
	for (uint8_t i = 0; i < DATUMS; i++)
		inputData[i].clear();
}
void applicationSetSbusConfiguration()
{
	uint8_t source = (uint8_t)InputSource::Source1;
	inputData[source +  0] = InputDatum( 310, 1690, InputType::Analogue, InputUsage::Roll,                 10, 10, 10, false,  true); // RSH
	inputData[source +  1] = InputDatum( 310, 1690, InputType::Analogue, InputUsage::Pitch,                10, 10, 10, false,  true); // RSV
	inputData[source +  2] = InputDatum( 310, 1690, InputType::Analogue, InputUsage::Throttle,             10,  0, 10, false,  true); // LSV
	inputData[source +  3] = InputDatum( 310, 1690, InputType::Analogue, InputUsage::Yaw,                  10, 10, 10, false,  true); // LSH
	inputData[source +  4] = InputDatum(  80, 1920, InputType::Digital2, InputUsage::Aux01,                 0,  0,  0, false, false); // Switch H
	inputData[source +  5] = InputDatum(  80, 1920, InputType::Digital3, InputUsage::FcuMode,               0,  0,  0, false,  true); // Switch G
	inputData[source +  6] = InputDatum(  80, 1920, InputType::Analogue, InputUsage::ThrottleDifferential, 30, 30, 30, false, false); // VR A
	inputData[source +  7] = InputDatum(  80, 1920, InputType::Analogue, InputUsage::Aux02,                30, 30, 30, false, false); // VR B
	inputData[source +  8] = InputDatum(  80, 1920, InputType::Analogue, InputUsage::Aux03,                30, 30, 30, false, false); // VR C
	inputData[source +  9] = InputDatum(  80, 1920, InputType::Digital2, InputUsage::Aux04,                 0,  0,  0, false, false); // Switch F
	inputData[source + 10] = InputDatum(  80, 1920, InputType::Digital3, InputUsage::Aux05,                 0,  0,  0, false, false); // Switch E
	inputData[source + 11] = InputDatum(  80, 1920, InputType::Analogue, InputUsage::Rssi,                  0,  0,  0, false, false); // RSSI
}
void applicationSetDsmxConfiguration()
{
	uint8_t source = (uint8_t)InputSource::Source2;
	inputData[source +  0] = InputDatum( 450, 1550, InputType::Analogue, InputUsage::Throttle,             40,  0, 20, false, false); // LSV
	inputData[source +  1] = InputDatum( 440, 1610, InputType::Analogue, InputUsage::Roll,                 30, 30, 30, false, false); // RSH
	inputData[source +  2] = InputDatum( 440, 1610, InputType::Analogue, InputUsage::Pitch,                30, 30, 30, false, false); // RSV
	inputData[source +  3] = InputDatum( 440, 1610, InputType::Analogue, InputUsage::Yaw,                  30, 30, 30, false, false); // LSH
	inputData[source +  4] = InputDatum( 440, 1610, InputType::Digital2, InputUsage::Override,             10,  0, 10, false, false); // Switch H
	inputData[source +  5] = InputDatum( 440, 1610, InputType::Digital3, InputUsage::FcuMode,              10, 20, 10, false, false); // Switch G
}
void applicationRun()
{
	// create the application...
	debugWriteInfo(textClassNameApplication, __LINE__, textRun);

	// clear the array of datums...
	applicationClearDatumArray();

	// radiolink at10-2 sbus configuration...
	applicationSetSbusConfiguration();

	// spektrum dx6e dsmx configuration...
	applicationSetDsmxConfiguration();

	// setup the fcu...
	fcu.setup();

	debugWriteInfo(textClassNameApplication, __LINE__, "cycling");

	// cycle the fcu forever...
	while (true)
	{
		fcu.cycle();
	}
}

#ifdef __cplusplus
}
#endif
