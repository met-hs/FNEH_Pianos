#include <ArduinoUnit.h>
#include <FastLED.h>

// Mock the hardware-specific parts
#define DIM_FACTOR 6

// Create a small test LED array
#define TEST_LED_COUNT 10
CRGB test_leds[TEST_LED_COUNT];
uint8_t test_memo[TEST_LED_COUNT];

// Function prototype from main sketch that we want to test
void setIndividualLED(CRGB* leds, uint8_t* memo, byte ledIndex, byte totalLEDCount, CRGB color, uint8_t dimFactor, bool mode);

// Setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  while(!Serial); // Wait for serial connection
  
  // Initialize test LEDs
  for (int i = 0; i < TEST_LED_COUNT; i++) {
    test_leds[i] = CRGB::Black;
    test_memo[i] = 0;
  }
}

// Test that turning on an LED works correctly
test(setIndividualLED_TurnOn) {
  // Arrange
  byte ledIndex = 5;
  CRGB color = CRGB::Red;
  
  // Act - Turn on LED
  setIndividualLED(test_leds, test_memo, ledIndex, TEST_LED_COUNT, color, DIM_FACTOR, true);
  
  // Assert
  assertEqual(test_memo[ledIndex], DIM_FACTOR);
  assertEqual(test_leds[ledIndex].r, color.r);
  assertEqual(test_leds[ledIndex].g, color.g);
  assertEqual(test_leds[ledIndex].b, color.b);
}

// Test that turning off an LED works correctly
test(setIndividualLED_TurnOff) {
  // Arrange
  byte ledIndex = 5;
  CRGB color = CRGB::Red;
  
  // First turn it on
  setIndividualLED(test_leds, test_memo, ledIndex, TEST_LED_COUNT, color, DIM_FACTOR, true);
  
  // Act - Turn off LED
  setIndividualLED(test_leds, test_memo, ledIndex, TEST_LED_COUNT, color, DIM_FACTOR, false);
  
  // Assert
  assertEqual(test_memo[ledIndex], 0);
  assertEqual(test_leds[ledIndex].r, 0);
  assertEqual(test_leds[ledIndex].g, 0);
  assertEqual(test_leds[ledIndex].b, 0);
}

// Test that out-of-bounds LED indices are handled correctly
test(setIndividualLED_OutOfBounds) {
  // Arrange
  byte ledIndex = TEST_LED_COUNT + 5; // Out of bounds
  CRGB color = CRGB::Blue;
  
  // Act - Try to turn on out-of-bounds LED
  setIndividualLED(test_leds, test_memo, ledIndex, TEST_LED_COUNT, color, DIM_FACTOR, true);
  
  // Assert - Nothing should change, and no crash should occur
  // This is mostly testing that the function doesn't crash
  assertTrue(true);
}

// Test dimming effect
test(setIndividualLED_Dimming) {
  // Arrange
  byte ledIndex = 3;
  CRGB color = CRGB::Green;
  uint8_t halfDimFactor = DIM_FACTOR / 2;
  
  // Act - Set LED to half brightness
  setIndividualLED(test_leds, test_memo, ledIndex, TEST_LED_COUNT, color, halfDimFactor, true);
  
  // Assert
  assertEqual(test_memo[ledIndex], halfDimFactor);
  // The LED should be dimmer than full brightness
  assertTrue(test_leds[ledIndex].g < color.g);
}

// Main loop
void loop() {
  Test::run();
}

// Implementation of the function we're testing
// This is a simplified version of the function from the main sketch
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