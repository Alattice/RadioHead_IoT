# RadioHead IoT

IoT style configuration of Arduino modules using the RadioHead library(1.84)

Tested using 433MHz ASK tx/rx modules paired with Arduino Uno & Nano

Requires the RadioHead library (1.84 was used). These are provided in /packages

Install RadioHead_IoT by using the IDE and 'Install from .zip' under tools.

By default RadioHead uses digital pin 11 for rx and digital pin 12 for tx.

# Dependencies

```
RadioHead
```

Dependencies are provided in /packages.

## Lookup Table

There is a lookup.xlsx excel file containing the id for each command. packets are sent as a struct containing sel,r/w,data.
sel: an 8 bit number 0-256 determining the function of the data sent (ie. sel==1 -> timekeeper; sel==2 -> daylight sensor).
w: a boolean that determines if the sender wants to save data to the local device or read from local device.[0==read, 1==write].
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

The select can go up to 256. 

All this can be changed by adjusting each struct in the header files (in case you want to remove some functions you wont need)

### Changing select assignments

The sel can be changed according to your use case by going into the header file and changing the sel assignment in the struct declaration. 

Below is an example from timekeeper.h

```
struct header{
  const uint8_t sel = 1; //<--change this to your liking
  bool w; //0 - read; 1 - write 
  dt content;
};
```

## timekeeper.h

include this for all timekeeping related function. An example of timekeeping alone can be found in examples/timekeeper.ino
your main file (.ino) will need to declare:

```
#include "timekeeper.h"
RH_ASK rf_driver;
local_time current_time;
```

where current_day_time = {second,minute,hour,weekday,day,month}.

In the main loop, add this line to keep the time ticking on the device:

```update_clock(&current_time);```

This will be saved to the time_date[6] array of the current_time struct.

### Transmitting

To transmit the local time, add this line anywhere you wish to send the local time over RF:

```
current_time.w = true;
transmit_clock(&rf_driver,&current_time);
```
The ```current_time.w = true;``` can be called whenever, and next time the code runs transmit_clock(), the time will be transmitted and current_time.w will be set to false.

### Recieving

To recieve the time from other modules, the local device needs to listen to the RadioHead buffer for data.

```
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];// Set buffer to size of expected message
uint8_t buflen = sizeof(buf);

if (rf_driver.recv((uint8_t*)buf, &buflen)){
```

then the sel and w of the recieved data must be determined to figure out how to work with the data

```
head header;
memcpy(&header,buf,2);
```

Optionally, you can also read ```buf[0]``` and ```buf[1]``` for sel and w respectively.

Then a large if statements are made for every packet the local device needs (in this case, timekeeper):

```
if(header.sel == current_time.sel){
	recieve_clock(&current_time,buf,8);// 8 is for 1 byte of each entry (sel,w,time_date[6])
}
```

## Additional Resources
- https://en.cppreference.com/w/cpp/preprocessor/conditional

- https://www.airspayce.com/mikem/arduino/RadioHead/classRH__ASK.html