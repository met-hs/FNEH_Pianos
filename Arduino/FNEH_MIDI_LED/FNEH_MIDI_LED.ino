// Version 1.4.0.1 - Increment this field for every commit so that we can track what version is pushed to the Arduino at any point in time

#include <USB-MIDI.h>
#include <FastLED.h>

#define DIM_FACTOR 6

#define FNEH_88_KEY 88
#define FNEH_63_KEY 63
#define FNEH_49_KEY 49
#define N_OVATION_25_KEY 25
#define MPK_MINI_25_KEY 25

#define FNEH_88_COLOUR CRGB::RoyalBlue
#define FNEH_63_COLOUR CRGB::Yellow
#define FNEH_49_COLOUR CRGB::Yellow
#define N_OVATION_25_COLOUR CRGB::RoyalBlue
#define MPK_MINI_25_COLOUR  CRGB::Yellow

#define FNEH_88_LED_COUNT 176 // 175 + skipping 1
#define FNEH_63_LED_COUNT 126 
#define FNEH_49_LED_COUNT 98
#define N_OVATION_25_LED_COUNT 51
#define MPK_MINI_25_LED_COUNT 49

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
#define N_OVATION_25_SKIP_LED_COUNT 0
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

uint8_t memo_88_key[FNEH_88_LED_COUNT];
uint8_t memo_63_key[FNEH_63_LED_COUNT];
uint8_t memo_49_key[FNEH_49_LED_COUNT];
uint8_t memo_25_novation[N_OVATION_25_LED_COUNT];
uint8_t memo_25_mpk_mini[MPK_MINI_25_LED_COUNT];

// Define the Keyboard struct
struct Keyboard {
    CRGB* leds;
    byte ledCount;
    byte startNote;
    byte ledPerNote;
    byte skipLEDCount;
    CRGB defaultColor;
    uint8_t* memo;

    // Constructor
    Keyboard(CRGB* leds, byte ledCount, byte startNote, byte ledPerNote, byte skipLEDCount, CRGB defaultColor, uint8_t* memo)
        : leds(leds),ledCount(ledCount), startNote(startNote), ledPerNote(ledPerNote), skipLEDCount(skipLEDCount), defaultColor(defaultColor), memo(memo) {}
};

Keyboard keyboards[5] = { 
  Keyboard(leds_88_key, FNEH_88_LED_COUNT, FNEH_88_SMALLEST_NOTE, FNEH_88_LED_PER_NOTE, FNEH_88_SKIP_LED_COUNT, FNEH_88_COLOUR, memo_88_key),
  Keyboard(leds_63_key, FNEH_63_LED_COUNT, FNEH_63_SMALLEST_NOTE, FNEH_63_LED_PER_NOTE, FNEH_63_SKIP_LED_COUNT, FNEH_63_COLOUR, memo_63_key),
  Keyboard(leds_49_key, FNEH_49_LED_COUNT, FNEH_49_SMALLEST_NOTE, FNEH_49_LED_PER_NOTE, FNEH_49_SKIP_LED_COUNT, FNEH_49_COLOUR, memo_49_key),
  Keyboard(leds_25_novation, N_OVATION_25_LED_COUNT, N_OVATION_25_SMALLEST_NOTE, N_OVATION_25_LED_PER_NOTE, N_OVATION_25_SKIP_LED_COUNT, N_OVATION_25_COLOUR, memo_25_novation),
  Keyboard(leds_25_mpk_mini, MPK_MINI_25_LED_COUNT, MPK_MINI_25_SMALLEST_NOTE, MPK_MINI_25_LED_PER_NOTE, MPK_MINI_25_SKIP_LED_COUNT, MPK_MINI_25_COLOUR, memo_25_mpk_mini)
};

Keyboard keyboardForChannel(byte channel) {
  return keyboards[channel-1];  //channels go from 1-16
}

#define LED_TYPE    WS2812B  
#define COLOR_ORDER GRB
// I strongly recommend that you adjust this value to the lowest possible level, 
// otherwise it may damage your eyesight
#define BRIGHTNESS  255

USBMIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t0 = millis();
bool hasLEDUpdated = false;

using namespace MIDI_NAMESPACE;

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
        fill_gradient_RGB(leds, i, keyboard.defaultColor, i+1, keyboard.defaultColor);
        FastLED.show();
        delay(2);
        if (i >= 1) {
          fill_gradient_RGB(leds, i-1,CRGB::Black, i,CRGB::Black);
          FastLED.show();
          delay(2);
        }
      }
      fill_gradient_RGB(leds, i,CRGB::Black, i , CRGB::Black);
      FastLED.show();
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
  if ((millis() - t0) > 25 && hasLEDUpdated)
  {
    t0 = millis();
    FastLED.show(); 
    hasLEDUpdated = false;
  }  
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

  alterLEDs(channel, true, note);

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

  alterLEDs(channel, false, note);
}

void alterLEDs(byte channel, bool mode, byte note) { //mode true is ON and mode false is OFF
  Keyboard keyboard = keyboardForChannel(channel);
  CRGB* leds = keyboard.leds;
  byte ledPerNote = keyboard.ledPerNote;
  byte startNote = keyboard.startNote;
  byte skipLEDCount = keyboard.skipLEDCount;
  byte totalLEDCount = keyboard.ledCount;
  uint8_t* memo = keyboard.memo;
  CRGB color = keyboard.defaultColor;

  byte notePosition = note - startNote;

  byte led1;
  byte led2;

  if(totalLEDCount == FNEH_88_LED_COUNT) {
    if(notePosition < 26) {
      led1 = skipLEDCount + 2 * notePosition;
      led2 = skipLEDCount + 2 * notePosition + 1;
    } else if(notePosition > 72 ){
      led1 = skipLEDCount + 2 * notePosition - 2;
      led2 = skipLEDCount + 2 * notePosition - 1;
    } else {
      led1 = skipLEDCount + 2 * notePosition - 1;
      led2 = skipLEDCount + 2 * notePosition;
    }
  } 
  else if(totalLEDCount == FNEH_63_LED_COUNT) {
    if(notePosition < 10) {
      led1 = skipLEDCount + 2 * notePosition;
      led2 = skipLEDCount + 2 * notePosition + 1;
    } else if(notePosition > 49 ){
      led1 = skipLEDCount + 2 * notePosition - 3;
      led2 = skipLEDCount + 2 * notePosition - 2;
    } else if(notePosition > 32 ){
      led1 = skipLEDCount + 2 * notePosition - 2;
      led2 = skipLEDCount + 2 * notePosition - 1;
    } else {
      led1 = skipLEDCount + 2 * notePosition - 1;
      led2 = skipLEDCount + 2 * notePosition;
    }
  } 
  else if(totalLEDCount == FNEH_49_LED_COUNT) {
    if(notePosition < 11) {
      led1 = skipLEDCount + 2 * notePosition;
      led2 = skipLEDCount + 2 * notePosition + 1;
    } else if(notePosition > 32 ){
      led1 = skipLEDCount + 2 * notePosition - 2;
      led2 = skipLEDCount + 2 * notePosition - 1;
    } else {
      led1 = skipLEDCount + 2 * notePosition - 1;
      led2 = skipLEDCount + 2 * notePosition;

    }
  } 
  else if(totalLEDCount == MPK_MINI_25_LED_COUNT) {
    byte ledPosition = 1.8 * notePosition;
    if(notePosition == 10) {
      led1 = skipLEDCount + ledPosition - 2;
      led2 = skipLEDCount + ledPosition - 1;      
    } else if(notePosition > 18 ){
      led1=skipLEDCount + ledPosition - 3;
      led2=skipLEDCount + ledPosition - 2;
    } else if(notePosition < 8) {
      led1=skipLEDCount + ledPosition;
      led2=skipLEDCount + ledPosition + 1;
    } else if(notePosition > 12 ){
      led1=skipLEDCount + ledPosition - 2;
      led2=skipLEDCount + ledPosition - 1;
    }  else {
      led1=skipLEDCount + ledPosition - 1;
      led2=skipLEDCount + ledPosition;
    }
  } 
  else {
    led1=skipLEDCount + 2 * notePosition;
    led2=skipLEDCount + 2 * notePosition + 1;    
  }

  setIndividualLED(leds, memo, led1, totalLEDCount, color, DIM_FACTOR, mode);
  setIndividualLED(leds, memo, led2, totalLEDCount, color, DIM_FACTOR, mode);

  for(int surroundIndex = 1; surroundIndex < DIM_FACTOR; surroundIndex++) {
    setIndividualLED(leds, memo, led1 - surroundIndex, totalLEDCount, color, DIM_FACTOR - surroundIndex, mode);
    setIndividualLED(leds, memo, led2 + surroundIndex, totalLEDCount, color, DIM_FACTOR - surroundIndex, mode);
  }
}

void setIndividualLED(CRGB* leds, uint8_t* memo, byte ledIndex, byte totalLEDCount, CRGB color, uint8_t dimFactor, bool mode){
  if(ledIndex < totalLEDCount && ledIndex > -1) {
   // Serial.print(dimFactor);
   // Serial.print(" ");
   // Serial.print(memo[ledIndex]);
   // Serial.print(" ");
    if(mode) {
      memo[ledIndex] = memo[ledIndex] + dimFactor;
    } else {
      memo[ledIndex] = memo[ledIndex] - dimFactor;
    }
    //Serial.print(memo[ledIndex]);
    //Serial.print(" ");
    if(memo[ledIndex] == 0) {
      leds[ledIndex] = CRGB::Black;
    } else if (memo[ledIndex] >= DIM_FACTOR) {
      leds[ledIndex] = color;
    } else {
      leds[ledIndex] = color/(5 * (DIM_FACTOR / memo[ledIndex]) * (DIM_FACTOR / memo[ledIndex]));
      //Serial.print((DIM_FACTOR/min(DIM_FACTOR, memo[ledIndex])));
      //Serial.print(" ");
    }
    hasLEDUpdated = true;
    //Serial.println("------");
  }
}


