//Measure distance from the sensor to the water surface and log to SD card every 5 minutes
#include <SPI.h>
#include <SD.h>
#include "Arduino.h"
#include "FS.h"

#define SD_CS    5   // Default Chip Select pin
#define SD_CLK   18  // Default SPI Clock pin
#define SD_MOSI  23  // Default SPI MOSI pin
#define SD_MISO  19  // Default SPI MISO pin
#define echoPin 16 // attach pin GPIO18 to pin Echo of JSN-SR04
#define trigPin 4  // attach pin GPIO5 ESP32 to pin Trig of JSN-SR04                     

long duration; // Time taken for the pulse to reach the receiver
int distance; //Distance calculated by the sensor measurement
int interval = 15000; //300000ms = 5 minutes
int fileNum = 1; //File number, increasing for every new file created on startup
char fileName[50];
int i = 10;
char str[14];
char lapsedTime[20];

void setup()
{
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);  
  Serial.begin(9600);
  Serial.println("Distance measurement using JSN-SR04T");
  delay(500);

  //SD Card setup
  if (!SD.begin(SD_CS, SPI, 4000000, "/sd")) {
      Serial.println("Card failed, or not present");
      return; // stop the program if there’s an error
    }
  Serial.println("Card initialized.");

    //Finds the existing file in the SD card with the highest number n, creates a new file n+1
    while(true)
    {
      sprintf(fileName, "/LOG_%d", fileNum);
      if (fileExists(fileName))
      {
        fileNum = fileNum+1;
      }
      else
      {
        break;
      }
    }
    writeFileln(fileName, "Secondes, Distance_(cm)");
}

void loop()
{
  //Measure distance with the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); 

  digitalWrite(trigPin, HIGH); // turn on the Trigger to generate pulse
  delayMicroseconds(20); // keep the trigger "ON" for 10 ms to generate pulse
  digitalWrite(trigPin, LOW); // Turn off the pulse trigger to stop pulse

  // If pulse reached the receiver echoPin
  // become high Then pulseIn() returns the
  // time taken by the pulse to reach the receiver
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0344 / 2; 
  //Print distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(interval); 
  itoa(distance, str, 10);
  itoa(millis()/1000,lapsedTime,10);
  //Add data point to file on SD card
  appendFile(fileName, lapsedTime);
  appendFile(fileName, ",");
  appendFileln(fileName, str);
}

void writeFileln(const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);
    File file = SD.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.println(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}

void appendFile(const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = SD.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}

void appendFileln(const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = SD.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.println(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}

bool fileExists(const char * path){
    Serial.printf("Checking if %s\n exists", path);
    if(SD.open(path)){
        Serial.println("File exists");
        return true;
    } 
    else {
        Serial.println("File does not exist");
        return false;
    }
}