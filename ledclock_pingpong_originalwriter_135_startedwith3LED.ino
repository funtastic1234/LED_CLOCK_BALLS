#include <FastLED.h>
#include <DS3232RTC.h>
#define LED_PIN 10
#define NUM_LEDS 135
#define MAX_BRIGHTNESS 240 // Thats almost full on, watch the power!
#define MIN_BRIGHTNESS 1 // set to a minimum
#define MIN_PIN 4 // Define Minutes adjust button pin -- other switch pin connected to ground
#define HUR_PIN 3 // Define Hours adjust button pin -- other switch pin connected to ground
DS3232RTC myRTC(false);
tmElements_t tm;
CRGB leds[NUM_LEDS];
CHSV colour( 0, 255, 180);
const int brightnessInPin = A0; // LDR and 10Kohm resistence in series to form voltage diviser... 5V, GND. A0 is connected in the middle
int counter = 0;

const int Digits[12][13] = // 12 numbers // 13 pixels
{
{4, 7, 8, 9, 14, 15,     18, 22, 23, 25, 28, 29}, //0 working feb 2022
{                        18, 22, 23,     28, 29}, //1
{4, 7,    9, 14, 15, 16, 18, 22, 23, 25, 28    }, //2
{4, 7,       14,     16, 18, 22, 23, 25, 28, 29}, //3
{4,    8, 9,         16, 18, 22, 23, 29, 28    }, //4
{4, 7, 8, 9, 14,     16, 18,     23, 25, 28, 29}, //5
{4, 7, 8, 9, 14, 15, 16, 18,     23, 25, 28, 29}, //6
{4, 7,                   18, 22, 23,     28, 29}, //7
{4, 7, 8, 9, 14, 15, 16, 18, 22, 23, 25, 28, 29}, //8
{4, 7, 8, 9, 14,     16, 18, 22, 23, 25, 28, 29}, //9
{5, 7, 8, 9, 14, 15,     18, 22, 23, 25, 28, 29}, //0a, only used for first digit
{5, 7,    9, 14, 15, 16, 18, 22, 23, 25, 28    }, //2a, only used for first digit
};


void setup()
{
FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
FastLED.setBrightness(MAX_BRIGHTNESS);
pinMode(MIN_PIN, INPUT_PULLUP); // Define Minutes adjust button pin
pinMode(HUR_PIN, INPUT_PULLUP); // Define Hours adjust button pin
myRTC.begin();
}

void TimeAdjust() {
int buttonH = digitalRead(HUR_PIN);
int buttonM = digitalRead(MIN_PIN);
if (buttonH == LOW || buttonM == LOW) {
delay(500);
tmElements_t Now;
RTC.read(Now);
int hour = Now.Hour;
int minutes = Now.Minute;
int second = Now.Second;
if (buttonH == LOW) {
if (Now.Hour == 23) {
Now.Hour = 0;
}
else {
Now.Hour += 1;
};
} else {
if (Now.Minute == 59) {
Now.Minute = 0;
}
else {
Now.Minute += 1;
};
};
myRTC.write(Now);
}
}

//function that uses the Digit aray to display numbers between 0 and 100
void displaynumber( int place , int number) {
for (int i = 0 ; i < 13 ; i++) {
if (Digits[number / 10][i] != 0) {
if (place == 0 && number / 10 == 2) { //used only for 1st digit - number 2
leds[(Digits[11][i])] = CRGB(255, 255, 255);; //Digits[11] porque esta em 11º na lista
}
else if
(place == 0 && number / 10 == 0) { //used only for 1st digit - number 0
leds[(Digits[10][i])] = CRGB(255, 255, 255);; //Digits[10] porque esta em 10º na lista
}
else {
leds[(Digits[number / 10][i] + place)] = CRGB(255, 255, 255);;
}
}
if (Digits[number % 10][i] != 0) {
leds[(Digits[number % 10][i] + 28 + place)] = CRGB(255, 255, 255);;
}
}
}

void loop() {
RTC.read(tm);
TimeAdjust(); // Check to se if time is geting modified
// map it to the range of the FastLED brightness:
int mappedValue = map(analogRead(brightnessInPin), 0, 1023, 0, 255);
FastLED.setBrightness(constrain(mappedValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS));


//changes the colour of background every 10 cycles
if (counter < 10) {
counter++;
} else {
colour.hue = (colour.hue + 1) % 256;
counter = 0;
}

// sets background to rainbow colours
for ( int i = 0; i < 135; i++) {
colour.hue = (colour.hue + 1) % 256;
leds[i] = colour;
}
colour.hue = (colour.hue + 121) % 256;

displaynumber(0, tm.Hour);
displaynumber(70, tm.Minute);

//display colons
if ( tm.Second % 2 == 0 ) {
leds[64] = CRGB(255, 255, 255); //sec blink
leds[71] = CRGB(255, 255, 255); //sec bink

}
FastLED.show();
}
