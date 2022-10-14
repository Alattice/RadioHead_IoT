#ifndef RHRF
#define RHRF
  #include <RH_ASK.h>
  #include <stdint.h>
#endif//STDINT
#ifndef PRESENCE_H
#define PRESENCE_H
class presence{
  private:
    volatile int _inner_pin;
    volatile int _outer_pin;
    int size;
  public:
    presence(uint8_t);
    presence(int out_pin,int in_pin,uint8_t);
    struct packet{
      const uint8_t sel PROGMEM = 10;//room assignment; muliple of 10 for call ease
      bool w;
      uint8_t sensor;
      bool    present;
    };
    void send(RH_ASK*, packet*);
    void recv(packet*,uint8_t*,uint8_t);
    void update(packet&);
};
#endif //PRESENCE_h
