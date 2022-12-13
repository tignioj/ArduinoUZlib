/**
 * StreamHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>


#define USE_SERIAL Serial
WiFiMulti wm;

#include "ArduinoZlib.h" 

void error(String msg) {
  USE_SERIAL.print("!!!ERROR:");
  USE_SERIAL.println(msg);
}


void setup() {
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    wm.addAP("xxx", "xxx");
}
// 此变量占用内存过大，请勿放入loop()中
// uint8_t outbuf[10240] = { 0 };
uint8_t *outbuf;
void loop() {
    // wait for WiFi connection
    if((wm.run() == WL_CONNECTED)) {
        delay(3000);
        HTTPClient http;
        USE_SERIAL.print("[HTTP] begin...\n");
        // configure server and url
        // http.begin("http://192.168.2.144:8082/test");
        http.begin("http://192.168.2.144:8082/test");
        // 接受Gzip
        http.addHeader("Accept-Encoding", "gzip");
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                  Serial.println("Code Ok");

                // get lenght of document (is -1 when Server sends no Content-Length header)
                int len = http.getSize();
                Serial.printf("len=%d\n",len);

                // create buffer for read
                uint8_t buff[4096] = { 0 };

                // get tcp stream
                WiFiClient * stream = http.getStreamPtr();
                Serial.println("stream Ok");
                // read all data from server
                while(http.connected() && (len > 0 || len == -1)) {
                    // get available data size
                    size_t size = stream->available(); // 还剩下多少数据没有读完？
                    // USE_SERIAL.print("avail size=");
                    USE_SERIAL.println(size);
                    if(size) {
                        // read up to 128 byte
                        size_t realsize=((size > sizeof(buff)) ? sizeof(buff) : size);
                        // USE_SERIAL.println(realsize);
                        size_t readBytesSize = stream->readBytes(buff, realsize);
                        // write it to Serial
                        // USE_SERIAL.write(buff,readBytesSize);
                        if(len > 0) {
                            len -= readBytesSize;
                        }
                        outbuf=(uint8_t*)malloc(sizeof(uint8_t)*20480);
                        uint32_t outprintsize=0;
                        
                        int result=ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 20480,outprintsize);
                        USE_SERIAL.write(outbuf,outprintsize);
                        free(outbuf);
                    } else {
                      USE_SERIAL.println("no avali size");
                    }
                    delay(1);
                }
                USE_SERIAL.println();
                USE_SERIAL.print("[HTTP] connection closed or file end.\n");

            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }

    delay(3000);
}

