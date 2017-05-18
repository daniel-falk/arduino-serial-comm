# arduino-serial-comm #

Class for communicating over serial port. Data received is expected to be of format
```
's' data0 data1 ... dataN checksum
```
where 's' indicates start of package and checksum is the xor of all data bytes 1..N.

After correct receive and decode of data an 2 byte acknowledge is sent back in the form of
```
'a' checksum
```
where checksum is the same as what was received.

# Copyright #

Daniel Falk  
+46 (0) 76-80 156 12  
daniel@da-robotteknik.se  

2016  
You are free to use and share but please leave an acknowledge.
Licensed under MIT license

