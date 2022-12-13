#include <ArduinoZlib.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <iostream>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>



// Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}
WiFiMulti WiFiMulti;


void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("xxx", "xxx");

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  setClock();  
}
uint8_t *outbuf;
void loop() {
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client->setInsecure();
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, "https://192.168.2.144:8082/test")) {  // HTTPS
      // if (https.begin(*client, "https://devapi.qweather.com/v7/weather/24h?location=101280306&key=ea0b39a3b04c472a83a310a1c1f8efdf")) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        https.addHeader("Accept-Encoding", "gzip");
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          if(httpCode == HTTP_CODE_OK) {
            Serial.printf("code=%d\n", httpCode);
              int len=-1;
            Serial.printf("size=%d\n", len);
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

              uint8_t buff[2048] = { 0 };
              WiFiClient * stream = https.getStreamPtr();
              Serial.println("stream ok");
            if(https.connected() && (len > 0 || len == -1)) {
                // get available data size
                size_t size = stream->available(); // 还剩下多少数据没有读完？
                Serial.printf("avail size=%d\n", size);
                if(size) {
                    size_t readBytesSize = stream->readBytes(buff, size);
                    if(len > 0) {
                      len -= readBytesSize;
                    }
                    outbuf=(uint8_t*)malloc(sizeof(uint8_t)*15000);
                    uint32_t outprintsize=0;
                    int result=ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 15000,outprintsize);
                    Serial.write(outbuf,outprintsize);
                    parseJSON((char*)outbuf, outprintsize);
                    free(outbuf);
                }
              }
          }
        }
       else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  Serial.println();
  Serial.println("Waiting 10s before the next round...");
  delay(10000);
}




void parseJSON(char *input, int inputLength) {
  // char* input;
// size_t inputLength; (optional)

DynamicJsonDocument doc(6144);

DeserializationError error = deserializeJson(doc, input, inputLength);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* code = doc["code"]; // "200"
const char* updateTime = doc["updateTime"]; // "2022-12-05T15:35+08:00"
const char* fxLink = doc["fxLink"]; // "http://hfx.link/1u0r1"

for (JsonObject hourly_item : doc["hourly"].as<JsonArray>()) {

  // const char* hourly_item_fxTime = hourly_item["fxTime"]; // "2022-12-05T17:00+08:00", ...
  String hourly_item_fxTime = hourly_item["fxTime"].as<String>(); // "2022-12-05T17:00+08:00", ...
  const char* hourly_item_temp = hourly_item["temp"]; // "15", "13", "13", "12", "11", "11", "10", "10", ...
  const char* hourly_item_icon = hourly_item["icon"]; // "100", "150", "150", "150", "150", "150", "150", ...
  const char* hourly_item_text = hourly_item["text"]; // "晴", "晴", "晴", "晴", "晴", "晴", "晴", "多云", "多云", ...
  const char* hourly_item_wind360 = hourly_item["wind360"]; // "22", "24", "30", "33", "33", "31", "30", ...
  const char* hourly_item_windDir = hourly_item["windDir"]; // "东北风", "东北风", "东北风", "东北风", "东北风", "东北风", ...
  const char* hourly_item_windScale = hourly_item["windScale"]; // "3-4", "3-4", "3-4", "3-4", "3-4", ...
  const char* hourly_item_windSpeed = hourly_item["windSpeed"]; // "16", "16", "16", "16", "14", "14", ...
  const char* hourly_item_humidity = hourly_item["humidity"]; // "57", "63", "63", "65", "66", "67", "68", ...
  const char* hourly_item_pop = hourly_item["pop"]; // "1", "3", "6", "6", "6", "6", "6", "6", "7", "7", ...
  const char* hourly_item_precip = hourly_item["precip"]; // "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", ...
  const char* hourly_item_pressure = hourly_item["pressure"]; // "1013", "1013", "1012", "1012", "1012", ...
  const char* hourly_item_cloud = hourly_item["cloud"]; // "5", "5", "4", "4", "7", "9", "11", "33", "54", ...
  const char* hourly_item_dew = hourly_item["dew"]; // "7", "6", "6", "6", "5", "5", "5", "5", "5", "4", ...

}

JsonArray refer_sources = doc["refer"]["sources"];
const char* refer_sources_0 = refer_sources[0]; // "QWeather"
const char* refer_sources_1 = refer_sources[1]; // "NMC"
const char* refer_sources_2 = refer_sources[2]; // "ECMWF"

const char* refer_license_0 = doc["refer"]["license"][0]; // "CC BY-SA 4.0"

}


