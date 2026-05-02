/*
 * command.cpp
 *
 * Created: 30/04/2023 13:43:48
 *  Author: Daniel.Johnson
 */ 
#include <string.h>
#include <stdint-gcc.h>
#include <stdio.h>
#include <Arduino.h>
#include "packet.h"
#include "crc.h"

void clearPacket(VtxCommandPacket* packet)
{
	packet->start = 0;
	packet->command = 0;
	packet->checksum = 0;
	packet->end = 0;
	for (uint8_t i = 0; i < 12; i++)
	packet->data.uint8s[i] = 0;
}

uint8_t calculateCrc(VtxCommandPacket* command)
{
	uint8_t data[13];
	data[0]  = command->command;
	data[1]  = command->data.uint8s[0];
	data[2]  = command->data.uint8s[1];
	data[3]  = command->data.uint8s[2];
	data[4]  = command->data.uint8s[3];
	data[5]  = command->data.uint8s[4];
	data[6]  = command->data.uint8s[5];
	data[7]  = command->data.uint8s[6];
	data[8]  = command->data.uint8s[7];
	data[9]  = command->data.uint8s[8];
	data[10] = command->data.uint8s[9];
	data[11] = command->data.uint8s[10];
	data[12] = command->data.uint8s[11];
	return crcQ(data, 13);
}
