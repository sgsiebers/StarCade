//Core Libraries
#include <stdint.h>
#include <Bounce2.h>
#include "SimpleAnimation.h"



//Debug mode
#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println (F(x))
#else
 #define DEBUG_PRINT(x)
#endif



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
#define SABER_LENGTH 55
//CRGB leftSaberLEDs[SABER_LENGTH];
//CRGB rightSaberLEDs[SABER_LENGTH];
LedStrip leftLightSaber = {CRGB[SABER_LENGTH],SABER_LENGTH};//{leftSaberLEDs,SABER_LENGTH};
LedStrip rightLightSaber = {rightSaberLEDs,SABER_LENGTH};

// Ambient Backlight LEDs
#define BACKLIGHT_LENGTH 90
CRGB leftBacklightLEDs[BACKLIGHT_LENGTH];
CRGB rightBacklightLEDs[BACKLIGHT_LENGTH];
LedStrip leftBacklight = {leftBacklightLEDs,BACKLIGHT_LENGTH};
LedStrip rightBacklight = {rightBacklightLEDs,BACKLIGHT_LENGTH};

// Death Star LEDs
#define DS_BEAM_LENGTH 1
#define DS_RING_LENGTH 1
CRGB deathStarLEDs[DS_BEAM_LENGTH];
CRGB deathStarRingLEDs[DS_RING_LENGTH];
LedStrip deathStarBeam = {deathStarLEDs,DS_BEAM_LENGTH};
LedStrip deathStarRing = {deathStarRingLEDs,DS_RING_LENGTH};



////////////////////////////
//  Command Definitions 
////////////////////////////

// Power Commands
FillCommand fillLeftSaberRed = FillCommand();
FillCommand fillRightSaberBlue = FillCommand();
FillCommand fillLeftSaberBlack = FillCommand();
FillCommand fillRightSaberBlack = FillCommand();
ParallelCommand<5> powerOnCommand = ParallelCommand<5>();
ParallelCommand<5> powerOffCommand = ParallelCommand<5>();

// Fire DS Commands
FadeCommand fadeLeftSaberOut = FadeCommand();
FadeCommand fadeRightSaberOut = FadeCommand();
FadeCommand fadeLeftSaberIn = FadeCommand();
FadeCommand fadeRightSaberIn = FadeCommand();
DelayCommand delay4seconds = DelayCommand();
DelayCommand delay10seconds = DelayCommand();
ParallelCommand<2> fireDSCommand = ParallelCommand<2>();
ParallelCommand<2> fadeSabersOut = ParallelCommand<2>();
ParallelCommand<2> fadeSabersIn = ParallelCommand<2>();
CommandSequence<4> fadeInOut = CommandSequence<4>();



//Sounds
SoundCommand lightSaberOnSound = SoundCommand(POWER_ON_SOUND_PIN,2500);
SoundCommand lightSaberOffSound = SoundCommand(POWER_OFF_SOUND_PIN,1500);
SoundCommand fireDeathStarSound = SoundCommand(FIRE_DS_SOUND_PIN,22000);


//Timing Definitions
#define LOOP_WAIT_MS 1
#define FIRE_DS_HOLD_MS 50
#define POWER_HOLD_MS 50
#define SABER_FILL_RATE 20






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
  FastLED.addLeds<NEOPIXEL, LEFT_LIGHTSABER_PIN>(leftLightSaber.leds, SABER_LENGTH);
  FastLED.addLeds<NEOPIXEL, RIGHT_LIGHTSABER_PIN>(rightLightSaber.leds, SABER_LENGTH);
  FastLED.addLeds<NEOPIXEL, LEFT_BACKLIGHT_PIN>(leftBacklight.leds, BACKLIGHT_LENGTH);
  FastLED.addLeds<NEOPIXEL, RIGHT_BACKLIGHT_PIN>(rightBacklight.leds, BACKLIGHT_LENGTH);
  FastLED.addLeds<NEOPIXEL, DEATHSTAR_BEAM_PIN>(deathStarBeam.leds, DS_BEAM_LENGTH);
  FastLED.addLeds<NEOPIXEL, DEATHSTAR_RING_PIN>(deathStarRing.leds, DS_RING_LENGTH);


  //Relays
  pinMode(MAIN_POWER_RELAY_PIN,OUTPUT);
  digitalWrite(MAIN_POWER_RELAY_PIN,HIGH);
#ifndef DEBUG
  pinMode(MARQUEE_RELAY_PIN,OUTPUT);
  pinMode(BUTTON_LED_RELAY_PIN,OUTPUT);
  digitalWrite(MARQUEE_RELAY_PIN,HIGH);
  digitalWrite(BUTTON_LED_RELAY_PIN,HIGH);
#endif

  setupAnimations();


}


void loop() {
  processPowerButton();
  processFireDS();
  delay(LOOP_WAIT_MS);
}



void setupAnimations(){
  // Power On
  fillLeftSaberRed.init(leftLightSaber,CRGB::Red,SABER_FILL_RATE,FillCommand::Direction::FORWARD);
  fillRightSaberBlue.init(rightLightSaber,CRGB::Blue,SABER_FILL_RATE,FillCommand::Direction::FORWARD);
  powerOnCommand.add(&fillLeftSaberRed).add(&fillRightSaberBlue).add(&lightSaberOnSound);
  
  //Power Off
  fillLeftSaberBlack.init(leftLightSaber,CRGB::Black,SABER_FILL_RATE,FillCommand::Direction::REVERSE);
  fillRightSaberBlack.init(rightLightSaber,CRGB::Black,SABER_FILL_RATE,FillCommand::Direction::REVERSE);
  powerOffCommand.add(&fillLeftSaberBlack).add(&fillRightSaberBlack).add(&lightSaberOffSound);

  //Death Star Animation
  delay4seconds.init(4000);
  fadeLeftSaberOut.init(leftLightSaber, CRGB::Red, 5000, FadeCommand::Direction::OUT);
  fadeRightSaberOut.init(rightLightSaber, CRGB::Blue, 5000, FadeCommand::Direction::OUT);
  fadeSabersOut.add(&fadeLeftSaberOut).add(&fadeRightSaberOut);
  delay10seconds.init(10000);
  fadeLeftSaberIn.init(leftLightSaber, CRGB::Red, 3000, FadeCommand::Direction::IN);
  fadeRightSaberIn.init(rightLightSaber, CRGB::Blue, 3000, FadeCommand::Direction::IN);
  fadeSabersIn.add(&fadeLeftSaberIn).add(&fadeRightSaberIn);
  fadeInOut.add(&delay4seconds).add(&fadeSabersOut).add(&delay10seconds).add(&fadeSabersIn);
  fireDSCommand.add(&fireDeathStarSound).add(&fadeInOut);  
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


