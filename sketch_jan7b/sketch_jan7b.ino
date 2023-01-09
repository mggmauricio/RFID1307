#include <EEPROM.h>
#include <MFRC522.h>
#include <SPI.h>
#include <string.h>

#define SS_PIN 21 // Pin for MFRC522 SS
#define RST_PIN 22 // Pin for MFRC522 reset
#define GREEN_PIN 5
#define RED_PIN 2
#define LED 15

MFRC522 mfrc522(SS_PIN, RST_PIN);
byte AuthorizedUID[4];
byte CardUID[4]; // Create MFRC522 instance
bool doorState = false;


byte saveAuthorizedCard(){
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Select the card
    if (mfrc522.PICC_ReadCardSerial()) {
      // Print the UID of the card
      for (byte i = 0; i < mfrc522.uid.size; i++) {
         AuthorizedUID[i] = mfrc522.uid.uidByte[i];
      }
      return 1;
    }
    return 0;
  }
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
boolean checkIfIsEqualToLastCard(){
      for (int i = 0; i < 4; i++) {
        if (AuthorizedUID[i] != CardUID[i]) {
          return false; // Return failure
        }
        else{
          return true;
        }
      }
}


byte readCard() {
  // Check if a card is present
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Select the card
    if (mfrc522.PICC_ReadCardSerial()) {
      // Print the UID of the card
      Serial.print("Card UID: ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
         CardUID[i] = mfrc522.uid.uidByte[i];
         Serial.print(CardUID[i]);
      }
      
      Serial.println("");
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
void blinkRed(){
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


void blinkGreen(){
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

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  SPI.begin();
  mfrc522.PCD_Init(); // Initialize MFRC522
  Serial.println("RFID reader ready!");
  digitalWrite(LED, LOW);
}

void loop() {
  if (doorState == false){
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
  }
  else{
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
  }
   if(readCard()){
     if(checkCardInEEPROM()){
       if(doorState == false){
         digitalWrite(LED, HIGH);
         saveAuthorizedCard();
         doorState = true;
         
       }else if(doorState == true){
         if(AuthorizedUID == CardUID){
           
           digitalWrite(LED, LOW);
           doorState == false;
         }else{
           blinkRed();
           Serial.println("CARTAO NAO É IGUAL AO ULTIMO");
         }
       }
       }else{
         return;
       }
       }
       delay(500);
     }

     
      // If a card was read successfully
//     if (checkCardInEEPROM()) {
//       Serial.println("Card UID found in EEPROM");
//       digitalWrite(GREEN_PIN, HIGH);
//       digitalWrite(RED_PIN, LOW);
//     } else {
//       Serial.println("Card UID not found in EEPROM");
//       digitalWrite(RED_PIN, HIGH);
//       digitalWrite(GREEN_PIN, LOW);
//     }
//   }
//   delay(3000); // Wait 500 milliseconds
// }


