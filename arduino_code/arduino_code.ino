#include <SPI.h>

//#include <nRF24L01.h>
//#include <printf.h>
#include <RF24.h>
//#include <RF24_config.h>

int pinIntMotion = 2;
//int pinIntSpeaker = 8;

RF24 radio(9,10);

void setup() {
  while(!Serial);
  Serial.begin(115200);


  
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  const uint64_t pipe = 0xE8E8F0F0E1LL;
  radio.openReadingPipe(1, pipe);
  
  
  radio.enableDynamicPayloads();
  radio.powerUp();

  
  
  pinMode(pinIntMotion, INPUT);
  //pinMode(pinIntSpeaker, OUTPUT);
  //tone(pinIntSpeaker, 3000, 3); 
}

void loop() {
 
  radio.startListening();
  Serial.println("Startibng loop. Radio on.");
  char receivedMessage[32] = {0};
  if(radio.available()){
    radio.read(receivedMessage, sizeof(receivedMessage));
    Serial.println(receivedMessage);
    Serial.println("Turning off the radio");
    radio.stopListening();

    String stringMessage(receivedMessage);

    if  (stringMessage == "GETSTRING" ) {
      Serial.println("Looks like they want a string");
      const char text[] =  "Hellow world";
      radio.write(text, sizeof(text));
      Serial.println("We sent our message");
    }
   
  }
  delay(100);

}





//void loop() {
 
  //const char text[] = "Motion!";
  //Serial.println(digitalRead(pinIntMotion));
  ///if (digitalRead(pinIntMotion) == 1) {
    //tone(pinIntSpeaker, 2000, 20); 
    //Serial.println(text);
    //radio.write(&text, sizeof(text));

  //}
  
  //delay(100);

//} 
