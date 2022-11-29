/*
 * using radiohead 1.84
 * This file is just for testing the timekeeper function
 * to check that time keeping and tx/rx works
 * it will send a tx write msg of the time every second
 * 
 * main loop is sorted in sections you can copy into your own code.
 * ROUTINES needed in both transmit and recieve
 * TRANSMIT only needed when local device is transmitter
 * RECIEVER only needed when local device is reciever
 * 
 * 
 * 
 * RESOURCES
 * possible time alternative: https://github.com/PaulStoffregen/Time
 * struct Radiohead: https://arduino.stackexchange.com/questions/37197/char-array-buffers-string-uint8
 * passing pointer in func: https://stackoverflow.com/questions/41930685/passing-pointer-from-function-to-function
 */
//include timekeeping module
//#include <RH_ASK.h>
#include "timekeeper.h"
#include <RTClib.h>

#define RF_TX_PIN 12
#define RF_RX_PIN 11

// Create Amplitude Shift Keying Object
RH_ASK rf_driver(2000,RF_RX_PIN,RF_TX_PIN);

//   DS1302 rtc(ce_pin, sck_pin, io_pin);
// ce_pin  (RST): default 4
// sck_pin (CLK): default 5
// io_pin  (DAT): default 6
DS1302 rtc;

local_time timek;
local_time::packet curr_time;
//local device transmit or reciever?
bool transmitter = 1;//1==transmitter; 0==reciever

//create structs that will be used (found in coresponding header files)

struct head{
  uint8_t sel;
  bool w;
};

//time keeper
uint8_t track_sec = curr_time.sec;//used to differenciate time and send RF every second
uint8_t track_day = curr_time.weekday;

void setup()
{
    rtc.begin();
    //Initialize ASK Object
    rf_driver.init();
    //rtc.adjust(DateTime(__DATE__, __TIME__));
    Serial.begin(9600);

    //optionally pre-assign time values

}

void loop()
{
  //ROUTINES
  timek.update(curr_time);//keep clock ticking
  if(track_day != curr_time.weekday){
    resync();
  }

  //RADIOHEAD TRANSMIT
  if(curr_time.sec != track_sec){//transmit every interval
    track_sec = curr_time.sec;
    serial_time();
    curr_time.w = 1;//set write to true, thus sending next time it runs transmit_clock()
    timek.send(&rf_driver,&curr_time);//will only transmit when curr_time.w == 1;
  }

  if(curr_time.sec != track_sec){//displays time at 1 sec intervals
      track_sec = curr_time.sec;
      serial_time();
  }
  
}

void resync(){
    DateTime now = rtc.now();
    curr_time.year  = now.year();
    curr_time.month = now.month();
    curr_time.weekday   = now.dayOfWeek();
    curr_time.hour  = now.hour();
    curr_time.min = now.minute();
    curr_time.sec = now.second();
}

//print the date &time to console
void serial_time(){
  Serial.print(curr_time.hour);
  Serial.print(":");
  Serial.print(curr_time.min);
  Serial.print(".");
  Serial.print(curr_time.sec);
  Serial.print("\t");
  switch(curr_time.weekday){
    case 1:{  Serial.print("Monday");    }break;
    case 2:{  Serial.print("Tueday");    }break;
    case 3:{  Serial.print("Wednesday"); }break;
    case 4:{  Serial.print("Thursday");  }break;
    case 5:{  Serial.print("Friday");    }break;
    case 6:{  Serial.print("Saturday");  }break;
    case 7:{  Serial.print("Sunday");    }break;
  }
  Serial.print("\t");
  switch(curr_time.month){
    case 1:{  Serial.print("Jan"); }break;
    case 2:{  Serial.print("Feb"); }break;
    case 3:{  Serial.print("Mar"); }break;
    case 4:{  Serial.print("Apr"); }break;
    case 5:{  Serial.print("May"); }break;
    case 6:{  Serial.print("Jun"); }break;
    case 7:{  Serial.print("Jul"); }break;
    case 8:{  Serial.print("Aug"); }break;
    case 9:{  Serial.print("Sep"); }break;
    case 10:{ Serial.print("Oct"); }break;
    case 11:{ Serial.print("Nov"); }break;
    case 12:{ Serial.print("Dec"); }break;
  }
  Serial.print("\t");
  Serial.println(curr_time.year);
}
