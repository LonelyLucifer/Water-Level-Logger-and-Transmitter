//Sends data from AJSR04 sensor to Thingspeak every 15 seconds

#include <WiFi.h>
#include <HTTPClient.h>

#define echoPin 16 // attach pin GPIO18 to pin Echo of JSN-SR04
#define trigPin 4  // attach pin GPIO5 ESP32 to pin Trig of JSN-SR04                     
int x = 0;

const char* ssid = "INRS Visiteurs"; //Wifi name
String serverName = "https://api.thingspeak.com/update?api_key=G70GEDD8Q972Z8EY&field2=0"; //Link to thingspeak channel. fieldX at the end can be changed to the appropriate field number. Also must be changed later in the code

WiFiServer server(80);

long duration;
int distance;
int timeInterval = 15000; //Milliseconds between data points

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  
  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin(115200);

  String macAddress = WiFi.macAddress();
  
  // Print the MAC address to the Serial Monitor
  Serial.print("MAC Address: ");
  Serial.println(macAddress);
}

void loop() {

          digitalWrite(trigPin, LOW);
          delayMicroseconds(2); 

          digitalWrite(trigPin, HIGH); // turn on the Trigger to generate pulse
          delayMicroseconds(20); // keep the trigger "ON" for 10 ms to generate pulse
          digitalWrite(trigPin, LOW); // Turn off the pulse trigger to stop pulse

          // If pulse reached the receiver echoPin
          // become high Then pulseIn() returns the
          // time taken by the pulse to reach the receiver
          duration = pulseIn(echoPin, HIGH);
          distance = duration * 0.0344 / 2; // Read the potentiometer value
          sendData(distance);
          delay(timeInterval);

}
//Send data to ThingSpeak
void sendData(int dta){
  HTTPClient http;
  Serial.println(dta);
  String url = serverName + "&field2="+dta; //&fieldX must also be changed here
  http.begin(url.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0)
  {
    Serial.print("Response code is: ");
    Serial.print(httpResponseCode);
  }
  else
  {
    Serial.print("ERROR CODE: ");
    Serial.print(httpResponseCode);
  }
  http.end();
}
