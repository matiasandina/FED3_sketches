/*
  Feeding experimentation device 3 (FED3)

  MIT LICENSE
  Copyright (c) 2020 Matias Andina

*/

#include "FED3.h"          //Include the local FED3 library 
String sketch = "Menu";  //Unique identifier text for each sketch
FED3 fed3(sketch);       //Start the FED3 object

// param lights
int lights_on_hour = 7;
int lights_off_hour = 19;
int experiment_end_hour = 14;
// assert that exp_end_hour < lights_off_hour ? 

// Global Variable Declaration //
DateTime day0_dt;
DateTime day1_dt;
int day1;
DateTime day1_lights_on;
DateTime day1_lights_off;
DateTime day1_experiment_end_hour;
DateTime day2_dt;
int day2;
DateTime day2_lights_on;
DateTime day2_lights_off;
DateTime day2_food_back;

void setup() {
  fed3.FED3Menu = true;  //Activate the menu function at startup
  fed3.begin();          //Setup the FED3 hardware
  fed3.EnableSleep = false;
  // Turn to true if you are using FEDWatcher
  fed3.setSerial(true);
  // try to read day0 from SD?
  auto one_day = TimeSpan(0, 24, 0, 0); 
  auto two_day = TimeSpan(0, 48, 0, 0);
  DateTime day0_dt = fed3.now();
  // save day0 to SD
  // Math for day 1
  auto day1_dt = day0_dt + one_day;
  auto day1 = day1_dt.day();
  // DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour=0, uint8_t min=0, uint8_t sec=0)
  DateTime day1_lights_on = DateTime(day1_dt.year(), day1_dt.month(), day1_dt.day(), lights_on_hour);
  DateTime day1_lights_off = DateTime(day1_dt.year(), day1_dt.month(), day1_dt.day(), lights_off_hour);
  DateTime day1_experiment_end_hour = DateTime(day1_dt.year(), day1_dt.month(), day1_dt.day(), experiment_end_hour);
  
  
  // Math for day 2
  DateTime day2_dt = day0_dt + two_day;
  int day2 = day2_dt.day();
  DateTime day2_lights_on = DateTime(day2_dt.year(), day2_dt.month(), day2_dt.day(), lights_on_hour);
  DateTime day2_lights_off = DateTime(day2_dt.year(), day2_dt.month(), day2_dt.day(), lights_off_hour);
  DateTime day2_food_back = DateTime(day2_dt.year(), day2_dt.month(), day2_dt.day(), experiment_end_hour);

}


void loop() {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 1: Free feeding
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 0) {
    fed3.sessiontype = "Free_feed";  //The text in "sessiontype" will appear on the screen and in the logfile
    fed3.DisplayPokes = false;       //Turn off poke indicators for free feeding mode
    fed3.UpdateDisplay();            //Update display for free feeding session to remove poke displayt (they are on by default)
    fed3.run();                      //Call fed.run at least once per loop
    fed3.timeout = 3;
    fed3.Feed();                     //Drop pellet
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Modes 2: Experimental Protocol
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (fed3.FEDmode == 1) {
    fed3.sessiontype = "Experiment";
    fed3.run();
    DateTime now = fed3.now(); 
    if (now < day1_lights_on) {
      fed3.Feed();
    } else if (now.day() == day1) {
      if (now > day1_experiment_end_hour && now < day1_lights_off){
        fed3.Feed();
      } else {
        /*do nothing*/
      }
    } else if (now.day() == day2){
      /* day 2 should have food restriction until before experiment ends*/
      if (now < day2_food_back)
       {
        // Do nothing
       } else {
        fed3.timeout = 3; // new ITI here?
        fed3.Feed();   
       } 
    } else {
      fed3.Feed();
    }
  }

 }
