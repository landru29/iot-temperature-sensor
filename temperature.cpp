#include <Arduino.h>
#include "temperature.h"

#include <OneWire.h>
#include <Wire.h>


OneWire  ds(TEMPERATURE_PIN);


int Temperature::count = 0;
Temperature** Temperature::sensor = (Temperature**)malloc(0);

Temperature::Temperature(byte* address) {
  for (int i=0; i<8; i++) {
    this->addr[i] = address[i];
  }
}

float Temperature::performMeasure() {
  byte type_s;
  byte data[12];
  byte i;
  byte present = 0;

   // the first ROM byte indicates which chip
  switch (this->addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      return 0;
  } 

  ds.reset();
  ds.select(this->addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(this->addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }

  return (float)raw / 16.0;
}

void Temperature::searchSensors() {
  bool more = true;
  byte addr[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  ds.reset_search();
    
  while (true) {
    Serial.println("Searching sensor ...");
    if (!ds.search(addr)) {
      Serial.println("No more sensor");
      return;
    }
    
    Serial.println("  => Found something");
    switch (addr[0]) {
      case 0x10:
      case 0x28:
      case 0x22:
        Serial.println("     ---> This is a sensor !");
        Temperature::count++;
        Temperature::sensor = (Temperature**)realloc(Temperature::sensor, Temperature::count * sizeof(Temperature*));
        Temperature::sensor[Temperature::count-1] = new Temperature(addr);
        break;
      default:
        Serial.println("Device is not a DS18x20 family device:");
        Serial.println(addr[0]);
    } 
  }
}