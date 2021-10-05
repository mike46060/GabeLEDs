/*
 * 1. 
 * 2. Use conductive touch pads for 3 buttons
 * 3. 
 * 4. wifi update
 * 5. twinkle fill
 * 6. single scan 
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
 */

#include <Adafruit_NeoPixel.h>

#define pixelPin 7
#define pixelCount 8

// **** Below is Python RGB color list converted to Arduino Arrays ****
// RGB = [(255,0,0),(255,127,0),(255,255,0),(127,255,0),(0,255,0),(0,255,127),(0,255,255),(0,127,255),(0,0,255),(127,0,255),(255,0,255),(255,0,127)]
uint8_t rgb_red[12] = {255,255,255,127,0,0,0,0,0,127,255,255};
uint8_t rgb_green[12] = {0,127,255,255,255,255,255,127,0,0,0,0};
uint8_t rgb_blue[12] = {0,0,0,0,0,127,255,255,255,255,255,127};
// ********************************************************************

long randomRGB;
int previousRainbowWait = 0;
int rainbowWait = 0;
int lastDebounceTime = millis();
int mode = 0;
const int offBtn = 4;           // pin number
const int modeBtn = 5;          // pin number
const int dimBtn = 6;           // pin number
short brightnessVal = 77;       // initial strip brightnessVal
int dimLevel = 2;                // initial dim val used in buttonCheck()
int dimLast = 2;
int autoOffTime = 3600000;      // 1hr auto off delay in mS
unsigned long lastAutoOff = millis();  // 1hr auto off delay

Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelCount, pixelPin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
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
  off(0); // Any value > 0 will scroll strip off
  solidFill(255,240,50,0); // solidFill(red, green, blue, delay)
  //rainbow(10);
  //twinkleFill();
  //randomFill(0); // any value > 0 will scroll strip to new color
  solidFill(255,0,0,0); // solidFill(red, green, blue, delay)
  solidFill(0,255,0,0); // solidFill(red, green, blue, delay)
  solidFill(0,0,255,0); // solidFill(red, green, blue, delay)
  //singleScan(0,5,false); // singleScan(delay, trailing pixels, cyclon movement)
  off(50); // Any value > 0 will scroll strip off
  }

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.setBrightness(brightnessVal);
    strip.show();
    btnCheck();
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
    btnCheck();
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
  btnCheck();
}

void randomFill(uint8_t wait){
  uint8_t r = rgb_red[random(0,sizeof(rgb_red))];
  uint8_t g = rgb_green[random(0,sizeof(rgb_green))];
  uint8_t b = rgb_blue[random(0,sizeof(rgb_blue))];
  solidFill(r,g,b,wait);
  btnCheck();
}

void theaterChase(uint32_t color, int wait) { // 'wait' var determines chase speed (i think)
  for(uint8_t a=0; a<10; a++) {  // Repeat 10 times...
    for(uint8_t b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(uint16_t c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      btnCheck();
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
      btnCheck();
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void btnCheck() {  //**************************************************
  if (millis() - lastDebounceTime > 200) {       // 200ms debounce time
    if (digitalRead(offBtn) == LOW) {                // debounce timeUpBtn
      lastDebounceTime = millis();//
      mode = 0;
      Serial.print("Mode: ");
      Serial.println(mode);
  }
  if (digitalRead(modeBtn) == LOW) {                // debounce timeDnBtn
      lastDebounceTime = millis();//
      mode++;
      lastAutoOff = millis();
      Serial.print("Mode: ");
      Serial.println(mode);
  }
  if (digitalRead(dimBtn) == LOW) {                // debounce timeDnBtn
      lastDebounceTime = millis();
      dimLevel++;  // dim levels 0=26, 1=77, 2=154, 3=255
      lastAutoOff = millis();
      if (dimLevel != dimLast){
        if (dimLevel > 3){
          dimLevel = 0;
        }
        dimLast = dimLevel;
        Serial.print("DimLevel: ");
        Serial.println(dimLevel);
        switch (dimLevel){
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
            dimLevel = 0;
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
}

void singleScan(uint8_t wait, uint8_t trailLength, bool cylon){
  uint8_t r = rgb_red[random(0,sizeof(rgb_red))];
  uint8_t g = rgb_green[random(0,sizeof(rgb_green))];
  uint8_t b = rgb_blue[random(0,sizeof(rgb_blue))];
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.clear();
    strip.setPixelColor(i,r,g,b);
    for(uint16_t n=1; n<trailLength; n++){
      uint8_t rate = 1-((n+1)/trailLength);
      strip.setPixelColor(i-n,r*rate,g*rate,b*rate);
    }
    strip.show();
    delay(wait);
    //btnCheck();
//    if(mode !=8){
//      return;
//    }
  }
  if (cylon == true){
    for(uint16_t i=0; i<strip.numPixels(); i--) {
      strip.clear();
      strip.setPixelColor(i,r,g,b);
      for(uint16_t n=1; n<trailLength; n++){
        uint8_t rate = 1-((n+1)/trailLength);
        strip.setPixelColor(i+n,r*rate,g*rate,b*rate);
      }
      strip.show();
      delay(wait);
      //btnCheck();
//      if(mode !=8){
//        return;
//      }
    }
  }
}
