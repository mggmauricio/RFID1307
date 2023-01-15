#include <MFRC522.h>
#include <SPI.h>
#include <EEPROM.h>

#define RED_PIN 15
#define GREEN_PIN 2
#define SS_PIN 21
#define RST_PIN 22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

boolean programMode = false;

MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
MFRC522 mfrc522(SS_PIN, RST_PIN);
const int EEPROM_ADDRESS = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  mfrc522.PCD_Init();
  Serial.println("Aproxime o cartão ou tag..");
  Serial.println();
}

void loop() {
  if (! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if (! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  
  int option = menu();
  if(option == 0){
    readData();
  }
  else if(option == 1){
    saveData();
  }
  else {
    Serial.println(F("Opção incorreta!"));
    return;
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();  
}

void readData(){

  for (byte i = 0; i < 6; i++){
    key.keyByte[i] = 0XFF;
  }

  byte buffer[SIZE_BUFFER] = {0};

  byte block = 1;
  byte size = SIZE_BUFFER;


  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if(status != MFRC522::STATUS_OK){
      Serial.print(F("Autenticação falhou: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      digitalWrite(RED_PIN, HIGH);
      delay(2000);
      digitalWrite(RED_PIN, LOW);
      return;      
    }
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if(status != MFRC522::STATUS_OK){
    Serial.print(F("Falha na leitura: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(RED_PIN, HIGH);
    delay(2000);
    digitalWrite(RED_PIN, LOW);
    return;      
  }
  else{
  digitalWrite(GREEN_PIN, HIGH);
  delay(2000);
  digitalWrite(GREEN_PIN, LOW);
  }
  Serial.print(F("\nDados do bloco ["));
  Serial.print(block);
  Serial.print(F("]:"));
  for(uint8_t i = 0; i < MAX_SIZE_BLOCK; i++)
  {
    Serial.write(buffer[i]);      
  }
  Serial.println(" ");  

  }

void saveData(){
  Serial.setTimeout(300000L);  
  Serial.println("Insira os dados a serem gravados: ");
  for(byte i = 0; i < 6; i++)
    key.keyByte[i] = 0xFF;
    byte buffer[MAX_SIZE_BLOCK] = "";
    byte block;
    byte sizeData;
    sizeData = Serial.readBytesUntil('#', (char*)buffer, MAX_SIZE_BLOCK);  
    for(byte i = sizeData; i < MAX_SIZE_BLOCK; i++){
      buffer[i] = ' ';      
    }
    block = 1;
    String str = (char*)buffer;
    Serial.println(str);
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if(status != MFRC522::STATUS_OK){
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(RED_PIN, HIGH);
    delay(2000);
    digitalWrite(RED_PIN, LOW);
    return;
  }
  status = mfrc522.MIFARE_Write(block, buffer, MAX_SIZE_BLOCK);
  if(status != MFRC522::STATUS_OK){
    Serial.print(F("MIFARE_Write() failed"));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(RED_PIN, HIGH);
    delay(2000);
    digitalWrite(RED_PIN, LOW);
    return;
  }
  else {
    Serial.println(F("MIFARE_Write() success:"));
    digitalWrite(GREEN_PIN, HIGH);
    delay(2000);
    digitalWrite(GREEN_PIN, LOW);
  }  
  }

  
int menu(){
  Serial.println("Escolha uma opção:");
  Serial.println(F("0 -- Leitura de dados"));
  Serial.println(F("1 -- Gravação de dados"));

  while(!Serial.available()){};

  int op = (int)Serial.read();

  while(Serial.available()){
    if(Serial.read() == '\n') break;
    Serial.read();
  }
  return (op - 48);
}