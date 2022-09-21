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

timekeeper::timekeeper(uint8_t sel)
  :
  _sel(sel)
  {}

class time_{ 
  struct datetime_{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t weekday;
    uint16_t day;
    uint8_t month;
  } curr_time;

  struct clocker{
    const uint8_t sel = 1;
    bool w; //0 - read; 1 - write 
    datetime_ curr_time;
  };
  public:
    void set_time(uint8_t sec = 0, uint8_t min = 0, uint8_t hour = 0, uint8_t weekday = 1, uint8_t day = 1, uint8_t month = 1){
      this->sec     = sec;
      this->min     = min;
      this->hour    = hour;
      this->weekday = weekday;
      this->day     = day;
      this->month   = month;
    }

    void tx(RH_ASK *rf_driver,clocker *command){
      if(command->w){
        rf_driver->send((uint8_t *)command, sizeof(*command));
        rf_driver->waitPacketSent();
        command->w = false;
      }
    }

    uint8_t rx(clocker *curr_time,uint8_t (*buf),uint8_t buflen){
      if(buf[1]){//if write == true, save time
        memcpy(curr_time,buf,buflen);
        curr_time->w = 0;
      }else if(!buf[1]){//if write == false, read local time and prep to send; only used for master timekeeper
        curr_time->w = 1;
      }
    }

    void update(clocker *time_k){
      static uint32_t curr_ms, prev_ms = millis();
      uint8_t month_end;
      //static bool am = false;

      //update millis and seconds
      curr_ms = millis();
      //reset rtc when millis resets
      if(curr_ms < prev_ms){prev_ms = 0;}
      if(curr_ms >= (prev_ms+1000)){
        prev_ms=curr_ms;
        time_k->time_date[0]++; 
      }
      //reset seconds, minutes, hour, day?, month?
      if(time_k->time_date[0] == 60){
        time_k->time_date[0] = 0;
        time_k->time_date[1]++;
      }//min>hour
      if(time_k->time_date[1] ==60){
        time_k->time_date[1] = 0;
        time_k->time_date[2]++;
      }//hour>weekday
      if(time_k->time_date[2] == 24){
        time_k->time_date[2] = 0;//hour
        time_k->time_date[3]++;//weekday
        time_k->time_date[4]++;//day
      }//weekday
      if(time_k->time_date[3] == 8){
        time_k->time_date[3] = 1;//weekday
      }//day>month
      if (time_k->time_date[5] == 2){month_end=28;}//set 28 when feb
      else if ((time_k->time_date[5]%((uint8_t)2))==0){month_end=30;}
      else{month_end = 31;}
      if(time_k->time_date[4] == (month_end+1)){//reset month
        time_k->time_date[4] = 1;//day
        time_k->time_date[5]++;
      }
      if(time_k->time_date[5]==13){//reset year
        time_k->time_date[5]=1;
        //curr_time[6]++;
      }
    }
}