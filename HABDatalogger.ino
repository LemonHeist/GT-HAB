#include <stdio.h>
#include <TinyGPSPlus.h>
#include <DHT20.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <SPI.h>

static const int GPSrx = 4;
static const int GPStx = 3;
static const int GPSBaud = 9600;
static const int SDcs = 5;

TinyGPSPlus gps;
SoftwareSerial ss(GPSrx, GPStx);
DHT20 dht;
File datalog;

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);

  if(dht.begin()) {
    Serial.println("DHT20 initialized");
  } else {
    Serial.println("DHT20 failed to initialize");
  }

  if(SD.begin(SDcs)) {
    Serial.println("SD Card initialized");
  } else {
    Serial.println("SD Card failed to intialize");
  }

  delay(5000);

  /*
  if(gps.charsProcessed() < 10) {
    Serial.println("GPS failed to initialze");
  } else {
    Serial.println("GPS initialized");
  }
  */

  datalog = SD.open("datalog.txt", FILE_WRITE);

  datalog.println("TIME----|LAT-----|LNG-----|ALT-----|TEMP----|HUM-----");
  datalog.close();

}

void loop() {
  datalog = SD.open("datalog.txt", FILE_WRITE);
  String data = formatData();
  datalog.println(data);
  Serial.println(data);
  datalog.close();
  smartDelay(1000);
}

static String formatData() {
  String lat, lng, alt, time, temp, hum;
  
  if (gps.location.isValid()) {
    lat = String(gps.location.lat(), 4);
    lng = String(gps.location.lng(), 4);
    alt = String(gps.altitude.meters(), 4);
    time = formatTime(gps.time);
  } else {
    lat = "000.000";
    lng = "000.0000";
    alt = "000.0000";
    time = "00:00:00";
  }

  dht.read();
  temp = String(dht.getTemperature(), 4);
  hum = String(dht.getHumidity(), 4);
  
  String data = String();

  data.concat(time);
  data.concat(",");
  data.concat(lat);
  data.concat(",");
  data.concat(lng);
  data.concat(",");
  data.concat(alt);
  data.concat(",");
  data.concat(temp);
  data.concat(",");
  data.concat(hum);
  
  return data;
}

static String formatTime(TinyGPSTime &t) {
  char sz[32];
  sprintf(sz, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
  return sz;  
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}