//Core Libraries
#include <stdint.h>

//FastLED
#include <noise.h>
#include <bitswap.h>
#include <fastspi_types.h>
#include <pixelset.h>
#include <fastled_progmem.h>
#include <led_sysdefs.h>
#include <hsv2rgb.h>
#include <fastled_delay.h>
#include <colorpalettes.h>
#include <color.h>
#include <fastspi_ref.h>
#include <fastspi_bitbang.h>
#include <controller.h>
#include <fastled_config.h>
#include <colorutils.h>
#include <chipsets.h>
#include <pixeltypes.h>
#include <fastspi_dma.h>
#include <fastpin.h>
#include <fastspi_nop.h>
#include <platforms.h>
#include <lib8tion.h>
#include <cpp_compat.h>
#include <fastspi.h>
#include <FastLED.h>
#include <dmx.h>
#include <power_mgt.h>

// Button Debounce
#include <Bounce2.h>
#include "SoundFx.h"



//Debug mode
#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println (x)
#else
 #define DEBUG_PRINT(x)
#endif



//Power On/Off
enum SYSTEM_STATE {OFF=0, POWERING_ON=1, ON=2, POWERING_OFF=3};
#define POWER_BUTTON 2
#define POWER_HOLD_MS 50
Bounce powerDebounce = Bounce();
SYSTEM_STATE powerState = OFF;


#define FIRE_DS_BUTTON 3


//LED Pins
#define RED_SABER_PIN 4
#define BLUE_SABER_PIN 5

//Relay Pins
#define MAIN_POWER_RELAY 11
#define MARQUEE_RELAY 12
#define BUTTON_LED_RELAY 13

//LEDs
#define SABER_LENGTH 50
CRGB redLightsaber[SABER_LENGTH];
CRGB blueLightsaber[SABER_LENGTH];
int saberIndex = 0;


//Timing Definitions
#define LOOP_WAIT_MS 10
#define FIRE_DS_HOLD_MS 50



Bounce fireDSDebounce = Bounce();
SoundFx soundPlayer = SoundFx();



void setup() {
  #ifdef DEBUG
    Serial.begin(19200);
  #endif
  DEBUG_PRINT("Welcome To StarCade!!!");

  soundPlayer.init();

  //Setup power button
  pinMode(POWER_BUTTON,INPUT_PULLUP);
  powerDebounce.attach(POWER_BUTTON);
  powerDebounce.interval(POWER_HOLD_MS);

  //Setup fire Death Star button
  pinMode(FIRE_DS_BUTTON,INPUT_PULLUP);
  fireDSDebounce.attach(FIRE_DS_BUTTON);
  fireDSDebounce.interval(FIRE_DS_HOLD_MS);

  //Setup LEDs
  FastLED.addLeds<NEOPIXEL, RED_SABER_PIN>(redLightsaber, SABER_LENGTH);
  FastLED.addLeds<NEOPIXEL, BLUE_SABER_PIN>(blueLightsaber, SABER_LENGTH);


  //Relays
  pinMode(MAIN_POWER_RELAY,OUTPUT);
  pinMode(MARQUEE_RELAY,OUTPUT);
  pinMode(BUTTON_LED_RELAY,OUTPUT);
  digitalWrite(MAIN_POWER_RELAY,LOW);
  digitalWrite(MARQUEE_RELAY,LOW);
  digitalWrite(BUTTON_LED_RELAY,LOW);

}


void loop() {
  processPowerButton();
  processFireDS();
  soundPlayer.update();
  delay(LOOP_WAIT_MS);
}




void processPowerButton(){
  bool lightSabersDone = false;
  powerDebounce.update(); 
  switch(powerState){
    case OFF:
      if(powerDebounce.fell()){
        DEBUG_PRINT("Powering on...");
        digitalWrite(MAIN_POWER_RELAY,HIGH);
        digitalWrite(MARQUEE_RELAY,HIGH);
        digitalWrite(BUTTON_LED_RELAY,HIGH);
        soundPlayer.playSound(SoundFx::SABER_ON);
        saberIndex = 0;
        powerState = POWERING_ON;
      }
      break;
    case POWERING_ON:
      lightSabersDone = processLightsaberOn();
      FastLED.show();
      if(lightSabersDone){
        DEBUG_PRINT("System ON");
        powerState = ON;
      }
      break;
    case ON:
      if(powerDebounce.fell()){
        DEBUG_PRINT("Powering off...");
        digitalWrite(MAIN_POWER_RELAY,LOW);
        digitalWrite(MARQUEE_RELAY,LOW);
        digitalWrite(BUTTON_LED_RELAY,LOW);
        soundPlayer.playSound(SoundFx::SABER_OFF);
        saberIndex = SABER_LENGTH-1;
        powerState = POWERING_OFF;
      }
      break;
    case POWERING_OFF:
      lightSabersDone = processLightsaberOff();
      FastLED.show();
      if(lightSabersDone){
        DEBUG_PRINT("System OFF");
        powerState = OFF;
      }
      break;
  }
}


bool processLightsaberOn(){
 if(saberIndex < SABER_LENGTH){
    redLightsaber[saberIndex] = CRGB::Red;
    blueLightsaber[saberIndex] = CRGB::Blue;
    saberIndex++;
    return false;
  }
  return true;
}





bool processLightsaberOff(){
 if(saberIndex >= 0){
    redLightsaber[saberIndex] = CRGB::Black;
    blueLightsaber[saberIndex] = CRGB::Black;
    saberIndex--;
    return false;
  }
  return true;
}


void processFireDS(){
  fireDSDebounce.update();
  if(fireDSDebounce.fell()){
    DEBUG_PRINT("Fire Death Star!!!!"); 
    soundPlayer.playSound(SoundFx::FIRE_DS);
  }
}


