#include <ArduinoUnit.h>
#include <FastLED.h>

// Mock the hardware-specific parts
#define DIM_FACTOR 6

// Define keyboard constants for testing
#define FNEH_88_KEY 88
#define FNEH_63_KEY 63
#define FNEH_49_KEY 49
#define N_OVATION_25_KEY 25
#define MPK_MINI_25_KEY 25

#define FNEH_88_LED_COUNT 176
#define FNEH_63_LED_COUNT 126 
#define FNEH_49_LED_COUNT 98
#define N_OVATION_25_LED_COUNT 51
#define MPK_MINI_25_LED_COUNT 49

#define FNEH_88_SMALLEST_NOTE 21
#define FNEH_63_SMALLEST_NOTE 36
#define FNEH_49_SMALLEST_NOTE 36
#define N_OVATION_25_SMALLEST_NOTE 48
#define MPK_MINI_25_SMALLEST_NOTE 48

// Create test LED arrays
CRGB test_leds_88[FNEH_88_LED_COUNT];
uint8_t test_memo_88[FNEH_88_LED_COUNT];

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
        : leds(leds), ledCount(ledCount), startNote(startNote), ledPerNote(ledPerNote), skipLEDCount(skipLEDCount), defaultColor(defaultColor), memo(memo) {}
};

// Function prototypes from main sketch
void alterLEDs(byte channel, bool mode, byte note);
void setIndividualLED(CRGB* leds, uint8_t* memo, byte ledIndex, byte totalLEDCount, CRGB color, uint8_t dimFactor, bool mode);

// Test keyboard
Keyboard testKeyboard = Keyboard(test_leds_88, FNEH_88_LED_COUNT, FNEH_88_SMALLEST_NOTE, 2, 1, CRGB::Blue, test_memo_88);

// Mock function to return our test keyboard
Keyboard keyboardForChannel(byte channel) {
  return testKeyboard;
}

void setup() {
  Serial.begin(9600);
  while(!Serial); // Wait for serial connection
  
  // Initialize test LEDs
  for (int i = 0; i < FNEH_88_LED_COUNT; i++) {
    test_leds_88[i] = CRGB::Black;
    test_memo_88[i] = 0;
  }
}

// Test MIDI note on handling
test(alterLEDs_NoteOn) {
  // Arrange
  byte channel = 1;
  byte note = FNEH_88_SMALLEST_NOTE + 10; // Middle C
  bool mode = true; // Note On
  
  // Reset LEDs
  for (int i = 0; i < FNEH_88_LED_COUNT; i++) {
    test_leds_88[i] = CRGB::Black;
    test_memo_88[i] = 0;
  }
  
  // Act
  alterLEDs(channel, mode, note);
  
  // Assert
  // Check that at least one LED is lit (we don't know exactly which ones due to the complex mapping)
  bool anyLit = false;
  for (int i = 0; i < FNEH_88_LED_COUNT; i++) {
    if (test_leds_88[i].r != 0 || test_leds_88[i].g != 0 || test_leds_88[i].b != 0) {
      anyLit = true;
      break;
    }
  }
  assertTrue(anyLit);
}

// Test MIDI note off handling
test(alterLEDs_NoteOff) {
  // Arrange
  byte channel = 1;
  byte note = FNEH_88_SMALLEST_NOTE + 10; // Middle C
  
  // First turn on the note
  alterLEDs(channel, true, note);
  
  // Act - Turn off the note
  alterLEDs(channel, false, note);
  
  // Assert
  // Check that all LEDs are off
  bool allOff = true;
  for (int i = 0; i < FNEH_88_LED_COUNT; i++) {
    if (test_memo_88[i] != 0) {
      allOff = false;
      break;
    }
  }
  assertTrue(allOff);
}

// Test note outside keyboard range
test(alterLEDs_OutOfRange) {
  // Arrange
  byte channel = 1;
  byte note = 0; // Note outside range
  bool mode = true;
  
  // Reset LEDs
  for (int i = 0; i < FNEH_88_LED_COUNT; i++) {
    test_leds_88[i] = CRGB::Black;
    test_memo_88[i] = 0;
  }
  
  // Act
  alterLEDs(channel, mode, note);
  
  // Assert - Nothing should change
  bool allOff = true;
  for (int i = 0; i < FNEH_88_LED_COUNT; i++) {
    if (test_leds_88[i].r != 0 || test_leds_88[i].g != 0 || test_leds_88[i].b != 0) {
      allOff = false;
      break;
    }
  }
  assertTrue(allOff);
}

void loop() {
  Test::run();
}

// Implementation of the functions we're testing
// These are simplified versions of the functions from the main sketch

void alterLEDs(byte channel, bool mode, byte note) {
  Keyboard keyboard = keyboardForChannel(channel);
  CRGB* leds = keyboard.leds;
  byte ledPerNote = keyboard.ledPerNote;
  byte startNote = keyboard.startNote;
  byte skipLEDCount = keyboard.skipLEDCount;
  byte totalLEDCount = keyboard.ledCount;
  uint8_t* memo = keyboard.memo;
  CRGB color = keyboard.defaultColor;

  byte notePosition = note - startNote;
  
  // Skip if note is out of range
  if (notePosition < 0) return;

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
  } else {
    led1 = skipLEDCount + 2 * notePosition;
    led2 = skipLEDCount + 2 * notePosition + 1;    
  }

  setIndividualLED(leds, memo, led1, totalLEDCount, color, DIM_FACTOR, mode);
  setIndividualLED(leds, memo, led2, totalLEDCount, color, DIM_FACTOR, mode);

  for(int surroundIndex = 1; surroundIndex < DIM_FACTOR; surroundIndex++) {
    setIndividualLED(leds, memo, led1 - surroundIndex, totalLEDCount, color, DIM_FACTOR - surroundIndex, mode);
    setIndividualLED(leds, memo, led2 + surroundIndex, totalLEDCount, color, DIM_FACTOR - surroundIndex, mode);
  }
}

void setIndividualLED(CRGB* leds, uint8_t* memo, byte ledIndex, byte totalLEDCount, CRGB color, uint8_t dimFactor, bool mode) {
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
  }
}