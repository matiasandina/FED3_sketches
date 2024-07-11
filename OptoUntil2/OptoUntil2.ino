/*
  Feeding experimentation device 3 (FED3)
  Opto Task
  Animals should receive opto stimulation after taking pelets_until_trigger (hence OptoUntilN for sketch name)
  Opto stimulation is handled by a separate pulse generator that receives a logic window (when BNC == HIGH, pulse generator will pulse according to user settings)
  Fed should continue feeding while opto is on. Once opto is off (as set by the BNCpulseLength), we start counting pellets towards pelets_until_trigger again    

  willow66@mit.edu
  April, 2024

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2024 Liu Yang
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "OptoUntil2";                             //Unique identifier text for each sketch (this will show up on the screen and in log file)
FED3 fed3 (sketch);                                     //Start the FED3 object

int previous_pellet_trigger = 0;
int pellets_until_trigger = 2;
int BNCpulseLength= 10;
int BNCstart = 0;
int BNCtimer = 0;
bool timer_started = false;  // Flag to indicate if the timer has started


void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.disableSleep();                                  // So that fed.run() does not go to sleep
  fed3.DisplayPokes = false;                            //Customize the DisplayPokes option to 'false' to not display the poke indicators
  fed3.timeout = 1;                                    //Set a timeout period (in seconds) after each pellet is taken
  Serial.println("Setup Finished");

}

void loop() {
  Serial.println("Loop start");
  fed3.run();                                           //Call fed.run at least once per loop
  Serial.println("Calling fed3.Feed()");
  fed3.Feed(); //Drop pellet 
  Serial.println("fed3.Feed() already done");
  if (fed3.PelletCount - previous_pellet_trigger == pellets_until_trigger) {
    Serial.println("BNC trigger condition is met. Current pellet count is");
    Serial.println(fed3.PelletCount);
    if (!timer_started) {
      Serial.println("Starting Timer.");
      BNCstart = millis() / 1000; // Start the timer only if it hasn't started already
      timer_started = true;  // Set the flag to indicate timer has started
      Serial.println("Switching BNC to HIGH");
      digitalWrite(BNC_OUT, HIGH);
    } else {
      Serial.println("Timer already started"); 
    }
    BNCtimer = (millis() / 1000) - BNCstart; // Check the actual elapsed time since the first BNC_OUT == HIGH
    Serial.println("Current Time Diff (s)");
    Serial.println(BNCtimer);
    if (BNCtimer > BNCpulseLength) {
      Serial.println("BNC Timer reached pulseLength setting");
      digitalWrite(BNC_OUT, LOW);
      previous_pellet_trigger = fed3.PelletCount;           //Bring the current pellet to the previous cycle
      timer_started = false;  // Reset the flag when the timer is stopped
    }
  }
 Serial.println("Loop stop");
}
