#include <RH_ASK.h>
#include <timekeeper.h>

#define RF_TX_PIN 12
#define RF_RX_PIN 11
// Create Amplitude Shift Keying Object
RH_ASK rf_driver(2000,RF_RX_PIN,RF_TX_PIN);

struct head{
  uint8_t sel;
  bool w;
};


void setup() {
  rf_driver.init();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
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
    }
}
