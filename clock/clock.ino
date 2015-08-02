/*************************************************************

CIRCADIAN CLOCK project by Anais Moisy

Arduino Yun
Stepper motor
Motor shield
NeoPixel Ring

Demo & Details @
https://player.vimeo.com/video/127464892
http://anaismoisy.com/essay-cardican-clock/

*************************************************************/

int delaylegnth_speed = 50;  //speed to go to the correct 'time' when it is launched
unsigned long delay_day1 = 5;  //I guess I will need only one delay and change it dynamicaly (see email to get how it is calculated)
unsigned long delay_day2 = 5;
unsigned long delay_night = 5;
int i=0;
int steps_day1 = 206; //tricks to get a full 360 degree (I don't know why it is not doing it in 200 steps)
int steps_night = 406;
int steps_day2 = 606;
int steps_current_time = 53; //number of steps to go to the correct 'time' when it is launched (see email to get how it is calculated)
unsigned long start_time;

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
  
}

void loop(){
  
  //make needle turn fast to the right time 
  while(i < steps_current_time){
    
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
  
  //set the new step limit for day 1
  steps_current_time = steps_day1;
  
  //make needle turn right speed until sunset day1
  while(i < steps_day1){
      
      step1(1);
      waitTime(delay_day1);
      
      step2(1);
      waitTime(delay_day1);
      
      step3(1);
      waitTime(delay_day1);
      
      step4(1);
      waitTime(delay_day1);
  }
  
  Serial.println("I is: " + String(i));
  Serial.println("Day 1 is over!");
  steps_current_time = steps_night;
  
  if (i==steps_day1){
  delay(5);
  colorWipe(strip.Color(255, 255, 255), 1); // Red
  delay(5);
  }
  
  //make needle turn right speed until sunrise day2
  while(i < steps_night){
   
      step1(1);
      waitTime(delay_night);
      
      step2(1);
      waitTime(delay_night);
      
      step3(1);
      waitTime(delay_night);
      
      step4(1);
      waitTime(delay_night);
  }
  
  if (i==steps_night){
  delay(5);
  colorWipe(strip.Color(0, 0, 0), 1); // Red
  delay(5);
  }
  
  //set the new step limit for day 2
  Serial.println("I is: " + String(i));
  Serial.println("Night 1 is over!");
  steps_current_time = steps_day2;
  
  //make needle turn right speed until sunset day2
  while(i < steps_day2){
      
      step1(1);
      waitTime(delay_day2);
      
      step2(1);
      waitTime(delay_day2);
      
      step3(1);
      waitTime(delay_day2);
      
      step4(1);
      waitTime(delay_day2);
  }
  
  Serial.println("I is: " + String(i));
  Serial.println("Day 2 is over!");
  
}


//delay between steps for day1
boolean waitTime(unsigned long time)
{
  
  Serial.println("time day one is being called:");
  Serial.println(start_time);
  Serial.println("Starttime + delay_day_1 is:");
  Serial.println(start_time + delay_day1);
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
 if (i < steps_current_time)
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
  if (i < steps_current_time)
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
  if (i < steps_current_time)
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
  if (i < steps_current_time)
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
