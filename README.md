# RadioHead IoT

IoT style configuration of Arduino modules using the RadioHead library(1.84)

Tested using 433MHz ASK tx/rx modules paired with Arduino Uno & Nano

Requires the RadioHead library (1.84 was used)

Move RadioHead_IoT folder to your Arduino program libraries (~Arduino/libraries) and restart the IDE.

## Lookup Table

There is a lookup.xlsx excel file containing the id for each command. packets are sent as a struct containing sel,r/w,data.
sel: an 8 bit number 0-256 determining the function of the data sent (ie. sel==1 -> timekeeper; sel==2 -> daylight sensor).
r/w: a boolean that determines if the sender wants to save data to the local device or read from local device.[0==read, 1==write].
data: the data thats associated with the sel and r/w. DataType changes accordingly. if the request was 'read', nothing goes here. if 'write', the local device saves the data.

| name          | select | read/write | datatype/struct                                                                             | definition                                        | example |
|---------------|--------|------------|---------------------------------------------------------------------------------------------|---------------------------------------------------|---------|
| syscomm       | 0      | 0          | uint16_t select                                                                             | read data contents of select id to serial console |         |
|               |        | 1          |                                                                                             |                                                   |         |
| clock         | 1      | 0          | return uint8_t[6]=[sec(0-59),min(0-59),hour(0-23),weekday(0-7),day(1-28/30/31),month(1-12)] | transmitter asks for time                         |         |
|               |        | 1          | uint8_t[6]=[sec(0-59),min(0-59),hour(0-23),weekday(0-7),day(1-28/30/31),month(1-12)]        | write command for date&time                       |         |
| walkie_talkie | 2      | 0          | char[99]=""                                                                                 | tx asks for this string to be returned            |         |
|               |        | 1          | char[99]=""                                                                                 | tx tells rx to send this to serial console        |         |
|               | 3      | 0          |                                                                                             |                                                   |         |
|               |        | 1          |                                                                                             |                                                   |         |
|               | 4      | 0          |                                                                                             |                                                   |         |
|               |        | 1          |                                                                                             |                                                   |         |
|               | 5      | 0          |                                                                                             |                                                   |         |
|               |        | 1          |                                                                                             |                                                   |         |
|               | 6      | 0          |                                                                                             |                                                   |         |
|               |        | 1          |                                                                                             |                                                   |         |
|               | 7      | 0          |                                                                                             |                                                   |         |
|               |        | 1          |                                                                                             |                                                   |         |
|               | 8      | 0          |                                                                                             |                                                   |         |
|               |        | 1          |                                                                                             |                                                   |         |
|               | 9      | 0          |                                                                                             |                                                   |         |
|               |        | 1          |                                                                                             |                                                   |         |
| presence      | 10     | 0          | return bool                                                                                 | ask if person present                             |         |
|               |        | 1          |                                                                                             | local device sends presence when setting changes  |         |


The select can go up to 256.

### changing sel assignments

The sel can be changed according to your use case by going into the header file and changing the sel assignment in the struct declaration.

## timekeeper.h

include this for all timekeeping related function.
main file (.ino) will need to declare:

```
<span style="color:orange">#include</span> "timekeeper.h"
RH_ASK rf_driver;
<span style="color:blue">uint8_t</span> name_of_timekeeper[6];
```

where name_of_timekeeper = {second,minute,hour,weekday,day,month}.

In the main loop, add this line to keep the time ticking on the device:

```update_clock(name_of_timekeeper);```

This will me saved to the array name_of_timekeeper.

### transmitting

To transmit the local time, add this line anywhere you wish to send the local time over RF:

```transmit_clock(&rf_driver,name_of_timekeeper);```

### Recieving

To recieve the time from other modules, the local device needs to listen to the RadioHead buffer for data.


## Additional Resources
https://en.cppreference.com/w/cpp/preprocessor/conditional
