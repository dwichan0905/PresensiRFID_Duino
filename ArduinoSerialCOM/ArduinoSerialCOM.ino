#include <ArduinoJson.h>
#include <LiquidCrystal.h>

#define RS 2
#define ENABLE 3
#define DATA4 4
#define DATA5 5
#define DATA6 6
#define DATA7 7
#define SPEAKER 8

int DEVICEID = 123;
int state = 0; // 0: Checking, 1: Register, 2: Not Register

LiquidCrystal LCD(RS, ENABLE, DATA4, DATA5, DATA6, DATA7);

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
        Serial.print(DEVICEID);
        LCD.setCursor(0, 0);
        String json = Serial.readString(); 
            
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
        LCD.print("COM RESPONSE:        ");
        LCD.setCursor(0, 1);
        LCD.print("                     ");
        String res;
    
        res = Serial.readString(); 
        if (res != "") {
          beep(50);
          beep(50);
          LCD.setCursor(0, 1);
          LCD.print(res);
          delay(3000);
        }
      } else {
        LCD.setCursor(0, 0);
        LCD.print("DEVICE NO REG       ");
        LCD.setCursor(0, 1);
        LCD.print("DEVID: 123          ");
        while(1) {}
      }
       
      
      
    delay(5);    
}
