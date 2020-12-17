#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>

#define DEBUG

#include <SPI.h>
#include <MFRC522Extended.h>

const byte numReaders = 4; // Number of RFID readers

const byte ssPins[] = {2, 3, 4, 10}; //Each reader has a unique Slave Select pin

const byte resetPin = 9;// Share the same reset pin

MFRC522 mfrc522[numReaders]; // initialise

String correctIDs[] = {"67e7c65f", "f956b1c1", "c91ed6c1", "695c9ec2"}; //67e7c65f, f956b1c1 , C91ED6C1, 695C9EC2

String currentIDs[numReaders];

const byte lockPin = A0; 
//pin which connects to the PIN pin of relay

void setup() {

   #ifdef DEBUG

   Serial.begin(9600);
   Serial.println(F("Serial communicatin started"));

   #endif

   pinMode(lockPin, OUTPUT);
   digitalWrite(lockPin, HIGH);

   SPI.begin();
    
   for(uint8_t i=0; i<numReaders; i++){

    mfrc522[i].PCD_Init(ssPins[i], resetPin);

    Serial.print(F("Reader #"));
    Serial.print(i);
    Serial.print(F(" initialised on pin "));
    Serial.print(String(ssPins[i]));
    Serial.print(F(". Antenna strength: "));
    Serial.print(mfrc522[i].PCD_GetAntennaGain());
    Serial.print(F(". Version : "));
    mfrc522[i].PCD_DumpVersionToSerial();

    delay(100);
   }

    Serial.println(F("-END SETUP-"));
}

void loop() {

  boolean puzzleSolved = true;
  boolean changeValue = false;
  String readRfid = "";

  for(uint8_t i=0; i<numReaders; i++){
    mfrc522[i].PCD_Init();
    if(mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial()){
      readRfid = (String)dump_byte_array(mfrc522[i].uid.uidByte, mfrc522[i].uid.size);
    }

    if(readRfid != currentIDs[i]){
      changeValue = true;
      currentIDs[i] = readRfid;
     }

    if(currentIDs[i] != correctIDs[i]){
      puzzleSolved = false; //Not solved 
    }

    mfrc522[i].PICC_HaltA();
    mfrc522[i].PCD_StopCrypto1();
  }

  if(changeValue){
    for(uint8_t i=0; i<numReaders; i++){
      Serial.print(F("Reader #"));
      Serial.print(String(i));
      Serial.print(F(" on Pin #"));
      Serial.print(String((ssPins[i])));
      Serial.print(F(" detected tag: "));
      Serial.print(currentIDs[i]);
    }
    Serial.println(F("----"));
  }
  if(puzzleSolved){
    onSolve();
  }
}

void onSolve(){
  #ifdef DEBUG
  Serial.println(F("Puzzle Solved"));
  #endif

  digitalWrite(lockPin, LOW);
  while(true){
   
  }
}

byte dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
