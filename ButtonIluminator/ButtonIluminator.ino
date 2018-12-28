#include <FastLED.h>
#include <Bounce2.h>

// hardware connections
#define LED_DATA_PIN 11
#define BUTTON_PIN 2

// FastLED
#define NUM_LEDS 3
#define LED_TYPE WS2812B
#define FRAMES_PER_SECOND  60
CRGB leds[NUM_LEDS];

// Button
Bounce button = Bounce();
const int BUTTON_DEBOUNCE_MILLIS = 50;

void setup() { 
  delay(1000); // delay for recovery

  // button debouncing  
  button.attach(BUTTON_PIN,INPUT_PULLUP);
  button.interval(BUTTON_DEBOUNCE_MILLIS);
  
  // led setup
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN>(leds, NUM_LEDS);
  for(int l = 0; l < NUM_LEDS; l++) { 
    leds[l] = CHSV( 45, 255, 170);
  }
  FastLED.show();
}

void loop() { 

  button.update();  

  // button press
  if (button.fell()) {
    for(int l = 0; l < NUM_LEDS; l++) { 
      leds[l] = CHSV( 65, 255, 255);
    }
    FastLED.show();
  }

  // button released
  if (button.rose()) {
    for(int l = 0; l < NUM_LEDS; l++) { 
      leds[l] = CHSV( 45, 255, 170);
    }
    FastLED.show();
  }
}
