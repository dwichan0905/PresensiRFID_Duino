#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
 
#define RS 2
#define ENABLE 3
#define DATA4 4
#define DATA5 5
#define DATA6 6
#define DATA7 7
#define SPEAKER 8
#define SS_PIN 10
#define RST_PIN 9

int DEVICEID = 123;
int state = 0; // 0: Checking, 1: Register, 2: Not Register

LiquidCrystal LCD(RS, ENABLE, DATA4, DATA5, DATA6, DATA7);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

StaticJsonBuffer<200> jsonBuffer;

void beep(int del) {
  tone(SPEAKER, 700);
  delay(del);
  noTone(SPEAKER);
}

void setup() {
  // put your setup code here, to run once:
  LCD.begin(16, 2);
  LCD.setCursor (0, 0);
  LCD.print("Booting...");
  Serial.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(SPEAKER, OUTPUT);
  beep(50);
  LCD.clear();
  while (!(Serial.available() > 0)) {
    LCD.setCursor(0, 0);
    LCD.print("COM Wait...");
  }
  beep(100);
  LCD.setCursor(0, 0);
  LCD.print("COM Detected!  ");
  delay(2000);
  
}

void loop() {
    // put your main code here, to run repeatedly:
     
      if (state == 0) {
        LCD.setCursor(0, 0);
        LCD.print("Checking Device      ");
        LCD.setCursor(0, 1);
        LCD.print("                ");
        Serial.print("DEVID=" + DEVICEID);
        LCD.setCursor(0, 0);
        String json = Serial.readString(); 
        delay(100);
            
        if(json.length() != 0) {
          JsonObject& root = jsonBuffer.parseObject(json);
          int code = root["code"];
          char* devname = root["result"];
          if(!root.success()) {
            LCD.setCursor(0, 0);
            LCD.print("Failed to fetch       ");
            LCD.setCursor(0, 1);
            LCD.print("                    ");
            delay(1000);
          } else {
            if (code == 200) {
              LCD.setCursor(0, 0);
              LCD.print("Device Name       ");
              LCD.setCursor(0, 1);
              LCD.print(devname);
              state = 1;
              delay(5000);
            } else {
              state = 2;
            }
          }
        }   
      } else if (state == 1) {
        LCD.setCursor(0, 0);
        LCD.print("Tempelkan Kartu      ");
        LCD.setCursor(0, 1);
        LCD.print("Mahasiswa Anda...    ");
        // Look for new cards
        if ( ! mfrc522.PICC_IsNewCardPresent()) 
        {
          return;
        }
        // Select one of the cards
        if ( ! mfrc522.PICC_ReadCardSerial()) 
        {
          return;
        }
        //Show UID on serial monitor
        Serial.print("UID tag :");
        String content= "";
        byte letter;
        for (byte i = 0; i < mfrc522.uid.size; i++) 
        {
           content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
           content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        Serial.println("UID=" + content);
        Serial.print("Message : ");
        content.toUpperCase();
        if (content.substring(1) == "BD 31 15 2B") //change here the UID of the card/cards that you want to give access
        {
          Serial.println("Authorized access");
          Serial.println();
          delay(3000);
        }
       
       else   {
          Serial.println(" Access denied");
          delay(3000);
        }
      } else {
        LCD.setCursor(0, 0);
        LCD.print("DEVICE NO REG       ");
        LCD.setCursor(0, 1);
        LCD.print("DEVID=");
        LCD.print(DEVICEID);
        while(1) {}
      }
       
      
      
    delay(5);    
}
