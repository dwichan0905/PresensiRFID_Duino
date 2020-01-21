// Include Library yang dibutuhkan
#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

// Penentuan Port Arduino
#define RS 2
#define ENABLE 3
#define DATA4 4
#define DATA5 5
#define DATA6 6
#define DATA7 7
#define SPEAKER 8
#define SS_PIN 10
#define RST_PIN 9

// Device ID yang akan dikaitkan dengan Website
int DEVICEID = 123;

// Keadaan perangkat saat itu
int state = 0; // 0: Checking, 1: Register, 2: Not Register, 3: Find and Get Result

// Load Instances
LiquidCrystal LCD(RS, ENABLE, DATA4, DATA5, DATA6, DATA7);
MFRC522 mfrc522(SS_PIN, RST_PIN);
StaticJsonBuffer<500> jsonBuffer;

// cuma buat bunyi bunyi manja aja wkwkkw
void beep(int del) {
  tone(SPEAKER, 700);
  delay(del);
  noTone(SPEAKER);
}

// buat pecah string jadi beberapa bagian
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

// Perintah yg akan dijalankan pertama
void setup() {
  // put your setup code here, to run once:
  LCD.begin(16, 2);
  LCD.setCursor (0, 0);
  // Proses memulai semua library dan PIN
  LCD.print("Booting...");
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(SPEAKER, OUTPUT);
  beep(50);
  LCD.clear();

  // nungguin kamu *eh nungguin koneksi USB via Serial
  while (!Serial) {
    LCD.setCursor(0, 0);
    LCD.print("Menghubungkan...");
  }

  // kalo mereka sudah saling bertemu jodohnya
  beep(100);
  LCD.setCursor(0, 0);
  LCD.print("Terhubung!      ");
  delay(2000);
  
}

void loop() {
    // put your main code here, to run repeatedly:
    String cmd = Serial.readString(); // baca perintah serial

    // Kalo mereka berdua putus :(
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
      delay(5000); // Nunggu yang lebih baik dari si dia :')
    }
    if (state == 0) {
      // Nyariin si dia yang tak kunjung ada kabarnya, hmm :')
      LCD.setCursor(0, 0);
      LCD.print("Cek perangkat...");
      LCD.setCursor(0, 1);
      LCD.print("                ");

      // Ngeloop kesini kalo gagal parse data JSON
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
            // Gagal parse?
            goto th;
        } else {
          // Yee berhasil :v
          if (code == 200) {
            // Ada nih perangkatnya di db :*
            LCD.setCursor(0, 0);
            LCD.print("Terdaftar:         ");
            LCD.setCursor(0, 1);
            LCD.print(devname);
            state = 1;
            delay(3000);
            jsonBuffer.clear();
          } else {
            // Kamu siapa?
            state = 2;
          }
        }
      }   
    } else if (state == 1) {
      // Proses nungguin kartu nikah *eh kartu RFID :v
      LCD.setCursor(0, 0);
      LCD.print("Tempelkan Kartu      ");
      LCD.setCursor(0, 1);
      LCD.print("Mahasiswa Anda...    ");
      
      // 2 baris di bawah cuma buat mode debugging aja, komentarin aja kalo kalian kesel :v
      delay(10000);
      state = 3;
      
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

      // Proses kirim perintah via serial
      String content= "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
         content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
         content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      content.toUpperCase();
      Serial.print("UID=");
      Serial.print(content);
      Serial.print(";DEVID=");
      Serial.print(DEVICEID);
      state = 3; // Ubah keadaan
      
//      Serial.print("Message : ");
//      content.toUpperCase();
//      if (content.substring(1) == "BD 31 15 2B") //change here the UID of the card/cards that you want to give access
//      {
//        Serial.println("Authorized access");
//        Serial.println();
//        delay(3000);
//      } else {
//        Serial.println(" Access denied");
//        delay(3000);
//      }

    } else if (state == 2) {
      // Kamu siapa?
      LCD.setCursor(0, 0);
      LCD.print("DEVICE NO REG       ");
      LCD.setCursor(0, 1);
      LCD.print("DEVID=");
      LCD.print(DEVICEID);
      while(1) {} // loop tak berujung, kek cinta :v
      
    } else if (state == 3) {
      LCD.setCursor(0, 0);
      LCD.print("Membaca Kartu   ");
      LCD.setCursor(0, 1);
      LCD.print("                ");
      
      // 2 baris di bawah cuma buat mode debugging aja, komentarin aja kalo kalian kesel :v
      Serial.print("UID=121 229 158 132;DEVID=");
      Serial.print(DEVICEID);    
      
      jsonBuffer.clear();   // bebersih bapernya *eh buffer :v 
      String json = Serial.readString(); 
      JsonObject& root = jsonBuffer.parseObject(json);    
      delay(1);
      if(json.length() != 0) {
        int code = root["code"];
          if (code == 200) {
            // kalo ketemunya mode presensi
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
            // kalo modenya tambah kartu, tapi udah ada
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
            // kalo masuk mode tambah, dan berhasil ditambah ke web
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
            // dah lah males
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
