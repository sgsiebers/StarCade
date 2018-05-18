//Core Libraries
#include <stdint.h>
#include <Bounce2.h>
#include "SimpleAnimation.h"



//Debug mode
#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println (x)
#else
 #define DEBUG_PRINT(x)
#endif


//Pins
//Reserve pins 0 & 1 for serial debug
#define POWER_BUTTON 2
#define FIRE_DS_BUTTON 3
#define LEFT_LIGHTSABER_PIN 4
#define RIGHT_LIGHTSABER_PIN 5
#define BACKLIGHT_PIN 6
#define DEATHSTAR_BEAM_PIN 7
// Sound Pins 8-10
#define MAIN_POWER_RELAY_PIN 11
#define MARQUEE_RELAY_PIN 12
#define BUTTON_LED_RELAY_PIN 13


//Power On/Off
enum SYSTEM_STATE {OFF=0, POWERING_ON=1, ON=2, POWERING_OFF=3, FIRING_DS = 4};
#define POWER_HOLD_MS 50
Bounce powerDebounce = Bounce();
SYSTEM_STATE systemState = OFF;


//LightSaber LEDs
#define SABER_LENGTH 50
CRGB leftSaberLEDs[SABER_LENGTH];
CRGB rightSaberLEDs[SABER_LENGTH];
LedStrip leftLightSaber = {leftSaberLEDs,SABER_LENGTH};
LedStrip rightLightSaber = {rightSaberLEDs,SABER_LENGTH};
FillCommand fillLeftSaberRed = FillCommand();
FillCommand fillRightSaberBlue = FillCommand();
FillCommand fillLeftSaberBlack = FillCommand();
FillCommand fillRightSaberBlack = FillCommand();
ParallelCommand<3> powerOnCommand = ParallelCommand<3>();
ParallelCommand<3> powerOffCommand = ParallelCommand<3>();


//Sounds
SoundCommand lightSaberOnSound = SoundCommand(8,2500);
SoundCommand lightSaberOffSound = SoundCommand(9,1500);
SoundCommand fireDeathStarSound = SoundCommand(10,22000);


//Timing Definitions
#define LOOP_WAIT_MS 1
#define FIRE_DS_HOLD_MS 50
#define SABER_FILL_RATE 20



Bounce fireDSDebounce = Bounce();



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


  //Relays
  pinMode(MAIN_POWER_RELAY_PIN,OUTPUT);
  pinMode(MARQUEE_RELAY_PIN,OUTPUT);
  pinMode(BUTTON_LED_RELAY_PIN,OUTPUT);
  digitalWrite(MAIN_POWER_RELAY_PIN,HIGH);
  digitalWrite(MARQUEE_RELAY_PIN,HIGH);
  digitalWrite(BUTTON_LED_RELAY_PIN,HIGH);

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
}


void processPowerButton(){
  powerDebounce.update(); 
  switch(systemState){
    case OFF:
      if(powerDebounce.fell()){
        DEBUG_PRINT("Powering on...");
        digitalWrite(MAIN_POWER_RELAY_PIN,LOW);
        digitalWrite(MARQUEE_RELAY_PIN,LOW);
        digitalWrite(BUTTON_LED_RELAY_PIN,LOW);
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
        digitalWrite(MARQUEE_RELAY_PIN,HIGH);
        digitalWrite(BUTTON_LED_RELAY_PIN,HIGH);
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
    fireDeathStarSound.begin();
    systemState = FIRING_DS;
  }else if(systemState == FIRING_DS){
    fireDeathStarSound.update();
    if(fireDeathStarSound.isDone()){
      DEBUG_PRINT("Alderaan has been destroyed!"); 
      systemState = ON;
    }
  }
}


