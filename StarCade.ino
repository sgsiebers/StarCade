//Core Libraries
#include <stdint.h>
#include <Bounce2.h>
#include "SimpleAnimation.h"





////////////////////////////
//  PIN Definitions 
////////////////////////////

// NOTE: Pins 0 & 1 inactive in DEBUG mode for serial diagnostics
#define MARQUEE_RELAY_PIN 0
#define BUTTON_LED_RELAY_PIN 1
#define MAIN_POWER_RELAY_PIN 2
#define POWER_BUTTON 3
#define FIRE_DS_BUTTON 4
#define LEFT_LIGHTSABER_PIN 5
#define RIGHT_LIGHTSABER_PIN 6
#define LEFT_BACKLIGHT_PIN 7
#define RIGHT_BACKLIGHT_PIN 8
#define POWER_ON_SOUND_PIN 9
#define POWER_OFF_SOUND_PIN 10
#define FIRE_DS_SOUND_PIN 11
#define DEATHSTAR_BEAM_PIN 12
#define DEATHSTAR_RING_PIN 13



//Timing Definitions
#define LOOP_WAIT_MS 1
#define FIRE_DS_HOLD_MS 50
#define POWER_HOLD_MS 50
#define SABER_FILL_RATE 20


////////////////////////////
//  Misc. State and Input
////////////////////////////


enum SYSTEM_STATE {OFF=0, POWERING_ON=1, ON=2, POWERING_OFF=3, FIRING_DS = 4};
SYSTEM_STATE systemState = OFF;

Bounce powerDebounce = Bounce();
Bounce fireDSDebounce = Bounce();



////////////////////////////
//  LED Definitions 
////////////////////////////

// LightSaber LEDs
#define SABER_LENGTH 50
CRGB leftSaberLEDs[SABER_LENGTH] = {CRGB::Black};
CRGB rightSaberLEDs[SABER_LENGTH] = {CRGB::Black};

// Ambient Backlight LEDs
#define BACKLIGHT_LENGTH 20
CRGB leftBacklightLEDs[BACKLIGHT_LENGTH] = {CRGB::Black};
CRGB rightBacklightLEDs[BACKLIGHT_LENGTH] = {CRGB::Black};

// Death Star LEDs
#define DS_BEAM_LENGTH 60
#define DS_RING_LENGTH 12
CRGB deathStarLEDs[DS_BEAM_LENGTH] = {CRGB::Black};
CRGB deathStarRingLEDs[DS_RING_LENGTH] = {CRGB::Black};



////////////////////////////
//  Command Definitions 
////////////////////////////

//Sounds
SoundCommand lightSaberOnSound = SoundCommand(POWER_ON_SOUND_PIN,2500);
SoundCommand lightSaberOffSound = SoundCommand(POWER_OFF_SOUND_PIN,1500);
SoundCommand fireDeathStarSound = SoundCommand(FIRE_DS_SOUND_PIN,22000);


// Power Commands
FillCommand fillLeftSaberRed = FillCommand(leftSaberLEDs,SABER_LENGTH,CRGB::Red,SABER_FILL_RATE,FillCommand::Direction::FORWARD);
FillCommand fillRightSaberBlue = FillCommand(rightSaberLEDs,SABER_LENGTH,CRGB::Blue,SABER_FILL_RATE,FillCommand::Direction::FORWARD);
FillCommand fillLeftSaberBlack = FillCommand(leftSaberLEDs,SABER_LENGTH,CRGB::Black,SABER_FILL_RATE,FillCommand::Direction::REVERSE);
FillCommand fillRightSaberBlack = FillCommand(rightSaberLEDs,SABER_LENGTH,CRGB::Black,SABER_FILL_RATE,FillCommand::Direction::REVERSE);
Command* powerOnCommmands[3] = {&fillLeftSaberRed,&fillRightSaberBlue,&lightSaberOnSound};
Command* powerOffCommands[3] = {&fillLeftSaberBlack,&fillRightSaberBlack,&lightSaberOffSound};
ParallelCommand powerOnCommand = ParallelCommand(powerOnCommmands,3);
ParallelCommand powerOffCommand = ParallelCommand(powerOffCommands,3);


// Fire DS Commands
FadeCommand fadeLeftSaberOut = FadeCommand(leftSaberLEDs,SABER_LENGTH,CRGB::Red, 5000, FadeCommand::Direction::OUT);
FadeCommand fadeRightSaberOut = FadeCommand(rightSaberLEDs,SABER_LENGTH,CRGB::Blue, 5000, FadeCommand::Direction::OUT);
FadeCommand fadeLeftSaberIn = FadeCommand(leftSaberLEDs,SABER_LENGTH,CRGB::Red, 3000, FadeCommand::Direction::IN);
FadeCommand fadeRightSaberIn = FadeCommand(rightSaberLEDs,SABER_LENGTH,CRGB::Blue, 3000, FadeCommand::Direction::IN);
DelayCommand delay4seconds = DelayCommand(4000);
DelayCommand delay10seconds = DelayCommand(10000);
Command* fadeSabersOutCmds[2] = {&fadeLeftSaberOut,&fadeRightSaberOut};
ParallelCommand fadeSabersOut = ParallelCommand(fadeSabersOutCmds,2);
Command* fadeSabersInCmds[2] = {&fadeLeftSaberIn,&fadeRightSaberIn};
ParallelCommand fadeSabersIn = ParallelCommand(fadeSabersInCmds,2);
Command* fadeInOutCmds[4] = {&delay4seconds,&fadeSabersOut,&delay10seconds,&fadeSabersIn};
CommandSequence fadeInOut = CommandSequence(fadeInOutCmds,4);
Command* fireDSCommands[2] = {&fireDeathStarSound,&fadeInOut};
ParallelCommand fireDSCommand = ParallelCommand(fireDSCommands,2);
  







void setup() {
  #ifdef DEBUG
    Serial.begin(19200);
  #endif
  DEBUG_PRINT("Welcome To StarCade!!!");

  for(uint8_t pin = 8; pin <11; pin++){
    pinMode(pin,OUTPUT);
    digitalWrite(pin,HIGH);  
  }

  //Setup power button
  pinMode(POWER_BUTTON,INPUT_PULLUP);
  powerDebounce.attach(POWER_BUTTON);
  powerDebounce.interval(POWER_HOLD_MS);

  //Setup fire Death Star button
  pinMode(FIRE_DS_BUTTON,INPUT_PULLUP);
  fireDSDebounce.attach(FIRE_DS_BUTTON);
  fireDSDebounce.interval(FIRE_DS_HOLD_MS);

  //Setup LEDs
  FastLED.addLeds<NEOPIXEL, LEFT_LIGHTSABER_PIN>(leftSaberLEDs, SABER_LENGTH);
  FastLED.addLeds<NEOPIXEL, RIGHT_LIGHTSABER_PIN>(rightSaberLEDs, SABER_LENGTH);
//  FastLED.addLeds<NEOPIXEL, LEFT_BACKLIGHT_PIN>(leftBacklightLEDs, BACKLIGHT_LENGTH);
//  FastLED.addLeds<NEOPIXEL, RIGHT_BACKLIGHT_PIN>(rightBacklightLEDs, BACKLIGHT_LENGTH);
//  FastLED.addLeds<NEOPIXEL, DEATHSTAR_BEAM_PIN>(deathStarLEDs, DS_BEAM_LENGTH);
//  FastLED.addLeds<NEOPIXEL, DEATHSTAR_RING_PIN>(deathStarRingLEDs, DS_RING_LENGTH);


  //Relays
  pinMode(MAIN_POWER_RELAY_PIN,OUTPUT);
  digitalWrite(MAIN_POWER_RELAY_PIN,HIGH);
#ifndef DEBUG
  pinMode(MARQUEE_RELAY_PIN,OUTPUT);
  pinMode(BUTTON_LED_RELAY_PIN,OUTPUT);
  digitalWrite(MARQUEE_RELAY_PIN,HIGH);
  digitalWrite(BUTTON_LED_RELAY_PIN,HIGH);
#endif


}


void loop() {
  processPowerButton();
  processFireDS();
  delay(LOOP_WAIT_MS);
}






void processPowerButton(){
  powerDebounce.update(); 
  switch(systemState){
    case OFF:
      if(powerDebounce.fell()){
        DEBUG_PRINT("Powering on...");
        digitalWrite(MAIN_POWER_RELAY_PIN,LOW);
#ifndef DEBUG        
        digitalWrite(MARQUEE_RELAY_PIN,LOW);
        digitalWrite(BUTTON_LED_RELAY_PIN,LOW);
#endif        
        powerOnCommand.begin();
        systemState = POWERING_ON;
      }
      break;
    case POWERING_ON:
      powerOnCommand.update();
      FastLED.show();
      if(powerOnCommand.isDone()){
        DEBUG_PRINT("System ON");
        systemState = ON;
      }
      break;
    case ON:
      if(powerDebounce.fell()){
        DEBUG_PRINT("Powering off...");
        digitalWrite(MAIN_POWER_RELAY_PIN,HIGH);
#ifndef DEBUG        
        digitalWrite(MARQUEE_RELAY_PIN,HIGH);
        digitalWrite(BUTTON_LED_RELAY_PIN,HIGH);
#endif        
        powerOffCommand.begin();
        systemState = POWERING_OFF;
      }
      break;
    case POWERING_OFF:
      powerOffCommand.update();
      FastLED.show();
      if(powerOffCommand.isDone()){
        DEBUG_PRINT("System OFF");
        systemState = OFF;
      }
    default:
      break;
  }
}


void processFireDS(){
  fireDSDebounce.update();
  if(systemState == ON && fireDSDebounce.fell()){
    DEBUG_PRINT("Fire Death Star!!!!"); 
    fireDSCommand.begin();
    systemState = FIRING_DS;
  }else if(systemState == FIRING_DS){
    fireDSCommand.update();
    FastLED.show();
    if(fireDSCommand.isDone()){
      DEBUG_PRINT("Alderaan has been destroyed!"); 
      systemState = ON;
    }
  }
}


