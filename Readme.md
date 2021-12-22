# This is for demonstration purposes only and is used on my layout

This sketch uses a MERG kit 50 block detector to sense the presence of a locomotive on the stretch of track with the level crossing on it. 

Once it is triggered the bells will sound and the 4 leds will flash. A few seconds later the 2 servos will lower the barrier arms.The sound only plays for a few seconds. once the train leaves the stretch of track the arms will raise and then the sound and leds will stop. Awaiting the next trigger.

The code is a adaptaion of https://rudysarduinoprojects.wordpress.com/2019/03/10/fun-with-arduino-21-railway-crossing-uk/

The code runs on a arduino Nano

The leds are set as 2 banks left bank pins 6 and 12 and right bank pins 7 and 13 blink speed is set to 400 milli seconds

The servos are on pins 4 and 5 and are set to0 deg and 100 degs and servo speed is 35

The Meg block detector is on pin 8

The dfrobot mp3 player is on pins 10 and 11 the sound file is on a sdcard and is taken from jmri resourse folder 

The rest of the code uses switch state to move between the logical steps
