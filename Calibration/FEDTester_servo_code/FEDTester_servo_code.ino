#include <Wire.h>
#include <Servo.h>


int BNC_port = 2;
Servo myservo;
int pos=0;
bool i;

void setup() {
  myservo.attach(10);
  Serial.begin(9600); 
  pinMode(BNC_port, INPUT); 
  Serial.println("Successfully ran setup");
  attachInterrupt(0, FEDSignalDetected, RISING);

}

void loop() {

  if (i == true){
    delay(1000);
    myservo.write(105);
    delay(500);
    i = false;
  } else {
    myservo.write(80);
    //Serial.println("No signal received :(");
    delay(100);
  }

}

void FEDSignalDetected() {
  //activated if DigitalPin nr 2 is going from LOW to HIGH
  Serial.println("Incoming signal detected");
  i = true;
}
