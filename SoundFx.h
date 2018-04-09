#ifndef SOUND_FX
#define SOUND_FX

#include <Arduino.h>

#define SOUND_PIN_HOLD_TIME 125

class SoundFx{

  public:

    enum Sound {SABER_ON = 6, SABER_OFF=7, FIRE_DS=8};
    
    // Constructor
    SoundFx();

    // Initialize 
    void init();

    // Process each time through loop()
    void update();

    // Play the given sound
    void playSound(Sound s);

  private:
    bool isPlaying;
    unsigned long startPlayTime;
    Sound currentSound;
  
};

#endif
