/*
  Created 2017
  by Kefircheggg
  Maximka Home Labs Inc.
*/


//-------БИБЛИОТЕКИ---------
#include <EEPROM.h>
#include <RTClib.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AmperkaKB.h>
//-------БИБЛИОТЕКИ---------

//-----------Настройки-----------
int Backlight_pin = 10; //Пин подсветки
int led_pin = 9;  //Пин светодиода
int dht_pin = 7; //Пин датчика температуры
int busser_pin = 8; //Пин пищалки
AmperkaKB KB(6, 5, 4, 3, 2, 1, 0); //Пины матричной клавиатуры
LiquidCrystal_I2C lcd(0x3F,16,2); //Адрес и размер дисплея
DHT dht(dht_pin,DHT11); //Тип дачтика: DHT11 или DHT22
//-----------Настройки----------- 
RTC_DS1307 rtc; //Тип часов
int stb_time; //В секундах!
long standby_timer;
int clockmode = 1;
int alarmtimeminute,alarmtimehour, alarmtimehour0,alarmtimeminute0,possettings,Showdatepos,lightpos,resetpos; //Всякие переменные
int passalarm0 = random(10);
int passalarm1 = random(10);
boolean Backlight_flag;
boolean alarm,ShowDate,calibration;
void setup() {
  pinMode(Backlight_pin, OUTPUT); //Установка пинов 
  pinMode(led_pin,OUTPUT); //Установка пинов 
  lcd.init(); //Инициализация дисплея     
  lcd.backlight();
  KB.begin(KB4x3);  //Тип клавиатуры
  dht.begin(); //Инициализация датчика температуры
  rtc.begin(); //Инициализация часов
  if (! rtc.isrunning()) { 
rtc.adjust(DateTime(__DATE__, __TIME__)); //Настройка часов
    }

}
 
void loop() {  
  calibration = EEPROM.read(5);
  alarmtimehour = EEPROM.read(4);
  alarmtimeminute = EEPROM.read(3);
  alarm = EEPROM.read(2);
  stb_time = EEPROM.read(1);
  ShowDate = EEPROM.read(0);
  DateTime now = rtc.now();
  if(calibration == true) { Calibrationfunction(); }
   if(KB.isPressed()) {
    if(KB.getNum == 1) {
    clockmode = 1;
      }
    if(KB.getNum == 2) {
    clockmode = 2;
      }
    if(KB.getNum == 3) {
    clockmode = 3;
      }
    }
    if(clockmode == 1) {
      if(now.hour() < 10) {
      lcd.print("0");
      lcd.print(now.hour(), DEC);
      }
      if(now.hour() >= 10) {
      lcd.print(now.hour(), DEC);      
      }
      lcd.print(":");
      if(now.minute() < 10) {
       lcd.print("0");
       lcd.print(now.minute(), DEC); 
      }
      if(now.minute() >= 10) {
        lcd.print(now.minute(), DEC);
      }
      lcd.print(" T:");
      if(dht.readTemperature() >= 10) {
      lcd.print(dht.readTemperature(), 0);
      }
      if(dht.readTemperature() < 10){
      lcd.print("0");
      lcd.print(dht.readTemperature(), 0);
      }
      if(alarm == true) {
        lcd.setCursor(15,0);
        lcd.print("\xed");
        }
      if(ShowDate == true) {
        lcd.setCursor(0,1);
        if(now.day() < 10) {
          lcd.print("0");
          lcd.print(now.day(),DEC);
        }
        if(now.day() >= 10) { lcd.print(now.day(),DEC); }
        lcd.print(".");
        if(now.month() < 10) {
          lcd.print("0");
          lcd.print(now.month(),DEC);
        }
        if(now.month() >= 10) { lcd.print(now.month(),DEC); }
        lcd.print(".");
        lcd.print(now.year(), DEC);
      }  
  }
if(clockmode == 2) {
  EEPROM.write(3,alarmtimeminute0);
  EEPROM.write(4,alarmtimehour0);
  lcd.print("Alarm setting");
  lcd.setCursor(0,1);
  lcd.print(alarmtimehour0);
  lcd.print(":");
  lcd.print(alarmtimeminute0);
  if(KB.isPressed()) {
    if(KB.getNum == 4)  { alarmtimehour0++; }
    if(KB.getNum == 7)  { alarmtimehour0--; }
    if(KB.getNum == 6)  { alarmtimeminute0++; }
    if(KB.getNum == 9)  { alarmtimeminute0--; }
    if(KB.getNum == 5)  { alarmtimeminute0=30;}
    if(KB.getNum == 8)  { alarmtimehour0=12;}
    if(KB.getNum == 0)  { alarmtimehour0 = now.hour(); alarmtimeminute0 = now.minute();}
    if(KB.getNum == 15 && alarm == false) {
      while(true) {
        digitalWrite(Backlight_pin, HIGH);
        lcd.setCursor(3,0);
        lcd.print("Set alarm?");
        lcd.setCursor(0,1);
        lcd.print("  Press * or #");
        if(KB.onPress()) {
          if(KB.getNum == 15) {
          EEPROM.write(2,true);
          lcd.clear();
          lcd.print("    Alarm set");
          delay(500);
          clockmode = 1;
          digitalWrite(Backlight_pin,LOW);
          break;
          }
          if(KB.getNum == 14) {
          EEPROM.write(2,false);
          lcd.clear();
          lcd.print(" Alarm dont set");
          delay(500);
          clockmode = 1;
          digitalWrite(Backlight_pin,LOW);
          break;
          }
        }
          delay(100);
          lcd.clear();
      }   
       }
  }
    if(KB.getNum == 15 && alarm == true && clockmode == 2) {
      while(true) {
        digitalWrite(Backlight_pin, HIGH);
        lcd.setCursor(0,0);
        lcd.print(" Disable alarm?");
        lcd.setCursor(0,1);
        lcd.print("  Press * or #");
        if(KB.onPress()) {
          if(KB.getNum == 15) {
          EEPROM.write(2,false);
          lcd.clear();
          lcd.print(" Alarm disabled");
          delay(500);
          clockmode = 1;
          digitalWrite(Backlight_pin,LOW);
          break;
          }
          if(KB.getNum == 14) {
          EEPROM.write(2,true);
          lcd.clear();
          lcd.print("Alarm ne dis");
          delay(500);
          clockmode = 1;
          digitalWrite(Backlight_pin,LOW);
          break;
          }
        }
          delay(100);
          lcd.clear();
        }   
       }
  if(alarmtimehour == 24) { alarmtimehour=0;}
  if(alarmtimeminute == 60) {alarmtimeminute=0; alarmtimehour++;}
}
if(clockmode == 3) {
  if(KB.isPressed()) {
    if(KB.getNum == 0) {
      possettings++;
    }
    if(KB.getNum == 5) {
      possettings--;
    }    
  }
  if(possettings == -1) { possettings = 2; }
  if(possettings == 3) { possettings = 0; }
  lcd.setCursor(11,0);
  if(possettings == 0 || possettings == 1) {
  if(stb_time == 5) { lcd.print("  5s"); }
  if(stb_time == 10) { lcd.print(" 10s"); }
  if(stb_time == 20) { lcd.print(" 20s"); }
  }
  lcd.setCursor(0,0);
  if(possettings == 0) {
  lcd.print(">Time light");
  if(KB.isPressed()) {
    if(KB.getNum == 7) { EEPROM.write(1, 5); }
    if(KB.getNum == 8) { EEPROM.write(1, 10); }
    if(KB.getNum == 9) { EEPROM.write(1, 20); }
  }
  lcd.setCursor(0,1);
  lcd.print(" Show date  ");
  lcd.print(ShowDate);  
  }
  if(possettings == 1) {
    if(KB.isPressed()) {
      if(KB.getNum == 7) { EEPROM.write(0, false); }
      if(KB.getNum == 9) { EEPROM.write(0, true); }
  }
  lcd.print(" Time light");
  lcd.setCursor(0,1);
  lcd.print(">Show date  ");
  lcd.print(ShowDate);
  } 
  if(possettings == 2){
    if(KB.isPressed()) {
      if(KB.getNum == 8) { 
        while(true) {
          digitalWrite(Backlight_pin,HIGH);
          if(KB.onPress()) { 
            if(KB.getNum == 6) { resetpos=1; } 
            if(KB.getNum == 4) { resetpos=0; }
            if(KB.getNum == 5 && resetpos == 1) { 
              for(int i = 0; i<5; i++) { EEPROM.write(i,0); delay(200); }
              EEPROM.write(5,true);
              lcd.print("reboot me please");
              delay(200);
              } 
            }
          lcd.print("     Reset?");
          lcd.setCursor(0,1);
          if(resetpos == 0) {
          lcd.print("   [NO]  YES");
          }
          if(resetpos == 1) {
          lcd.print("    NO  [YES]");
          }
          delay(100);
          lcd.clear();
        }
      }
     }
  lcd.print(" Show Date  ");
  lcd.print(ShowDate);
  lcd.setCursor(0,1);
  lcd.print(">Reset  ");
  } 
}
if(KB.isPressed()) {
  if(KB.getNum == 14) {
  digitalWrite(Backlight_pin, HIGH);
  Backlight_flag = true;
  standby_timer = millis();
  }
}
  if(Backlight_flag == true) {
      if(millis() - standby_timer > 1000*stb_time) {
          Backlight_flag = false;
          digitalWrite(Backlight_pin, LOW);
          standby_timer = millis();
      }
  }
    if(alarm == true) {
      if(alarmtimehour == now.hour()) {
        if(alarmtimeminute == now.minute()) {
          if(clockmode == 1) {
            alarmFunction();
          }
        }
      }
    }
    delay(100);
    lcd.clear();
}

  void alarmFunction() {
            while(true){
            digitalWrite(led_pin,HIGH);
            digitalWrite(Backlight_pin,HIGH);
            lcd.clear();
            lcd.setCursor(4,0);
            lcd.print("Get Up!");
            lcd.setCursor(0,1);
            lcd.print(passalarm0);
            lcd.print(" ");
            lcd.print(passalarm1);
            tone(8,3500,250);
            tone(8,100,100);
            if(KB.onPress()) {
              if(KB.getNum != passalarm0) {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Dont this button");
                lcd.setCursor(0,1);
                lcd.print("Button: ");
                lcd.print(passalarm0);                
                delay(1000);
              }
              if(KB.getNum == passalarm0) {
                while(true) {
                lcd.clear();
                lcd.print("Next step");
                if(KB.onPress()) {
                if(KB.getNum != passalarm1) {
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Dont this button");
                  lcd.setCursor(0,1);
                  lcd.print("Button: ");
                  lcd.print(passalarm1);
                  delay(1000);
                }
                if(KB.getNum == passalarm1) {
                EEPROM.write(2, false);
                digitalWrite(led_pin,LOW);
                digitalWrite(Backlight_pin,LOW);
                clockmode = 1;
                break;
                  }
                }
                  delay(200);
                }
              }
            }
              delay(100);
                if(EEPROM.read(2) == false) { 
                passalarm0 = random(0,9); 
                passalarm1 = random(0,9); 
                break;
             }
             delay(50);
            }  
  }
  void pressfornext(int button) {
    while(true) {
      if(KB.onPress()) {
        if(KB.getNum == button) {
          break;
        }
      }
      delay(50);
    }
  }
  void Calibrationfunction() {
    digitalWrite(Backlight_pin,HIGH);
    lcd.setCursor(5,0);
    lcd.print("H");
    delay(100);
    lcd.print("e");
    delay(100);
    lcd.print("l");
    delay(100);
    lcd.print("l");
    delay(100);
    lcd.print("o");
    delay(100);
    lcd.print("!");
    delay(5000);
    lcd.clear();
    lcd.print("Its calibration");
    lcd.setCursor(0,1);
    lcd.print("For next press 5");
    pressfornext(5);
    delay(100);
    lcd.clear();
      while(true) {
        if(KB.onPress()) {
          if(KB.getNum == 4) { Showdatepos=0; }
          if(KB.getNum == 6) { Showdatepos=1; }
          if(KB.getNum == 5) { 
            if(Showdatepos == 0) {
              EEPROM.write(0,true);
              lcd.clear();
              lcd.print(" I Show date:)");
              delay(3000);
              break;
            }
            if(Showdatepos == 1) {
              EEPROM.write(0,false);
              lcd.clear();
              lcd.print("I dont show date");
              delay(3000);
              break;
            }
          }
        }
        lcd.setCursor(3,0);
        lcd.print("Show Date?");
        lcd.setCursor(0,1);
        if(Showdatepos == 0) {
          lcd.print("  [Yes]    No   ");
          }
        if(Showdatepos == 1) {
          lcd.print("   Yes    [No]  ");
          }
        delay(100);
        lcd.clear();
    }
    lcd.clear();
    lcd.print("Ok, Go next");
    delay(2000);
    lcd.clear();
    lcd.print(" Now we choise ");
    lcd.setCursor(0,1);
    lcd.print("time work light");
    pressfornext(5);
    while(true) {
        if(KB.onPress()) {
          if(KB.getNum == 4) { lightpos=0; }
          if(KB.getNum == 5) { lightpos=1; }
          if(KB.getNum == 6) { lightpos=2; }
          if(KB.getNum == 8) { 
            if(lightpos == 0) {
              EEPROM.write(1, 5); 
              lcd.clear();
              lcd.print("light work 5s");
              delay(3000);
              break;
            }
            if(lightpos == 1) {
              EEPROM.write(1, 10); 
              lcd.clear();
              lcd.print("light work 10s");
              delay(3000);
              break;  
            }
            if(lightpos == 2) {
              EEPROM.write(1, 20);
              lcd.clear();
              lcd.print("light work 20s");
              delay(3000);
              break;
            }
          }
        }
        lcd.print("Time work light?");
        lcd.setCursor(0,1);
        if(lightpos == 0) {
          lcd.print(" [5] 10  20");
          }
        if(lightpos == 1) {
          lcd.print("  5 [10] 20");
          }
        if(lightpos == 2) {
          lcd.print("  5  10 [20]");
          }
        delay(100);
        lcd.clear();
    }
    lcd.clear();
    lcd.print("calibration end,");
    lcd.setCursor(0,1);
    lcd.print("you can restart");
    pressfornext(5);
    lcd.setCursor(0,0);
    lcd.print("you can restart");
    lcd.setCursor(0,1);
    lcd.print("it from settings");
    pressfornext(5);
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("in future");
    pressfornext(5);
    EEPROM.write(5, false);
    digitalWrite(Backlight_pin,LOW);
  }

