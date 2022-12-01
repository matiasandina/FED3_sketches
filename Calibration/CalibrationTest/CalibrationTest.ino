#include "FED3.h"  //includes the modified FED3 library in the calibration folder

String sketch = "CalibrationTest";
FED3 fed3 (sketch);

void setup() {
  // put your setup code here, to run once:
  fed3.beginCalibration();
  Serial.begin(9600);
  fed3.timeout = 0; 
}

void loop() {
  // put your main code here, to run repeatedly:

  while (fed3.PelletCount < 1000){
  fed3.run();
  fed3.CalibrationFeed();
  }
  
  exit(0);
}
