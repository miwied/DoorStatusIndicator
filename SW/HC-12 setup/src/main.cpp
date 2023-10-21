/*
 * DoorStatusIndicator V1.0 (original code from 08/2021)
 * Code for configuration of the HC-12
 * Channel: 001
 * Baud rate: 9600
 */

/*
  NOTE:
  Both HC-12 may need to be configured to communicate with each other before use.
  Flash this code to the microcontroller to set up each radio module serially.
  Read about "HC-12 AT commands".
*/

#include <Arduino.h>
#include <SoftwareSerial.h>

#define setPin 4
#define rx 0 // Connect to the TX pin of the HC-12
#define tx 2 // Connect to the RX pin of the HC-12

SoftwareSerial HC12(rx, tx);

void setup()
{
  Serial.begin(9600);
  HC12.begin(9600);
  pinMode(setPin, OUTPUT);
  digitalWrite(setPin, LOW);
}

void loop()
{
  if (HC12.available())
  {
    Serial.write(HC12.read());
  }
  if (Serial.available())
  {
    HC12.write(Serial.read());
  }
}