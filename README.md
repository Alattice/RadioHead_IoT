IoT style configuration of Arduino modules using the RadioHead library(1.84)

Tested using 433MHz ASK tx/rx modules paired with Arduino Uno & Nano

Requires the RadioHead library (1.84 was used)

Move RadioHead_IoT folder to your Arduino program libraries (~Arduino/libraries) and restart the IDE.

##lookup table
There is a lookup.xlsx excel file containing the id for each command. packets are sent as a struct containing sel,r/w,data.
sel: an 8 bit number 0-256 determining the function of the data sent (ie. sel==1 -> timekeeper; sel==2 -> daylight sensor).
r/w: a boolean that determines if the sender wants to save data to the local device or read from local device.[0==read, 1==write].
data: the data thats associated with the sel and r/w. DataType changes accordingly. if the request was 'read', nothing goes here. if 'write', the local device saves the data.

##changing sel assignments
The sel can be changed according to your use case by going into the header file and changing the sel assignment in the struct declaration.
##clocker.h:
include this for all timekeeping related function.
main file (.ino) will need to declare
```#include "clocker.h"
RH_ASK rf_driver;
uint8_t name_of_timekeeper[4] = {0,0,0,0};
```

where name_of_timekeeper = {seconds, minute, hour, day}.
In main loop:
```update_clock(name_of_timekeeper);```
this keeps the clock running and will save the current local time to name_of_timekeeper.

###transmitting
To transmit the local time, add this line anywhere you wish to send the local time over RF:
```transmit_clock(&rf_driver,name_of_timekeeper);```

###Recieving
T recieve the time from other modules, the local device needs to listen to the RadioHead buffer for data.


##Additional Resources
https://en.cppreference.com/w/cpp/preprocessor/conditional