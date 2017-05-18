/*
 *  Class for communicating over serial port. Data received is expected to be of format
 *  's' data0 data1 ... dataN checksum
 *  where 's' is a start value and checksum is the xor of all data bytes 1..N.
 * 
 *  At correct receive and decode of data an acknowledge is sent back
 *  'a' checksum
 *  where checksum is the same as what was received.
 *
 *  Daniel Falk
 *  DA-Robotteknik
 *  +46 (0) 76-80 156 12
 *  daniel @T da-robotteknik.se
 *
 *  2016
 *  You are free to use and share but please leave a acknowledge.
 */

 #ifndef SERIAL_COM_H
 #define SERIAL_COM_H

#include "Arduino.h"

 class SerialComm {
    public:
      // Set up a serial object that can receive commands with dataLength bytes.
      SerialComm(int dataLength, long baudRate);
     
      // Check if there is new data in buffer and try to decode it
      // Returns true if data package was succesfully decoded
      boolean processBuffer();
     
      // Get the i:th byte received
      byte getByte(byte i);
     
      // Get an int stored in i:th (high byte) and j:th (low byte) byte
      int getInt(byte i, byte j);
     
      // Get a long stored in i:th (highest byte) to the j:th (lowest byte) byte
      long getLong(byte i, byte j);

    private:
      byte *serialDecodeBuffer;
      byte serialDecoderBufferSize;
      byte serialDecodeBufferStart; // Index of first byte in decode buffer
      byte serialDecodeBufferStop; // Next free index in decode buffer
      boolean checksumOk;

      // Drop bytes until the first byte in decode buffer is a startbyte
      void shiftToStart();
     
      // Pull out and remove the first byte in decode buffer
      byte popByte();
     
      // Peek on i:th byte in decode buffer (without removing it)
      byte peekByte(byte i);
     
      // Add a byte to the end of decode buffer
      void addByte(byte b);
     
      // Is decode buffer full?
      boolean decodeBufferFull();
     
      // Is decode buffer empty?
      boolean decodeBufferEmpty();
     
      // Check how many byte that is in the decode buffer
      int inBuffer();
 };

 #endif
