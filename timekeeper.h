#ifndef RHRF
#define RHRF
  #include <RH_ASK.h>
  #include <stdint.h>
#endif//STDINT
#ifndef RH_TIME_H
#define RH_TIME_H

class local_time{
  //private:

  public:
    local_time();
    struct packet{
      const uint8_t sel PROGMEM = 1;
      bool w;
      uint8_t sec;
      uint8_t min;
      uint8_t hour;
      uint8_t weekday;
      uint8_t yearday;
      uint8_t  month;
      uint16_t year;
      bool     leap;
    };

   //call func to transmit clock data to RadioHead
    void send(RH_ASK*, packet*);

   //call to update local time from RadioHead recieved time
    void recv(packet*,uint8_t*,uint8_t);
  
   //call func to keep local time up-to-date
    void update(packet&);
};
#endif
