#include <USB-MIDI.h>
#include <FastLED.h>
#define NUM_LEDS    176 // number of leds
#define DATA_PIN    9   // output pin number
#define LED_TYPE    WS2812  
#define COLOR_ORDER GRB
// I strongly recommend that you adjust this value to the lowest possible level, 
// otherwise it may damage your eyesight
#define BRIGHTNESS  4 

USBMIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t0 = millis();
unsigned long tClock = millis();

using namespace MIDI_NAMESPACE;

CRGB leds[NUM_LEDS];
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

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // turn all leds off
  fill_solid(leds,NUM_LEDS,CRGB::Black);
  FastLED.show();

  for (int i = 0; i < NUM_LEDS; i++) {
    fill_gradient_RGB(leds, i,myColor, i+1,myColor);
    FastLED.show();
    delay(2);
    if (i >= 1) {
      fill_gradient_RGB(leds, i-1,CRGB::Black, i,CRGB::Black);
      FastLED.show();
      delay(2);
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
  Serial.println(velocity);
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
