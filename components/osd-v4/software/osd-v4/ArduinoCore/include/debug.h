#pragma once
#ifndef _DEBUG_H
#define _DEBUG_H

void dumpHex(Stream* stream, const uint8_t data)
{
  stream->print("0x");
  if (data < 0x10)
    stream->print("0");
  stream->print(data, HEX);  
}

void dumpHexLn(Stream* stream, const uint8_t data)
{
  dumpHex(stream, data);
  stream->println("");
}

void dumpPacket(const char* rxOrTx, Stream* stream, uint8_t* packet, uint8_t length)
{
  stream->print(rxOrTx);
  stream->print(" PACKET: ");
  dumpHex(stream, packet[0]);
  stream->print(" ");
  dumpHexLn(stream, packet[1]);
  stream->print("      cmd= ");
  dumpHexLn(stream, packet[2]);

  uint8_t payloadLength = packet[3];
  stream->print("      len= ");
  dumpHexLn(stream, payloadLength);
  
  if (payloadLength > 0)
  {
    stream->print("      pay= ");
    for (uint8_t i = 0; i < payloadLength; i++)
    {
      dumpHex(stream, packet[i + 4]);
      stream->print(" ");
    }
    stream->println("");
  }
  stream->print("      crc= ");
  dumpHexLn(stream, packet[length - 1]);
  stream->println("");
}

void dumpBuffer(const char* rxOrTx, Stream* stream, uint8_t* packet, uint8_t length)
{
	stream->print(rxOrTx);
	stream->print(" BUFFER:");
	for (uint8_t index = 0; index < length; index++)
	{
		stream->print(" 0x");
		uint8_t datum = packet[index];
		if (datum < 0x10)
		stream->print("0");
		stream->print(datum, HEX);
	}
	stream->println("");
	stream->print("          ");
	for (uint8_t index = 0; index < length; index++)
	{
		stream->print("  ");
		uint8_t datum = packet[index];
		if (datum < 100)
		stream->print("0");
		if (datum < 10)
		stream->print("0");
		stream->print(datum, DEC);
	}
	stream->println("");
}

#endif // _DEBUG_H
