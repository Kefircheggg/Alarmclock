
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
byte Backlight_pin = 10; //Пин подсветки
byte led_pin = 9;  //Пин светодиода
byte dht_pin = 7; //Пин датчика температуры
byte buzzer_pin = 8; //Пин пищалки
AmperkaKB KB(0, 1, 2, 3, 4, 5, 6); //Пины матричной клавиатуры
LiquidCrystal_I2C lcd(0x3F,16,2); //Адрес и размер дисплея
DHT dht(dht_pin,DHT11); //Тип дачтика: DHT11 или DHT22
//-----------Настройки----------- 

//----------Служебное------------
RTC_DS1307 rtc; //Тип часов
String version = "V2.3.5"; 
unsigned long standby_timer;
byte clockmode = 1;
byte alarmtimeminute, alarmtimehour, possettings; //Всякие переменные
byte Showdatepos, resetpos, max_temp, min_temp, now_temp, stb_time, kostyl = 0;
boolean Backlight_flag, alarm,ShowDate;
//----------Служебное------------

void setup() {
  randomSeed(analogRead(0));
  pinMode(Backlight_pin, OUTPUT); //Установка пинов 
  pinMode(led_pin,OUTPUT); //Установка пинов
  lcd.init(); //Инициализация дисплея     
  lcd.backlight();
  KB.begin(KB4x3);  //Тип клавиатуры
  dht.begin(); //Инициализация датчика температуры
  rtc.begin(); //Инициализация часов
  if (! rtc.isrunning()) { rtc.adjust(DateTime(__DATE__, __TIME__)); } //Настройка часов 
}
 
void loop() {
  if(now_temp > max_temp) { max_temp = now_temp; }
  if(now_temp < min_temp) { min_temp = now_temp; }
  if(min_temp == 0) { min_temp = 50; }
  alarm = EEPROM.read(2); //Выставление значений переменным в зависимости от EEPROM
  if(EEPROM.read(5) == true) { Calibrationfunction(); } //Если ардуина не настроена, то запустить этот процесс
  if(kostyl == 0) {
    alarmtimehour = EEPROM.read(4); //Выставление значений переменным в зависимости от EEPROM
    alarmtimeminute = EEPROM.read(3); //Выставление значений переменным в зависимости от EEPROM
    stb_time = EEPROM.read(1); //Выставление значений переменным в зависимости от EEPROM
    kostyl++;} 
  ShowDate = EEPROM.read(0); //Выставление значений переменным в зависимости от EEPROM
  DateTime now = rtc.now(); //Время
  if(KB.onPress()) { //Процесс выбора меню
    if(KB.getNum == 1 && clockmode > 1) { clockmode--; } //Если нажата кнопка 1 - то увеличиваем значение режима
    if(KB.getNum == 2) { clockmode = 1; } //Если нажата кнопка 2 - то меню 2
    if(KB.getNum == 3 && clockmode < 4) { clockmode++; } //Если нажата кнопка 3 - то меню 3
    if(KB.getNum == 14 && Backlight_flag == true) {
      Backlight_flag = false; //Опускаем флажок
      for(byte i = 255; i>0; i--) { analogWrite(Backlight_pin, i); delay(2); } //Система плавного выключения подсветки
      digitalWrite(Backlight_pin, LOW); //Выключаем подсветку
    } 
    if(KB.getNum == 14) { //Если кнопка - *, то
      for(byte i = 0; i<255; i++) { analogWrite(Backlight_pin, i); delay(2); } //Система плавного включения подсветки
      digitalWrite(Backlight_pin, HIGH); //Включаем подсветку
      Backlight_flag = true; //Поднимаем флажок подсветки
      standby_timer = millis(); //``\_(-_-)_/``
    }
  }

if(clockmode == 1) { //Меню 1 - главное
      if(now.hour() < 10) { //Тут у меня система красивого написания чисел, если число меньше 10 то,
      lcd.print("0"); //В начале приписывается 0
      lcd.print(now.hour(), DEC); //И уже потом идет число
      }
      if(now.hour() >= 10) { lcd.print(now.hour(), DEC); }//Если число больше 10 то просто пишем число  
      lcd.print(":");
      if(now.minute() < 10) {//С минутами такая-же история
       lcd.print("0");
       lcd.print(now.minute(), DEC); 
      }
      if(now.minute() >= 10) { lcd.print(now.minute(), DEC); }
      lcd.print(" T:");
      if(now_temp >= 10) { lcd.print(now_temp); } //И с температурой тоже
      if(now_temp < 10){ lcd.print("0"); lcd.print(now_temp); }
      
      if(alarm == true) { //Если будильник включен, то
        lcd.setCursor(15,0);
        lcd.print("\xed"); //Отображаем значок на дисплее
        }
      if(ShowDate == true) {
        lcd.setCursor(0,1);
        if(now.day() < 10) { //У даты - тоже такая же фигня
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

if(clockmode == 2) { //Меню 2 - меню с установками будильника
  /*
  В втором меню управление такое - кнопки 4 и 7 это управление часами(временем) + -
  Кнопки 6 и 9 это управление минутами + -
  Кнопка 5 - выставить 30 минут
  Кнопка 8 - выставить 12 часов
  Кнопка 0 - выставить текущее время
  */
  lcd.print("Alarm setting"); //Отображаем текст
  lcd.setCursor(0,1);
  lcd.print(alarmtimehour);
  lcd.print(":");
  lcd.print(alarmtimeminute);
  if(KB.isPressed()) {
    if(KB.getNum == 4)  { alarmtimehour++; } //если нажата кнопка 4 то прибавить часы
    if(KB.getNum == 7)  { alarmtimehour--; } //если нажата кнопка 7 то убавить часы
    if(KB.getNum == 6)  { alarmtimeminute++; } //если нажата кнопка 6 то прибавить минуты
    if(KB.getNum == 9)  { alarmtimeminute--; } //если нажата кнопка 9 то убавить минуты
    if(KB.getNum == 5)  { alarmtimeminute=30;} //если нажата кнопка 5 то выставить 30 минут
    if(KB.getNum == 8)  { alarmtimehour=12;} //если нажата кнопка 8 то выставить 12 часов
    if(KB.getNum == 0)  { alarmtimehour = now.hour(); alarmtimeminute = now.minute();} //если нажата кнопка 0 то выставить текущее время
    if(KB.getNum == 15 && alarm == false) {
      while(true) {  //Цикл подтверждения установки будильника
        digitalWrite(Backlight_pin, HIGH); //Включаем подсветку
        lcd.setCursor(3,0); //Ставим курсор
        lcd.print("Set alarm?"); //Отображаем информацию
        lcd.setCursor(0,1); //Ставим курсор
        lcd.print("  Press * or #"); //Отображаем информацию
        if(KB.onPress()) { //Если нажата кнопка
          if(KB.getNum == 15) { //Если кнопка - #
          EEPROM.write(2,true); //Включаем будильник
          lcd.clear(); //Очищаем дисплей
          lcd.print("    Alarm set"); //Отображаем информацию
          delay(1000); //Ждем, пока юзер прочитает
          clockmode = 1; //Включаем режим 1
          EEPROM.write(3,alarmtimeminute); 
          EEPROM.write(4,alarmtimehour);
          digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
          break; //выходим из цикла
          }
          if(KB.getNum == 14) { //Если кнопка - *
          EEPROM.write(2,false); //Не включаем будильник
          lcd.clear(); //Очищаем дисплей
          lcd.print(" Alarm dont set"); //Отображаем информацию
          delay(1000); //Ждем, пока юзер прочитает
          clockmode = 1; //Включаем режим 1
          EEPROM.write(3, 0); 
          EEPROM.write(4, 0);
          digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
          break; //выходим из цикла
          }
        }
          delay(100); //Ждем для стабильности
          lcd.clear(); //Очищаем дисплей
      }   
       }
    if(KB.getNum == 15 && alarm == true) { //Если будильник уже включен
      while(true) { //Запускаем цикл подтверждения выключения будильника
        digitalWrite(Backlight_pin, HIGH); //Включаем подсветку
        lcd.setCursor(0,0); //Ставим курсор
        lcd.print(" Disable alarm?"); //Отображаем информацию
        lcd.setCursor(0,1); //Ставим курсор
        lcd.print("  Press * or #"); //Отображаем информацию
        if(KB.onPress()) { //Если нажата кнопка
          if(KB.getNum == 15) { //Если кнопка - #
          EEPROM.write(2,false); //Выключаем будильник
          lcd.clear(); //Очищаем дисплей
          lcd.print(" Alarm disabled"); //Отображаем информацию
          delay(1000); //Ждем пока юзер прочитает 
          clockmode = 1; //Ставим режим 1
          digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
          break; //выходим из цикла
          }
          if(KB.getNum == 14) { //Если кнопка - *
          lcd.clear(); //Очищаем дисплей
          lcd.print("Alarm ne dis"); //Отображаем информацию
          delay(1000); //Ждем пока юзер прочитает
          clockmode = 1; //Ставим режим 1
          digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
          break; //выходим из цикла
          }
        }
          delay(100); //ждем для стабильности
          lcd.clear(); //Очищаем дисплей
        }   
       }
      } 
  if(alarmtimehour == 24) { alarmtimehour=0;} //при переполнении часов сбрасываем
  if(alarmtimeminute == 60) {alarmtimeminute=0; alarmtimehour++;} //при переполнении минут сбрасываем
  if(alarmtimeminute < 0) {alarmtimeminute = 59;};
  if(alarmtimehour < 0) {alarmtimehour = 23;};
}

if(clockmode == 3) { //Меню 3 - меню настроек
  /*
  Кнопка 0 - ниже
  Кнопка 5 - выше
  Кнопка 7 - если пункт отображение даты - то это выставить нет, если время работы подсветки - то выставить 5сек
  Кнопка 8 - если пункт сброса(reset) - то это сбросить, если время работы подсветки - то выставить 10сек
  Кнопка 9 - если пункт отображение даты - то это выставить да, если время работы подсветки - то выставить 20сек
  */
  if(possettings == -1) { possettings = 3; } //Это при 
  if(possettings == 4) { possettings = 0; } //Переполнении
  if(KB.isPressed()) { //Если кнопка нажата
    if(KB.getNum == 0) { possettings++; }  //Если кнопка - 0 опускаем курсор
    if(KB.getNum == 5) { possettings--; } //Если кнопка - 5 Поднимаем курсор   
  }
  lcd.setCursor(11,0);  //Выставляем курсор
  if(possettings == 0 || possettings == 1) { //Алгоритм отрисовки времени работы подсветки
  lcd.setCursor(12,0);
  lcd.print(stb_time);
  }
  lcd.setCursor(0,0); //Выставляем курсор
  if(possettings == 0) { //Это система выставления > в зависимости от позиции  
    lcd.print(">Time light "); //Отображаем информацию
    if(KB.isPressed()) { //Если кнопка нажата
      if(KB.getNum == 7) { stb_time--;} //Если кнопка - 7, то выставить 5 секунд
      if(KB.getNum == 8) { EEPROM.write(1, stb_time);}
      if(KB.getNum == 9) { stb_time++; } //Если кнопка - 9, то выставить 20 секунд
    }
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print(" Show date  "); //Отображаем информацию
    lcd.print(ShowDate);  //Флажок отображения даты
  }
  if(possettings == 1) { //Если пользователь нажал 0
    if(KB.isPressed()) { //Если кнопка нажата
      if(KB.getNum == 7) { EEPROM.write(0, false); } //Если кнопка - 7, то опустить флажок и запиисать в EEPROM 0
      if(KB.getNum == 9) { EEPROM.write(0, true); } //Если кнопка - 9, то поднять флажок и запиисать в EEPROM 1
    }
    lcd.print(" Time light");  //Отображаем информацию
    lcd.setCursor(0,1); //Ставим курсор
    lcd.print(">Show date  "); //Отображаем информацию
    lcd.print(ShowDate); //Отображаем состояние флажка отображения даты
  } 
  if(possettings == 2){ //Это раздел сброса настроек
    if(KB.isPressed()) { //Если кнопка нажата
      if(KB.getNum == 8) { //Если кнопка - 8,то
        while(true) { //Входим в цикл выбора: сбросить, или нет
          digitalWrite(Backlight_pin,HIGH); //Включаем подсветку
          if(KB.onPress()) { //Если кнопка нажата 
            if(KB.getNum == 9) { resetpos=1; } //Если кнопка - 6 то ставим в положение YES
            if(KB.getNum == 7) { resetpos=0; } //Если кнопка - 4 то ставим в положение NO
            if(KB.getNum == 8 && resetpos == 1) { //Если кнопка - 5, и положение YES 
              for(byte i = 0; i<5; i++) { EEPROM.write(i,0); delay(200); } //Сбрасываем все значения EEPROM
              EEPROM.write(5,true); //Поднимаем флажок настройки
              lcd.print("reboot me please"); //Отображаем информацию
              delay(3000); //ждем, пока юзер прочитает
              }
            if(KB.getNum == 8 && resetpos == 0) { //Если кнопка - 5, и положение NO 
              clockmode = 1; //Ставим режим 1
              delay(200); //Ждем
              break; //выходим из цикла
             }   
            }
          lcd.print("     Reset?"); //Отображаем информацию
          lcd.setCursor(0,1); //Ставим курсор
          if(resetpos == 0) { //Система отображения [], простая
          lcd.print("   [NO]  YES");
          }
          if(resetpos == 1) {
          lcd.print("    NO  [YES]");
          }
          delay(100); //Ждем
          lcd.clear(); //Очищаем дисплей
        }
      }
     }
    lcd.print(" Show Date  "); //Это уже не цикл, это продолжение отдела настроек, Отображаем информацию
    lcd.print(ShowDate); //Отображаем состояние флажка отображения даты
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print(">Reset  "); //Отображаем информацию
  }
  if(possettings == 3) {
    lcd.print(" Reset");
    lcd.setCursor(0,1);
    lcd.print(">About program");
    if(KB.isPressed()) { if(KB.getNum == 8) { AboutProgramm(); } }
  } 
}

if(clockmode == 4) { //Меню 4 - меню в котором отображается минимальная/максимальная температура и время срабатывания будильника если он включен
  lcd.print("Max T:");
  lcd.print(max_temp);
  lcd.print(" Min:");
  lcd.print(min_temp);
  lcd.setCursor(0,1);
  if(alarm == true) {
    lcd.print(alarmtimehour);
    lcd.print(":");
    lcd.print(alarmtimeminute);
    }
  if(alarm == false) { lcd.print("No alarm set!"); }
}

now_temp = dht.readTemperature();

  if(Backlight_flag == true) { //Если флажок поднят
      if(millis() - standby_timer >= 1000*stb_time) { //Если с момента нажатия прошло больше чем stb_time*1000
          Backlight_flag = false; //Опускаем флажок
          for(byte i = 255; i>0; i--) { analogWrite(Backlight_pin, i); delay(2); } //Система плавного выключения подсветки
          digitalWrite(Backlight_pin, LOW); //Выключаем подсветку
          standby_timer = millis(); //см. строка 314
      }
  }

    if(alarm == true && alarmtimehour == now.hour() && alarmtimeminute == now.minute()) { alarmFunction(); } //Система включения будильника

    delay(60); //Задержа для стабильности  
    lcd.clear(); //Очищаем дисплей
}

  void alarmFunction() { //Это функция будильника, тут можно творить что угодно
            byte passalarm0 = random(10);
            byte passalarm1 = random(10);
            while(true) { //Бесконечный цикл, который прерывается только по нажатию на две рандомные кнопки
            DateTime now = rtc.now();
            digitalWrite(Backlight_pin, HIGH);
            digitalWrite(led_pin, HIGH);
            lcd.clear(); //Очищаем дисплей
            lcd.setCursor(4,0); //Выставляем курсор
            lcd.print("Get Up!"); //Отображаем информацию, тут можно написать что угодно
            lcd.setCursor(0,1); //Выставляем курсор
            lcd.print(passalarm0); //Пишем первое радномное число
            lcd.print(" "); //Пробел
            lcd.print(passalarm1); //Пишем второе радномное число
            tone(buzzer_pin,3500,250); //Пищим пищалкой
            tone(buzzer_pin,100,100); //Пищим пищалкой
            if(KB.onPress()) { //Если кнопка нажата
              if(KB.getNum == 14) {
                EEPROM.write(3, now.minute() + 5);
                lcd.clear();
                lcd.print("You have 5 min");
                pressfornext(14);
                for(byte i = 255; i>255; i--) {
                  analogWrite(led_pin, i);
                  analogWrite(Backlight_pin, i);
                  delay(2);
                  }
                digitalWrite(Backlight_pin, LOW);
                digitalWrite(led_pin, LOW);
                break;
              }
              if(KB.getNum != passalarm0) { //Если нажатая кнопка не равна рандомной кнопке
                lcd.clear(); //Очищаем дисплей
                lcd.setCursor(0,0); //Выставляем курсор 
                lcd.print("Dont this button"); //Отображаем информацию
                lcd.setCursor(0,1); //Выставляем курсор 
                lcd.print("Button: "); //Отображаем информацию
                lcd.print(passalarm0); //Отображаем нужную кнопку               
                delay(1000);
              }
              if(KB.getNum == passalarm0) { //Если нажатая кнопка равна рандомной кнопке
                while(true) { //Запускаем цикл
                lcd.clear(); //Очищаем дисплей 
                lcd.print("Next step"); //Отображаем информацию
                delay(30);
                if(KB.onPress()) { //Если кнопка нажата
                if(KB.getNum != passalarm1) { //Если вторая нажатая кнопка не равна второй рандомной кнопке
                  lcd.clear(); //Очищаем дисплей
                  lcd.setCursor(0,0); //Выставляем курсор 
                  lcd.print("Dont this button"); //Отображаем информацию
                  lcd.setCursor(0,1); //Выставляем курсор
                  lcd.print("Button: "); //Отображаем информацию
                  lcd.print(passalarm1); //Отображаем вторую рандомную кнопку
                  delay(1000); //Ждем, пока юзер прочитает
                }
                if(KB.getNum == passalarm1) { //Если все правильно
                EEPROM.write(2, false); //Опускаем флажок будильника
                for(byte i = 255; i>255; i--) {
                  analogWrite(led_pin, i);
                  analogWrite(Backlight_pin, i);
                  delay(1);
                  }
                digitalWrite(Backlight_pin, LOW);
                digitalWrite(led_pin, LOW);
                break;
                    }
                  }
                }
              }
            }
            digitalWrite(Backlight_pin, LOW);
            digitalWrite(led_pin, LOW);
            delay(100); //задержка
            if(EEPROM.read(2) == false) { break; }
    }  
  }
  
  void pressfornext(byte button) { //Это очень удобная функция, ждет нажатия, и только тогда идет дальше
    while(true) { //бесконечный цикл
      if(KB.onPress()) { //При нажатии кнопки
        if(KB.getNum == button) { break; } //Если кнопка равна указанной выходим из цикла
      }
      delay(10); //задержка
    }
  }

  void Calibrationfunction() { //Это функция первоначальной настройки
    for(byte i = 0; i<255; i++) { analogWrite(Backlight_pin, i); delay(3); }
    digitalWrite(Backlight_pin,HIGH);  //Включаем подсветку
    lcd.setCursor(5,0); //Ставим курсор
    lcd.print("H"); //Анимация слова "Hello!"  
    delay(150);
    lcd.print("e");
    delay(150);
    lcd.print("l");
    delay(150);
    lcd.print("l");
    delay(150);
    lcd.print("o");
    delay(150);
    lcd.print("!");
    pressfornext(8);
    lcd.clear(); //Очищаем дисплей 
    lcd.print("This calibration,"); //Отображаем информацию
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print("to continue press 8"); //Отображаем информацию
    pressfornext(8); //Запускаем ту функцию
    delay(20); //Ждем
    lcd.clear(); //Очищаем дисплей
      while(true) { //Цикл настройки отображения даты
        if(KB.onPress()) { //Если нажата кнопка 
          if(KB.getNum == 7) { Showdatepos=0; } //Если кнопка - 4, то ставим на YES
          if(KB.getNum == 9) { Showdatepos=1; } //Если кнопка - 6, то ставим на NO
          if(KB.getNum == 8) { //Если кнопка - 5, то записываем в EEPROM 1 или 0
            if(Showdatepos == 0) { //Если была нажата кнопка 4, то
              EEPROM.write(0,true); //Записываем в EEPROM 1
              lcd.clear(); //Очищаем дисплей
              lcd.print("Date will be"); //Отображаем информацию
              lcd.setCursor(0,1);
              lcd.print("Displayed");
              pressfornext(8); //Ждем, пока юзер прочитает
              break; //Выходим из цикла
            }
            if(Showdatepos == 1) { //Если была нажата кнопка 6, то
              EEPROM.write(0,false); //Записываем в EEPROM 0
              lcd.clear(); //Очищаем дисплей
              lcd.print("Date will not be displayed"); //Отображаем информацию
              pressfornext(8); //Ждем, пока юзер прочитает
              break; //Выходим из цикла
            }
          }
        }
        lcd.setCursor(3,0); //Ставим курсор
        lcd.print("Show Date?"); //Отображаем информацию
        lcd.setCursor(0,1); //Ставим курсор
        if(Showdatepos == 0) { lcd.print("  [Yes]    No   "); }//Тут система отрисовки [] в зависимости от нажатой кнопки
        if(Showdatepos == 1) { lcd.print("   Yes    [No]  "); }
        delay(50); //Ждем для стабильности
        lcd.clear(); //Очищаем дисплей
    }
    lcd.clear(); //Очищаем дисплей
    lcd.print("Go ahead"); //Отображаем информацию
    pressfornext(8); //Ждем
    lcd.clear(); //Очищаем дисплей
    lcd.print("Choose how long"); //Отображаем информацию
    lcd.setCursor(0,1); //Выставляем курсор 
    lcd.print("the backlight"); //Отображаем информацию
    pressfornext(8); //см. строку 373
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("the backlight");
    lcd.setCursor(0,1);
    lcd.print("will work");
    pressfornext(8);
    delay(100);
    while(true) { //Цикл выбора времени работы подсветки
        if(KB.isPressed()) { //При нажатии кнопки 
          if(KB.getNum == 7 || stb_time > 99) { stb_time--;} //Если кнопка - 7, то выставить 5 секунд
          if(KB.getNum == 9 || stb_time < 0) { stb_time++; } //Если кнопка - 9, то выставить 20 секунд
          if(KB.getNum == 8) {
            EEPROM.write(1, stb_time);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Backlight will ");
            lcd.setCursor(0,1);
            lcd.print("work ");
            lcd.print(stb_time);
            lcd.print(" seconds");
            pressfornext(8);
            break;
          }
          delay(7);
        }
        lcd.print(" Backlight time"); //Отображаем информацию
        lcd.setCursor(7,1); //Выставляем курсор
        lcd.print(stb_time);
        delay(50); //Ждем
        lcd.clear(); //Очищаем дисплей
    }
    lcd.clear(); //Очищаем дисплей
    lcd.print("calibration end,"); //Отображаем информацию
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print("you can restart"); //Отображаем информацию
    pressfornext(8); //см. строку 373
    lcd.setCursor(0,0); //Выставляем курсор
    lcd.print("you can restart"); //Отображаем информацию
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print("it from settings"); //Отображаем информацию
    pressfornext(8); //см. строку 373
    lcd.setCursor(0,0); //Выставляем курсор
    lcd.clear(); //Очищаем дисплей 
    lcd.print("in future"); //Отображаем информацию
    pressfornext(8); //см. строку 373
    EEPROM.write(5, false); //Опускаем флажок настройки
    for(byte i = 255; i>0; i--) { analogWrite(Backlight_pin, i); delay(2); }
    digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
    //Далее юзера перекидывает в 1 меню
  } 

  void AboutProgramm() {
    lcd.clear();
          lcd.print("  Created 2017  ");
          lcd.setCursor(0,1);
          lcd.print(" By kefircheggg ");
          pressfornext(8);
          lcd.clear();
          lcd.print(" By kefircheggg ");
          lcd.setCursor(0,1);
          lcd.print("  Github.com/   ");
          pressfornext(8);
          lcd.clear();
          lcd.print("  Github.com/   ");
          lcd.setCursor(0,1);
          lcd.print("  Kefircheggg   ");
          pressfornext(8);
          lcd.clear();
          lcd.print("  Kefircheggg   ");
          lcd.setCursor(0,1);
          lcd.print(" Vk.com/maxnic  ");
          pressfornext(8);
          lcd.clear();
          lcd.print(" Vk.com/maxnic  ");
          lcd.setCursor(4,1);
          lcd.print(version);
          pressfornext(8);      
          lcd.clear();
  }
