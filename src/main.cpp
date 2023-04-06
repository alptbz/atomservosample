//=====================================================================
// LVGL : How-to
//      : M5Core2 slow fps. Scrol slider breaks when moving horizontally
// 2 Dec,2020
// https://forum.lvgl.io
//  /t/m5core2-slow-fps-scrol-slider-breaks-when-moving-horizontally/3931
// Arduino IDE 1.8.15
// https://github.com/mhaberler/m5core2-lvgl-demo
// Check : 2021.06.13 : macsbug
// https://macsbug.wordpress.com/2021/06/18/how-to-run-lvgl-on-m5stack-esp32/
//=====================================================================

#include <Arduino.h>
#include <M5Atom.h>
#include "networking.h"
#include <Wire.h>
#include <SPI.h>  
#include <AtomMotion.h>


void init_sensor() {
  Wire.begin();
}

AtomMotion _AtomMotion;

CRGB mainLED;


void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // Parse Payload into String
  char * buf = (char *)malloc((sizeof(char)*(length+1)));
  memcpy(buf, payload, length);
  buf[length] = '\0';
  String payloadS = String(buf);
  payloadS.trim();
  Serial.println(payloadS);

  if(String(topic) == "servo/angle") {
    Serial.println(payloadS);
    int angle = payloadS.toInt();
    if(angle >= 0 && angle < 360) {
      _AtomMotion.SetServoAngle(1, angle);
    }
  }
}

void setup() {
  M5.begin(true, false, true);
  _AtomMotion.Init();
  FastLED.addLeds<NEOPIXEL, 27>(&mainLED, 1);
  FastLED.setBrightness(10);
  Serial.begin(115200);
  Serial.println("Conneting to wifi...");
  delay(5);
  setup_wifi();
  mqtt_init(mqtt_callback);
  
}

long nextSensorRead = 0;
unsigned long next_lv_task = 0;

uint8_t swapState = false;


//=====================================================================
void loop(){
  M5.update();
  mqtt_loop();
 if(nextSensorRead < millis()) {
  if(M5.Btn.isPressed()) {
    if(swapState) {
        mainLED = CRGB::Red;
        FastLED.show();
      }else{
        mainLED = CRGB::Blue;
        FastLED.show();
      }
      swapState = !swapState;
      nextSensorRead = millis() + 1000;
      }
  }
}
//=====================================================================
