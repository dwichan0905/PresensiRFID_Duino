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

StaticJsonBuffer<500> jsonBuffer;

void beep(int del) {
  tone(SPEAKER, 700);
  delay(del);
  noTone(SPEAKER);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
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
  while (!Serial) {
    LCD.setCursor(0, 0);
    LCD.print("Menghubungkan...");
  }
  beep(100);
  LCD.setCursor(0, 0);
  LCD.print("Terhubung!      ");
  delay(2000);
  
}

void loop() {
    // put your main code here, to run repeatedly:
    String cmd = Serial.readString();
    if (cmd == "DC") {
      beep(50);
      delay(50);
      beep(50);
      delay(50);
      beep(50);
      LCD.setCursor(0, 0);
      LCD.print("Koneksi Server  ");
      LCD.setCursor(0, 1);
      LCD.print("Terputus.       ");
      state = 0;
      delay(5000);
    }
      
      if (state == 0) {
        LCD.setCursor(0, 0);
        LCD.print("Cek perangkat...");
        LCD.setCursor(0, 1);
        LCD.print("                ");
        th:
        Serial.print("DEVID=");
        Serial.println(DEVICEID);
        LCD.setCursor(0, 0);
        String json = Serial.readString(); 
        delay(1);
        JsonObject& root = jsonBuffer.parseObject(json);    
        if(json.length() != 0) {
          int code = root["code"];
          char* devname = root["result"];
          if(!root.success()) {
              goto th;
          } else {
            if (code == 200) {
              LCD.setCursor(0, 0);
              LCD.print("Terdaftar:         ");
              LCD.setCursor(0, 1);
              LCD.print(devname);
              state = 1;
              delay(3000);
              jsonBuffer.clear();
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
        delay(10000);
        state = 3;
//        // Look for new cards
//        if ( ! mfrc522.PICC_IsNewCardPresent()) 
//        {
//          return;
//        }
//        // Select one of the cards
//        if ( ! mfrc522.PICC_ReadCardSerial()) 
//        {
//          return;
//        }
//        //Show UID on serial monitor
//        String content= "";
//        byte letter;
//        for (byte i = 0; i < mfrc522.uid.size; i++) 
//        {
//           content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
//           content.concat(String(mfrc522.uid.uidByte[i], HEX));
//        }
//        Serial.print("UID=121 229 158 132" + content + ";DEVID=" + DEVICEID);
//        //Serial.print("Message : ");
//        content.toUpperCase();
//        if (content.substring(1) == "BD 31 15 2B") //change here the UID of the card/cards that you want to give access
//        {
//          Serial.println("Authorized access");
//          Serial.println();
//          delay(3000);
//        }
//       
//       else   {
//          Serial.println(" Access denied");
//          delay(3000);
//        }
      } else if (state == 2) {
        LCD.setCursor(0, 0);
        LCD.print("DEVICE NO REG       ");
        LCD.setCursor(0, 1);
        LCD.print("DEVID=");
        LCD.print(DEVICEID);
        while(1) {}
      } else if (state == 3) {
        LCD.setCursor(0, 0);
        LCD.print("Membaca Kartu   ");
        LCD.setCursor(0, 1);
        LCD.print("                ");
        
        Serial.print("UID=121 229 158 132;DEVID=");
        Serial.print(DEVICEID);    
        jsonBuffer.clear();    
        String json = Serial.readString(); 
        JsonObject& root = jsonBuffer.parseObject(json);    
        delay(1);
        if(json.length() != 0) {
          int code = root["code"];
            if (code == 200) {
              String nim = getValue(root["result"], ';', 0);
              String nama = getValue(root["result"], ';', 1);
              LCD.setCursor(0, 0);
              LCD.print("                ");
              LCD.setCursor(0, 1);
              LCD.print("                ");
              beep(700);
              //delay(50);
              LCD.setCursor(0, 0);
              LCD.print(nim);
              LCD.setCursor(0, 1);
              LCD.print(nama);
              delay(3000);
              state = 1;
              jsonBuffer.clear();
            } else if (code == 1) {
              String uid = root["result"];
              LCD.setCursor(0, 0);
              LCD.print("                ");
              LCD.setCursor(0, 1);
              LCD.print("                ");
              beep(50);
              delay(50);
              beep(50);
              delay(150);
              beep(100);
              LCD.setCursor(0, 0);
              LCD.print("Cek Web:        ");
              LCD.setCursor(0, 1);
              LCD.print(uid + "           ");
              delay(10000);
              state = 1;
              jsonBuffer.clear();
            } else if (code == 2) {
              String uid = root["result"];
              LCD.setCursor(0, 0);
              LCD.print("                ");
              LCD.setCursor(0, 1);
              LCD.print("                ");
              beep(100);
              delay(150);
              beep(100);
              delay(150);
              beep(100);
              LCD.setCursor(0, 0);
              LCD.print("Tambah ke Web:        ");
              LCD.setCursor(0, 1);
              LCD.print(uid + "           ");
              delay(10000);
              state = 1;
              jsonBuffer.clear();
            } else if (code == 404) {
              LCD.setCursor(0, 0);
              LCD.print("KTM tidak      ");
              LCD.setCursor(0, 1);
              LCD.print("terdaftar!     ");
              beep(500);
              delay(50);
              beep(500);
              //LCD.print(code);
              delay(3000);
            }
          
        } 
      }
    delay(5);    
}
