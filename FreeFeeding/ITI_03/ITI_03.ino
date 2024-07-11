/*
  Feeding experimentation device 3 (FED3)
  Free Feeding mode

  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include "FED3.h"                                       //Include the FED3 library 
String sketch = "FreeFeed";                             //Unique identifier text for each sketch (this will show up on the screen and in log file)
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.DisplayPokes = false;                            //Customize the DisplayPokes option to 'false' to not display the poke indicators
  fed3.timeout = 3; //Set a timeout period (in seconds) after each pellet is taken
  
  // Turn to true if you are using FEDWatcher
  fed3.setSerial(true);
  // set a 15 min timeout 
  //fed3.Timeout(15 * 60);
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop
  fed3.Feed();                                          //Drop pellet
}
