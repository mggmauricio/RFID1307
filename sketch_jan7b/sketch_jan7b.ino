#include <EEPROM.h>
#include <MFRC522.h>
#include <SPI.h>
#include <string.h>

#define SS_PIN 5 // Pin for MFRC522 SS
#define RST_PIN 2 // Pin for MFRC522 reset
#define GREEN_PIN 22
#define RED_PIN 13
#define BLUE_LED 12 
#define IRQ_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);
String AuthorizedID = "";
String CardID = ""; // Create MFRC522 instance
byte CardUID[4];
bool isDoorOpen = false;
bool isProgrammingMode = false;

void setProgrammingState(){
  isProgrammingMode = true;  
}

void setReadingState(){
  isProgrammingMode = false;  
}

void setDoorOpen(){
  digitalWrite(BLUE_LED, HIGH);
  isDoorOpen = true;
}

void setDoorClosed(){
  isDoorOpen = false;
  digitalWrite(BLUE_LED, LOW);
}

void setOpenLight(){
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(RED_PIN, LOW);    
}

void setClosedLight(){
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
}

void setProgrammingLight(){
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  delay(500);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(RED_PIN, LOW);
  delay(500);
}

void blinkCloseLight(){
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(RED_PIN, HIGH);
  delay(500);
  digitalWrite(RED_PIN, LOW);
  delay(500);
  digitalWrite(RED_PIN, HIGH);
  delay(500);
  digitalWrite(RED_PIN, LOW);
  delay(500);
  digitalWrite(RED_PIN, HIGH);
}

void blinkOpenLight(){
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  delay(500);
  digitalWrite(GREEN_PIN, LOW);
  delay(500);
  digitalWrite(GREEN_PIN, HIGH);
  delay(500);
  digitalWrite(GREEN_PIN, LOW);
  delay(500);
  digitalWrite(GREEN_PIN, HIGH);
}

boolean checkCardInEEPROM() {
  // Compare the last card UID with the one stored in EEPROM
  EEPROM.begin(32); // Start EEPROM access
  for (int i = 0; i < 4; i++) {
    if (EEPROM.read(i) != CardUID[i]) {
      Serial.println("Not in EEPROM");
      EEPROM.end(); // End EEPROM access
      return false; // Return failure
    }
  }
  EEPROM.end(); // End EEPROM access
  Serial.println("UID in EEPROM");
  return true; // Return success
}

byte readCard() {
  CardID = "";
  // Check if a card is present
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Select the card
    if (mfrc522.PICC_ReadCardSerial()) {
      // Print the UID of the card
      Serial.print("Card UID: ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
         CardUID[i] = mfrc522.uid.uidByte[i];
         CardID += String(CardUID[i], HEX);
      }
      Serial.println(CardID);
      mfrc522.PICC_HaltA(); // halt PICC
      mfrc522.PCD_StopCrypto1();
      return 1; // Return success
    }
  }
  return 0; // Return failure
}

void saveCardToEEPROM() {
  // Save the UID to EEPROM
  EEPROM.begin(32); // Start EEPROM access
  for (int i = 0; i < mfrc522.uid.size; i++) {
    EEPROM.write(i, mfrc522.uid.uidByte[i]);
  }
  EEPROM.commit(); // Save data to EEPROM
  EEPROM.end(); // End EEPROM access
  Serial.println("UID saved to EEPROM");
}

void deleteCardFromEEPROM() {
  // Check if a card is present
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Select the card
    if (mfrc522.PICC_ReadCardSerial()) {
      // Clear the UID from the EEPROM
      EEPROM.begin(32); // Start EEPROM access
      for (int i = 0; i < mfrc522.uid.size; i++) {
        EEPROM.write(i, 0x00);
      }
      EEPROM.commit(); // Save data to EEPROM
      EEPROM.end(); // End EEPROM access
      Serial.println("UID deleted from EEPROM");
    }
  }
}



void handleProgrammingMode(){
  // Serial.println("PROGRAMMING MODE");
  if(readCard() && checkCardInEEPROM()){
    deleteCardFromEEPROM();
    Serial.println("deveria apagar");
    return;
  }else if(readCard() && !checkCardInEEPROM()){
    saveCardToEEPROM();
    Serial.println("Deveria salvar");
    
    return;
  }
}

void handleReadingMode(){
  while(!isDoorOpen){
    if(readCard() && checkCardInEEPROM()){
      blinkOpenLight();    
      AuthorizedID = CardID;
      Serial.println(CardID);
      Serial.println(AuthorizedID);
      Serial.println(isDoorOpen);
      setDoorOpen();  
    }
  }
  while(isDoorOpen){
    // Serial.println("TA ABERTA");
    if(readCard() && (CardID == AuthorizedID)){
      setDoorClosed(); 
    }
  }
  delay(500);
}

void ManageDoorMode(bool isProgrammingMode){
  if(!isProgrammingMode) return handleReadingMode();
      
  handleProgrammingMode();
}


void ManageLightMode(bool isDoorOpen, bool isProgrammingMode){
  if(isProgrammingMode){
    return setProgrammingLight();
    
    Serial.println("RETURN MAS CONTINUOU");
  }
  Serial.print("doorStatus:");
  Serial.println(isDoorOpen);
  if(!isDoorOpen){
    return setClosedLight();
    Serial.println("RETURN MAS CONTINUOU");
  }
  Serial.println("Setando luzopen");  
  setOpenLight();  
}


void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  SPI.begin();
  mfrc522.PCD_Init(); // Initialize MFRC522
  Serial.println("RFID reader ready!");
}

void loop() {  
  ManageDoorMode(isProgrammingMode);  
  ManageLightMode(isDoorOpen, isProgrammingMode);
}
