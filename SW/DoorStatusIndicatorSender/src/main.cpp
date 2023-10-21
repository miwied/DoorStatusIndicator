/*
 * DoorStatusIndicator V1.0 (original code from 08/2021)
 * Sender code
 * Channel: 001
 * Baud rate: 9600
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

// pin definitions
#define sensorInput 5
#define ledYellow 14
#define ledWhite 12
#define timeShort 50
#define timeLong 300
#define setPin 4
#define tx 0 // TX from HC12 module
#define rx 2 // RX from HC12 module

// connection check timer
unsigned long previousMillisCon = 0;
unsigned long currentMillisCon = 0;
unsigned long conncectionTimeout = 5000;

String message = "";
String doorStatusStr = "";

int retries = 0;
bool connectionSuccessful = false;

bool doorIsClosedOld = false;
bool doorIsClosedNew;

SoftwareSerial HC12(tx, rx);

void ConnectionTest()
{
  connectionSuccessful = false;
  retries = 0;
  digitalWrite(ledYellow, LOW);

  while (retries <= 3)
  {
    // reset timer
    previousMillisCon = millis();
    currentMillisCon = millis();
    while ((currentMillisCon - previousMillisCon) <= conncectionTimeout)
    {
      currentMillisCon = millis();
      digitalWrite(ledYellow, HIGH);
      delay(timeShort);
      digitalWrite(ledYellow, LOW);
      delay(timeShort);
      if (HC12.available())
      {
        message = HC12.readString();
        if (message == "echo")
        {
          connectionSuccessful = true;
          currentMillisCon = previousMillisCon - 5000; // end while loop
        }
      }
    }
    delay(timeLong);
    if (connectionSuccessful == true)
    {
      delay(timeShort);
      retries = 1000;
    }
    else
    {
      HC12.print(doorStatusStr);
    }
    retries++;
  }
  if (connectionSuccessful == false)
  {
    digitalWrite(ledYellow, HIGH);
  }
  connectionSuccessful = false;
  retries = 0;
}

void setup()
{
  HC12.begin(9600);

  // pin config
  pinMode(sensorInput, INPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledWhite, OUTPUT);
  pinMode(setPin, OUTPUT);
  digitalWrite(setPin, HIGH);
}

void loop()
{
  message = "";

  if (HC12.available())
  {
    message = HC12.readString();
    if (message == "ping")
    {
      HC12.print("pong," + (String)doorStatusStr);
      digitalWrite(ledYellow, HIGH);
      delay(timeShort);
      digitalWrite(ledYellow, LOW);
    }
  }

  // check input sensor
  if (digitalRead(sensorInput) == LOW)
  {
    if (doorIsClosedNew == false)
    {
      // door is closed
      doorIsClosedNew = true;
      digitalWrite(ledWhite, HIGH);
    }
    delay(250);
  }
  else
  {
    if (doorIsClosedNew == true)
    {
      doorIsClosedNew = false;
      digitalWrite(ledWhite, LOW);
    }
    delay(250);
  }

  // door status from open to closed
  if (doorIsClosedNew == true && doorIsClosedOld == false)
  {
    doorIsClosedOld = true;
    doorStatusStr = "CL"; // CL = closed
    HC12.print(doorStatusStr);
    ConnectionTest();
  }
  // door state from closed to open
  if (doorIsClosedNew == false && doorIsClosedOld == true)
  {
    doorIsClosedOld = false;
    doorStatusStr = "OP"; // OP = open
    HC12.print(doorStatusStr);
    ConnectionTest();
  }
}