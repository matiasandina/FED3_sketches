/*
  Basic Pavlovian CS+/CS- Conditioning
  FED3 will play a tone CS 
  mandina@mit.edu
  October, 2021

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2021 Matias Andina
*/


/*
 ===================================================================
 Program parameters below:
 -------------------------------------------------------------------
 habituation_time = time before conditioning starts in seconds. There will be a counter
 tone_duration = standard duration from fed
 ms_after_cs = ms delay between CS and US
 ms_after_us_min = min time between trials in ms
 ms_after_us_max = max time between trials in ms
 lights_on = hour of ligths-on start
 lights_off = hour of ligths-off start
 high_freq = tone freq (Hz)
 low_freq = tone freq (Hz)
 ===================================================================
*/

#include "FED3.h"                                       //Include the FED3 library 
String sketch = "PavCSPlusMinus";                 //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

int habituation_seconds = 30 * 60; 
int tone_duration = 200; 
int ms_after_cs = 3 * 1000; 
int high_freq = 4000; 
int low_freq = 440;  
int reward_freq, no_reward_freq;
int lights_on = 7;
int lights_off = 19;
int ms_after_us_min = 4* 60 * 1000; 
int ms_after_us_max = 8 * 60 * 1000; 

bool reward_high = true;


/*
 ===================================================================
 Program code below:
 ===================================================================
*/

void setup() {
  fed3.DisplayPokes = false;                            //turn off poke indicators on screen
  fed3.begin();
  // Turn to true if you are using FEDWatcher
  fed3.setSerial(true);          
  // Desire: randomize reward high on initialization
  // Problem: if you turn off the device and turn it again for the same animal, it could switch reward tone
  // TODO: think about implementation
  if (reward_high == true){
    reward_freq = high_freq;
    no_reward_freq = low_freq;
  } else {
    reward_freq = low_freq;
    no_reward_freq = high_freq;
  }
  // habituation
  fed3.Timeout(habituation_seconds);
}

void loop() {
  fed3.run();
  // randomize if trial is rewarded or not (CS+ or CS-)
  // weights are 50% 50%
  // in random(0, 2) the only options are 0 and 1
  // TODO: add weights
  // possible implementation of a 75% 25%
  // double trial_type = random(1,5);
  // if(trial_type > 2) {reward} else {no_reward}
  bool trial_type = random(0, 2);
  // Serial.println(trial_type);
  if (trial_type) {
  fed3.Tone(reward_freq, tone_duration);
  fed3.Event = "reward_freq";
  fed3.logdata();
  delay(ms_after_cs);
  fed3.Feed();
  } else {
  fed3.Tone(no_reward_freq, tone_duration);
  fed3.Event = "no_reward_freq";
  fed3.logdata();
  delay(ms_after_cs);
  }
  while (digitalRead (PELLET_WELL) == LOW) {            //Wait here while there's a pellet in the well
    delay (10);
  }
  delay(random(ms_after_us_min, ms_after_us_max));
}


// This is not working as expected
//    // TODO: this should be abstracted in a function
//    // Notice that variables will only be accessible inside the {} of each if statement
//    // that's why we have the duplicate delay() call
//  if (fed3.currentHour <= lights_on && fed3.currentHour >= lights_off){
//    int ms_after_us_min = 30 * 1000; 
//    int ms_after_us_max = 60 * 1000; 
//    // delay (inter trial interval)
//    delay(random(ms_after_us_min, ms_after_us_max));
//  } else {
//    int ms_after_us_min = 10;//5 * 60 * 1000; 
//    int ms_after_us_max = 50;//10 * 60 * 1000;     
//    delay(random(ms_after_us_min, ms_after_us_max));
// }
