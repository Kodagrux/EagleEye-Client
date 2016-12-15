#include <SPI.h>

//#include <nRF24L01.h>
//#include <printf.h>
#include <RF24.h>
//#include <RF24_config.h>

int pinIntMotion = 2;
int pinIntSpeaker = 8;

RF24 radio(9,10);

void setup() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableDynamicPayloads();
  radio.powerUp();

  Serial.begin(115200);
  pinMode(pinIntMotion, INPUT);
  pinMode(pinIntSpeaker, OUTPUT);
  tone(pinIntSpeaker, 3000, 3); 
}

void loop() {
 
  const char text[] = "Motion!";
  Serial.println(digitalRead(pinIntMotion));
  if (digitalRead(pinIntMotion) == 1) {
    tone(pinIntSpeaker, 2000, 20); 
    Serial.println(text);
    radio.write(&text, sizeof(text));

  }
  
  delay(100);
  
 
 

} 
