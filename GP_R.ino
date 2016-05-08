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





// Designate digital pin #6 for LCD serial communication
const int TxPin = 6;

// Include “SoftwareSerial” header file from the Arduino Library
#include <SoftwareSerial.h>

// Creating a “SoftwareSerial” object
SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

// Include the Virtual Wire header file needed for the receiver
#include "VirtualWire.h"

// Declare a local variable needed for the receiver
int count;

// Declare local variables for the base alarm setpoints.
// Note, RH base alarm setpoint is set high for testing purposes.
double tempSetpoint = 76.00;
double humidSetpoint = 90.00;

// Declare local variables for the alarm setpoint offsets.
// These are the values that the increment and decrement buttons
// set so that the offset is created, which gets added to the base setpoint.
double tempSetpointOffset;
double humidSetpointOffset;

// Character arrays to hold the messages that comes from the
// the transmitter.
char humidMessage[40];
char tempMessage[40];

// Character arrays to hold the message in a downsized form
// so that the sensor value can be compared against the alarm
// setpoint value and the alarm generated.
char humidMessageCompare[5];
char tempMessageCompare[5];

// Converting the alarm setpoint double into a char.
// These variables take the alarm setpoint double values
// and convert them to an array.
char tempSetpointCharArray[5];
char humidSetpointCharArray[5];

// Float points for temporary conversion and comparison against one another
// for alarm logic purposes.
float temp1;
float temp2;
float temp3;
float temp4;





// The following void setup method prints to the serial monitor
// This is used for testing sensor readings
void setup()
{
  // The folowing code is for debugging only
  Serial.begin(9600);
  // Prints "Setup" to the serial monitor
  Serial.println("setup");
  // Sets pin D12 as the RX Pin
  vw_set_rx_pin(12);
  // Required for DR3100
  vw_set_ptt_inverted(true);
  // Bits per sec
  vw_setup(4000);
  // Start the receiver PLL running
  vw_rx_start();

  // Below are the pin declaration for the pushbuttons as well as
  // the onbard LED as used for testing purposes. Note, to exhance
  // the project, minimize costs and add value the inputs were configured
  // to use the internal pullup resistor that the Arduino board comes
  // equipped with. Otherwise, external resistors and more wiring would
  // would have been required to complet the pushbutton circuitry.
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(13, OUTPUT);

}





// This is the main loop for checking the validity of the data from the sensor
// as well as reading and reporting the data from the sensor and writing it to the LCD.
void loop()
{
  // Read the pushbutton value into a variable.
  // valDown is the decrement button.
  int valDown = digitalRead(2);
  // valUp is the increment button.
  int valUp = digitalRead(3);
  // tempAdjust is the temp alarm setpoint adjustment.
  int tempAdjust = digitalRead(4);
  // humidAdjust is the humid alarm setpoint adjustment.
  int humidAdjust = digitalRead(5);


  // This is the main loop for the RF Receiver.
  // The following writes from the transmitter to the buffer.
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;



  // Checking the buffer.
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    // Flash a light to show received good message.
    int i;
    // Write to the onboard LED.
    digitalWrite(13, true);
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");

    // Iterate through the buffer.
    for (i = 0; i < buflen; i++)
    {
      // One by one print the characters.
      // This is for testing purposes to the serial monitor.
      char c = (buf[i]);
      Serial.print(c);
      Serial.print(" ");

      // The following iterates through to display the transmitter data to LCD.
      // Set baud rate and begin serial port communication to LCD.
      mySerial.begin(9600);
      // Sending turn backlight on command to screen.
      mySerial.write(17);
      // Print the character.
      mySerial.print(c);

      // Populate one of the designated character arrays based on the modulus of the
      // of the count variable, since the message alternates between humid and temp,
      // amd the count variable can help distinguish between the two.
      if (count % 2 == 1)
      {
        // If the modules is 1 then write to the message array.
        tempMessage[i] = (c);
      }
      else
      {
        // If there is no remainder them write to the humidity array
        humidMessage[i] = (c);
      }
    }

    // Increase the counter for the iteration.
    count++;



    // This is the start of the humidity pushbutton logic.
    // If the humidAdjust pushbutton is reading LOW,
    // thus not reading voltage, then;
    if (humidAdjust == LOW)
    {
      // Clear the LCD screen.
      mySerial.write(12);
      // Write a message to state the humidity setpoint.
      mySerial.print("humid alarm stptis: ");
      // The effective setpoint is the base setpoint plus the offset.
      // Write this to the LCD following the message.
      mySerial.print(humidSetpoint + humidSetpointOffset);
      // Add short delay.
      delay(2100);

      // With the adjustHumid pushbutton held down,
      // and the valDown pushbutton is reading LOW,
      // thus not reading voltage, then;
      if (valDown == LOW)
      {
        // Decrease the setpoint offset.
        --humidSetpointOffset;
        // Clear the screen.
        mySerial.write(12);
        // Give message at to what the setpoint is.
        mySerial.print("humid alarm stptis now: ");
        // Write the variables to the LCD.
        mySerial.print(humidSetpoint + humidSetpointOffset);
      }
      // With the adhustHumid pushbutton held down,
      // and the valUp pushbutton is reading LOW,
      // thus not reading voltage, then;
      else if (valUp == LOW)
      {
        // Decrease the setpoint offset.
        ++humidSetpointOffset;
        // Clear the screen.
        mySerial.write(12);
        // Give message at to what the setpoint is.
        mySerial.print("humid alarm stptis now: ");
        // Write the variables to the LCD.
        mySerial.print(humidSetpoint + humidSetpointOffset);
      }
      // Otherwise do nothing.
      else
      {
      }
    }
    // Otherwise do nothing.
    else
    {
    }



    // This is the start of the temp pushbutton logic.
    // If the tempAdjust pushbutton is reading LOW,
    // thus not reading voltage, then;
    if (tempAdjust == LOW)
    {
      // Clear the LCD screen.
      mySerial.write(12);
      // Write a message to state the humidity setpoint.
      mySerial.print("temp alarm stpt is: ");
      // The effective setpoint is the base setpoint plus the offset.
      // Write this to the LCD following the message.
      mySerial.print(tempSetpoint + tempSetpointOffset);
      // Add short delay.
      delay(2100);

      // With the tempAdjust pushbutton held down,
      // and the valDown pushbutton is reading LOW,
      // thus not reading voltage, then;
      if (valDown == LOW)
      {
        // Decrease the setpoint offset.
        --tempSetpointOffset;
        // Clear the screen.
        mySerial.write(12);
        // Give message at to what the setpoint is.
        mySerial.print("temp alarm stpt is now: ");
        // Write the variables to the LCD.
        mySerial.print(tempSetpoint + tempSetpointOffset);
      }
      // With the adhustTemp pushbutton held down,
      // and the valUp pushbutton is reading LOW,
      // thus not reading voltage, then;
      else if (valUp == LOW)
      {
        // Decrease the setpoint offset.
        ++tempSetpointOffset;
        // Clear the screen.
        mySerial.write(12);
        // Give message at to what the setpoint is.
        mySerial.print("temp alarm stpt is now: ");
        // Write the variables to the LCD.
        mySerial.print(tempSetpoint + tempSetpointOffset);
      }
      // Otherwise do nothing.
      else
      {
      }
    }
    // Otherwise do nothing.
    else
    {
    }



    // This is the start of the humidity comparison and
    // alarm logic (separate from the temp logic).
    // If the count moduls results in a remainder then;
    if (count % 2 == 1)
    {
      // Command to convert the double into a char array
      dtostrf((humidSetpoint + humidSetpointOffset), 5, 2, humidSetpointCharArray);
      // Iterate through for five elements, two integers followed by a decimal
      // followed by two integers.
      for (i = 0; i < 5; i++)
      {
        // Fill the five element array with the first five elements from the message.
        humidMessageCompare[i] = humidMessage[i];
      }
      // Execute the array to float command, so that the floats can be compared.
      temp3 = atof(humidMessageCompare);
      temp4 = atof(humidSetpointCharArray);
      // Compare the floats.
      if (temp3 > temp4)
      {
        // Clear the screen.
        mySerial.write(12);
        // State a message about the alarm.
        mySerial.print("ALM: HUMID ABOVESTPT @ ");
        // Use the temporary variable.
        mySerial.print(temp3);
        // Cap off the statement with units.
        mySerial.print("%RH!");
        // Send an audible alarm.
        mySerial.write(220);    // Musical note that is commented out
      }
    }
    // If count modulus has not remainder then;
    else
    {
      // Command to convert the double into a char array
      dtostrf((tempSetpoint + tempSetpointOffset), 5, 2, tempSetpointCharArray);
      for (i = 0; i < 5; i++)
      {
        // Fill the five element array with the first five elements from the message.
        tempMessageCompare[i] = tempMessage[i];
      }
      // Execute the array to float command, so that the floats can be compared.
      temp1 = atof(tempMessageCompare);
      temp2 = atof(tempSetpointCharArray);
      // Compare the floats.
      if (temp1 > temp2)
      {
        // Clear the screen.
        mySerial.write(12);
        // State a message about the alarm.
        mySerial.print("ALM: TEMP ABOVE STPT @ ");
        // Use the temporary variable.
        mySerial.print(temp1);
        // Cap off the statement with units.
        mySerial.print("F!");
        // Send an audible alarm.
        mySerial.write(220);    // Musical note that is commented out
      }
    }



    // Below is all Serial Monitor test code
    // to make sure the variables are behaving
    // as expected during testing.
    // None of this shows outside of the Serial Monitor.
    // Test print of the pushbutton variables
    Serial.println("");
    Serial.print(count);
    Serial.println("");
    Serial.print(valDown);
    Serial.println("");
    Serial.print(valUp);
    Serial.println("");
    Serial.print(tempAdjust);
    Serial.println("");
    Serial.print(humidAdjust);
    // Test print of the setpoint variables
    Serial.println("");
    Serial.print(tempSetpoint);
    Serial.println("");
    Serial.print(humidSetpoint);
    Serial.println("");
    Serial.print(tempSetpointOffset);
    Serial.println("");
    Serial.print(humidSetpointOffset);
    // Test print of the helper char array variables
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.print(tempMessageCompare);
    Serial.println("");
    Serial.print(tempSetpointCharArray);
    Serial.println("");
    Serial.print(humidMessageCompare);
    Serial.println("");
    Serial.print(humidSetpointCharArray);
    // Test print of the helper float variables
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.print(temp1);
    Serial.println("");
    Serial.print(temp2);
    Serial.println("");
    Serial.print(temp3);
    Serial.println("");
    Serial.print(temp4);

    Serial.println("");
    Serial.println("");
    Serial.println("");


    // Short delay
    delay(2100);

    // Turn off the onbboard LED
    digitalWrite(13, false);
    // Sending clear command to screen
    mySerial.write(12);
  }
}

