#include "temperature.h"
#include "Wire.h"
#include "wifi.h"
#include "metrics.h"
#include "warp10.h"
#include "config.h"

WifiConnect* myWifi;
Metrics* metricsClient;
WarpTen* warp10;
long ts=0;


void setup() {
  Wire.begin(); 
  Serial.begin(115200);

  myWifi = new WifiConnect();
  metricsClient = new Metrics(SENSOR_NAME);
  warp10 = new WarpTen();
  
  Temperature::searchSensors();

  Serial.println("IOT Temperature");
 

}

void wait(int seconds) {
  delay(seconds * 1000);
  ts += seconds;
}

void loop() {
  if (ts==0) {
    myWifi->connectWithCred(WIFI_SSID, WIFI_PASSWORD);
    ts = warp10->getTimestamp();
    myWifi->disconnect();
  }
  
  if (Temperature::count) {
    float celcius = Temperature::sensor[0]->performMeasure();
    
    metricsClient->addValue(celcius, ts);
    
    Serial.println(celcius);
  } else {
    Serial.println("No sensor available");
  }

  if (metricsClient->getBufferSize() > 10) {
    myWifi->connectWithCred(WIFI_SSID, WIFI_PASSWORD);
    metricsClient->flushData();
    ts = warp10->getTimestamp();
    myWifi->disconnect();
  }
    
  wait(10);

}
