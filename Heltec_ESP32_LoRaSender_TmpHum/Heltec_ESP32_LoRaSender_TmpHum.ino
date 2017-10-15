/*
  Read temperature and humidity from a Si7021 sensor and send a packet
  with these value using LoRa. It also displays values on the OLED display.

  Code based on the Hello World example from U8x8lib.

  ----

  HelloWorld.ino
  
  "Hello World" version for U8x8 API

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8x8lib.h>
#include <LoRa.h>
#include "Adafruit_Si7021.h"
#include <SPI.h>
#include <WiFi.h>

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

Adafruit_Si7021 sensor = Adafruit_Si7021();

byte mac[6];
char deviceId[12];

void setup(void)
{
  Serial.begin(115200);
  
  // initialize OLED display
  u8x8.begin();
  u8x8.setPowerSave(0);

  // initialize Si7021 temp & humid sensor
  sensor.begin();

  // initialize LoRa
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");

  WiFi.begin();
  WiFi.macAddress(mac);
  sprintf(deviceId, "%2X%2X%2X%2X%2X%2X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  Serial.println(deviceId);
  WiFi.disconnect();
}

void loop(void)
{
  // read temperature and humidity from Si7021 sensor
  double temperature = sensor.readTemperature();
  double humidity = sensor.readHumidity();
  Serial.print("Humidity:    "); Serial.print(humidity, 2);
  Serial.print("\tTemperature: "); Serial.println(temperature, 2);
  delay(100);

  // show temperature and humidity on OLED display
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  char buftemp[10];
  char bufhum[10];
  sprintf(buftemp, "tmp: %.2f", temperature);
  u8x8.drawString(0,0,buftemp);
  sprintf(bufhum, "hum: %.2f", humidity);
  u8x8.drawString(0,1,bufhum);
  u8x8.drawString(0,2,deviceId);
  u8x8.refreshDisplay();		// only required for SSD1606/7  

  // send packet with sensor values
  LoRa.beginPacket();
  LoRa.print("temp: ");
  LoRa.print(temperature);
  LoRa.print("; hum: ");
  LoRa.print(humidity);
  LoRa.print("; dev: ");
  LoRa.print(deviceId);
  LoRa.endPacket();

  delay(3000);
}
