// This code listens for LoRa packages and prints the package message on Serial and OLED display

// Based on example: https://github.com/thewenneur/esp32-1/tree/master/esp32/libraries/LoRa 
// and https://hackaday.io/project/26991-esp32-board-wifi-lora-32

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>

// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6

void setup() {

  // setup OLED
  pinMode(16,OUTPUT); 
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED 
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 to high 
  Wire.begin(4,15);
  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(0,0);              // Set cursor position, start of line 0
  oled.putString("LoRa Receiver");
  
  Serial.begin(115200);
  while (!Serial); //if just the the basic function, must connect to a computer
  delay(1000);
  
  Serial.println("LoRa Receiver"); 
  
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    String packet = "";
    char c;
    while (LoRa.available()) {
      c = (char)LoRa.read();
      packet += c;
    }
    
    // read RSSI of packet
    int rssi = LoRa.packetRssi();

    Serial.print(packet);
    Serial.print("' with RSSI ");
    Serial.println(rssi);
    
    // print on OLED
    oled.setTextXY(1,0);
    oled.putString("Received: ");
    oled.putString(packet);
    oled.putString(" (RSSI: ");
    oled.putNumber(rssi);
    oled.putString(")");

  }
}
