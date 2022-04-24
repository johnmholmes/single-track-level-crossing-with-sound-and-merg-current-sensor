/*
 * running on an arduino nano
 * This program uses 1 MERG current sensor to trigger a level crossing while the train is in the block, pin 7
 * 4  red leds are used for the warning signal and are used in pairs, pins 10 + 11 and pins 12 + 13
 * 2 servo's are used to lift the gate up and down
 * there are 6 free io pins left for other uses
 * the code is adaptation of https://rudysarduinoprojects.wordpress.com/2020/09/23/fun-with-arduino-42-railway-crossing-multi-track-two-way/
 */
//Include the software serial library 
#include "SoftwareSerial.h"

//Include the Dfrobot player library you will neded to download it into your Arduino library location to use it.
#include "DFRobotDFPlayerMini.h"

//Include the servo h library which makes controlling the servo much easier
#include <Servo.h>

//setup the software serial port for controlling the mp3 player on pins 10 and 11
SoftwareSerial mySoftwareSerial(10,11);
DFRobotDFPlayerMini myDFPlayer;

//setup the speed for the servos to move
#define GATE_SPEED          35         // [ms] lower number is higher servo speed

//setup the speed for the leds to blink
#define BLINK_SPEED         400        // [ms] smaller number is faster blinking

//setup 2 time delays for the leds before movements
#define GATE_DELAY_CLOSE    2000       // [ms] time between start blinking and gate closing
#define GATE_DELAY_OPEN     4500       // [ms] time between start blinking and gate opening

//setup the 2 servos used required angles 0 deg and 100 deg and give meaninful variable names
#define GATE_OPEN_ANGLE       0         // servo position for open 0 deg
#define GATE_CLOSED_ANGLE    100        // servo position for closed 95  deg

//setup the 2 servos used in the sketch on pins 4 and 5 and give meaninful variable names
#define SERVO_PIN            4         // right gate servo
#define SERVO_PIN1           5         // left gate servo

//setup the 4 leds used in the sketch on pins 6,7,12,13 and give meaninful variable names
#define LED1_PIN             6         // left bank led's
#define LED2_PIN             7         // right bank led's
#define LED3_PIN             12        // left bank led's
#define LED4_PIN             13        // right bank led's

//setup the 1 trigger pin used in the sketch on pin 8 and give meaninful variable name
#define SENSOR1_PIN          8         // MERG current sensor

byte led1, led2, blink_enabled;        // variables led1 = on,  led2 = off, blink_enabled 0 = off 1 = on
byte    angle = GATE_OPEN_ANGLE;       // variable angle = servo angle = 0 deg
byte setpoint = GATE_OPEN_ANGLE;       // variable setpoint = servo angle = 0 deg
byte state = 1;                        // variable sets the state to 1 on startup waiting for sensor to activated
unsigned long time_to_blink;           // variable to hold the millis time to control the led blink
unsigned long time_to_close_gate;      // variable to hold the millis time to control the closing of the gate
unsigned long time_to_open_gate;       // variable to hold the millis time to control the opening of the gate
unsigned long time_for_servo_step;     // variable to hold the millis time to control the slow movement of the servo

byte sound_bell;

Servo gate_servo;                     // declaes the right gate servo for servo.h
Servo gate_servo1;                    // declaes the left gate servo for servo.h


void setup() {
  pinMode(SENSOR1_PIN, INPUT_PULLUP); // setup senesor pin 8 as an input uses the built in resistor
  pinMode(LED1_PIN, OUTPUT);          // setup led pin 6 as an output
  pinMode(LED2_PIN, OUTPUT);          // setup led pin 7 as an output
  pinMode(LED3_PIN, OUTPUT);          // setup led pin 12 as an output
  pinMode(LED4_PIN, OUTPUT);          // setup led pin 13 as an output
  gate_servo.attach(SERVO_PIN);       // setup the right gate servo for pin 4
  gate_servo1.attach(SERVO_PIN1);     // setup the left gate servo on pin 5
  gate_servo.write(angle);            // sets the servos to open on startup
  gate_servo1.write(angle);           // sets the servos to open on startup

  mySoftwareSerial.begin(9600);
  myDFPlayer.begin(mySoftwareSerial);
  myDFPlayer.setTimeOut(200);
  myDFPlayer.volume(20);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

}
void loop() {
    switch (state) {
    case 1:                                 // Waiting for the train
      if(digitalRead(SENSOR1_PIN) == LOW)  // this may need to be swoped once connected  to current sensor
      state = 2;
    break;

    case 2:                                 //Sensor 1 Triggered - blinking started and start the sound
      blink_enabled = 1;  //variable set to on 
      myDFPlayer.volume(20);
      myDFPlayer.play(1);
      time_to_close_gate = millis() + (unsigned long)GATE_DELAY_CLOSE;
      state = 3; 
    break; 

    case 3:                                 // blinking, wait until it's time to close the gate
      if (millis() > time_to_close_gate)    // gate delay time has passed
      state = 4; 
    break;

    case 4:                                 // close the gate
      gate_servo.attach(SERVO_PIN);
      gate_servo1.attach(SERVO_PIN1);
      setpoint = GATE_CLOSED_ANGLE;
      state = 5;
    break;

    case 5:                                 // Waiting for the train to leave the block
      if(digitalRead(SENSOR1_PIN) == HIGH) 
      
      state = 6;
    break;

    case 6:                                 // train departure detected, open the gate, blinking
      gate_servo.attach(SERVO_PIN);
      gate_servo1.attach(SERVO_PIN1);
      setpoint = GATE_OPEN_ANGLE; 
      time_to_open_gate = millis() + (unsigned long)GATE_DELAY_OPEN;  
      myDFPlayer.volume(20);
      myDFPlayer.play(1);     
      state = 7;
    break; 

    case 7:                                 // delay the blink stop for gate delay value
      if (millis() > time_to_open_gate)  // gate delay time has passed
      state = 8; 
    break;

    case 8:                                 // blinking stop and end the loop
      blink_enabled = 0;                    // variable set to off
      time_to_blink = 0;                    // variable returned to 0
      led1 = 0;                             // sets led to off
      led2 = 0;                             // sets led to off
      gate_servo.detach();                  // turns off the servo motor to avoid servo flutter
      gate_servo1.detach();                 // turns off the servo motor to avoid servo flutter
      state = 1;                            // returns the state back to 1 being idle waiting sensor 1 active
    break;
    }


  if (millis() > time_for_servo_step) {
    time_for_servo_step = millis() + (unsigned long)GATE_SPEED;
    if (angle < setpoint) angle++;
    if (angle > setpoint) angle--;
    gate_servo.write(angle);
    gate_servo1.write(angle);
  }
       // this controls the blink rate using millis instead of the delay so we can interupt the blinking to move to the next state.
  if(blink_enabled == 1) {
    if(millis() > time_to_blink) {
      time_to_blink = millis() + (unsigned long)BLINK_SPEED;
      led1 = !led1;
      led2 = !led1;
    }
  }
  digitalWrite(LED1_PIN, led1);
  digitalWrite(LED2_PIN, led2);
  digitalWrite(LED3_PIN, led1);
  digitalWrite(LED4_PIN, led2);
  
}
