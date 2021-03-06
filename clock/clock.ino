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
//for the LEDs
//#include <Wire.h>
//#include <Adafruit_NeoPixel.h> 
//#define NEOPIN 6  // define pins
//#define BRIGHTNESS 64 // set max brightness 
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIN, NEO_GRB + NEO_KHZ800); // strip object

#include <Bridge.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

// Storage for JSON parsing
#define MAX_JSON_STR 500
char jsonStr[MAX_JSON_STR];
const int BUFFER_SIZE = 500;
int steps_day = 206; //tricks to get a full 360 degree (I don't know why it is not doing it in 200 steps)
int steps_night = 406;
String lat("55.9531");
String lon("-3.1889");

unsigned long interval=5000;  // the time we need to wait
unsigned long previousMillis=0;

int delaylegnth_speed = 50;  //speed to go to the correct 'time' when it is launched
int i = 0;
unsigned long start_time;
int steps_current_time;


void setup() {
  delay(2000);
  Bridge.begin();
  Serial.begin(9600);
  
  //establish motor direction toggle pins
  pinMode(12, OUTPUT); //CH A -- HIGH = forwards and LOW = backwards???
  pinMode(13, OUTPUT); //CH B -- HIGH = forwards and LOW = backwards???
  digitalWrite(13, LOW);
  
  //establish motor brake pins
  pinMode(9, OUTPUT); //brake (disable) CH A
  pinMode(8, OUTPUT); //brake (disable) CH B 
  
  start_time = millis();
    
//  //setup LEDs
//  Wire.begin();        	// Begin I2C
//  pinMode(NEOPIN, OUTPUT);
//  strip.begin();
//  strip.setBrightness(BRIGHTNESS); // set brightness
//  delay(500);
//  colorWipe(strip.Color(0, 0, 0), 1); // Red
//  delay(500);
  
  Serial.println("Main Setup complete");
}

void loop() {
  // Get the sunrise and sunset times
  uint32_t daySteps = 0;
  uint32_t nightSteps = 0;
  uint32_t stepsToInit = 0;
  
  do {
    Serial.println("Calling getSunRiseSet()");
    getSunRiseSet(lat, lon, daySteps, nightSteps, stepsToInit);
  } while (daySteps == 0 && nightSteps == 0);
    
 
  Serial.print("\ndaySteps ");
  Serial.println(daySteps);
  Serial.print("nightSteps ");
  Serial.println(nightSteps);
  Serial.print("stepsToInit ");
  Serial.println(stepsToInit); 
  
  Serial.println("Loop Setup complete");
  delay(2000);
  
  steps_current_time = stepsToInit;
    //make needle turn fast to the right time 
  while (i < stepsToInit) {
    step1(delaylegnth_speed);
    step2(delaylegnth_speed);
    step3(delaylegnth_speed);
    step4(delaylegnth_speed);
  }
  
  Serial.println("Time was initialized");
  
  start_time = millis();
  Serial.println(String(start_time));
  
  //set the new step limit for day 1
  steps_current_time = steps_day;
  
  //make needle turn right speed until sunset day1
  while (i < steps_day) {
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
  steps_current_time = steps_night;
  
//  if (i == steps_day) {
//    delay(5);
//    colorWipe(strip.Color(255, 255, 255), 1); // Red
//    delay(5);
//  }
  
  //make needle turn right speed until sunrise day2
  while (i < steps_night) {
    step1(1);
    waitTime(nightSteps);
    step2(1);
    waitTime(nightSteps);
    step3(1);
    waitTime(nightSteps);
    step4(1);
    waitTime(nightSteps);
  }
  
//  if (i == steps_night) {
//    delay(5);
//    colorWipe(strip.Color(0, 0, 0), 1); // Red
//    delay(5);
//  }
  i = 0;
}

// Get the sunrise and sunset times from openweather.org
void getSunRiseSet(String lat, String lon, uint32_t &daySteps, uint32_t &nightSteps, uint32_t &stepsToInit) {
  // Connect to server
  HttpClient client;

  // Create a HTTP request:
  String url = "http://anaismoisy.com/apps/clock/api.php?lat=" + lat + "&lng=" + lon + "&steps=" + steps_day;
  Serial.println("Getting " + url);

  client.get(url);
  
  // Get the JSON data
  int j = 0;
  while (client.available()) {
    char c = client.read();
    jsonStr[j++] = c;
  }
  jsonStr[j++] += '\0';
  
  int i = 0;
  while (jsonStr[i]) {
    Serial.print(jsonStr[i++]);
  }
  Serial.print("\0");
  
  // Parse the JSON data
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonStr);

  if (!root.success()) 
  {
    Serial.println("parseObject() failed");
  }

  daySteps = root["daySteps"];
  nightSteps = root["nightSteps"];
  stepsToInit = root["stepsToInit"];
}

//delay between steps
boolean waitTime(unsigned long time) {
  while ((start_time + time) > millis()) {
    delay(5);
  }
  
  Serial.println("Setting new Starttime to now:");
  Serial.println(millis());
    start_time = millis();
    return true;
}
  
void step1 (int delayMilliseconds) {
 if (i < steps_current_time) {
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

void step2 (int delayMilliseconds) {
  if (i < steps_current_time) {
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

void step3 (int delayMilliseconds) {
  if (i < steps_current_time) {
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

void step4 (int delayMilliseconds) {
  if (i < steps_current_time) {
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

//// Fill the dots one after the other with a color
//void colorWipe(uint32_t c, uint8_t wait) {
//  for (uint16_t i = 0; i< strip.numPixels(); i++) {
//	strip.setPixelColor(i, c);
//	strip.show();
//	delay(wait);
//  }
//}
