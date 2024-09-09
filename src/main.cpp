#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    134
#define MIDDLE_1_LED  13
#define MIDDLE_2_LED  67
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 100
#define POT_MASK 0b1111111111100000

CRGB leds[NUM_LEDS];

// Define pins for the potentiometers
#define POT_R1  A0  // Red control for left side
#define POT_G1  A1  // Green control for left side
#define POT_B1  A2  // Blue control for left side
#define POT_R2  A3  // Red control for right side
#define POT_G2  A4  // Green control for right side
#define POT_B2  A5  // Blue control for right side

// Define the class colorControl
class colorControl {
  private:
    int pins[3]; // Array to store pins for Red, Green, Blue potentiometers
    int lastStates[3]; // Array to store the last values of potentiometers

  public:
    // Constructor to initialize pins using an array and set initial states
    colorControl(int pinArray[3]) {
      for (int i = 0; i < 3; i++) {
        pins[i] = pinArray[i];
        lastStates[i] = analogRead(pins[i]); // Initialize last states
      }
    }

    // Method to get the color based on the potentiometer values
    CRGB getColor() {
      return CRGB(map(analogRead(pins[0]) & POT_MASK, 0, 1023, 0, 255),  // Map Red value
                  map(analogRead(pins[1]) & POT_MASK, 0, 1023, 0, 255),  // Map Green value
                  map(analogRead(pins[2]) & POT_MASK, 0, 1023, 0, 255)); // Map Blue value
    }

    // Method to update the last known values of the potentiometers
    void updateStates() {
      for (int i = 0; i < 3; i++) {
        lastStates[i] = analogRead(pins[i]); // Update each potentiometer value
      }
    }
};

// Arrays for the pin numbers for each side
int leftPins[3] = {POT_R1, POT_G1, POT_B1};
int rightPins[3] = {POT_R2, POT_G2, POT_B2};

// Create pointers for colorControl instances
colorControl* leftControl;
colorControl* rightControl;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  
  FastLED.setBrightness(1);
  fill_solid(leds, NUM_LEDS , CRGB::Black);
  FastLED.show();
  delay(2000);
  fill_solid(leds, NUM_LEDS , CRGB::Red);
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS , CRGB::Green);
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS , CRGB::Blue);
  FastLED.show();
  delay(1000);

  FastLED.setBrightness(BRIGHTNESS);
  // Initialize colorControl instances inside setup
  leftControl = new colorControl(leftPins);
  rightControl = new colorControl(rightPins);
}

void loop() {
  // Get the updated colors
  CRGB colorLeft = leftControl->getColor();
  CRGB colorRight = rightControl->getColor();

  // Update the LED strip with the new colors for both sides
  fill_solid(leds, MIDDLE_1_LED, colorLeft);                // Left side 1
  fill_solid(leds + MIDDLE_1_LED, MIDDLE_2_LED, colorRight); // Right side
  fill_solid(leds + MIDDLE_1_LED + MIDDLE_2_LED, NUM_LEDS - MIDDLE_1_LED - MIDDLE_2_LED, colorLeft); // Left side 2

  // Show the updated LEDs
  FastLED.show();

  // Update the states of both potentiometer sets after each loop
  leftControl->updateStates();
  rightControl->updateStates();

  delay(50); // Short delay to reduce flickering and debounce
}