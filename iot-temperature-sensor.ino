#include "temperature.h"
#include "Wire.h"
#include "wifi.h"
#include "metrics.h"
#include "warp10.h"
#include "config.h"

WifiConnect* myWifi;
Metrics* metricsClient;
WarpTen* warp10;


void setup() {
  Wire.begin(); 
  Serial.begin(115200);

  myWifi = new WifiConnect();
  metricsClient = new Metrics(SENSOR_NAME);
  warp10 = new WarpTen();
  
  Temperature::searchSensors();

  Serial.println("IOT Temperature");
}

void loop() {
  if (Temperature::count) {
    
    // connect to the local wifi
    myWifi->connectWithCred(WIFI_SSID, WIFI_PASSWORD);

    // Perform the measure
    float celcius = Temperature::sensor[0]->performMeasure();

    // get the current timestamp from a warp10 server
    long ts = warp10->getTimestamp();

    // Push metrics to OVH
    metricsClient->addValueAndFlush(celcius, ts);

    // Disconnect from the local wifi
    myWifi->disconnect();
    
    Serial.println(celcius);
  } else {
    Serial.println("No sensor available");
  }

  // wait for 60 seconds
  delay(60000);

}
