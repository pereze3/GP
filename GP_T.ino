// PEREZE3
// GROUP PROJECT
// WIRELESS TRANSMITTER RECEIVER PAIR
// TRANSMITTING HUMIDITY AND TEMP TO REMOTE LCD
// RECEIVER EQUIPPED WITH A USER INTERFACE THAT ALLOWS
// CONFIGURABLE ALARM PARAMETERS AND VISUAL AS
// WELL AS AUDIO ALARM INDICATION

// 05/1/16

// GP - Temp/humidity sensor and LCD Integration over Wirless RF,
// equipped complete with user interface that allows alarm setpoint
// changes and gives visual and audio alarm indication.
// This is the receiver parent code for the GP.
// In this code we read temp and humidity sensor data
// and output the results to an LCD over Wireless RF and
// create necessary logic for the user interface.

// BELOW ARE CREDITS WHERE DUE TO ORIGINATORS OF PART OF THIS CODE

// Thanks to Robert Tillart for providing base code on the combo
// sensor read values

// See VirtualWire.h for detailed API docs
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2008 Mike McCauley
// $Id: receiver.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $





// Include “dht” header file created by Rob Tillart
#include "dht.h"
// Create a “dht” object from the header file
dht DHT;
// Designate digital pin #5 for sensor communication
#define DHT11_PIN 5

// Defining the transmit pin for the combination sensor
const int TxPin = 6;
// Include “SoftwareSerial” header file from the Arduino Library
#include <SoftwareSerial.h>
// Creating a “SoftwareSerial” object
SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

// Include the Virtual Wire header file needed for the receiver
#include "VirtualWire.h"





// The following  void setup method prints to the serial monitor
// This is used for testing sensor readings
void setup()
{
  // Code for the wireless transmitter
  // Debugging only
  Serial.begin(9600);
  // Prints "Setup to the serial monitor"
  Serial.println("setup");

  // Sets pin D12 as the TX pin
  vw_set_tx_pin(12);
  // Required for DR3100
  vw_set_ptt_inverted(true);
  // Bits per sec
  vw_setup(4000);

  // Code for the combination sensor
  // Establish serial communications
  Serial.begin(115200);
  // Print line to serial monitor
  Serial.println("DHT TEST PROGRAM ");
  // Print line to serial monitor
  Serial.print("LIBRARY VERSION: ");
  // Print line to serial monitor
  Serial.println(DHT_LIB_VERSION);
  // Print line to serial monitor
  Serial.println();
  // Print line to serial monitor
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");

  // Define pin as output
  pinMode(TxPin, OUTPUT);
  // Start signal to combination sensor
  digitalWrite(TxPin, HIGH);  // Start signal to combination sensor
}

// This is the main loop for checking the validity of the data from the sensor
// as well as reading and reporting the data from the sensor and writing it to the LCD
void loop()
{
  // Read data loop for the combination sensor
  // Print line to serial monitor
  Serial.print("DHT11, \t");
  // Integer variable “chk” set to the read value
  int chk = DHT.read11(DHT11_PIN);
  // Checking the value of “chk” and switching based on the result
  switch (chk)
  {
    // Sensor reading is OK
    case DHTLIB_OK:
      Serial.print("OK,\t");
      break;
    // Sensor reading has checksum error
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    // Sensor reading has timeout error
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    // Sensor reading has unknown error
    default:
      Serial.print("Unknown error,\t");
      break;
  }

  // Display data loop for the combination sensor
  // Display humidity
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  // Display temperature
  Serial.println(DHT.temperature, 1);
  // Note, the above code is strictly to send readings to the serial monitor, for testing purposes

  // Short delay
  delay(2000);

  // Transmitter code is below
  // Leader string
  String stringOne = "";
  // Append the string with the temperature reading
  stringOne += (((DHT.temperature * 9 / 5)) + 32);
  // Append the string with the temperature reading
  stringOne += "F is the   Temperature";
  // NOTE, THE APPENDING IS ADJUSTED TO ALLOW THE
  // NUMERIC VALUES TO PRECEDE THE STRING MESSAGE
  // THIS ALLOWS THE FIRST FIVE ELEMENTS OF THE MESSAGE
  // TO BE COPIED AT THE RECEIVER FOR FURTHER PROCESSING

  // Leader string
  String stringTwo = "";
  // Append the string with the humidity reading
  stringTwo += (DHT.humidity);
  // Append the string with the temperature reading
  stringTwo += "%RH is the Humidity";
  // NOTE, THE APPENDING IS ADJUSTED TO ALLOW THE
  // NUMERIC VALUES TO PRECEDE THE STRING MESSAGE
  // THIS ALLOWS THE FIRST FIVE ELEMENTS OF THE MESSAGE
  // TO BE COPIED AT THE RECEIVER FOR FURTHER PROCESSING

  // First buffer is populated
  char charBuf1[50];
  stringOne.toCharArray(charBuf1, 50);

  // First buffer is transmitted
  // Message to be sent
  const char *msg = charBuf1;
  // Flash a light to show transmitting
  digitalWrite(13, true);
  //Sending the message
  vw_send((uint8_t *)msg, strlen(msg));
  // Wait until the whole message is gone
  vw_wait_tx();
  // Turn the LED off.
  digitalWrite(13, false);

  // A short delay
  delay(4000);

  // First buffer is populated
  char charBuf2[50];
  stringTwo.toCharArray(charBuf2, 50);

  // First buffer is transmitted
  // Message to be sent
  const char *msg2 = charBuf2;
  // Flash a light to show transmitting
  digitalWrite(13, true);
  //Sending the message
  vw_send((uint8_t *)msg2, strlen(msg2));
  // Wait until the whole message is gone
  vw_wait_tx();
  // Turn the LED off.
  digitalWrite(13, false);
}

