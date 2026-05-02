/*
 * hardware-macros.h
 *
 * Created: 04/05/2023 09:17:42
 *  Author: Daniel.Johnson
 */ 
#ifndef HARDWARE_MACROS_H_
	#define HARDWARE_MACROS_H_

	#define LED_ERR_ON		PORTK |=  (1<<PK0);
	#define LED_ERR_OFF		PORTK &= ~(1<<PK0);
	#define LED_CMD_ON		PORTK |=  (1<<PK1);
	#define LED_CMD_OFF		PORTK &= ~(1<<PK1);
	#define LED_OSD_ON		PORTK |=  (1<<PK2);
	#define LED_OSD_OFF		PORTK &= ~(1<<PK2);

#endif /* HARDWARE_MACROS_H_ */