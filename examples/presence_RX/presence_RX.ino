#include "presence_rx.h"


#define RF_TX_PIN 12
#define RF_RX_PIN 11

RH_ASK rf_driver(2000,RF_RX_PIN,RF_TX_PIN);
presence pres(0);
presence::packet pack;

struct head{
  uint8_t sel;
  bool w;
};

void setup(){
  rf_driver.init();
  Serial.begin(9600);
}

void loop(){
  pres.update(pack);
  pack.w=1;
  pres.send(&rf_driver,&pack);
  Serial.println("send");
  delay(800);

  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);
  if (rf_driver.recv((uint8_t*)buf, &buflen)) {
    head header;
    digitalWrite(13,1);
    //determine sel
    memcpy(&header, buf, 2);
    Serial.print("rcv ");
    Serial.print(header.sel);
    Serial.print(" , ");
    Serial.println(header.w);
    //if write; save presence status; if read, save status of presence detect
    if(header.sel == pack.sel){
      if(header.w){
        memcpy(&pres, buf, 6);
        pack.w = false;
        Serial.println(pack.present);
      }
    }
    digitalWrite(13,0);
  }
}
