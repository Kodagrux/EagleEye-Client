#include <SPI.h>
#include <RF24.h>


int pinIntMotion = 2; //Digital pin for motion sensor


bool debug = true;
bool alarmArmed = false;
bool alarmTriggered = false;
int heartBeatInterval = 10;
unsigned long prevMillis = 0;
int sensorUnitID = 1;

//int pinIntSpeaker = 8;

RF24 radio(9,10);

void setup() {
  if (debug) {
    while(!Serial);
    Serial.begin(115200);
  }

  
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  const uint64_t pipe = 0xE8E8F0F0E1LL;
  radio.openReadingPipe(1, pipe);
  
  
  radio.enableDynamicPayloads();
  radio.powerUp();

  // Get into standby mode
  radio.startListening();
  radio.stopListening();

  
  pinMode(pinIntMotion, INPUT);


  
  //pinMode(pinIntSpeaker, OUTPUT);
  //tone(pinIntSpeaker, 3000, 3); 

  char comReply[] = "1,AWOKE"; //Awoke
  radio.write(comReply, sizeof(comReply));
  if (debug) { Serial.println("Setup complete! Starting...");}
}










void loop() {
  //char comReply[] = "1,HEARTBEAT"; //Creating reply, ID + What
  //radio.write(comReply, sizeof(comReply));
  
  
  char com[32] = {0}; //Communication array (String)

  //If alarm is not triggered
  if (!alarmTriggered) {






    //If alarm is armed
    if (alarmArmed) {
      if (debug) { Serial.println("Alarm: Armed!");}
      int motion = digitalRead(pinIntMotion);
  
      //If there is motion
      if (motion == 1) {
        alarmTriggered = true;
        if (debug) { Serial.println("Something is moving!");}
        char comReply[] = "1,MOTION"; //Creating reply, ID + What
        radio.stopListening();
        radio.write(comReply, sizeof(comReply));
        if (debug) { Serial.println("Messaging base!");}
        
        //delay(100); //Waiting so we don't spam the network
        if (debug) {Serial.println("Sleeping for a bit...");}
        
      //If there is no motion
      } else {

        if (debug) { Serial.println("Listening for Disarm-order");}
        radio.startListening();
        delayMicroseconds(225);

        //If message is recived
        if(radio.available()){
          if (debug) { Serial.println("Message Recived!");}
          radio.read(com, sizeof(com)); //Read the message
          radio.stopListening(); //Turn of radio
          String stringCom(com); //Convert message to string
          if (debug) { Serial.println("Message was: '" + stringCom + "'");}
          
          //If unit is told to be disarmed
          if  (stringCom == "DISARM" ) {
            if (debug) { Serial.println("Disarming order recived!");}
            alarmArmed = false;
            char comReply[] = "1,DISARMED"; //Creating reply, ID + What
            radio.write(comReply, sizeof(comReply));
            radio.startListening();
            if (debug) { Serial.println("Replied: " + String(comReply));}
    
          //Anything else
          } else {
            if (debug) { Serial.println("Didn't understand '" + stringCom + "'");}
          }
        } else {
          if (debug) { Serial.println("No disarm-orders available");}
          //radio.stopListening(); //Stop listening
        }
        
        if (debug) { Serial.println("All clear!, going to sleep for a bit...");}
        //delay(100);
      }









  //If alarm is disarmed
    } else {
      if (debug) { Serial.println("Alarm: Disarmed!");}

      if (debug) { Serial.println("Listening for Arm-order");}
      radio.startListening(); //See if there is something recived
      delayMicroseconds(225);

      //If message is recived
      if(radio.available()){
        if (debug) { Serial.println("Message Recived!");}
        radio.read(com, sizeof(com)); //Read the message
        radio.stopListening(); //Turn of radio
        String stringCom(com); //Convert message to string
        if (debug) { Serial.println("Message was: '" + stringCom + "'");}
        
        //If unit is told to be armed
        if  (stringCom == "ARM" ) {
          if (debug) { Serial.println("Arming order recived!");}
          alarmArmed = true;
          char comReply[] = "1,ARMED"; //Creating reply, ID + What
          radio.write(comReply, sizeof(comReply));
          radio.startListening();
          if (debug) { Serial.println("Replied: " + String(comReply));}
  
        //Anything else
        } else {
          if (debug) { Serial.println("Didn't understand '" + stringCom + "'");}
        }
      } else {
        if (debug) { Serial.println("No arm-orders available");}
        //radio.stopListening(); //Stop listening
      }

      if (debug) {Serial.println("Not armed, sleeping for a bit...");}
      
    }













  //If alarm is triggered!
  } else {
    if (debug) { Serial.println("Alarm has been triggered!");}
    char comReply[] = "1,MOTION"; //Creating reply, ID + What
    radio.write(comReply, sizeof(comReply));
    if (debug) { Serial.println("Messaging base again!");}

    //delay(100); //Waiting so we don't spam the network
    if (debug) {Serial.println("Sleeping for a bit...");}
  } 







  //Heartbeats
  if (((millis()/1000 - prevMillis) > heartBeatInterval) && !alarmTriggered) {
    prevMillis = millis()/1000;
    if (debug) { Serial.println("Sending Heartbeat: " + String(millis())); }
    char comReply[] = "1,HEARTBEAT"; //Creating reply, ID + What
    radio.stopListening();
    radio.write(comReply, sizeof(comReply));
    radio.startListening();
  }





  //If millis bugs out after XXXXX
  if (millis()/100 < prevMillis) {
    prevMillis = 0;
  }



  
  
  
  if (debug) { Serial.println("Loop complete!"); Serial.println(""/*"           " + String(millis()) + "     " + String(prevMillis)*/);}

  //Delays
  if (alarmArmed && !alarmTriggered) {
    delay(1000);
  } else if (alarmArmed && alarmTriggered) {
    delay(100);
  } else if (!alarmArmed) {
    delay(2000);
  }

  
  
}











/*
void loop() {


  if (alarmArmed) {
    radio.startListening();
    Serial.println("Starting loop. Radio on.");
    char receivedMessage[32] = {0};
    if(radio.available()){
      radio.read(receivedMessage, sizeof(receivedMessage));
      Serial.println(receivedMessage);
      Serial.println("Turning off the radio");
      radio.stopListening();
  
      String stringMessage(receivedMessage);
  
      if  (stringMessage == "DISARM" ) {
        Serial.println("Looks like they want a string");
        alarmArmed = false;
      }
     
    }
    delay(100);
  } else {
    const char text[] =  "Disarmed!";
    radio.write(text, sizeof(text));
    Serial.println("We sent our message");
    delay(100);
  }

}
*/









/*
void loop() {
 
  const char text[] = "Motion!";
  Serial.println(digitalRead(pinIntMotion));
  if (digitalRead(pinIntMotion) == 1) {
    //tone(pinIntSpeaker, 2000, 20); 
    Serial.println(text);
    radio.write(&text, sizeof(text));

  }
  
  delay(100);

}*/ 
