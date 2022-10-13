/*|================================================|
 *|  //===\\ ||       //===\\    //===\\  ||  //   |
 *| ||       ||      ||     ||  ||        ||//     |
 *| ||       ||      ||     ||  ||        ||\\     |
 *|  \\===// ||_____  \\===//    \\===//  ||  \\   |
 *|================================================|
 *
 * Ver 1.0
 */
/* uint8_t[6] = [second,minute,hour,weekday,day,month]
 *           0[0-59] 1[0-59] 2[0-23] 3[1-7]  4[1-29/30/31] 5[1-12]
 */
#include "timekeeper.h"
#include <Arduino.h>

local_time::local_time(){
  //check valid init values

}
//    void local_time::set_time(uint8_t sec = 0, uint8_t min = 0, uint8_t hour = 0, uint8_t weekday = 1, uint8_t day = 1, uint8_t month = 1){
//      this->sec     = sec;
//      this->min     = min;
//      this->hour    = hour;
//      this->weekday = weekday;
//      this->day     = day;
//      this->month   = month;
//    };

void local_time::send(RH_ASK *rf_driver,header *command){
  if(command->w){
    rf_driver->send((uint8_t *) command, sizeof(*command));
    rf_driver->waitPacketSent();
    command->w = false;
  }
}

void local_time::recv(header* curr_time,uint8_t (*buf),uint8_t buflen){
  if(buf[1]){//if write == true, save time
    memcpy(curr_time,buf,buflen);
    curr_time->w = 0;
  }else if(!buf[1]){//if write == false, read local time and prep to send; only used for master timekeeper
    curr_time->w = 1;
  }
}

void local_time::update(header& time_k){
  static uint32_t curr_ms, prev_ms = millis();

  //update millis and seconds
  curr_ms = millis();
  //reset rtc when millis resets
  if(curr_ms < prev_ms){prev_ms = 0;}
  if(curr_ms >= (prev_ms+1000)){
    prev_ms=curr_ms;
    time_k.content.sec++; 
    //reset seconds, minutes, hour, day?, month?
    if(time_k.content.sec == 60){
      time_k.content.sec = 0;
      time_k.content.min++;
      if(time_k.content.min == 60){
        time_k.content.min = 0;
        time_k.content.hour++;
        if(time_k.content.hour == 24){
          time_k.content.hour = 0;//hour
          time_k.content.weekday++;//weekday
          time_k.content.yearday++;//day
          if(time_k.content.weekday == 8){
            time_k.content.weekday = 1;//weekday
          }//weekday
          uint8_t mo_sum;
          const uint8_t cal[] = {31,28,31,30,31,30,31,31,30,31,30,31};
          for(int a = 0; a < time_k.content.month; a++){
            mo_sum += cal[a];
            if(time_k.content.yearday < mo_sum){
              time_k.content.month = a+1;
              if(a == 12){ 
                time_k.content.month = 1;
                time_k.content.yearday = 1;//day
                time_k.content.year++;
              }
            }//mo>yr
          }//for mo
        }//hour>weekday
      }//min>hour
    }//sec>min
  }//ms>sec
}//func

