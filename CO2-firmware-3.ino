/*
 * Include needed libraries
 */
#include <Arduino.h>
#include <co2-sense.h>

#define LED_DATA_PIN 9    // Data pin connected to LED data line through 220 Ohm resistor
#define LED_COUNT 4         // Number of installed LEDs
#define HM10_RX 13           // TX pin of HM10 module (RX of MCU)
#define HM10_TX 12           // RX pin of HM10 module (TX of MCU)
#define MHZ19_RX 10        // TX pin of CO2 sensor (RX of MCU)
#define MHZ19_TX 11        // RX pin of CO2 sensor (TX of MCU)

#define DEFAULT_BRIGHTNESS 128    //Default brightness level of LEDs (8 bit range: 0-255);

Co2Sense::ledRing myLEDs(LED_DATA_PIN,LED_COUNT,DEFAULT_BRIGHTNESS);
Co2Sense::SerialInterface HM10(HM10_RX,HM10_TX);
Co2Sense::Co2Sensor MHZ19(MHZ19_RX,MHZ19_TX);

void setup() {
  Serial.begin(9600);
  myLEDs.begin();
  MHZ19.begin();
  HM10.begin(9600);
}

void loop() {
    Co2Sense::CommandListener(HM10,myLEDs,MHZ19);
    Co2Sense::co2_control2(MHZ19,myLEDs,HM10);
}
