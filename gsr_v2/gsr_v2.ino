#include <Arduino.h>
#include <Wire.h>
#include "pitches.h"
#include <ThreeClor.h>

#define ANALOG_IN A0
#define MAX_COND 474 // conductivity if the two pads are in direct contact w/ each other
#define SAMPLE_RATE 50    // Sleep for 50ms, which provides the recommended sample rate (20Hz)
#define SPEAKER_PIN 3 // pin for 8ohm speaker, must be PWM



// rolling minimum conductivity
int min_cond = -999
  , max_cond = -999
  , r_i = 0
  , bpm = 120;

const int whole_note = 1000;

const int r_pin = 9
        , g_pin = 10
        , b_pin = 11;

int rhythm[] = { 8, 8, 8, 8, 8, 8, 8, 8
               , 4,    4,    8, 8, 8, 8
               , 4,    8, 8, 4,    8, 8
               , 8, 8, 4,    4,    8, 8
               , 4,    8, 4, 8, 2   };
int scale[] =
{ NOTE_B0
, NOTE_C1
, NOTE_CS1
, NOTE_D1
, NOTE_DS1
, NOTE_E1
, NOTE_F1
, NOTE_FS1
, NOTE_G1
, NOTE_GS1
, NOTE_A1
, NOTE_AS1
, NOTE_B1
, NOTE_C2
, NOTE_CS2
, NOTE_D2
, NOTE_DS2
, NOTE_E2
, NOTE_F2
, NOTE_FS2
, NOTE_G2
, NOTE_GS2
, NOTE_A2
, NOTE_AS2
, NOTE_B2
, NOTE_C3
, NOTE_CS3
, NOTE_D3
, NOTE_DS3
, NOTE_E3
, NOTE_F3
, NOTE_FS3
, NOTE_G3
, NOTE_GS3
, NOTE_A3
, NOTE_AS3
, NOTE_B3
, NOTE_C4
, NOTE_CS4
, NOTE_D4
, NOTE_DS4
, NOTE_E4
, NOTE_F4
, NOTE_FS4
, NOTE_G4
, NOTE_GS4
, NOTE_A4
, NOTE_AS4
, NOTE_B4
, NOTE_C5
, NOTE_CS5
, NOTE_D5
, NOTE_DS5
, NOTE_E5
, NOTE_F5
, NOTE_FS5
, NOTE_G5
, NOTE_GS5
, NOTE_A5
, NOTE_AS5
, NOTE_B5
, NOTE_C6
, NOTE_CS6
, NOTE_D6
, NOTE_DS6
, NOTE_E6
, NOTE_F6
, NOTE_FS6
, NOTE_G6
, NOTE_GS6
, NOTE_A6
, NOTE_AS6
, NOTE_B6
, NOTE_C7
, NOTE_CS7
, NOTE_D7
, NOTE_DS7
, NOTE_E7
, NOTE_F7
, NOTE_FS7
, NOTE_G7
, NOTE_GS7
, NOTE_A7
, NOTE_AS7
, NOTE_B7
, NOTE_C8
, NOTE_CS8
, NOTE_D8
, NOTE_DS8
};

int led[] = {10, 11, 12};                                   //10 = redPin, 11 = greenPin, 9 = bluePin

const boolean ON = HIGH;     //Define on as LOW (this is because we use a common
                            //Anode RGB LED (common pin is connected to +5 volts)
const boolean OFF = LOW;   //Define off as HIGH

//Predefined Colors
const boolean RED[] = {ON, OFF, OFF};
const boolean GREEN[] = {OFF, ON, OFF};
const boolean BLUE[] = {OFF, OFF, ON};
const boolean YELLOW[] = {ON, ON, OFF};
const boolean CYAN[] = {OFF, ON, ON};
const boolean MAGENTA[] = {ON, OFF, ON};
const boolean WHITE[] = {ON, ON, ON};
const boolean BLACK[] = {OFF, OFF, OFF};

//An Array that stores the predefined colors (allows us to later randomly display a color)
const boolean* COLORS[] = {RED, MAGENTA, YELLOW, GREEN, BLUE, CYAN, WHITE, BLACK};

void setup(){
    Wire.begin(8);                // join i2c bus with address #8
    Wire.onReceive(receiveBPM); // register event
    Serial.begin(9600);
}

void receiveBPM(int n) {
    bpm = Wire.read();
    Serial.print(bpm);
    Serial.println(" BPM");
}

void loop(){
  // read the GSR sensor & write to serial
  int a = analogRead(A0);

  // if the sample is greater than zero, play a tone.
  // otherwise, if nobody's touching the contacts, just
  // wait until we get a non-zero reading.
  if (a > 100) {

    if (min_cond == -999 && max_cond == -999) {
      min_cond = max_cond = a;
    } else if (a < min_cond) {
      // if the observed conductivity is less than the
      // rolling minimum conductivity, then the observed
      // value is the new minimum
      min_cond = a;
    } else if (a > max_cond) {
      max_cond = a;
    }

    // map the analog input range (min_cond - MAX_COND)
    // to the output pitch range (120 - 1500Hz)
    int thisPitch = map(a, min_cond, max_cond, 0, 88);
    int thisColor = map(a, min_cond, max_cond, 0, 7);
    int note_length = ((bpm / 60) * whole_note) / rhythm[r_i];
    setColor(led, COLORS[thisColor]);
     // play the pitch for 50ms
    tone(SPEAKER_PIN, scale[thisPitch], note_length);
    // pause between notes
    delay(note_length * 1.30); // TODO: determine if this sounds good
    r_i = (r_i + 1) % 31;

    Serial.print("val: ");
    Serial.print(a);
    Serial.print(" min: ");
    Serial.print(min_cond);
    Serial.print(" max: ");
    Serial.println(max_cond);


  } else {
    setColor(led, BLACK);
    // wait 50ms for a 20hz sample rate
    delay(SAMPLE_RATE);
    // reset
    min_cond =  max_cond = -999;
    r_i = 0;
  }

}

/* Sets an led to any color
   led - a three element array defining the three color pins (led[0] = redPin, led[1] = greenPin, led[2] = bluePin)
   color - a three element boolean array (color[0] = red value (LOW = on, HIGH = off), color[1] = green value, color[2] =blue value)
*/
void setColor(int* led, boolean* color){
 for(int i = 0; i < 3; i++){
   digitalWrite(led[i], color[i]);
 }
}

/* A version of setColor that allows for using const boolean colors
*/
void setColor(int* led, const boolean* color){
  boolean tempColor[] = {color[0], color[1], color[2]};
  setColor(led, tempColor);
}
