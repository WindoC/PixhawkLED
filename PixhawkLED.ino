#include <Wire.h>
#include "FastLED.h"

#define NUM_LEDS 8

// Data pin that led data will be written out over
#define DATA_PIN 15
// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

long lasttimeLED = 0;
#define delayLED 10 //ms
long lasttimeCYC = 0;
#define delayCYC 50 //ms

CRGB leds[NUM_LEDS];
bool thergbled[3] = {0, 0, 0};
bool changeflag = false;
bool i2cflag = false;
#define downrate 1

void setup() {
  Wire.begin(0x55);                // join i2c bus with address
  Wire.onReceive(receiveEvent); // register event
//  Serial.begin(9600);           // start serial for output
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // init LED
}

void loop() {

  LED();
  CYC();

}

void LED() {
  if ((millis() - lasttimeLED) > delayLED) {
    lasttimeLED = millis();
//    for (int i = 0; i < 8; i++) {
//      leds[i] = CRGB(255 * thergbled[2], 255 * thergbled[1], 255 * thergbled[0]);
//    }
     if(i2cflag) {
      leds[0] = CRGB(255 * thergbled[2], 255 * thergbled[1], 255 * thergbled[0]);
      FastLED.show();
      i2cflag = false;
      changeflag = false; // skip CYC change since already update in this loop
    }
    else if(changeflag) {
      FastLED.show();
      changeflag = false;
    }
  }
}

void CYC() {
  if ((millis() - lasttimeCYC) > delayCYC) {
    lasttimeCYC = millis();
    for(int i=(NUM_LEDS-1);i>=1;i--) {
      leds[i] = CRGB(leds[i-1].r *downrate , leds[i-1].g *downrate, leds[i-1].b *downrate );
//        leds[i+1] = leds[i];
    }
    changeflag = true;
  }
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {

  while ( Wire.available() >= 2 ) { // loop through all but the last
    int ledreg = Wire.read(); // receive byte as a regid
    int ledval = Wire.read(); // receive byte as a ledcolorsonoff
//    Serial.print(ledreg, HEX);        // print the character
//    Serial.print(" : ");         // print the character
//    Serial.println(ledval, HEX);        // print the character
    switch (ledreg) {
      case 0x81:
        thergbled[0] = ledval; // B
        break;
      case 0x82:
        thergbled[1] = ledval; // G
        break;
      case 0x83:
        thergbled[2] = ledval; // R
        i2cflag = true;
        break;
      default:
        break;
    }
  }
}
