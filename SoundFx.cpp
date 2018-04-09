#include "SoundFx.h"



SoundFx::SoundFx(){
  isPlaying = false;
  startPlayTime = 0;
}


void SoundFx::init(){
  for(int sound = SoundFx::SABER_ON; sound <= SoundFx::FIRE_DS; sound++){
    pinMode(sound,OUTPUT);
    digitalWrite(sound,HIGH);
  }
}


void SoundFx::update(){
 if(isPlaying && (millis()-startPlayTime > SOUND_PIN_HOLD_TIME)){
  digitalWrite(currentSound, HIGH);
  startPlayTime = 0;
  isPlaying = false;
 }
}




void SoundFx::playSound(Sound s){
  if(isPlaying){
    return;
  }else{
    digitalWrite(s,LOW);
    startPlayTime = millis();
    currentSound = s;
    isPlaying = true;
 }
}

