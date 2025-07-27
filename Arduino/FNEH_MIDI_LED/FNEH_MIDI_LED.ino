// Version 1.5.1.0

#include <USB-MIDI.h>
#include <FastLED.h>

#define DIM_FACTOR 6

#define FNEH_88_KEY 88
#define FNEH_63_KEY 63
#define FNEH_49_KEY 49
#define N_OVATION_25_KEY 25
#define MPK_MINI_25_KEY 25

// I strongly recommend that you adjust this value to the lowest possible level, 
// otherwise it may damage your eyesight
#define BRIGHTNESS  255

// Colors (the LED strips have the Green and Red values reversed so we have to define our own) GGRRBB
#define myRed             0xFF0000
#define myGreen           0x008000
#define myBlue            0x0000FF
#define myWhite           0xFFFFFF
#define myPurple          0x800080
#define myGold            0xFFCC00
#define myOrange          0xFFA500
#define myPowderBlue      0xB0E0E6
#define myYellow          0xFFFF00
#define myPink            0xFFCCFF
#define myLightGreen      0xCCFF33
#define myBurntRed        0x880000

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

#define FNEH_88_PIN 8
#define FNEH_63_PIN 9
#define FNEH_49_PIN 7
#define N_OVATION_25_PIN 5
#define MPK_MINI_25_PIN 6

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

CRGB actual_leds_88_key[FNEH_88_LED_COUNT];
CRGB actual_leds_63_key[FNEH_63_LED_COUNT];
CRGB actual_leds_49_key[FNEH_49_LED_COUNT];
CRGB actual_leds_25_novation[N_OVATION_25_LED_COUNT];
CRGB actual_leds_25_mpk_mini[MPK_MINI_25_LED_COUNT];

uint8_t memo_88_key[FNEH_88_LED_COUNT];
uint8_t memo_63_key[FNEH_63_LED_COUNT];
uint8_t memo_49_key[FNEH_49_LED_COUNT];
uint8_t memo_25_novation[N_OVATION_25_LED_COUNT];
uint8_t memo_25_mpk_mini[MPK_MINI_25_LED_COUNT];

// Define the Keyboard struct
struct Keyboard {
    CRGB* leds;
    CRGB* ledsActual;
    byte ledCount;
    byte startNote;
    byte ledPerNote;
    byte skipLEDCount;
    CRGB defaultColor;
    CRGB currentColor;   
    uint8_t* memo;
    uint8_t maximumBrightness;

    // Constructor
    Keyboard(CRGB* leds, CRGB* ledsActual, byte ledCount, byte startNote, byte ledPerNote, byte skipLEDCount, CRGB defaultColor, CRGB currentColor, uint8_t* memo)
        : leds(leds), ledsActual(ledsActual), ledCount(ledCount), startNote(startNote), ledPerNote(ledPerNote), skipLEDCount(skipLEDCount), defaultColor(defaultColor), currentColor(currentColor), memo(memo) { 
      maximumBrightness = BRIGHTNESS;
    }
};

Keyboard keyboards[5] = { 
  Keyboard(leds_88_key, actual_leds_88_key,  FNEH_88_LED_COUNT, FNEH_88_SMALLEST_NOTE, FNEH_88_LED_PER_NOTE, FNEH_88_SKIP_LED_COUNT, FNEH_88_COLOUR, FNEH_88_COLOUR, memo_88_key),
  Keyboard(leds_63_key, actual_leds_63_key, FNEH_63_LED_COUNT, FNEH_63_SMALLEST_NOTE, FNEH_63_LED_PER_NOTE, FNEH_63_SKIP_LED_COUNT, FNEH_63_COLOUR, FNEH_63_COLOUR, memo_63_key),
  Keyboard(leds_49_key, actual_leds_49_key, FNEH_49_LED_COUNT, FNEH_49_SMALLEST_NOTE, FNEH_49_LED_PER_NOTE, FNEH_49_SKIP_LED_COUNT, FNEH_49_COLOUR, FNEH_49_COLOUR, memo_49_key),
  Keyboard(leds_25_novation, actual_leds_25_novation, N_OVATION_25_LED_COUNT, N_OVATION_25_SMALLEST_NOTE, N_OVATION_25_LED_PER_NOTE, N_OVATION_25_SKIP_LED_COUNT, N_OVATION_25_COLOUR, N_OVATION_25_COLOUR, memo_25_novation),
  Keyboard(leds_25_mpk_mini, actual_leds_25_mpk_mini, MPK_MINI_25_LED_COUNT, MPK_MINI_25_SMALLEST_NOTE, MPK_MINI_25_LED_PER_NOTE, MPK_MINI_25_SKIP_LED_COUNT, MPK_MINI_25_COLOUR, MPK_MINI_25_COLOUR, memo_25_mpk_mini)
};

Keyboard keyboardForChannel(byte channel) {
  return keyboards[channel-1];  //channels go from 1-16
}

//    C = Red, C# = Green, D = Blue, D#= White, E = Purple, F = Gold,
//    F# = Orange, G = Powder Blue, G#=Yellow, A=Pink, A#=LightGreen, B=BurntRed
//C3 - B3 = 60 -> 71
CRGB colorMapping[12] = {myRed, myGreen, myBlue, myWhite, myPurple, myGold, myOrange, myPowderBlue, myYellow, myPink, myLightGreen, myBurntRed};

CRGB getColorFromNote(byte note) {
  if(note > 59 && note < 72) {
    return colorMapping[note - 60];
  }
}

#define LED_TYPE    WS2812B  
#define COLOR_ORDER GRB


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

  FastLED.addLeds<LED_TYPE,FNEH_88_PIN,COLOR_ORDER>(actual_leds_88_key, FNEH_88_LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,FNEH_63_PIN,COLOR_ORDER>(actual_leds_63_key, FNEH_63_LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,FNEH_49_PIN,COLOR_ORDER>(actual_leds_49_key, FNEH_49_LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,N_OVATION_25_PIN,COLOR_ORDER>(actual_leds_25_novation, N_OVATION_25_LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,MPK_MINI_25_PIN,COLOR_ORDER>(actual_leds_25_mpk_mini, MPK_MINI_25_LED_COUNT).setCorrection(TypicalLEDStrip);

  for (Keyboard keyboard : keyboards) {
    Serial.println(keyboard.ledCount);
    // turn all leds off
    fill_solid(keyboard.leds, keyboard.ledCount, CRGB::Black);
  }

  FastLED.setBrightness(BRIGHTNESS);

  FastLED.show();

  //Flash 10 LEDs of each strip so we know they are ready.
  if(debug) {
    for (Keyboard keyboard : keyboards) {
      CRGB* leds = keyboard.ledsActual;
      byte numLeds = keyboard.ledCount;
      byte i = 0;
      for (i ; i < 10; i++) {      
        fill_gradient_RGB(leds, i, keyboard.currentColor, i+1, keyboard.currentColor);
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
  MIDI.setHandleControlChange(OnControlChange);
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
    for(int i = 0; i< 5 ; i++) {
        scaleAndSetBrightness(keyboards[i].leds, keyboards[i].ledsActual, keyboards[i].ledCount, keyboards[i].maximumBrightness);
    }
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

  if(channel < 11) {
    alterLEDs(channel, true, note);
  } else {
    alterColor(channel, note);
  }
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

  if(channel < 11) {
    alterLEDs(channel, false, note);
  }
}


// ControlChange from channel: 15, number: 102, value: 35
static void OnControlChange(byte channel, byte number, byte value) {

  if(debug) {
    Serial.print(F("ControlChange from channel: "));
    Serial.print(channel);
    Serial.print(F(", number: "));
    Serial.print(number);
    Serial.print(F(", value: "));
    Serial.println(value);
  }
  if(channel > 10) {
    alterBrightness(channel, number, value);
  }
  hasLEDUpdated = true;
}

void alterBrightness(byte channel, byte number, byte value) {
  Keyboard keyboard = keyboardForChannel(channel - 10);
  if(number != 102) {
    return;
  } 
  keyboard.maximumBrightness = 2 * value + 1;
  //TODO fix the code here
  keyboards[channel-11] = keyboard;
}

void alterColor(byte channel, byte note) {
  Keyboard keyboard = keyboardForChannel(channel - 10);
  CRGB newColor = getColorFromNote(note);
  keyboard.currentColor = newColor;
  keyboards[channel-11] = keyboard;
}

// Helper function to calculate LED positions based on keyboard type and note position
void calculateLEDPositions(byte totalLEDCount, byte notePosition, byte skipLEDCount, byte &led1, byte &led2) {
  // Default calculation (used for most keyboards)
  int offset1 = 0;
  int offset2 = 1;
  float multiplier = 2.0;
  
  if(totalLEDCount == FNEH_88_LED_COUNT) {
    if(notePosition < 26) {
      // No change from default
    } else if(notePosition > 72) {
      offset1 = -2;
      offset2 = -1;
    } else {
      offset1 = -1;
      offset2 = 0;
    }
  } 
  else if(totalLEDCount == FNEH_63_LED_COUNT) {
    if(notePosition < 10) {
      // No change from default
    } else if(notePosition > 49) {
      offset1 = -3;
      offset2 = -2;
    } else if(notePosition > 32) {
      offset1 = -2;
      offset2 = -1;
    } else {
      offset1 = -1;
      offset2 = 0;
    }
  } 
  else if(totalLEDCount == FNEH_49_LED_COUNT) {
    if(notePosition < 11) {
      // No change from default
    } else if(notePosition > 32) {
      offset1 = -2;
      offset2 = -1;
    } else {
      offset1 = -1;
      offset2 = 0;
    }
  } 
  else if(totalLEDCount == MPK_MINI_25_LED_COUNT) {
    multiplier = 1.8;
    if(notePosition == 10) {
      offset1 = -2;
      offset2 = -1;
    } else if(notePosition > 18) {
      offset1 = -3;
      offset2 = -2;
    } else if(notePosition < 8) {
      // No change from default
    } else if(notePosition > 12) {
      offset1 = -2;
      offset2 = -1;
    } else {
      offset1 = -1;
      offset2 = 0;
    }
  }
  
  // Calculate final LED positions
  led1 = skipLEDCount + (multiplier * notePosition) + offset1;
  led2 = skipLEDCount + (multiplier * notePosition) + offset2;
}

void alterLEDs(byte channel, bool mode, byte note) { //mode true is ON and mode false is OFF
  Keyboard keyboard = keyboardForChannel(channel);
  CRGB* leds = keyboard.leds;
  byte ledPerNote = keyboard.ledPerNote;
  byte startNote = keyboard.startNote;
  byte skipLEDCount = keyboard.skipLEDCount;
  byte totalLEDCount = keyboard.ledCount;
  uint8_t* memo = keyboard.memo;
  CRGB color = keyboard.currentColor;
  byte notePosition = note - startNote;

  byte led1;
  byte led2;
  
  // Calculate LED positions based on keyboard type and note position
  calculateLEDPositions(totalLEDCount, notePosition, skipLEDCount, led1, led2);

  // Set the main LEDs
  setIndividualLED(leds, memo, led1, totalLEDCount, color, DIM_FACTOR, mode);
  setIndividualLED(leds, memo, led2, totalLEDCount, color, DIM_FACTOR, mode);

  // Set surrounding LEDs with diminishing brightness for glow effect
  for(int surroundIndex = 1; surroundIndex < DIM_FACTOR; surroundIndex++) {
    setIndividualLED(leds, memo, led1 - surroundIndex, totalLEDCount, color, DIM_FACTOR - surroundIndex, mode);
    setIndividualLED(leds, memo, led2 + surroundIndex, totalLEDCount, color, DIM_FACTOR - surroundIndex, mode);
  }
}

void setIndividualLED(CRGB* leds, uint8_t* memo, byte ledIndex, byte totalLEDCount, CRGB color, uint8_t dimFactor, bool mode){
  if(ledIndex < totalLEDCount && ledIndex > -1) {
    if(mode) {
      memo[ledIndex] = memo[ledIndex] + dimFactor;
    } else {
      memo[ledIndex] = memo[ledIndex] - dimFactor;
    }
    if(memo[ledIndex] == 0) {
      leds[ledIndex] = CRGB::Black;
    } else if (memo[ledIndex] >= DIM_FACTOR) {
      leds[ledIndex] = color;
    } else {
      leds[ledIndex] = color/(5 * (DIM_FACTOR / memo[ledIndex]) * (DIM_FACTOR / memo[ledIndex]));
    }
    hasLEDUpdated = true;
  }
}

void scaleAndSetBrightness(CRGB* leds, CRGB* ledsActual, int count, uint8_t brightness) {
  if(brightness == 1) {
    brightness = 0;
  }
  for (int i = 0; i < count; i++) {
    ledsActual[i] = leds[i];
    ledsActual[i].nscale8(brightness);
  }
}


