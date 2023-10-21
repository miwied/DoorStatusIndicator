/*
 * DoorStatusIndicator V1.0 (original code from 08/2021)
 * Receiver code
 * Channel: 001
 * Baud rate: 9600
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

// pin definitions
#define button 5
#define ledYellow 15
#define ledGreen 14
#define ledRed 16
#define timeShort 50
#define timeLong 300
#define setPin 4
#define tx 0 // TX from HC12 module
#define rx 2 // RX from HC12 module

// connection check timer
unsigned long previousMillisCon = 0;
unsigned long currentMillisCon = 0;
unsigned long conncectionTimeout = 5000;
int retries = 0;
bool connection = false;
String message = "";
String doorStatusStr = "";

SoftwareSerial HC12(tx, rx);

void ConnectionTest()
{
  HC12.print("ping"); // send "ping" to the sender
  connection = false;
  retries = 0;
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);

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
        doorStatusStr = message.substring(5, 7);
        if (message.substring(0, 4) == "pong")
        {
          // "pong" was sent back
          connection = true;
          currentMillisCon = previousMillisCon - 5000; // end while loop
        }
      }
    }

    delay(timeLong);

    if (connection == true)
    {
      retries = 1000;
      for (int i = 0; i < 3; i++)
      {
        digitalWrite(ledYellow, HIGH);
        delay(timeLong);
        digitalWrite(ledYellow, LOW);
        delay(timeLong);
      }
      if (doorStatusStr == "au")
      {
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
      }
      if (doorStatusStr == "zu")
      {
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, HIGH);
      }
      if (doorStatusStr == "")
      {
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, HIGH);
      }
    }
    else
    {
      HC12.print("ping");
    }
    retries++;
  }

  if (connection == false)
  {
    digitalWrite(ledYellow, HIGH);
  }
}

void setup()
{
  HC12.begin(9600);

  // pin config
  pinMode(button, INPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(setPin, OUTPUT);
  digitalWrite(setPin, HIGH);

  delay(500);

  ConnectionTest();
}

void loop()
{
  message = "";
  // manual connection check
  if (digitalRead(button) == LOW)
  {
    ConnectionTest();
  }

  if (HC12.available())
  {
    digitalWrite(ledYellow, HIGH);
    delay(timeShort);
    digitalWrite(ledYellow, LOW);
    delay(timeShort);
    message = HC12.readString();
    if (message == "zu")
    {
      digitalWrite(ledGreen, HIGH);
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      HC12.print("echo");
      delay(timeShort);
    }

    if (message == "au")
    {
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledYellow, LOW);
      HC12.print("echo");
      delay(timeShort);
    }
  }
}