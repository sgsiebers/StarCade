# StarCade

I was looking for a fun father/son project that I could work with my bright, tech-minded 8 year old when I stumbled upon the work of Bob @ _I Like to Make Stuff_.  His impressive series on [How to Make An Arcade Cabinet](https://www.iliketomakestuff.com/how-to-make-an-arcade-cabinet-part-1/) inspired me to take on my own version of this classic 80's throwback.  So, fresh off of binge watching the entire Star Wars(TM) franchise with my young apprentice, our theme was immediately obvious: **StarCade** - a space themed battle station complete with lighted marquee, animated LED-powered lightsabers, LED ambient backlighting, specialized sound effects, and even a working Death Star that you can fire on-demand!  (Ok, so we went a little overboard with the special effects, but it's hard to contain an 8-year-old's imagination!)

StarCade is powered by a Raspberry Pi running the fantastic [RetroPie Emulator](https://retropie.org.uk/) for core gaming functions, and an Arduino Mega to run all the special effects.  Eventually I may get around to posting our basic plans for this build, but for now I'm at least sharing the source code for anyone who may want learn from our work.  This repository contains the main business logic, and I've also created a lightweight [SimpleAnimation](https://github.com/sgsiebers/SimpleAnimation) library to model the timed sequences of commands that run all the special effects.

This is still very much a work in progress.  Hopefully you won't have to wait years to see the final result!  For now, the 3D model we built will have to serve as a placeholder for the real thing.

<img src="StarcadeModel.png" width=50% height=50% align="middle">


## 6/12/2018:
Working Lightsabers!  These fun accents are made from [NeoPixel strips](https://www.amazon.com/ALITOVE-Individually-Addressable-Flexible-Waterproof/dp/B00VQ0D2TY) threaded trough a frosted acrylic tube and animated using my [SimpleAnimation](https://github.com/sgsiebers/SimpleAnimation) library.  Sound effects are coordinated by the Arduino triggering pins on an [Adafruit Mini Sound Board](https://www.amazon.com/Adafruit-Audio-Mini-Sound-Board/dp/B01GQFVV2I).  I also wired up a simple passive audio mixer so the Raspberry Pi and Sound Fx board could both drive the same speaker output.  Here's a great [example](https://www.instructables.com/id/Altoids-Tin-18-Stereo-Mixer/) of how you can make one yourself, although mine is much more crude.  I simply hacked the ends off some old Aux cables, soldered the resistors, taped it up, and stuffed it into a random plastic container.

It's pretty cool to see them in action, but for the complete experience with sound [watch this!](https://youtu.be/m8hfb_nRrew)

<img src="misc/Lightsabers.gif">


## 11/1/2018:

Here's a high-level wiring diagram for all the electronics:

<img src="misc/WiringDiagram.png" width=100% height=100%>


## 11/29/2018:
My young apprentice has been busy helping me code a fairly complex LED animation sequence that will play when you "fire the Death Star" from an auxiliary button.  Check out our [bench test video!](https://youtu.be/5v-LOUCcdTg).  Also, here's a quick shot playing with the RetroPie emulator:
<img src="misc/OwenTesting.jpg">

## 1/4/2019:
We prototyped the control deck this week, wiring up the game buttons and joystick to an [I-PAC2](https://www.ultimarc.com/control-interfaces/i-pacs/i-pac2/) USB button encoder.  My daughter has decided she wants to join in the fun, so here's a shot of her taking the controls for a test drive:

<img src="misc/VioletTestingControls.jpg">

