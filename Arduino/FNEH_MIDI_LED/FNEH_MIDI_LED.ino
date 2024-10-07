#include <USB-MIDI.h>
#include <FastLED.h>

#define FNEH_88_KEY 88
#define FNEH_63_KEY 63
#define FNEH_49_KEY 49
#define N_OVATION_25_KEY 25
#define MPK_MINI_25_KEY 25

#define FNEH_88_LED_COUNT 176
#define FNEH_63_LED_COUNT 126
#define FNEH_49_LED_COUNT 98
#define N_OVATION_25_LED_COUNT 50
#define MPK_MINI_25_LED_COUNT 50

#define FNEH_88_PIN 9
#define FNEH_63_PIN 10
#define FNEH_49_PIN 11
#define N_OVATION_25_PIN 12
#define MPK_MINI_25_PIN 13

CRGB leds_88_key[FNEH_88_LED_COUNT];
CRGB leds_63_key[FNEH_63_LED_COUNT];
CRGB leds_49_key[FNEH_49_LED_COUNT];
CRGB leds_25_novation[N_OVATION_25_LED_COUNT];
CRGB leds_25_mpk_mini[MPK_MINI_25_LED_COUNT];

// Define the Keyboard struct
struct Keyboard {
    CRGB* leds;
    int ledCount;
    // Constructor
    Keyboard(CRGB* leds, int ledCount)
        : leds(leds),ledCount(ledCount) {}
};

Keyboard keyboards[2] = { Keyboard(leds_88_key, FNEH_88_LED_COUNT), Keyboard(leds_63_key, FNEH_63_LED_COUNT)};

Keyboard keyboardForChannel(int channel) {
  return keyboards[channel-1];
}

#define LED_TYPE    WS2812  
#define COLOR_ORDER GRB
// I strongly recommend that you adjust this value to the lowest possible level, 
// otherwise it may damage your eyesight
#define BRIGHTNESS  4 

USBMIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t0 = millis();
unsigned long tClock = millis();

using namespace MIDI_NAMESPACE;

// if you want use another color, just change this line below
// sample: CRGB myColor(50,0,0); means red color
CRGB myColor(255,255,255); // white

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

  for (Keyboard keyboard : keyboards) {
    CRGB* leds = keyboard.leds;
    int numLeds = keyboard.ledCount;
    for (int i = 0; i < numLeds; i++) {      
      fill_gradient_RGB(leds, i,myColor, i+1,myColor);
      FastLED.show();
      delay(2);
      if (i >= 1) {
        fill_gradient_RGB(leds, i-1,CRGB::Black, i,CRGB::Black);
        FastLED.show();
        delay(2);
      }
    }
 }

  // Listen for MIDI messages on channel 1
  MIDI.begin(MIDI_CHANNEL_OMNI);

  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);
  MIDI.setHandleAfterTouchPoly(OnAfterTouchPoly);
  MIDI.setHandleControlChange(OnControlChange);
  MIDI.setHandleProgramChange(OnProgramChange);
  MIDI.setHandleAfterTouchChannel(OnAfterTouchChannel);
  MIDI.setHandlePitchBend(OnPitchBend);
  MIDI.setHandleSystemExclusive(OnSystemExclusive);
  MIDI.setHandleTimeCodeQuarterFrame(OnTimeCodeQuarterFrame);
  MIDI.setHandleSongPosition(OnSongPosition);
  MIDI.setHandleSongSelect(OnSongSelect);
  MIDI.setHandleTuneRequest(OnTuneRequest);
  MIDI.setHandleClock(OnClock);
  MIDI.setHandleStart(OnStart);
  MIDI.setHandleContinue(OnContinue);
  MIDI.setHandleStop(OnStop);
  MIDI.setHandleActiveSensing(OnActiveSensing);
  MIDI.setHandleSystemReset(OnSystemReset);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  // Listen to incoming notes
  MIDI.read();

  // send a note every second
  // (dont cáll delay(1000) as it will stall the pipeline)
  if ((millis() - t0) > 1000)
  {
    t0 = millis();
    //   Serial.print(F(".");

    byte note = random(1, 127);
    byte velocity = 55;
    byte channel = 1;

    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
  }

  if ((millis() - tClock) > 20)
  {
    tClock = millis();
    MIDI.sendRealTime(MidiType::Clock);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print(F("NoteOn  from channel: "));
  Serial.print(channel);
  Serial.print(F(", note: "));
  Serial.print(note);
  Serial.print(F(", velocity: "));
  Serial.print(velocity);


  Serial.print(F(", keyboad led count is: "));
  Serial.println(keyboardForChannel(channel).ledCount);


  CRGB* leds = keyboardForChannel(channel).leds;
  leds[note] = myColor;
  leds[note+1] = myColor;
  FastLED.show();   

}

static void OnNoteOff(byte channel, byte note, byte velocity) {
  Serial.print(F("NoteOff from channel: "));
  Serial.print(channel);
  Serial.print(F(", note: "));
  Serial.print(note);
  Serial.print(F(", velocity: "));
  Serial.println(velocity);

  CRGB* leds = keyboardForChannel(channel).leds;
  leds[note] = CRGB::Black;
  leds[note+1] = CRGB::Black;
  FastLED.show();   
}

static void OnAfterTouchPoly(byte channel, byte note, byte pressure) {
  Serial.print(F("AfterTouchPoly from channel: "));
  Serial.print(channel);
  Serial.print(F(", note: "));
  Serial.print(note);
  Serial.print(F(", pressure: "));
  Serial.println(pressure);
}

static void OnControlChange(byte channel, byte number, byte value) {
  Serial.print(F("ControlChange from channel: "));
  Serial.print(channel);
  Serial.print(F(", number: "));
  Serial.print(number);
  Serial.print(F(", value: "));
  Serial.println(value);
}

static void OnProgramChange(byte channel, byte number) {
  Serial.print(F("ProgramChange from channel: "));
  Serial.print(channel);
  Serial.print(F(", number: "));
  Serial.println(number);
}

static void OnAfterTouchChannel(byte channel, byte pressure) {
  Serial.print(F("AfterTouchChannel from channel: "));
  Serial.print(channel);
  Serial.print(F(", pressure: "));
  Serial.println(pressure);
}

static void OnPitchBend(byte channel, int bend) {
  Serial.print(F("PitchBend from channel: "));
  Serial.print(channel);
  Serial.print(F(", bend: "));
  Serial.println(bend);
}

static void OnSystemExclusive(byte * array, unsigned size) {
  Serial.println(F("SystemExclusive"));
}

static void OnTimeCodeQuarterFrame(byte data) {
  Serial.print(F("TimeCodeQuarterFrame: "));
  Serial.println(data, HEX);
}

static void OnSongPosition(unsigned beats) {
  Serial.print(F("SongPosition: "));
  Serial.println(beats);
}

static void OnSongSelect(byte songnumber) {
  Serial.print(F("SongSelect: "));
  Serial.println(songnumber);
}

static void OnTuneRequest() {
  Serial.println(F("TuneRequest"));
}

static void OnClock() {
  Serial.println(F("Clock"));
}

static void OnStart() {
  Serial.println(F("Start"));
}

static void OnContinue() {
  Serial.println(F("Continue"));
}

static void OnStop() {
  Serial.println(F("Stop"));
}

static void OnActiveSensing() {
  Serial.println(F("ActiveSensing"));
}

static void OnSystemReset() {
  Serial.println(F("SystemReset"));
}
