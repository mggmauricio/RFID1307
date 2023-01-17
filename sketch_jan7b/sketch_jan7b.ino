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
// String CardID = ""; // Create MFRC522 instance
byte CardUID[4];
bool isDoorOpen = false;
bool isProgrammingMode = true;
bool detected = false;

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


String readCard() {
  String CardID = "";
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
      return CardID;
      // Return success
    }
  }
  return ""; // Return failure
}

void saveCardToEEPROM(String cardUID) {
  // Save the UID to EEPROM
  // Get the current number of stored cards
  int numCards = EEPROM.read(0);

  // Calculate the next available address for the new card
  int nextAddress = numCards * 4 + 1;

  // Convert the string to an array of bytes
  uint8_t cardBytes[4];
  for (int i = 0; i < 4; i++) {
    cardBytes[i] = cardUID.charAt(i);
  }

  // Write the card UID to EEPROM
  for (int i = 0; i < 4; i++) {
    EEPROM.write(nextAddress + i, cardBytes[i]);
  }

  // Increment the number of stored cards
  numCards++;
  EEPROM.write(0, numCards);

  // Commit the changes to EEPROM
  EEPROM.commit();
  Serial.println("Cartao salvo no endereço: " + nextAddress);
}


void deleteCardFromEEPROM(String cardUID){
  // Check if a card is present
  // Convert the string to an array of bytes
  uint8_t cardBytes[4];
  for (int i = 0; i < cardUID.length(); i++) {
    cardBytes[i] = cardUID.charAt(i);
  }

  // Get the number of stored cards
  int numCards = EEPROM.read(0);

  // Search for the card in EEPROM
  for (int i = 0; i < numCards; i++) {
    int address = i * 4 + 1;
    bool match = true;
    for (int j = 0; j < cardUID.length(); j++) {
      if (EEPROM.read(address + j) != cardBytes[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      // Shift all the cards after the matched card to the left
      for (int j = i + 1; j < numCards; j++) {
        int nextAddress = j * 4 + 1;
        for (int k = 0; k < 4; k++) {
          EEPROM.write(address + k, EEPROM.read(nextAddress + k));
        }
        address = nextAddress;
      }
      // Decrement the number of stored cards
      numCards--;
      EEPROM.write(0, numCards);
      // Commit the changes to EEPROM
      EEPROM.commit();
      break;
    }
  }
}




void handleProgrammingMode(){
  // Serial.println("PROGRAMMING MODE");
  String Card = "";
  while(Card == ""){
    Card = readCard();
  }
  Serial.println("Saiu do while");
  if((Card != "") && checkCardInEEPROM()){
    deleteCardFromEEPROM(Card);
    Serial.println("deveria apagar");
    return;
  }else if((Card =! "") && !checkCardInEEPROM()){
    Serial.println("DEVERIA SALVAR AGORA");
    saveCardToEEPROM(Card);
    return;
  }
}

void handleReadingMode(){
  
  while(!isDoorOpen){
    String Card = readCard();
    if((Card != "") && checkCardInEEPROM()){
      blinkOpenLight();    
      AuthorizedID = Card;
      Serial.println(Card);
      Serial.println(AuthorizedID);
      Serial.println(isDoorOpen);
      setDoorOpen();  
    }
  }
  while(isDoorOpen){
    String Card = readCard();
    // Serial.println("TA ABERTA");
    if(Card != "" && (Card == AuthorizedID)){
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
