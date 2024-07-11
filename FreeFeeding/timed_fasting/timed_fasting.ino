/*
  Feeding experimentation device 3 (FED3)
  Timed Fasting

  mandina@mit.edu
  July, 2024

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2024 Matias Andina
*/

#include "FED3.h"                                       //Include the FED3 library 
String sketch = "FreeFeed";                             //Unique identifier text for each sketch (this will show up on the screen and in log file)
String fasting_start = "fasting_start";
String fasting_stop = "fasting_stop";
// Declare global variables
DateTime fasting_start_dt;
DateTime fasting_stop_dt;

FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  Serial.begin(9600); // Initialize Serial communication first to ensure it's ready for output
  fed3.begin(); // Setup the FED3 hardware
  fed3.DisplayPokes = false; // Customize the DisplayPokes option to 'false'
  fed3.timeout = 3; // Set a timeout period (in seconds) after each pellet is taken

  // Set fasting_start_dt and fasting_stop_dt using the methods from the FED3 object
  fasting_start_dt = fed3.SetEventDt(fasting_start);
  fasting_stop_dt = fed3.SetEventDt(fasting_stop);

  // Correctly format and print the datetime info to Serial
  char time_buffer[100];
  Serial.println("Fasting Start");
  sprintf(time_buffer, "%d-%02d-%02d %02d:%02d", fasting_start_dt.year(), fasting_start_dt.month(), fasting_start_dt.day(), fasting_start_dt.hour(), fasting_start_dt.minute());
  Serial.println(time_buffer);
  Serial.println("Fasting Stop");
  sprintf(time_buffer, "%d-%02d-%02d %02d:%02d", fasting_stop_dt.year(), fasting_stop_dt.month(), fasting_stop_dt.day(), fasting_stop_dt.hour(), fasting_stop_dt.minute());
  Serial.println(time_buffer);
}


void loop() {
  fed3.run(); // Call fed.run at least once per loop
  DateTime now = fed3.now(); // get the current time from FED3's RTC
  if (now >= fasting_start_dt && now <= fasting_stop_dt) {
    Serial.println("Fasting and Checking Well");
    fed3.CheckWell();
  } else {
    Serial.println("Feeding is Active");
    fed3.Feed(); // Drop pellet 
  }
}
