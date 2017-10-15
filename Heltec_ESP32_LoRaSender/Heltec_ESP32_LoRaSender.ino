// This code sends a packet with the message hello and a counter value using LoRa
// It also displays the counter on the OLED display

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

// note: set upload speed to 115200 and look at serial at 115200 as well

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6

int counter = 0;

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
  oled.putString("LoRa Sender");
  
  pinMode(25,OUTPUT); //Send success, LED will bright 1 second
  
  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer
  
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
//  Serial.println("LoRa Sender");

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  
  // blink LED
  /*
  digitalWrite(25, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(25, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  */

  // print counter on OLED
  oled.setTextXY(1,0);
  oled.putString("Sent: ");
  oled.putNumber(counter);  

  counter++;
  
  delay(3000);
}
