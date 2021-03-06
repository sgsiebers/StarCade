//Core Libraries
#include <stdint.h>
#include <Bounce2.h>
#include "SimpleAnimation.h"





////////////////////////////
//  PIN Definitions 
////////////////////////////

#define MARQUEE_RELAY_PIN 45
#define BUTTON_LED_RELAY_PIN 47
#define MAIN_POWER_RELAY_PIN 43
#define POWER_BUTTON 12
#define FIRE_DS_BUTTON 13
#define LEFT_LIGHTSABER_PIN 37
#define RIGHT_LIGHTSABER_PIN 34
#define LEFT_BACKLIGHT_PIN 35
#define RIGHT_BACKLIGHT_PIN 36
#define POWER_ON_SOUND_PIN 23
#define POWER_OFF_SOUND_PIN 25
#define FIRE_DS_SOUND_PIN 27
#define DEATHSTAR_BEAM_PIN 16
#define DEATHSTAR_RING_PIN 17



//Timing Definitions
#define LOOP_WAIT_MS 1
#define FIRE_DS_HOLD_MS 50
#define POWER_HOLD_ON_MS 50
#define POWER_HOLD_OFF_MS 3500
#define SABER_FILL_RATE 20
#define BACKLIGHT_FADE_MS 2000


////////////////////////////
//  Misc. State and Input
////////////////////////////


enum SYSTEM_STATE {OFF=0, POWERING_ON=1, ON=2, POWERING_OFF=3, FIRING_DS = 4};
SYSTEM_STATE systemState = OFF;

Bounce powerOnDebounce = Bounce();
Bounce powerOffDebounce = Bounce();
Bounce fireDSDebounce = Bounce();



////////////////////////////
//  LED Definitions 
////////////////////////////

// LightSaber LEDs
#define SABER_LENGTH 50
CRGB leftSaberLEDs[SABER_LENGTH] = {CRGB::Black};
CRGB rightSaberLEDs[SABER_LENGTH] = {CRGB::Black};

// Ambient Backlight LEDs
#define BACKLIGHT_LENGTH 90
CRGB leftBacklightLEDs[BACKLIGHT_LENGTH] = {CRGB::Black};
CRGB rightBacklightLEDs[BACKLIGHT_LENGTH] = {CRGB::Black};

// Death Star LEDs
#define DS_BEAM_LENGTH 70
#define DS_RING_LENGTH 12
CRGB deathStarBeamLEDs[DS_BEAM_LENGTH] = {CRGB::Black};
CRGB deathStarRingLEDs[DS_RING_LENGTH] = {CRGB::Black};



////////////////////////////
//  Command Definitions 
////////////////////////////

//Sounds
SoundCommand lightSaberOnSound = SoundCommand(POWER_ON_SOUND_PIN,2500);
SoundCommand lightSaberOffSound = SoundCommand(POWER_OFF_SOUND_PIN,1500);
SoundCommand fireDeathStarSound = SoundCommand(FIRE_DS_SOUND_PIN,22000);


// Power On Sequence
FillCommand fillLeftSaberRed = FillCommand(leftSaberLEDs,SABER_LENGTH,CRGB::Red,SABER_FILL_RATE,FillCommand::Direction::FORWARD);
FillCommand fillRightSaberBlue = FillCommand(rightSaberLEDs,SABER_LENGTH,CRGB::Blue,SABER_FILL_RATE,FillCommand::Direction::FORWARD);
FadeCommand fadeLeftBackRed = FadeCommand(leftBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Red,BACKLIGHT_FADE_MS,FadeCommand::Direction::IN);
FadeCommand fadeRightBackBlue = FadeCommand(rightBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Blue,BACKLIGHT_FADE_MS,FadeCommand::Direction::IN);
Command* powerOnCommmands[5] = {&fillLeftSaberRed,&fillRightSaberBlue,&fadeLeftBackRed,&fadeRightBackBlue,&lightSaberOnSound};
ParallelCommand powerOnCommand = ParallelCommand(powerOnCommmands,5);

//Power Off Sequence
FillCommand fillLeftSaberBlack = FillCommand(leftSaberLEDs,SABER_LENGTH,CRGB::Black,SABER_FILL_RATE,FillCommand::Direction::REVERSE);
FillCommand fillRightSaberBlack = FillCommand(rightSaberLEDs,SABER_LENGTH,CRGB::Black,SABER_FILL_RATE,FillCommand::Direction::REVERSE);
FadeCommand fadeLeftBackBlack = FadeCommand(leftBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Red,BACKLIGHT_FADE_MS,FadeCommand::Direction::OUT);
FadeCommand fadeRightBackBlack = FadeCommand(rightBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Blue,BACKLIGHT_FADE_MS,FadeCommand::Direction::OUT);
Command* powerOffCommands[5] = {&fillLeftSaberBlack,&fillRightSaberBlack,&fadeLeftBackBlack,&fadeRightBackBlack,&lightSaberOffSound};
ParallelCommand powerOffCommand = ParallelCommand(powerOffCommands,5);


// Fire DS Commands
//Lightsabers
// |<----- 5s ----->|<-- Fade Out 2s -->|<------ 6s ------>|<-- Flash 2s -->|<- Fade In 1s ->|<---- 4s ---->|<-- Fade Out 2s -->|<-- 2s -->|<--- Fade In 3s --->|
DelayCommand delay5secondsLS = DelayCommand(5000);
FadeCommand fadeLeftSaberOut = FadeCommand(leftSaberLEDs,SABER_LENGTH,CRGB::Red, 2000, FadeCommand::Direction::OUT);
FadeCommand fadeRightSaberOut = FadeCommand(rightSaberLEDs,SABER_LENGTH,CRGB::Blue, 2000, FadeCommand::Direction::OUT);
Command* fadeSabersOutCmds[2] = {&fadeLeftSaberOut,&fadeRightSaberOut};
ParallelCommand fadeSabersOut = ParallelCommand(fadeSabersOutCmds,2);
DelayCommand delay6secondsLS = DelayCommand(6000);
FlashCommand leftSaberFlash = FlashCommand(leftSaberLEDs, SABER_LENGTH, CRGB::Red, 2000, 8);
FlashCommand rightSaberFlash = FlashCommand(rightSaberLEDs, SABER_LENGTH, CRGB::Blue, 2000, 8);
FadeCommand fadeLeftSaberWhite = FadeCommand(leftSaberLEDs, SABER_LENGTH, CRGB::White, 1000, FadeCommand::Direction::IN);
FadeCommand fadeRightSaberWhite = FadeCommand(rightSaberLEDs, SABER_LENGTH, CRGB::White, 1000, FadeCommand::Direction::IN);
DelayCommand delay4secondsLLS = DelayCommand(4000);
DelayCommand delay4secondsRLS = DelayCommand(4000);
FadeCommand fadeLeftSaberBlack = FadeCommand(leftSaberLEDs, SABER_LENGTH, CRGB::White, 2000, FadeCommand::Direction::OUT);
FadeCommand fadeRightSaberBlack = FadeCommand(rightSaberLEDs, SABER_LENGTH, CRGB::White, 2000, FadeCommand::Direction::OUT);
DelayCommand delayLLS2seconds = DelayCommand(2000);
DelayCommand delayRLS2seconds = DelayCommand(2000);
FadeCommand fadeLeftSaberIn = FadeCommand(leftSaberLEDs,SABER_LENGTH,CRGB::Red, 3000, FadeCommand::Direction::IN);
FadeCommand fadeRightSaberIn = FadeCommand(rightSaberLEDs,SABER_LENGTH,CRGB::Blue, 3000, FadeCommand::Direction::IN);
Command* llsExplosionCmds[6] = {&leftSaberFlash,&fadeLeftSaberWhite,&delay4secondsLLS,&fadeLeftSaberBlack, &delayLLS2seconds, &fadeLeftSaberIn};
CommandSequence llsExplosionSeq = CommandSequence(llsExplosionCmds,6);
Command* rlsExplosionCmds[6] = {&rightSaberFlash,&fadeRightSaberWhite,&delay4secondsRLS,&fadeRightSaberBlack, &delayRLS2seconds, &fadeRightSaberIn,};
CommandSequence rlsExplosionSeq = CommandSequence(rlsExplosionCmds,6);
Command* explosionCmds[2] = {&llsExplosionSeq,&rlsExplosionSeq};
ParallelCommand explosionSeq = ParallelCommand(explosionCmds,2);
Command* fireDSSaberCmds[4] = {&delay5secondsLS, &fadeSabersOut, &delay6secondsLS, &explosionSeq};
CommandSequence fireDSSaberSeq = CommandSequence(fireDSSaberCmds, 4);



//Backlights
// |<-- 2s -->|<--- Flash 3s --->|<-- Fade Out 2s -->|<------ 6s ------>|<-- Flash 2s -->|<- Fade In 1s ->|<---- 4s ---->|<-- Fade Out 2s -->|<-- 2s -->|<-- Fade In 2s-->|
DelayCommand delayLBL2seconds = DelayCommand(2000);
DelayCommand delayRBL2seconds = DelayCommand(2000);
FlashCommand flashLeftBack1 = FlashCommand(leftBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Red, 3000, 9);
FlashCommand flashRightBack1 = FlashCommand(rightBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Blue, 3000, 9);
FadeCommand fadeLBLOut = FadeCommand(leftBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Red, 2000, FadeCommand::Direction::OUT);
FadeCommand fadeRBLOut = FadeCommand(rightBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Blue, 2000, FadeCommand::Direction::OUT);
DelayCommand delayLBL6seconds = DelayCommand(6000);
DelayCommand delayRBL6seconds = DelayCommand(6000);
FlashCommand flashLeftBack2 = FlashCommand(leftBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Red, 2000, 8);
FlashCommand flashRightBack2 = FlashCommand(rightBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Blue, 2000, 8);
FadeCommand fadeLBLWhite = FadeCommand(leftBacklightLEDs,BACKLIGHT_LENGTH,CRGB::White, 1000, FadeCommand::Direction::IN);
FadeCommand fadeRBLWhite = FadeCommand(rightBacklightLEDs,BACKLIGHT_LENGTH,CRGB::White, 1000, FadeCommand::Direction::IN);
DelayCommand LBLdelay4seconds = DelayCommand(4000);
DelayCommand RBLdelay4seconds = DelayCommand(4000);
FadeCommand fadeLBLBlack = FadeCommand(leftBacklightLEDs,BACKLIGHT_LENGTH,CRGB::White, 2000, FadeCommand::Direction::OUT);
FadeCommand fadeRBLBlack = FadeCommand(rightBacklightLEDs,BACKLIGHT_LENGTH,CRGB::White, 2000, FadeCommand::Direction::OUT);
DelayCommand lblHoldBlack = DelayCommand(2000);
DelayCommand rblHoldBlack = DelayCommand(2000);
FadeCommand fadeLBLIn = FadeCommand(leftBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Red, 2000, FadeCommand::Direction::IN);
FadeCommand fadeRBLIn = FadeCommand(rightBacklightLEDs,BACKLIGHT_LENGTH,CRGB::Blue, 2000, FadeCommand::Direction::IN);
Command* lblDSCmds[10] = {&delayLBL2seconds, &flashLeftBack1, &fadeLBLOut, &delayLBL6seconds, &flashLeftBack2, &fadeLBLWhite, &LBLdelay4seconds, &fadeLBLBlack, &lblHoldBlack, &fadeLBLIn}; 
CommandSequence lblDSSeq = CommandSequence(lblDSCmds, 10);
Command* rblDSCmds[10] = {&delayRBL2seconds, &flashRightBack1, &fadeRBLOut, &delayRBL6seconds, &flashRightBack2, &fadeRBLWhite, &RBLdelay4seconds, &fadeRBLBlack, &rblHoldBlack, &fadeRBLIn}; 
CommandSequence rblDSSeq = CommandSequence(rblDSCmds, 10);


//Marquee
// |<-----5s----->|< Off >|<--------8s-------->|<--Flash 2s-->|< On >|
PinCommand turnOffM = PinCommand(MARQUEE_RELAY_PIN, HIGH, 5000);
DelayCommand marqueeDelay8s = DelayCommand(8000);
PinCommand mOnForFlash = PinCommand(MARQUEE_RELAY_PIN, LOW, 250);
PinCommand mOffForFlash = PinCommand(MARQUEE_RELAY_PIN, HIGH, 250);
Command* marqueeFlashCmds [2] = {&mOnForFlash , &mOffForFlash};
CommandSequence marqueeFlashSeq = CommandSequence(marqueeFlashCmds, 2);
LoopCommand marqueeFlashLoop = LoopCommand(4,&marqueeFlashSeq);
PinCommand turnOnM =PinCommand(MARQUEE_RELAY_PIN, LOW, 0);
Command* marqueeCmds [4] = {&turnOffM, &marqueeDelay8s, &marqueeFlashLoop, &turnOnM};
CommandSequence marqueeSeq = CommandSequence(marqueeCmds, 4);



// Death Star
// |<----- 5s ----->|<-- Fill 2s -->|<--- Fade Out 3s --->|<-- Flash 2s -->|<- Fill 1s ->|<------- 7s ------->|<-- Fade Out 2s -->|
DelayCommand delayDS5Seconds = DelayCommand(5000);
FillCommand fillDSRing = FillCommand(deathStarRingLEDs, DS_RING_LENGTH, CRGB::Green, 2000/DS_RING_LENGTH, FillCommand::Direction::FORWARD);
FadeCommand fadeDSRing = FadeCommand(deathStarRingLEDs, DS_RING_LENGTH, CRGB::Green, 3000, FadeCommand::Direction::OUT);
FlashCommand flashDSRing = FlashCommand(deathStarRingLEDs, DS_RING_LENGTH, CRGB::Green, 2000, 8);
FillCommand fillDSBeam = FillCommand(deathStarBeamLEDs, DS_BEAM_LENGTH, CRGB::Green, 1000/DS_BEAM_LENGTH, FillCommand::Direction::FORWARD);
DelayCommand delayDS7seconds = DelayCommand(7000);
FadeCommand resetDSBeam = FadeCommand(deathStarBeamLEDs, DS_BEAM_LENGTH, CRGB::Green, 2000, FadeCommand::Direction::OUT);
FadeCommand resetDSRing = FadeCommand(deathStarRingLEDs, DS_RING_LENGTH, CRGB::Green, 2000, FadeCommand::Direction::OUT);
Command* resetDSCmds[2] = {&resetDSBeam, &resetDSRing};
ParallelCommand resetDS = ParallelCommand(resetDSCmds, 2);
Command* fireDSRingBeamCmds[7] = {&delayDS5Seconds, &fillDSRing, &fadeDSRing, &flashDSRing, &fillDSBeam, &delayDS7seconds, &resetDS};
CommandSequence fireDSRingBeamSeq = CommandSequence(fireDSRingBeamCmds, 7);


Command* fireDSCommands[6] = {&fireDeathStarSound,&fireDSSaberSeq, &lblDSSeq, &rblDSSeq, &fireDSRingBeamSeq, &marqueeSeq};
ParallelCommand fireDSCommand = ParallelCommand(fireDSCommands,6);
  







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
  powerOnDebounce.attach(POWER_BUTTON);
  powerOnDebounce.interval(POWER_HOLD_ON_MS);
  powerOffDebounce.attach(POWER_BUTTON);
  powerOffDebounce.interval(POWER_HOLD_OFF_MS);

  //Setup fire Death Star button
  pinMode(FIRE_DS_BUTTON,INPUT_PULLUP);
  fireDSDebounce.attach(FIRE_DS_BUTTON);
  fireDSDebounce.interval(FIRE_DS_HOLD_MS);

  //Setup LEDs
  FastLED.addLeds<NEOPIXEL, LEFT_LIGHTSABER_PIN>(leftSaberLEDs, SABER_LENGTH);
  FastLED.addLeds<NEOPIXEL, RIGHT_LIGHTSABER_PIN>(rightSaberLEDs, SABER_LENGTH);
  FastLED.addLeds<NEOPIXEL, LEFT_BACKLIGHT_PIN>(leftBacklightLEDs, BACKLIGHT_LENGTH);
  FastLED.addLeds<NEOPIXEL, RIGHT_BACKLIGHT_PIN>(rightBacklightLEDs, BACKLIGHT_LENGTH);
//  FastLED.addLeds<NEOPIXEL, DEATHSTAR_BEAM_PIN>(deathStarLEDs, DS_BEAM_LENGTH);
//  FastLED.addLeds<NEOPIXEL, DEATHSTAR_RING_PIN>(deathStarRingLEDs, DS_RING_LENGTH);


  //Relays
  pinMode(MAIN_POWER_RELAY_PIN,OUTPUT);
  pinMode(MARQUEE_RELAY_PIN,OUTPUT);
  pinMode(BUTTON_LED_RELAY_PIN,OUTPUT);
  digitalWrite(MAIN_POWER_RELAY_PIN,HIGH);
  digitalWrite(MARQUEE_RELAY_PIN,HIGH);
  digitalWrite(BUTTON_LED_RELAY_PIN,HIGH);


}


void loop() {
  processPowerButton();
  processFireDS();
  delay(LOOP_WAIT_MS);
}






void processPowerButton(){
  powerOnDebounce.update();
  powerOffDebounce.update(); 
  switch(systemState){
    case OFF:
      if(powerOnDebounce.fell()){
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
      if(powerOffDebounce.fell()){
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
