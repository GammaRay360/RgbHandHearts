#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN     8
#define NUM_LEDS    7
#define MIDDLE_LED  3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// Define pins for the toggle switches
#define SW_R1  2  // Red control for left side
#define SW_G1  3  // Green control for left side
#define SW_B1  4  // Blue control for left side
#define SW_R2  5  // Red control for right side
#define SW_G2  6  // Green control for right side
#define SW_B2  7  // Blue control for right side

// Define the class colorSwitchs
class colorSwitchs {
  private:
    int pins[3]; // Array to store pins for Red, Green, Blue
    int pinStates[3]; // Array to store the last states of switches
    bool isAnyChanged = false;

  public:
    // Constructor to initialize pins using an array and set initial states
    colorSwitchs(int pinArray[3]) {
      for (int i = 0; i < 3; i++) {
        pins[i] = pinArray[i];
        pinMode(pins[i], INPUT_PULLUP);
        pinStates[i] = 1 - digitalRead(pins[i]); // Initialize last states. Use NOT because of the pullup resistor
        Serial.print(i);
        Serial.print(": ");
        Serial.println(pinStates[i]);
      }
    }

    // Method to get the color based on the last known states of the switches
    CRGB getColor() {
      return CRGB(pinStates[0] * 255, pinStates[1] * 255, pinStates[2] * 255);
    }

    // Method to check if any of the switches' states have changed
    bool isChanged() {
      return isAnyChanged;
    }

    // Method to update the last known states of the switches
    void updateStates() {
      isAnyChanged = false;
      for (int i = 0; i < 3; i++) {
        bool lastState = pinStates[i];
        pinStates[i] = 1 - digitalRead(pins[i]); // Update each switch state.  Use NOT because of the pullup resistor
        if(pinStates[i] != lastState)
          isAnyChanged = true;
      }
    }
};


// Arrays for the pin numbers for each side
int leftPins[3] = {SW_R1, SW_G1, SW_B1};
int rightPins[3] = {SW_R2, SW_G2, SW_B2};

// Create pointers for colorSwitchs instances
colorSwitchs* leftSwitches;
colorSwitchs* rightSwitches;


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(5);
  
  // Starting indication
  fill_solid(leds, NUM_LEDS, CRGB(255,0,0));
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  FastLED.show();
  delay(100);

  // Initialize colorSwitchs instances inside setup
  leftSwitches = new colorSwitchs(leftPins);
  rightSwitches = new colorSwitchs(rightPins);
  delay(100);
}

void loop() {
  // Check if there is a change in the left or right switches
  if (leftSwitches->isChanged() || rightSwitches->isChanged()) {
    // Get the updated colors
    CRGB colorLeft = leftSwitches->getColor();
    CRGB colorRight = rightSwitches->getColor();

    // Update the LED strip with the new colors for both sides
    fill_solid(leds, MIDDLE_LED, colorLeft);                // Left side
    fill_solid(leds + MIDDLE_LED, NUM_LEDS - MIDDLE_LED, colorRight); // Right side

    // Show the updated LEDs
    FastLED.show();
  }

  // Update the states of both switch sets after each loop, regardless of changes
  leftSwitches->updateStates();
  rightSwitches->updateStates();

  delay(100); // Short delay to reduce flickering and debounce
}
