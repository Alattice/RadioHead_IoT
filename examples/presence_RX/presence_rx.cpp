#include "presence_rx.h"

presence::presence(uint8_t sensor){
  
}

presence::presence(int out_pin,int in_pin, uint8_t sensor){
  //packet.sensor = sensor;
  pinMode(in_pin, INPUT);
  pinMode(out_pin, INPUT);
  _outer_pin = out_pin;
  _inner_pin  = in_pin;
}

void presence::send(RH_ASK *rf_driver,packet *command){
  if(command->w){
    rf_driver->send((uint8_t *) command, sizeof(*command));
    rf_driver->waitPacketSent();
    command->w = false;
  }
}

void presence::recv(packet* pres,uint8_t (*buf),uint8_t buflen){
  if(buf[1]){//if write == true, save pres
    memcpy(pres,buf,buflen);
    pres->w = 0;
  }else if(!buf[1]){//if write == false, read local time and prep to send; only used for master timekeeper
    pres->w = 1;
  }
}

void presence::update(packet& pres){
  static uint8_t num_present = 0;   //number of people considered present
  static bool    last_state  = false;
  static bool    lock = false;      //lock the reading when person passes
         
  if(digitalRead(_outer_pin)&&digitalRead(_inner_pin)){
    lock = false;
  }else if((!digitalRead(_outer_pin))&&(!lock)){
    num_present--;
    lock=true;
  }else if((!digitalRead(_inner_pin))&&(!lock)){
    num_present++;
    lock=true;
  }

  if((num_present == 255)||(num_present == 0)){ //uint8_t doesnt have negative, when it rolls over to 255, reset
    num_present = 0;
    pres.present = false;
  }else{ 
    pres.present = true;
  }
  pres.w = true;  
  //determine if new presence needs to be broadcasted
  if(pres.present != last_state){
    pres.w = true;                 //prepare write to transmitter
    last_state = pres.present;
  }
//  Serial.print(pres->present);
//  Serial.print("\t");
//  Serial.println(num_present);
}
