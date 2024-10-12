// Version 1.1.0.0 - Increment this field for every commit so that we can track what version is pushed to the Arduino at any point in time

#include <USB-MIDI.h>
#include <FastLED.h>

#define FNEH_88_KEY 88
#define FNEH_63_KEY 63
#define FNEH_49_KEY 49
#define N_OVATION_25_KEY 25
#define MPK_MINI_25_KEY 25

//We have 11 LEDs per each 3 inch. LEDs to light up are the ones that are located more than 50% on top of the key.
//Black notes in an octave are [1,4,7,9,11].

/** FNEH_88
48" 3' Length
First and last notes are 15'
Black notes are 9'
All other white notes are 8'
Starts with large white note + a 2w1b and ends with a large white note
**/

#define FNEH_88_LED_COUNT 176 // 175 + skipping 1
#define FNEH_63_LED_COUNT 126 
#define FNEH_49_LED_COUNT 98
#define N_OVATION_25_LED_COUNT 51
#define MPK_MINI_25_LED_COUNT 46

#define FNEH_88_PIN 5
#define FNEH_63_PIN 6
#define FNEH_49_PIN 7
#define N_OVATION_25_PIN 8
#define MPK_MINI_25_PIN 9

#define FNEH_88_LED_PER_NOTE 2
#define FNEH_63_LED_PER_NOTE 2
#define FNEH_49_LED_PER_NOTE 2
#define N_OVATION_25_LED_PER_NOTE 2
#define MPK_MINI_25_LED_PER_NOTE 2

#define FNEH_88_SKIP_LED_COUNT 1
#define FNEH_63_SKIP_LED_COUNT 1
#define FNEH_49_SKIP_LED_COUNT 1
#define N_OVATION_25_SKIP_LED_COUNT 1
#define MPK_MINI_25_SKIP_LED_COUNT 0

#define FNEH_88_SMALLEST_NOTE 21
#define FNEH_63_SMALLEST_NOTE 36
#define FNEH_49_SMALLEST_NOTE 36
#define N_OVATION_25_SMALLEST_NOTE 48
#define MPK_MINI_25_SMALLEST_NOTE 48

#define debug false // A debug true should never be pushed to the git repository

CRGB leds_88_key[FNEH_88_LED_COUNT];
CRGB leds_63_key[FNEH_63_LED_COUNT];
CRGB leds_49_key[FNEH_49_LED_COUNT];
CRGB leds_25_novation[N_OVATION_25_LED_COUNT];
CRGB leds_25_mpk_mini[MPK_MINI_25_LED_COUNT];

// Define the Keyboard struct
struct Keyboard {
    CRGB* leds;
    byte ledCount;
    byte startNote;
    byte ledPerNote;
    byte skipLEDCount;

    // Constructor
    Keyboard(CRGB* leds, byte ledCount, byte startNote, byte ledPerNote, byte skipLEDCount)
        : leds(leds),ledCount(ledCount), startNote(startNote), ledPerNote(ledPerNote), skipLEDCount(skipLEDCount) {}
};

Keyboard keyboards[5] = { 
  Keyboard(leds_63_key, FNEH_63_LED_COUNT, FNEH_63_SMALLEST_NOTE, FNEH_63_LED_PER_NOTE, FNEH_63_SKIP_LED_COUNT),
  Keyboard(leds_25_mpk_mini, MPK_MINI_25_LED_COUNT, MPK_MINI_25_SMALLEST_NOTE, MPK_MINI_25_LED_PER_NOTE, MPK_MINI_25_SKIP_LED_COUNT),
  Keyboard(leds_25_novation, N_OVATION_25_LED_COUNT, N_OVATION_25_SMALLEST_NOTE, N_OVATION_25_LED_PER_NOTE, N_OVATION_25_SKIP_LED_COUNT),
  Keyboard(leds_49_key, FNEH_49_LED_COUNT, FNEH_49_SMALLEST_NOTE, FNEH_49_LED_PER_NOTE, FNEH_49_SKIP_LED_COUNT),
  Keyboard(leds_88_key, FNEH_88_LED_COUNT, FNEH_88_SMALLEST_NOTE, FNEH_88_LED_PER_NOTE, FNEH_88_SKIP_LED_COUNT)
};

Keyboard keyboardForChannel(byte channel) {
  return keyboards[channel-1];            //channels go from 1-16
}

#define LED_TYPE    WS2812  
#define COLOR_ORDER GRB
// I strongly recommend that you adjust this value to the lowest possible level, 
// otherwise it may damage your eyesight
#define BRIGHTNESS  6

USBMIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t0 = millis();
unsigned long tClock = millis();

using namespace MIDI_NAMESPACE;

// if you want use another color, just change this line below
// sample: CRGB myColor(50,0,0); means red color
CRGB myColor = CRGB::Cyan; // white

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial);

  FastLED.addLeds<LED_TYPE,FNEH_88_PIN,COLOR_ORDER>(leds_88_key, FNEH_88_LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,FNEH_63_PIN,COLOR_ORDER>(leds_63_key, FNEH_63_LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,FNEH_49_PIN,COLOR_ORDER>(leds_49_key, FNEH_49_LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,N_OVATION_25_PIN,COLOR_ORDER>(leds_25_novation, N_OVATION_25_LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,MPK_MINI_25_PIN,COLOR_ORDER>(leds_25_mpk_mini, MPK_MINI_25_LED_COUNT).setCorrection(TypicalLEDStrip);

  for (Keyboard keyboard : keyboards) {
    Serial.println(keyboard.ledCount);
    // turn all leds off
    fill_solid(keyboard.leds,keyboard.ledCount,CRGB::Black);
  }

  FastLED.setBrightness(BRIGHTNESS);

  FastLED.show();

  //Flash 10 LEDs of each strip so we know they are ready.
  if(debug) {
    for (Keyboard keyboard : keyboards) {
      CRGB* leds = keyboard.leds;
      byte numLeds = keyboard.ledCount;
      byte i = 0;
      for (i ; i < 10; i++) {      
        fill_gradient_RGB(leds, i,myColor, i+1,myColor);
        FastLED.show();
        delay(2);
        if (i >= 1) {
          fill_gradient_RGB(leds, i-1,CRGB::Black, i,CRGB::Black);
          FastLED.show();
          delay(2);
        }
      }
      fill_gradient_RGB(leds, i,CRGB::Black, i,CRGB::Black);
    }
 }

  // Listen for MIDI messages on all channels, each channel represents a keyboard.
  MIDI.begin(MIDI_CHANNEL_OMNI);

  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  // Listen to incoming notes
  MIDI.read();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static void OnNoteOn(byte channel, byte note, byte velocity) {

  if(debug) {
    Serial.print(F("NoteOn  from channel: "));
    Serial.print(channel);
    Serial.print(F(", note: "));
    Serial.print(note);
    Serial.print(F(", velocity: "));
    Serial.print(velocity);
    Serial.print(F(", keyboad led count is: "));
    Serial.println(keyboardForChannel(channel).ledCount);
  }

  alterLEDs(channel, myColor, note);

}

static void OnNoteOff(byte channel, byte note, byte velocity) {

  if(debug) {
    Serial.print(F("NoteOff from channel: "));
    Serial.print(channel);
    Serial.print(F(", note: "));
    Serial.print(note);
    Serial.print(F(", velocity: "));
    Serial.println(velocity);
  }

  alterLEDs(channel, CRGB::Black, note);
}

void alterLEDs(byte channel, CRGB myColor, byte note) {
  Keyboard keyboard = keyboardForChannel(channel);
  CRGB* leds = keyboard.leds;
  byte ledPerNote = keyboard.ledPerNote;
  byte startNote = keyboard.startNote;
  byte skipLEDCount = keyboard.skipLEDCount;
  byte totalLEDCount = keyboard.ledCount;

  byte notePosition = note - startNote;

  if(totalLEDCount == 176) {
    if(notePosition < 26) {
      leds[skipLEDCount + 2 * notePosition] = myColor;
      leds[skipLEDCount + 2 * notePosition + 1] = myColor;
    } else if(notePosition > 72 ){
      leds[skipLEDCount + 2 * notePosition - 2] = myColor;
      leds[skipLEDCount + 2 * notePosition - 1] = myColor;
    } else {
      leds[skipLEDCount + 2 * notePosition - 1] = myColor;
      leds[skipLEDCount + 2 * notePosition] = myColor;
    }
  } 
  if(totalLEDCount == 126) {
    if(notePosition < 10) {
      leds[skipLEDCount + 2 * notePosition] = myColor;
      leds[skipLEDCount + 2 * notePosition + 1] = myColor;
    } else if(notePosition > 49 ){
      leds[skipLEDCount + 2 * notePosition - 3] = myColor;
      leds[skipLEDCount + 2 * notePosition - 2] = myColor;
    } else if(notePosition > 32 ){
      leds[skipLEDCount + 2 * notePosition - 2] = myColor;
      leds[skipLEDCount + 2 * notePosition - 1] = myColor;
    } else {
      leds[skipLEDCount + 2 * notePosition - 1] = myColor;
      leds[skipLEDCount + 2 * notePosition] = myColor;
    }
  } 
  else if(totalLEDCount == 98) {
    if(notePosition < 11) {
      leds[skipLEDCount + 2 * notePosition] = myColor;
      leds[skipLEDCount + 2 * notePosition + 1] = myColor;
    } else if(notePosition > 32 ){
      leds[skipLEDCount + 2 * notePosition - 2] = myColor;
      leds[skipLEDCount + 2 * notePosition - 1] = myColor;
    } else {
      leds[skipLEDCount + 2 * notePosition - 1] = myColor;
      leds[skipLEDCount + 2 * notePosition] = myColor;
    }
  } 
  else if(totalLEDCount == 46) {
    if(notePosition < 2) {
      leds[skipLEDCount + 2 * notePosition] = myColor;
      leds[skipLEDCount + 2 * notePosition + 1] = myColor;
    } else if(notePosition > 5 ){
      leds[skipLEDCount + 2 * notePosition - 2] = myColor;
      leds[skipLEDCount + 2 * notePosition - 1] = myColor;
    } else {
      leds[skipLEDCount + 2 * notePosition - 1] = myColor;
      leds[skipLEDCount + 2 * notePosition] = myColor;
    }
  } 
  else {
    leds[skipLEDCount + 2 * notePosition] = myColor;
    leds[skipLEDCount + 2 * notePosition + 1] = myColor;    
  }

  FastLED.show();   
}