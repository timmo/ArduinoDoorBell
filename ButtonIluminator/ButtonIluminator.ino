#include <FastLED.h>

#define NUM_LEDS 3
#define DATA_PIN 11
#define LED_TYPE WS2812B

CRGB leds[NUM_LEDS];
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  60

static uint8_t brightness = 0;
static uint8_t direction = 1;

void setup() { 
  delay(1000); // delay for recovery
  
  FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  
  for(int l = 0; l < NUM_LEDS; l++) { 
    leds[l] = CHSV( HUE_YELLOW, 255, 255);
  }
}

void loop() { 
//      for(int dot = 0; dot < NUM_LEDS; dot++) { 
//            leds[dot] = CRGB::Blue;
//            FastLED.show();
//            // clear this led for the next time around the loop
//            leds[dot] = CRGB::Black;
//            delay(500);
//        }
      
      if (brightness == 255) 
        direction = -1;
      else if (brightness == 0) 
        direction = 1;

      brightness = brightness + (direction * 1);

      CHSV hsv = CHSV( 64, 255, brightness);
      
      for(int l = 0; l < NUM_LEDS; l++) { 
//            leds[l].fadeLightBy( brightness );
          leds[l] = hsv;
        }
      FastLED.delay(20);
      
}
