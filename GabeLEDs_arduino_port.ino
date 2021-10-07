/*
 * 1. 
 * 2. Use conductive touch pads for 3 buttons
 * 3. 
 * 4. wifi update
 * 5. 
 * 6. fix single scan 'crash' after 1st pass
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
int rgb_red[12] = {255,255,255,127,0,0,0,0,0,127,255,255}; // update getRandomRGB() if this array changes size
int rgb_green[12] = {0,127,255,255,255,255,255,127,0,0,0,0}; // update getRandomRGB() if this array changes size
int rgb_blue[12] = {0,0,0,0,0,127,255,255,255,255,255,127}; // update getRandomRGB() if this array changes size
int r, g, b; // global RGB variables
// ********************************************************************

int lastDebounceTime = millis();
int mode = 0;
const int offBtn = 4;           // pin number
const int modeBtn = 5;          // pin number
const int dimBtn = 6;           // pin number
int brightnessVal = 77;     // initial strip brightnessVal
int dimLevel = 2;               // initial dim val used in buttonCheck()
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
  btnCheck();
  autoOff();
  
  switch (mode){
  case '0': //
    off(5);
    break;
  case '1': //
    Serial.print("Mode 1: ");
    Serial.println(mode);
    solidFill(255,240,10,5); // Warm White for reading
    break;
  case '2': //
    Serial.print("Mode 2: ");
    Serial.println(mode);
    randomFill(10);// any value > 0 will scroll strip to new color
    break;
  case '3': //
    Serial.print("Mode 3: ");
    Serial.println(mode);
    rainbow(10);
    break;
  case '4': //
    Serial.print("Mode 4: ");
    Serial.println(mode);
    rainbowScroll(2);
    break;
  case '5': //
    Serial.print("Mode 5: ");
    Serial.println(mode);
    getRandomRGB();
    theaterChase(strip.Color(r, g, b), 5);
    break;
  case '6': //
    Serial.print("Mode 6: ");
    Serial.println(mode);
    theaterChaseRainbow(100);
    break;
  case '7': //
    Serial.print("Mode 7: ");
    Serial.println(mode);
    twinkleFill();
    break;
  case '8': //
    Serial.print("Mode 8: ");
    Serial.println(mode);
    singleScan(10,5,true); // (speed, number of pixels, down and back)
    break;
  default: 
    Serial.print("Default: ");
    Serial.println(mode);
    mode = 0;
    break;
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
      dimLevel++;
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
            brightnessVal = 26; // 26/255 = 10%
            break;
          case '1': //
            brightnessVal = 77; // 26/255 = 30%
            break;
          case '2': //
            brightnessVal = 154; // 154/255 = 60%
            break;
          case '3': //
            brightnessVal = 255; // 255/255 = 100%
            break;
          default: 
            brightnessVal = 26; // 26/255 = 10%
            dimLevel = 0;
            break;
        }
      }
    }
  }
}// end btnCheck()

void off(uint8_t wait){ // clear all LEDs
  if(wait !=0){
    for(uint16_t i=0;i<strip.numPixels();i++){
      strip.setPixelColor(i,(0,0,0));
      delay(wait);
      strip.show();
      Serial.print("off(), wait != 0");
      Serial.println(mode);
    }
  }
  else {
      strip.clear();// turn entire strip off at once
      strip.show();
      Serial.print("off(), wait == 0");
      Serial.println(mode);
  }
}

void autoOff(){
  if (millis() - lastAutoOff >= autoOffTime) {
    lastAutoOff = millis();
    mode = 0;
    Serial.print("autoOff()");
    Serial.println(mode);
  }
}

void getRandomRGB(){ // pick a random color from the RBG arrays above
  int i = random(0,12); // update this if RGB arrays change size
  r = rgb_red[i] * brightnessVal / 255;
  g = rgb_green[i] * brightnessVal / 255;
  b = rgb_blue[i] * brightnessVal / 255;
  Serial.print("getRandomRGB()");
  Serial.println(mode);
}

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
  Serial.print("Wheel()");
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
      btnCheck();
      Serial.print("rainbow()");
    }
    strip.show();
    delay(wait);
  }
}

void rainbowScroll(int wait) {
  // Hue of first pixel runs one complete loop through the color wheel (65536).
  for(long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      btnCheck();
      Serial.print("rainbowScroll()");
    }
    strip.show();
    delay(wait);
  }
}

void solidFill(uint8_t r, uint8_t g,uint8_t b, uint8_t wait){
  for(uint16_t i=0; i<pixelCount; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
    btnCheck();
    if(wait>0){
      strip.show();
      delay(wait);  // controls speed of incremental fill
      Serial.print("solidFil(), wait > 0");
    }
  }
  if(wait==0){ // if no wait, fill strip all at once
    strip.show();  
    Serial.print("solidFill(), wait == 0");
  }
}

void randomFill(uint8_t wait){
  getRandomRGB();
  solidFill(r,g,b,wait);
  Serial.print("randomFill()");
}

void theaterChase(uint32_t color, int wait) { // 'wait' var determines chase speed (i think)
  for(uint8_t a=0; a<30; a++) {   // Repeat 30 times...
    for(uint8_t b=0; b<3; b++) {  //  'b' sets gap between pixels
      for(uint16_t c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        btnCheck();
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
      Serial.print("theaterChase()");
    }
  }
}

void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' sets gap between pixels
      strip.clear();
      for(int c=b; c<strip.numPixels(); c += 3) {
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color);
        btnCheck();
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
      Serial.print("theaterChaseRainbow()");
    }
  }
}

void twinkleFill(){
  strip.clear();
  for(int a=0; a<pixelCount*2;a++){
    getRandomRGB();
    strip.setPixelColor(random(pixelCount), strip.Color(r, g, b)); //multi-color twinkle
    strip.show();
    delay(5);
    btnCheck();
    Serial.print("twinkleFill()");
  }
}

void singleScan(uint8_t wait, uint8_t trailLength, bool scan){ // scan travels down and back
  getRandomRGB();
  for(uint16_t i=0; i<(strip.numPixels()+trailLength); i++) {
    strip.clear();
    strip.setPixelColor(i,r,g,b);
    for(uint16_t n=1; n<trailLength; n++){
      uint8_t rate = 1-((n+1)/trailLength);
      strip.setPixelColor(i-n,r*rate,g*rate,b*rate);
    }
    strip.show();
    delay(wait);
    btnCheck();
    Serial.print("singleScan(), up");
  }
  if (scan == true){
    getRandomRGB();
    for(uint16_t i=strip.numPixels(); i>(0-trailLength); i--) {
      strip.clear();
      strip.setPixelColor(i,r,g,b);
      for(uint16_t n=1; n<trailLength; n++){
        uint8_t rate = 1-((n+1)/trailLength);
        strip.setPixelColor(i+n,r*rate,g*rate,b*rate);
      }
      strip.show();
      delay(wait);
      btnCheck();
      Serial.print("singleScan(), down");
    }
  }
}
