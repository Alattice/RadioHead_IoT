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
#include "timekeeper.h"
//#include <SPI.h>
// Create Amplitude Shift Keying Object
RH_ASK rf_driver;
/* #define RXPIN 11;
 * #define TXPIN 12;
 *RH_ASK rf_driver(2000,RXPIN,TXPIN);
 */

//local device transmit or reciever?
bool transmitter = 0;//1==transmitter; 0==reciever

//create structs that will be used (found in coresponding header files)
struct clocker curr_time;
struct head{
  uint8_t sel;
  bool w;
};

//time keeper
uint8_t track_sec = curr_time.time_date[0];//used to differenciate time and send RF every second

void setup()
{
    //Initialize ASK Object
    rf_driver.init();
    Serial.begin(9600);

    //optionally pre-assign time values
    curr_time.time_date[0] = 13;//sec
    curr_time.time_date[1] = 53;//min
    curr_time.time_date[2] = 21;//hour
    curr_time.time_date[3] = 7; //weekday
    curr_time.time_date[4] = 14;//day
    curr_time.time_date[5] = 11;//month
}

void loop()
{
  //ROUTINES
  update_clock(&curr_time);//keep clock ticking
  
  if(transmitter){
    //RADIOHEAD TRANSMIT
    if(curr_time.time_date[0] != track_sec){//transmit every interval
      track_sec = curr_time.time_date[0];
      serial_time();
      curr_time.w = 1;//set write to true, thus sending next time it runs transmit_clock()
      transmit_clock(&rf_driver,&curr_time);//will only transmit when curr_time.w == 1;
    }
  }else{
    //RADIOHEAD RECIEVER
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];// Set buffer to size of expected message
    uint8_t buflen = sizeof(buf);
    //check for any messages 
    if (rf_driver.recv((uint8_t*)buf, &buflen)){
      //init header and determine sel
      head header;
      memcpy(&header,buf,2);
      Serial.print("rcv ");
      Serial.print(header.sel);
      Serial.print(" , ");
      Serial.println(header.w);
      //act based on select and read/write
      if(header.sel == curr_time.sel){
        recieve_clock(&curr_time,buf,8);
      }
    }
    if(curr_time.time_date[0] != track_sec){//displays time at 1 sec intervals
       track_sec = curr_time.time_date[0];
       serial_time();
    }
  }
}

//print the date &time to console
void serial_time(){
  Serial.print(curr_time.time_date[2]);
  Serial.print(":");
  Serial.print(curr_time.time_date[1]);
  Serial.print(".");
  Serial.print(curr_time.time_date[0]);
  Serial.print("\t");
  switch(curr_time.time_date[3]){
    case 1:{  Serial.print("Monday");    }break;
    case 2:{  Serial.print("Tueday");    }break;
    case 3:{  Serial.print("Wednesday"); }break;
    case 4:{  Serial.print("Thursday");  }break;
    case 5:{  Serial.print("Friday");    }break;
    case 6:{  Serial.print("Saturday");  }break;
    case 7:{  Serial.print("Sunday");    }break;
  }
  Serial.print("\t");
  switch(curr_time.time_date[5]){
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
  Serial.println(curr_time.time_date[4]);
}
