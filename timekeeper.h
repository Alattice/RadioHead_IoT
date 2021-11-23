#ifndef RHRF
#define RHRF
  #include <RH_ASK.h>
#endif//RHRF
#ifndef STDINT
#define STDINT
  #include <stdint.h>
#endif//STDINT
#ifndef TIMEKEEP_H
#define TIMEKEEP_H
struct clocker{
  const uint8_t sel = 1;
  bool w; //0 - read; 1 - write 
  uint8_t time_date[6];
};
  //update_clock:
  //call func to keep local time up-to-date
  void update_clock(clocker *time_k);
  
  //transmit_clock(*rf_driver, int &local_current_time)
  //call func to transmit clock data to RadioHead
  void transmit_clock(RH_ASK *rf_driver,clocker *command);

  //recieve_clock
  //call to update local time from RadioHead recieved time
  uint8_t recieve_clock(clocker *curr_time,uint8_t (*buf),uint8_t buflen);
#endif
