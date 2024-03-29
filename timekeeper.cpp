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

void local_time::send(RH_ASK *rf_driver,packet *command){
  if(command->w){
    rf_driver->send((uint8_t *) command, sizeof(*command));
    rf_driver->waitPacketSent();
    command->w = false;
  }
}

void local_time::recv(packet* curr_time,uint8_t (*buf),uint8_t buflen){
  if(buf[1]){//if write == true, save time
    memcpy(curr_time,buf,buflen);
    curr_time->w = 0;
  }else if(!buf[1]){//if write == false, read local time and prep to send; only used for master timekeeper
    curr_time->w = 1;
  }
}

void local_time::update(packet& time_k){
  static uint32_t curr_ms, prev_ms = millis();

  //update millis and seconds
  curr_ms = millis();
  //reset rtc when millis resets
  if(curr_ms < prev_ms){prev_ms = 0;}
  if(curr_ms >= (prev_ms+1000)){
    prev_ms=curr_ms;
    time_k.sec++; 
    //reset seconds, minutes, hour, day?, month?
    if(time_k.sec == 60){
      time_k.sec = 0;
      time_k.min++;
      if(time_k.min == 60){
        time_k.min = 0;
        time_k.hour++;
        if(time_k.hour == 24){
          time_k.hour = 0;//hour
          time_k.weekday++;//weekday
          time_k.yearday++;//day
          if(time_k.weekday == 8){
            time_k.weekday = 1;//weekday
          }//weekday
          uint8_t mo_sum;
          uint8_t cal[] = {31,time_k.leap,31,30,31,30,31,31,30,31,30,31};
          for(int a = 0; a < time_k.month; a++){
            mo_sum += cal[a];
            if(time_k.yearday < mo_sum){
              time_k.month = a+1;
              if(a == 12){ 
                time_k.month = 1;
                time_k.yearday = 1;//day
                time_k.year++;
                if(time_k.year%4==0){
                  time_k.leap = 29;
                }else{
                  time_k.leap = 28;
                }//leap year
              }//yr++
            }//mo>yr
          }//for loop mo
        }//hour>weekday & dayofyear
      }//min>hour
    }//sec>min
  }//ms>sec
}//func

