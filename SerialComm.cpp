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
 *  You are free to use and share but please leave an acknowledge.
 */

 #include "SerialComm.h"

// Set up a serial object that can receive commands with dataLength bytes.
SerialComm::SerialComm(int dataLength, long baudRate){
  // Memoryspace for startbyte, data bytes and checksum
  serialDecoderBufferSize = dataLength + 2 + 1; // Buffer is one more than bytes used in transmission
  serialDecodeBuffer = new byte[serialDecoderBufferSize];
  serialDecodeBufferStart = 0;  // Index of first byte in buffer
  serialDecodeBufferStop = 0;  // Index of next free space in buffer
  checksumOk = 0;
  Serial.begin(baudRate);
}

// Check if there is new data in buffer and try to decode it
// Returns true if data package was succesfully decoded
boolean SerialComm::processBuffer(){
  if (checksumOk)
    popByte(); // Move to next package
  checksumOk = 0;
  shiftToStart();
  // If bytes in input buffer fill up decode buffer
  while (Serial.available() && !decodeBufferFull()){
    // Get byte from UART buffer and put in decoder buffer
    addByte(Serial.read());
    // Drop bytes if the startbyte isn't the first
    shiftToStart();
  }
  // If decode buffer is full (i.e. whole command has been received) then check the checksum and answer
  if (decodeBufferFull()){
    byte checksum = peekByte(1);
    for (int i = 2; i < serialDecoderBufferSize - 2; i ++)
      checksum = checksum ^ peekByte(i);
    if (checksum == peekByte(serialDecoderBufferSize-2)){
      Serial.write('a');
      Serial.write(checksum);
      checksumOk = 1;
    } else {
      // Look for the next start byte
      popByte(); // Remove current start byte
      processBuffer(); // Redo process until either no more data in USART buffer or data successfully decoded
    }
  }
  return checksumOk;
}

// Get the i:th byte received
byte SerialComm::getByte(byte i){
  if (!checksumOk)
    return 0;
  return peekByte(i+1); // skip the start byte
}

// Get an int stored in i:th (high byte) and j:th (low byte) byte
int SerialComm::getInt(byte i, byte j){
  if (!checksumOk)
    return 0;
  return ((int)peekByte(i+1))<<8 + peekByte(j+1); // skip the start byte
}

// Get a long stored in i:th (highest byte) to the j:th (lowest byte) byte
long SerialComm::getLong(byte i, byte j){
  if (!checksumOk)
    return 0;
  long l = 0;
  signed int bytes = j-i;
  if (bytes < 0)
    bytes = -bytes;
  bytes++; // Number of bytes is one more than diff between start and stop numbers
  byte index;
  for (int k = 0; k < bytes; k ++){
    if (i < j)
      index = i+k;
    else
      index = i-k;
    l = (l<<8) + peekByte(index+1); // Skip start byte in index calc
  }
  return l;
}

// Drop bytes until the first byte in decode buffer is a startbyte
void SerialComm::shiftToStart() {
  while (serialDecodeBuffer[serialDecodeBufferStart] != 's' && !decodeBufferEmpty())
    popByte();
}

// Pull out and remove the first byte in decode buffer
byte SerialComm::popByte(){
  if (decodeBufferEmpty())
    return 0;
  byte b = serialDecodeBuffer[serialDecodeBufferStart];
  serialDecodeBufferStart = (serialDecodeBufferStart<serialDecoderBufferSize-1)?serialDecodeBufferStart+1:0;
  return b;
}

// Peek on i:th byte in decode buffer (without removing it)
byte SerialComm::peekByte(byte i){
  if (inBuffer() < i)
    return 0;
  byte index = serialDecodeBufferStart + i;
  if (index > serialDecoderBufferSize-1)
    index = index - serialDecoderBufferSize;
  return serialDecodeBuffer[index];
}

// Add a byte to the end of decode buffer
void SerialComm::addByte(byte b){
  if (decodeBufferFull())
    return;
  serialDecodeBuffer[serialDecodeBufferStop] = b;
  serialDecodeBufferStop = (serialDecodeBufferStop<serialDecoderBufferSize-1)?serialDecodeBufferStop+1:0;
}

// Is decode buffer full?
boolean SerialComm::decodeBufferFull() {
  return inBuffer() >= serialDecoderBufferSize - 1;
}

// Is decode buffer empty?
boolean SerialComm::decodeBufferEmpty() {
  return inBuffer() == 0;
}

// Check how many byte that is in the decode buffer
int SerialComm::inBuffer(){
  if (serialDecodeBufferStop >= serialDecodeBufferStart)
    return serialDecodeBufferStop-serialDecodeBufferStart;
  else
    return serialDecoderBufferSize - serialDecodeBufferStart + serialDecodeBufferStop;
}
