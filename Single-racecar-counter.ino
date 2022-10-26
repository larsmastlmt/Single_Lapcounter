
// include the library code:
#include "NewPing.h"
#include "SPI.h"
#include "Adafruit_GFX.h"    // Include core graphics library
#include "Adafruit_ILI9341.h" // Include Adafruit_ILI9341 library to drive the display
#include "Fonts/FreeSerif24pt7b.h"  // Add a custom font



//Pins:
const int red = 6;
const int yellow1 = 7;
const int yellow2 = 17; //use the A3 as digital pin
const int green = 18; //use the A2 as digital pin
const int speakerPin = 15; //use the A1 as digital pin
const int buttonPin2 = 4;
const int buttonPin = 5;
const int pingPin = 3; 
const int rightLed = A0; 
//const int leftLed = A1; //not used
const int inPin = A2; //pin which delivers time to receive echo using pulseIn()

int buttonState = 1;
int buttonState2 = 1;


// TFT Pins for Display:
#define TFT_DC 9
#define TFT_RST 8  // You can also connect this to the Arduino reset in which case, set this #define pin to -1!
#define TFT_CS 10
#define TFT_MOSI 11
#define TFT_MISO 12
#define TFT_CLK 13
//Hardware SPI Pins: * Arduino Uno   SCK=13, SDA=11

// Create display:
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

/* INFO
Common colors:
 * BLACK    0x0000
 * BLUE     0x001F
 * RED      0xF800
 * GREEN    0x07E0
 * CYAN     0x07FF
 * MAGENTA  0xF81F
 * YELLOW   0xFFE0
 * WHITE    0xFFFF
*/

int Variable1;

// Music Variables:
int songLength = 4; // the number of notes
char notes[] = "cccC "; // a space represents a rest
int beats[] = {2, 2, 2, 3 };
int tempo = 400;
boolean firstTime = true;

// LED Variables;
int leds[] = {red,yellow1,yellow2,green};
int ledLength = 4;

// Timer Variables:
double start;
double stop;
boolean raceMode = false;
double lapTimes[10];

boolean refresh=true;
//int triggerRef;
int lapCounter = 1;
double previousLap = 0;
double thisLap;
double lapTime;
double previousLapTime;
double bestLap;
double trackRecordLap = 10.00;

//range in cm which is considered safe to enter, anything 
//coming within less than 5 cm triggers red LED
int rightTrack = 45; //range beetween 45cm
//int leftTrack = 13; //not used
//right and Lef LED pin numbers

int flash = 0;
double fastestPossible = 0.75;

#define TRIGGER_PIN  3  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 50 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);




void setup() {
 
  Serial.begin(9600);
  Serial.println("ILI9341 Test!");
  tft.begin();

  pinMode(red,OUTPUT); //6
  pinMode(yellow1,OUTPUT); //7
  pinMode(yellow2,OUTPUT); //17
  pinMode(green,OUTPUT); //16
  pinMode(speakerPin, OUTPUT); //15
  pinMode(buttonPin,INPUT); //8
  pinMode(buttonPin2,OUTPUT); //7
  
  pinMode(pingPin, OUTPUT); //3
  pinMode(rightLed, OUTPUT); //A0
  //pinMode(leftLed, OUTPUT); //A1
  pinMode(inPin, INPUT);  //2
  
  
  tft.fillScreen(ILI9341_BLACK);  // Fill screen with black
  tft.setRotation(1);
  tft.setCursor(60,85);
  tft.setTextColor(0xFFFF);  // Set color of text. First is the color of text and after is color of background
  //tft.setFont(&FreeSerif24pt7b);
  tft.setTextSize(3);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
  tft.print("Press white");
  tft.setCursor(60,115);
  tft.print("button to");
  tft.setCursor(60,145);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(3);
  tft.print("Begin!");
  tft.setFont();
}

void countTone(){
  delay(100);
  if(firstTime){
    int countDown = 3;
    int ledIndex = 0;

    for (int i = 0; i < songLength; i++) {
      if (notes[i] == ' ') {
        delay(beats[i] * tempo); // rest
      } 
      else {
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(60,85);
        tft.setTextColor(0xFFFF,ILI9341_BLACK);  // Set color of text. First is the color of text and after is color of background
        tft.setTextSize(3);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
        tft.print("Get Ready!");
        tft.setCursor(130,130);
        tft.setTextColor(0xFFFF,ILI9341_BLACK);  // Set color of text. First is the color of text and after is color of background
        tft.setTextSize(6);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
        if(countDown>0) tft.print(countDown);
        else{
        tft.fillScreen(ILI9341_BLACK);
        tft.setTextColor(0x07E0,ILI9341_BLACK);
        tft.setTextSize(9);
        tft.setCursor(110,95);
        tft.print("GO!");}
        
            

        digitalWrite(leds[ledIndex],HIGH);       
        countDown--;     
        ledIndex++;
        playNote(notes[i], beats[i] * tempo);       
      }
      // pause between notes
      delay(tempo / 2); 
    }
    firstTime = true;
  }
}

void playNote(char note, int duration) {
  char names[] = { 
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'         };
  int tones[] = { 
    1915, 1700, 1519, 1432, 1275, 1136, 1014, 956         };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

double getTime(){
  return millis()/1000.0;
}

void victory(){
}


void loop() {

  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);



  if(buttonState == HIGH){   // THIS USED TO BE LOW
    lapCounter = 1;
    firstTime = true;
    for(int i=0; i<ledLength;i++){
      digitalWrite(leds[i],LOW);
    }
   
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(5,10);
    //tft.print("Get ready for race!");
    countTone();
    start = getTime();//grabs the time at which
    raceMode = true;
    refresh = true; 
    //    startTime();

  }
 
 
 if(raceMode){
    if(refresh)tft.fillScreen(ILI9341_BLACK); 
    if(flash%3>0){
    tft.setTextColor(0xFFFF,ILI9341_BLACK);
    tft.setTextSize(3);
    tft.setCursor(60,30);    
    tft.print("Time: ");
    tft.print(getTime()-start);
    }
    
    pong();

    
    refresh = false;
    //delay(10);
  flash++;
  if(flash>99)flash=0;  
  }//end raceMode 



    }//end loop
void rightLapCount(){

  //tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(0xFFFF,ILI9341_BLACK);
  tft.setCursor(5,150);
  tft.print("Lap ");
  tft.print(lapCounter);
  tft.print(": ");

  
  
   if(lapCounter == 1)previousLap = start; //previousLap in MILLIS TIME
      thisLap = getTime();
      lapTime = thisLap - previousLap;
      previousLap = thisLap;
      //store the Fastest Lap this Run
      if(lapTime < previousLapTime)bestLap = lapTime;
      previousLapTime = lapTime; 
      tft.print(lapTime);
      
      //Check for Best Ever
      if(lapTime<trackRecordLap)victory();

      if(bestLap<trackRecordLap){
          for(int i =0; i<11; i++){
          //tft.fillScreen(ILI9341_BLACK);
          //bestLap = LapTime
          tft.setCursor(5,100);
          tft.print("Best Lap: ");
          tft.print(bestLap);
          }  }
      
      //At End of Race      
      if(lapCounter >= 10){
         
        double stop = getTime();
        double total = stop-start;
        //tft.home();
        
        //tft.print("Time: ");
        //tft.print(total);
        //tft.setCursor(60,5);
        //tft.print("Best Lap: ");
        //Case for Broken Record       
        if(bestLap<trackRecordLap){
          for(int i =0; i<10; i++){
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(50,10);
          tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
          tft.print("Race FINISH");
          tft.setCursor(5,80);
          tft.setTextColor(0xFFFF,ILI9341_BLACK);
          tft.print("Total time: ");
          tft.print(total);
          tft.setCursor(5,130);
          tft.setTextColor(0xFFFF,ILI9341_BLACK);
          tft.print("Best Lap: ");
          tft.print(bestLap);
          digitalWrite(yellow1,LOW);
          digitalWrite(yellow2,LOW);
          digitalWrite(green,LOW);
          delay(1000);
          
          //tft.print("Time: ");
          //tft.print(total);
          //tft.setCursor(5,30);
          
          delay(250);}  }
        else{
          
          tft.fillScreen(ILI9341_BLACK);
          tft.print(bestLap);
          
        }
        raceMode = false;
      }//end of At End of Race
      
      //delay(100);     
      lapCounter++;     
      //digitalWrite(rightLed,LOW);
}

void pong(){
  delay(29);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  int d = uS / US_ROUNDTRIP_CM;
  Serial.print(d); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");
  if(d<=60 && d>=1)digitalWrite(rightLed,HIGH);
  else digitalWrite(rightLed,LOW);
  if(d<=60 && d>=1 && (getTime()-previousLap)>fastestPossible){
    //digitalWrite(rightLed,HIGH);
    rightLapCount();
  }
}

void usr(){
  //raw duration in milliseconds, cm is the 
  //converted amount into a distance
  long duration, cm;
 
  //sending the signal, starting with LOW for a clean signal
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  //setting up the input pin, and receiving the duration in 
  //microseconds for the sound to bounce off the object infront
  duration = pulseIn(inPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  
  //printing the current readings to ther serial display
  //Serial.print(cm), Serial.print("cm"), Serial.println();
  
  //checking if anything is within the safezone
  if (cm < rightTrack)
  {
    digitalWrite(rightLed, HIGH); //red is right!
    //digitalWrite(leftLed, LOW);
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(5,80);
    tft.print("BYAHH!!!");

  }
  else if(cm >rightTrack)
  {
    //digitalWrite(leftLed, HIGH); //green is left!
    digitalWrite(rightLed, LOW);
  }
  else{
    //digitalWrite(leftLed,LOW);
    digitalWrite(rightLed,LOW);
  }  
  delay(100);
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
