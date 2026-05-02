/*
 * crc.h
 *
 * Created: 01/05/2023 21:49:04
 *  Author: Daniel.Johnson
 */ 


#ifndef CRC_H_
	#define CRC_H_
	
	#include <stdint-gcc.h>

	#define CRC_POLYGEN 0xd5
	
	uint8_t crc8(const uint8_t *data, uint8_t len);
	uint8_t crcQ(const uint8_t *data, const uint8_t len);

#endif /* CRC_H_ */