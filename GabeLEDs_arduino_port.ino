/*
 * 1. 
 * 2. Use conductive touch pads for 3 buttons
 * 3. 
 * 4. wifi update
 * 5. twinkle fill
 * 6. single scan 
 * 
 * 
 * uint8_t = 0 to 255 /memory = 1 bytes
 * uint16_t = 0 to 65535 /memory = 2 bytes
 * uint32_t = 0 to 4294967295 /memory = 4 bytes
 * uint64_t = 0 to 18446744073709551615 /memory = 8 bytes 
 * long = -2,147,483,648 to 2,147,483,647 /memory = 4 bytes
 * unsigned long = 0 to 4,294,967,295 /memory = 4 bytes
 * 
 * int = size depends on chip (16-bit = 2 bytes)/(32-bit = 4 bytes)/(64-bit = 8 bytes)
 * 
 * 
 * Basic non-blocking delay *
 *    if (millis() - previousTime >= delayTime) {
 *      previousTime = millis();
 *    }
 * 
 */

#include <Adafruit_NeoPixel.h>

#define pixelPin 14
#define pixelCount 50

// **** Below is Python RGB color list converted to Arduino Arrays ****
// RGB = [(255,0,0),(255,127,0),(255,255,0),(127,255,0),(0,255,0),(0,255,127),(0,255,255),(0,127,255),(0,0,255),(127,0,255),(255,0,255),(255,0,127)]
uint8_t rgb_red[12] = {255,255,255,127,0,0,0,0,0,127,255,255};
uint8_t rgb_green[12] = {0,127,255,255,255,255,255,127,0,0,0,0};
uint8_t rgb_blue[12] = {0,0,0,0,0,127,255,255,255,255,255,127};
// ********************************************************************

long randomRGB;
unsigned long previousRainbowWait = 0;
unsigned long rainbowWait = 0;
unsigned long lastDebounceTime = millis();
uint8_t mode = 0;
const byte offBtn = 4;    // pin number
const byte modeBtn = 5;   // pin number
const byte dimBtn = 7;    // pin number
short brightnessVal = 77; // initial strip brightnessVal
uint8_t dim = 2;          // initial dim val used in buttonCheck()
uint8_t dimLast = 2;      // used in buttonCheck()
uint8_t autoOffTime = 3600;      // 1hr auto off delay
uint8_t lastAutoOff = millis();  // 1hr auto off delay


Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelCount, pixelPin, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(pixelPin, OUTPUT);
  pinMode(offBtn, INPUT_PULLUP); // use a touch enabled pin to avoid buttons
  pinMode(modeBtn, INPUT_PULLUP);// use a touch enabled pin to avoid buttons
  pinMode(dimBtn, INPUT_PULLUP); // use a touch enabled pin to avoid buttons
  
  randomSeed(analogRead(0));

  strip.begin();
  strip.clear();
  strip.show();
}

void loop() {
  autoOff();
  switch (mode){
    case 0: // Off
      off(0); // any value > 0 will scroll strip off
    break;
    case 1: // Warm White
      solidFill(255,240,50,0); // first three values are color, the last is scroll delay
      lastAutoOff = millis();
    break;
    case 2: // Rainbow
      rainbow(1000);
      lastAutoOff = millis();
    break;
    case 3: // Random Twinkle
      twinkleFill();
      lastAutoOff = millis();
    break;
    case 4: // Random Color Fill
      randomFill(0); // any value > 0 will scroll strip to new color
      lastAutoOff = millis();
    break;
    case 5: // Solid Red
      solidFill(255,0,0,0); // first three values are color, the last is scroll delay
      lastAutoOff = millis();
    break;
    case 6: // Solid Green
      solidFill(0,255,0,0); // first three values are color, the last is scroll delay
      lastAutoOff = millis();
    break;
    case 7: // Solid Blue
      solidFill(0,0,255,0); // first three values are color, the last is scroll delay
      lastAutoOff = millis();
    break;
    case 8: // Single Pixel Scan
      singleScan();
      lastAutoOff = millis();
    break;
    default:
    off(0);
    mode = 0;
    break;
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.setBrightness(brightnessVal);
    strip.show();
    btnCheck()
    delay(wait); // Figure out how to remove this blocking wait.
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(uint8_t WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void solidFill(uint8_t r, uint8_t g,uint8_t b, uint8_t wait){
  for(uint16_t i=0; i<pixelCount; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
    strip.setBrightness(brightnessVal);
    strip.show();
    btnCheck()
    delay(wait);  // controls speed of incremental fill
  }
}

void off(uint8_t wait){ // clear all LEDs
//  for(uint16_t i=0;i<strip.numPixels();i++){
//    strip.setPixelColor(i,(0,0,0));
//    delay(wait); 
//    }
  strip.clear();// turn entire strip off at once
  strip.show();
  btnCheck()
}

void randomFill(uint8_t wait){
  uint8_t r = rgb_red[random(0,sizeof(rgb_red))];
  uint8_t g = rgb_green[random(0,sizeof(rgb_green))];
  uint8_t b = rgb_blue[random(0,sizeof(rgb_blue))];
  solidFill(r,g,b,wait);
  btnCheck()
}

void theaterChase(uint32_t color, int wait) { // 'wait' var determines chase speed (i think)
  for(uint8_t a=0; a<10; a++) {  // Repeat 10 times...
    for(uint8_t b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(uint16_t c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      btnCheck()
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void twinkleFill(){
  for (uint8_t i; (pixelCount * 3); i++);
//        btn_check()
//        if mode == 3:
//            pixels[random.randrange(num_pixels)] = (random.choice(RGB))
//            pixels.show()
//            time.sleep(DELAY)
}

void theaterChaseRainbow(uint8_t wait) {
  uint8_t firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(uint8_t a=0; a<30; a++) {  // Repeat 30 times...
    for(uint8_t b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(uint16_t c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        uint16_t hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      btnCheck()
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void btnCheck() {  //**************************************************
  if (digitalRead(offBtn) == LOW) {                // debounce timeUpBtn
    if (millis() - lastDebounceTime > 200ul) {              // 100ms debounce time
      mode = 0;
      lastDebounceTime = millis();//
    }
  }
  if (digitalRead(modeBtn) == LOW) {                // debounce timeDnBtn
    if (millis() - lastDebounceTime > 200ul) {              // 100ms debounce time
      mode++;
      lastDebounceTime = millis();//
    }
  }
  if (digitalRead(dimBtn) == LOW) {                // debounce timeDnBtn
    if (millis() - lastDebounceTime > 200ul) {              // 100ms debounce time
      dim++;  // dim levels 0=26, 1=77, 2=154, 3=255
      lastDebounceTime = millis();//
      lastAutoOff = millis();
      if (dim != dimLast){
        dimLast = dim;
        switch (dim){
          case '0': //
            brightnessVal = 26;
            break;
          case '1': //
            brightnessVal = 77;
            break;
          case '2': //
            brightnessVal = 154;
            break;
          case '3': //
            brightnessVal = 255;
            break;
          default: 
            brightnessVal = 26;
            dim = 0;
            break;
        }
      }
    }
  }
}// end btnCheck()

void autoOff(){
  if (millis() - lastAutoOff >= autoOffTime) {
    lastAutoOff = millis();
    mode = 0;
}

void singleScan(){
//    randomColor = (random.choice(RGB))
//    for j in range(1):   
//        for i in range(num_pixels):
//            btn_check()
//            if mode == 8:
//                pixels.fill(0)
//                pixels[i] = (randomColor)
//                pixels.show()
//                #time.sleep(DELAY)    
//            else:
//                return
//        for i in range(num_pixels-2, 0,-1):
//            btn_check()
//            if mode == 8:
//                pixels.fill(0)
//                pixels[i] = (randomColor)
//                pixels.show()
//                time.sleep(DELAY)    
//            else:
//                return
}
