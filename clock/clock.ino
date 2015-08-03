/*************************************************************

CIRCADIAN CLOCK project by Anais Moisy

Requirements:
- Arduino Yun
- Stepper motor
- Motor shield
- NeoPixel Ring

Demo & Details @
- https://player.vimeo.com/video/127464892
- http://anaismoisy.com/essay-cardican-clock/

*************************************************************/

#include <Bridge.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

// Storage for JSON parsing
#define MAX_JSON_STR 500
char jsonStr[MAX_JSON_STR];
const int BUFFER_SIZE = 500;
int steps_day = 206;    //tricks to get a full 360 degree (I don't know why it is not doing it in 200 steps)
int steps_night = 406; 
String lat("55.9531");
String lon("-3.1889");



int delaylegnth_speed = 20;  //speed to go to the correct 'time' when it is launched

int i=0;
unsigned long start_time;
unsigned long daySteps;
unsigned long nightSteps;
unsigned long stepsToInit;

//for the LEDs
#include <Wire.h>
#include <Adafruit_NeoPixel.h> 
#define NEOPIN 6  // define pins
#define BRIGHTNESS 64 // set max brightness 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIN, NEO_GRB + NEO_KHZ800); // strip object






void setup() {
  
  //establish motor direction toggle pins
  pinMode(12, OUTPUT); //CH A -- HIGH = forwards and LOW = backwards???
  pinMode(13, OUTPUT); //CH B -- HIGH = forwards and LOW = backwards???
  
  //establish motor brake pins
  pinMode(9, OUTPUT); //brake (disable) CH A
  pinMode(8, OUTPUT); //brake (disable) CH B 
  
  Serial.begin(9600);
  start_time = millis();
  
  Serial.println("Setup complete");
  
  //setup LEDs
  Wire.begin();        	// Begin I2C
  pinMode(NEOPIN, OUTPUT);
  strip.begin();
  strip.setBrightness(BRIGHTNESS); // set brightness
  delay(500);
  colorWipe(strip.Color(0, 0, 0), 1); // Red
  delay(500);
  
  digitalWrite(13, LOW);
  Bridge.begin();
  Serial.begin(9600);
  while(!Serial);
  
}



void loop(){
  
  // Get the sunrise and sunset times
  uint32_t daySteps = 0;
  uint32_t nightSteps = 0;
  uint32_t stepsToInit = 0;
  
  if (getSunRiseSet(lat, lon, daySteps, nightSteps, stepsToInit))
  {
    Serial.print("daySteps ");
    Serial.println(daySteps);
    Serial.print("nightSteps ");
    Serial.println(nightSteps);
    Serial.print("stepsToInit ");
    Serial.println(stepsToInit);
  }
  else
  {
    Serial.println("Failed to get times");
  }  
  
  Serial.println("Setup complete");
  

  delay(5000);
  
  //make needle turn fast to the right time 
  while(i < stepsToInit){
    
    step1(delaylegnth_speed);
    step2(delaylegnth_speed);
    step3(delaylegnth_speed);
    step4(delaylegnth_speed);
  }
  
  Serial.println("Time was initialized");
  Serial.println("I is: " + String(i));
  
  
  start_time = millis();
  Serial.println("HEEEERREEE startime: ");
  Serial.println(String(start_time));
  Serial.println(String(millis()));
  
  //set the new step limit for day 
  stepsToInit = steps_day;
  
  //make needle turn right speed until sunset day
  while(i < steps_day){
      
      step1(1);
      waitTime(daySteps);
      
      step2(1);
      waitTime(daySteps);
      
      step3(1);
      waitTime(daySteps);
      
      step4(1);
      waitTime(daySteps);
  }
  
  Serial.println("I is: " + String(i));
  Serial.println("Day 1 is over!");
  stepsToInit = steps_night;
  
  if (i==steps_day){
  delay(5);
  colorWipe(strip.Color(255, 255, 255), 1); // Red
  delay(5);
  }
  
  //make needle turn right speed until sunrise day2
  while(i < steps_night){
   
      step1(1);
      waitTime(nightSteps);
      
      step2(1);
      waitTime(nightSteps);
      
      step3(1);
      waitTime(nightSteps);
      
      step4(1);
      waitTime(nightSteps);
  }
  
  if (i==steps_night){
  delay(5);
  colorWipe(strip.Color(0, 0, 0), 1); // Red
  delay(5);
  }
  
}


//delay between steps for day1
boolean waitTime(unsigned long time)
{
  
  Serial.println("time day one is being called:");
  Serial.println(start_time);
  Serial.println("Starttime + delay_day is:");
  Serial.println(start_time + daySteps);
  while ((start_time + time) > millis())
  {
    delay(5);
  }
  
  Serial.println("Setting new Starttime to now:");
  Serial.println(millis());
    start_time = millis();
    return true;
}


  
void step1(int delayMilliseconds)
{
 if (i < stepsToInit)
    {
    digitalWrite(9, LOW);  //ENABLE CH A
    digitalWrite(8, HIGH); //DISABLE CH B
  
    digitalWrite(12, HIGH);   //Sets direction of CH A
    analogWrite(3, 255);   //Moves CH A
    Serial.println("step 1");
    Serial.println("delay is: " + String(delayMilliseconds));
    delay(delayMilliseconds);
    i++;
    } 
}

void step2(int delayMilliseconds)
{
  if (i < stepsToInit)
    {
    digitalWrite(9, HIGH);  //DISABLE CH A
    digitalWrite(8, LOW); //ENABLE CH B
  
    digitalWrite(13, LOW);   //Sets direction of CH B
    analogWrite(11, 255);   //Moves CH B
    Serial.println("step 2");
    Serial.println("delay is: " + String(delayMilliseconds));
    delay(delayMilliseconds);
    i++;
    }
}

void step3(int delayMilliseconds)
{
  if (i < stepsToInit)
    {
    digitalWrite(9, LOW);  //ENABLE CH A
    digitalWrite(8, HIGH); //DISABLE CH B
  
    digitalWrite(12, LOW);   //Sets direction of CH A
    analogWrite(3, 255);   //Moves CH A
    Serial.println("step 3");
    Serial.println("delay is: " + String(delayMilliseconds));
    delay(delayMilliseconds);
    i++;
    }
}

void step4(int delayMilliseconds)
{
  if (i < stepsToInit)
    {
    digitalWrite(9, HIGH);  //DISABLE CH A
    digitalWrite(8, LOW); //ENABLE CH B
  
    digitalWrite(13, HIGH);   //Sets direction of CH B
    analogWrite(11, 255);   //Moves CH B
    Serial.println("step 4");
    Serial.println("delay is: " + String(delayMilliseconds));
    delay(delayMilliseconds);
    i++;
    }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
	strip.setPixelColor(i, c);
	strip.show();
	delay(wait);
  }
}

// Get the sunrise and sunset times from openweather.org
boolean getSunRiseSet(String lat, String lon, uint32_t &daySteps, uint32_t &nightSteps, uint32_t &stepsToInit)
{
  // Connect to server
  HttpClient client;

  // Create a HTTP request:
  String url = "http://anaismoisy.com/apps/clock/api.php?lat=" + lat + "&lng=" + lon + "&steps=" + steps_day;
  Serial.println("Getting " + url);

  client.get(url);

  // if there are incoming bytes available
  // from the server, read them and print them:
  
  // Get the JSON data
  int j = 0;
  while (client.available()) 
  {
    char c = client.read();
    jsonStr[j++] = c;
    Serial.print(c);
    
  }
  jsonStr[j++] += '\0';
  
  int i = 0;
  while(jsonStr[i])
  {
    Serial.print(jsonStr[i++]);
  }
  Serial.print("\0");
  
  // Parse the JSON data
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonStr);

  if (!root.success()) 
  {
    Serial.println("parseObject() failed");
    return false;
  }

  daySteps = root["daySteps"];
  nightSteps = root["nightSteps"];
  stepsToInit = root["stepsToInit"];

  // Success
  return true;
}
