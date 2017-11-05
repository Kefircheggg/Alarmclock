/*
  Created 2017
  by Kefircheggg
  Maximka Home Labs Inc.
  rus lang version test
*/

//-------БИБЛИОТЕКИ---------
#include <EEPROM.h>
#include <RTClib.h>
#include "DHT.h"
#include <Wire.h>
#include <LCD_1602_RUS.h>
#include <AmperkaKB.h>
//-------БИБЛИОТЕКИ---------

//-----------Настройки-----------
#define Backlight_pin  10 //Пин подсветки
#define led_pin 9  //Пин светодиода
#define dht_pin 7 //Пин датчика температуры
#define buzzer_pin 8 //Пин пищалки
AmperkaKB KB(6, 5, 4, 3, 2, 1, 0); //Пины матричной клавиатуры
LCD_1602_RUS lcd(0x3F, 16, 2);//Адрес и размер дисплея
DHT dht(dht_pin,DHT11); //Тип дачтика: DHT11 или DHT22
//-----------Настройки----------- 
RTC_DS1307 rtc; //Тип часов
String version = "V2.1.1"; 
unsigned long standby_timer;
byte clockmode = 1;
byte alarmtimeminute,alarmtimehour, alarmtimehour0,alarmtimeminute0,possettings,Showdatepos,lightpos,resetpos, max_temp, last_temp, min_temp, now_temp; //Всякие переменные
int stb_time;
byte passalarm0 = random(10);
byte passalarm1 = random(10);
boolean Backlight_flag, Backlight_constant_flag, alarm,ShowDate,calibration;

void setup() {
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
  calibration = EEPROM.read(5); //Выставление значений переменным в зависимости от EEPROM
  alarmtimehour = EEPROM.read(4); //Выставление значений переменным в зависимости от EEPROM
  alarmtimeminute = EEPROM.read(3); //Выставление значений переменным в зависимости от EEPROM
  alarm = EEPROM.read(2); //Выставление значений переменным в зависимости от EEPROM
  stb_time = EEPROM.read(1); //Выставление значений переменным в зависимости от EEPROM
  ShowDate = EEPROM.read(0); //Выставление значений переменным в зависимости от EEPROM
  DateTime now = rtc.now(); //Время
  if(calibration == true) { Calibrationfunction(); } //Если ардуина не настроена, то запустить этот процесс
  if(KB.isPressed()) { //Процесс выбора меню 
    if(KB.getNum == 1) { clockmode--; } //Если нажата кнопка 1 - то увеличиваем значение режима
    if(KB.getNum == 2) { clockmode = 1; } //Если нажата кнопка 2 - то меню 2
    if(KB.getNum == 3) { clockmode++; } //Если нажата кнопка 3 - то меню 3
  }
    if(clockmode == 0) { clockmode = 4;}
    if(clockmode == 5) { clockmode = 1;}

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
      if(last_temp == now_temp) {
      if(now_temp >= 10) { lcd.print(now_temp); } //И с температурой тоже
      if(now_temp < 10){ lcd.print("0"); lcd.print(now_temp); }
      }
      if(last_temp != now_temp) { last_temp = now_temp; }
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
  EEPROM.write(3,alarmtimeminute0); 
  EEPROM.write(4,alarmtimehour0);
  lcd.setCursor(0,0);  
  lcd.print(L"Будильник"); //Отображаем текст
  lcd.setCursor(0,1);
  lcd.print(alarmtimehour0);
  lcd.print(":");
  lcd.print(alarmtimeminute0);
  if(KB.isPressed()) {
    if(KB.getNum == 4)  { alarmtimehour0++; } //если нажата кнопка 4 то прибавить часы
    if(KB.getNum == 7)  { alarmtimehour0--; } //если нажата кнопка 7 то убавить часы
    if(KB.getNum == 6)  { alarmtimeminute0++; } //если нажата кнопка 6 то прибавить минуты
    if(KB.getNum == 9)  { alarmtimeminute0--; } //если нажата кнопка 9 то убавить минуты
    if(KB.getNum == 5)  { alarmtimeminute0=30;} //если нажата кнопка 5 то выставить 30 минут
    if(KB.getNum == 8)  { alarmtimehour0=12;} //если нажата кнопка 8 то выставить 12 часов
    if(KB.getNum == 0)  { alarmtimehour0 = now.hour(); alarmtimeminute0 = now.minute();} //если нажата кнопка 0 то выставить текущее время
    if(KB.getNum == 15 && alarm == false) {
      while(true) {  //Цикл подтверждения установки будильника
        digitalWrite(Backlight_pin, HIGH); //Включаем подсветку
        lcd.setCursor(3,0); //Ставим курсор
        lcd.print(L"Уcтaнoвить?"); //Отображаем информацию
        lcd.setCursor(0,1); //Ставим курсор
        lcd.print(L"    * или #"); //Отображаем информацию
        if(KB.onPress()) { //Если нажата кнопка
          if(KB.getNum == 15) { //Если кнопка - #
          EEPROM.write(2,true); //Включаем будильник
          lcd.clear(); //Очищаем дисплей
          lcd.setCursor(0,0);
          lcd.print(L"  Установлено  "); //Отображаем информацию
          delay(1000); //Ждем, пока юзер прочитает
          clockmode = 1; //Включаем режим 1
          digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
          break; //выходим из цикла
          }
          if(KB.getNum == 14) { //Если кнопка - *
          EEPROM.write(2,false); //Не включаем будильник
          lcd.clear(); //Очищаем дисплей
          lcd.setCursor(0,0);
          lcd.print(" Не установлено"); //Отображаем информацию
          delay(1000); //Ждем, пока юзер прочитает
          clockmode = 1; //Включаем режим 1
          digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
          break; //выходим из цикла
          }
        }
          delay(50); //Ждем для стабильности
          lcd.clear(); //Очищаем дисплей
      }   
       }
    if(KB.getNum == 15 && alarm == true && clockmode == 2) { //Если будильник уже включен
      while(true) { //Запускаем цикл подтверждения выключения будильника
        digitalWrite(Backlight_pin, HIGH); //Включаем подсветку
        lcd.setCursor(0,0); //Ставим курсор
        lcd.print("Дeaктивирoвать?"); //Отображаем информацию
        lcd.setCursor(0,1); //Ставим курсор
        lcd.print("    * или #"); //Отображаем информацию
        if(KB.onPress()) { //Если нажата кнопка
          if(KB.getNum == 15) { //Если кнопка - #
          EEPROM.write(2,false); //Выключаем будильник
          lcd.clear(); //Очищаем дисплей
          lcd.setCursor(0,0); //Ставим курсор 
          lcd.print(" Деактивировано"); //Отображаем информацию
          delay(1000); //Ждем пока юзер прочитает 
          clockmode = 1; //Ставим режим 1
          digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
          break; //выходим из цикла
          }
          if(KB.getNum == 14) { //Если кнопка - *
          lcd.clear(); //Очищаем дисплей
          lcd.setCursor(0,0); //Ставим курсор
          lcd.print("Не Деактивирован"); //Отображаем информацию
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
}

if(clockmode == 3) { //Меню 3 - меню настроек
  /*
  Кнопка 0 - ниже
  Кнопка 5 - выше
  Кнопка 7 - если пункт отображение даты - то это выставить нет, если время работы подсветки - то выставить 5сек
  Кнопка 8 - если пункт сброса(reset) - то это сбросить, если время работы подсветки - то выставить 10сек
  Кнопка 9 - если пункт отображение даты - то это выставить да, если время работы подсветки - то выставить 20сек
  */
  if(KB.isPressed()) { //Если кнопка нажата
    if(KB.getNum == 0) { possettings++; }  //Если кнопка - 0 опускаем курсор
    if(KB.getNum == 5) { possettings--; } //Если кнопка - 5 Поднимаем курсор   
  }
  if(possettings == -1) { possettings = 3; } //Это при 
  if(possettings == 4) { possettings = 0; } //Переполнении
  lcd.setCursor(12,0);  //Выставляем курсор
  if(possettings == 0 || possettings == 1) { //Алгоритм отрисовки времени работы подсветки 
  if(stb_time == 5) { lcd.print("  5s"); } //Тут все просто
  if(stb_time == 10) { lcd.print(" 10s"); } //Даже очень
  if(stb_time == 20) { lcd.print(" 20s"); } //Просто
  }
  lcd.setCursor(0,0); //Выставляем курсор
  if(possettings == 0) { //Это система выставления > в зависимости от позиции  
    lcd.print(">Bremя cвeта"); //Отображаем информацию
      if(KB.isPressed()) { //Если кнопка нажата
      if(KB.getNum == 7) { EEPROM.write(1, 5); } //Если кнопка - 7, то выставить 5 секунд
      if(KB.getNum == 8) { EEPROM.write(1, 10); } //Если кнопка - 8, то выставить 10 секунд
      if(KB.getNum == 9) { EEPROM.write(1, 20); } //Если кнопка - 9, то выставить 20 секунд
    }
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print(" Пoкaз дaты  "); //Отображаем информацию
    lcd.print(ShowDate);  //Флажок отображения даты
  }
  if(possettings == 1) { //Если пользователь нажал 0
    if(KB.isPressed()) { //Если кнопка нажата
      if(KB.getNum == 7) { EEPROM.write(0, false); } //Если кнопка - 7, то опустить флажок и запиисать в EEPROM 0
      if(KB.getNum == 9) { EEPROM.write(0, true); } //Если кнопка - 9, то поднять флажок и запиисать в EEPROM 1
    }
    lcd.print(" Bremя cвeта");  //Отображаем информацию
    lcd.setCursor(0,1); //Ставим курсор
    lcd.print(">Пoкaз дaты  "); //Отображаем информацию
    lcd.print(ShowDate); //Отображаем состояние флажка отображения даты
  } 
  if(possettings == 2){ //Это раздел сброса настроек
    if(KB.isPressed()) { //Если кнопка нажата
      if(KB.getNum == 8) { //Если кнопка - 8,то
        while(true) { //Входим в цикл выбора: сбросить, или нет
          digitalWrite(Backlight_pin,HIGH); //Включаем подсветку
          if(KB.onPress()) { //Если кнопка нажата 
            if(KB.getNum == 6) { resetpos=1; } //Если кнопка - 6 то ставим в положение YES
            if(KB.getNum == 4) { resetpos=0; } //Если кнопка - 4 то ставим в положение NO
            if(KB.getNum == 5 && resetpos == 1) { //Если кнопка - 5, и положение YES 
              for(int i = 0; i<5; i++) { EEPROM.write(i,0); delay(50); } //Сбрасываем все значения EEPROM
              EEPROM.write(5,true); //Поднимаем флажок настройки
              lcd.setCursor(0,0);
              lcd.clear();
              lcd.print("Пeрeзaгрyзи мeня"); //Отображаем информацию
              pressfornext(19);
              }
            if(KB.getNum == 5 && resetpos == 0) { //Если кнопка - 5, и положение NO 
              clockmode = 1; //Ставим режим 1
              delay(200); //Ждем
              break; //выходим из цикла
             }   
            }
          lcd.setCursor(0,0);
          lcd.print("   Сбросить?"); //Отображаем информацию
          lcd.setCursor(0,1); //Ставим курсор
          if(resetpos == 0) { //Система отображения [], простая
          lcd.print("   [Нет]  Да");
          }
          if(resetpos == 1) {
          lcd.print("    Нет  [Да]");
          }
          delay(100); //Ждем
          lcd.clear(); //Очищаем дисплей
        }
      }
     }
    lcd.print(" Пoкaз дaты  "); //Это уже не цикл, это продолжение отдела настроек, Отображаем информацию
    lcd.print(ShowDate); //Отображаем состояние флажка отображения даты
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print(">Cбрoc  "); //Отображаем информацию
  }
  if(possettings == 3) {
    lcd.print(" Сброс");
    lcd.setCursor(0,1);
    lcd.print(">О программе");
    if(KB.isPressed()) {
      if(KB.getNum == 8) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" Создано в 2017 ");
          lcd.setCursor(0,1);
          lcd.print("  kefircheggg ");
          pressfornext(8);
          lcd.clear();
          lcd.print("  kefircheggg ");
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
    }
  } 
}

if(clockmode == 4) { //Меню 4 - меню в котором отображается минимальная/максимальная температура и время срабатывания будильника если он включен
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Makc Т:");
  lcd.print(max_temp);
  lcd.print(" Mин:");
  lcd.print(min_temp);
  lcd.setCursor(0,1);
  if(alarm == true) {
    lcd.print(alarmtimehour);
    lcd.print(":");
    lcd.print(alarmtimeminute);
    }
  if(alarm == false) { lcd.print("Hety бyдильниkoв"); }
}

now_temp = dht.readTemperature();

//Это система работы подсветки, начало
if(KB.isPressed()) {
  if(KB.getNum == 14 && Backlight_flag == true) {
    Backlight_flag = false; //Опускаем флажок
    for(int i = 255; i>0; i--) { analogWrite(Backlight_pin, i); delay(2); } //Система плавного выключения подсветки
    digitalWrite(Backlight_pin, LOW); //Выключаем подсветку
  }  
}
if(KB.isPressed()) { //Если нажата кнопка
  delay(200); 
  if(KB.getNum == 14) { //Если кнопка - *, то
  for(int i = 0; i<255; i++) { analogWrite(Backlight_pin, i); delay(2); } //Система плавного включения подсветки
  digitalWrite(Backlight_pin, HIGH); //Включаем подсветку
  Backlight_flag = true; //Поднимаем флажок подсветки
  standby_timer = millis(); //Даже не знаю что написать, поэтому пук кек
  }
}
  if(Backlight_flag == true) { //Если флажок поднят
      if(millis() - standby_timer > 1000*stb_time) { //Если с момента нажатия прошло больше чем stb_time*1000
          Backlight_flag = false; //Опускаем флажок
          for(int i = 255; i>0; i--) { analogWrite(Backlight_pin, i); delay(2); } //Система плавного выключения подсветки
          digitalWrite(Backlight_pin, LOW); //Выключаем подсветку
          standby_timer = millis(); //см. строка 293
      }
  }
  //Конец
  //Функция срабатывания будильника, начало
    if(alarm == true) { //Если флажок будильника поднят
      if(alarmtimehour == now.hour()) { //Если час срабатывания настал
        if(alarmtimeminute == now.minute()) { //Если минута срабатывания настала 
          if(clockmode == 1 || clockmode == 3) { //Если меню 1 или 3
            alarmFunction(); //запускается сама функция будильника
          }
        }
      }
    }
  //Функция срабатывания будильника, конец

    delay(50); //Задержа для стабильности  
    lcd.clear(); //Очищаем дисплей
}

  void alarmFunction() { //Это функция будильника, тут можно творить что угодно
            while(true){ //Бесконечный цикл, который прерывается только по нажатию на две рандомные кнопки
            DateTime now = rtc.now();
            digitalWrite(Backlight_pin, HIGH);
            digitalWrite(led_pin, HIGH);
            lcd.clear(); //Очищаем дисплей
            lcd.setCursor(0,0); //Выставляем курсор
            lcd.print("  Прocыпaйcя!"); //Отображаем информацию, тут можно написать что угодно
            lcd.setCursor(6,1); //Выставляем курсор
            lcd.print(passalarm0); //Пишем первое радномное число
            lcd.print(" "); //Пробел
            lcd.print(passalarm1); //Пишем второе радномное число
            tone(buzzer_pin,3500,250); //Пищим пищалкой
            tone(buzzer_pin,100,100); //Пищим пищалкой
            if(KB.onPress()) { //Если кнопка нажата
              if(KB.getNum == 14) {
                EEPROM.write(3, now.minute() + 5);
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("У тебя 5 минут");
                pressfornext(14);
                for(int i = 255; i>255; i--) {
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
                lcd.print("Нe этa кнoпкa"); //Отображаем информацию
                lcd.setCursor(0,1); //Выставляем курсор 
                lcd.print("Kнoпкa: "); //Отображаем информацию
                lcd.print(passalarm0); //Отображаем нужную кнопку               
                delay(1000);
              }
              if(KB.getNum == passalarm0) { //Если нажатая кнопка равна рандомной кнопке
                while(true) { //Запускаем цикл
                lcd.clear(); //Очищаем дисплей 
                lcd.print("Слeдyющий шaг"); //Отображаем информацию
                delay(30);
                if(KB.onPress()) { //Если кнопка нажата
                if(KB.getNum != passalarm1) { //Если вторая нажатая кнопка не равна второй рандомной кнопке
                  lcd.clear(); //Очищаем дисплей
                  lcd.setCursor(0,0); //Выставляем курсор 
                  lcd.print("Нe этa кнoпкa"); //Отображаем информацию
                  lcd.setCursor(0,1); //Выставляем курсор
                  lcd.print("Kнoпкa: "); //Отображаем информацию
                  lcd.print(passalarm1); //Отображаем вторую рандомную кнопку
                  delay(1000); //Ждем, пока юзер прочитает
                }
                if(KB.getNum == passalarm1) { //Если все правильно
                EEPROM.write(2, false); //Опускаем флажок будильника
                for(int i = 255; i>255; i--) {
                  analogWrite(led_pin, i);
                  analogWrite(Backlight_pin, i);
                  delay(1);
                  }
                digitalWrite(Backlight_pin, LOW);
                digitalWrite(led_pin, LOW);
                passalarm0 = random(0,9); //Генерируем новое рандомное число
                passalarm1 = random(0,9); //Генерируем новое рандомное число
                break;
                    }
                  }
                }
              }
            }
            digitalWrite(Backlight_pin, LOW);
            digitalWrite(led_pin, LOW);
            delay(50); //задержка
            if(EEPROM.read(2) == false) { break; }
    }  
  }
  
  void pressfornext(int button) { //Это очень удобная функция, ждет нажатия, и только тогда идет дальше
    while(true) { //бесконечный цикл
      if(KB.onPress()) { //При нажатии кнопки
        if(KB.getNum == button) { break; } //Если кнопка равна указанной выходим из цикла
      }
      delay(10); //задержка
    }
  }

  void Calibrationfunction() { //Это функция первоначальной настройки
    digitalWrite(Backlight_pin,HIGH);  //Включаем подсветку
    lcd.setCursor(4,0); //Ставим курсор
    lcd.print("П"); //Анимация слова "Hello!"  
    delay(100);
    lcd.print("Р");
    delay(100);
    lcd.print("И");
    delay(100);
    lcd.print("В");
    delay(100);
    lcd.print("Е");
    delay(100);
    lcd.print("Т");
    delay(100);
    lcd.print("!");
    delay(5000);
    lcd.clear(); //Очищаем дисплей 
    lcd.setCursor(0,0);
    lcd.print("Этo нacтрoйкa"); //Отображаем информацию
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print("нaжми 5"); //Отображаем информацию
    pressfornext(5); //Запускаем ту функцию
    delay(100); //Ждем
    lcd.clear(); //Очищаем дисплей
      while(true) { //Цикл настройки отображения даты
        if(KB.onPress()) { //Если нажата кнопка 
          if(KB.getNum == 4) { Showdatepos=0; } //Если кнопка - 4, то ставим на YES
          if(KB.getNum == 6) { Showdatepos=1; } //Если кнопка - 6, то ставим на NO
          if(KB.getNum == 5) { //Если кнопка - 5, то записываем в EEPROM 1 или 0
            if(Showdatepos == 0) { //Если была нажата кнопка 4, то
              EEPROM.write(0,true); //Записываем в EEPROM 1
              lcd.clear(); //Очищаем дисплей
              lcd.setCursor(0,0);
              lcd.print("Я буду показывать"); //Отображаем информацию
              lcd.setCursor(0,1);
              lcd.print("дату");
              delay(3000); //Ждем, пока юзер прочитает
              break; //Выходим из цикла
            }
            if(Showdatepos == 1) { //Если была нажата кнопка 6, то
              EEPROM.write(0,false); //Записываем в EEPROM 0
              lcd.clear(); //Очищаем дисплей
              lcd.setCursor(0,0);              
              lcd.print("Я не буду "); //Отображаем информацию
              lcd.setCursor(0,1);
              lcd.print("показывать дату");
              delay(3000); //Ждем, пока юзер прочитает
              break; //Выходим из цикла
            }
          }
        }
        lcd.setCursor(0 ,0); //Ставим курсор
        lcd.print("Пokazывaть дaтy?"); //Отображаем информацию
        lcd.setCursor(0,1); //Ставим курсор
        if(Showdatepos == 0) { lcd.print("  [Дa]    Heт   "); }//Тут система отрисовки [] в зависимости от нажатой кнопки
        if(Showdatepos == 1) { lcd.print("   Дa    [Heт]  "); }
        delay(100); //Ждем для стабильности
        lcd.clear(); //Очищаем дисплей
    }
    lcd.clear(); //Очищаем дисплей
    lcd.print("Хoрoшo, идeм дaльшe"); //Отображаем информацию
    delay(2000); //Ждем
    lcd.clear(); //Очищаем дисплей
    lcd.print("Сейчас ты выберешь "); //Отображаем информацию
    lcd.setCursor(0,1); //Выставляем курсор 
    lcd.print("time work light"); //Отображаем информацию
    pressfornext(5); //см. строку 373
    while(true) { //Цикл выбора времени работы подсветки
        if(KB.onPress()) { //При нажатии кнопки 
          if(KB.getNum == 4) { lightpos=0; } //Если кнопка - 4 Ставим [] на 5
          if(KB.getNum == 5) { lightpos=1; } //Если кнопка - 5 Ставим [] на 10
          if(KB.getNum == 6) { lightpos=2; } //Если кнопка - 6 Ставим [] на 20
          if(KB.getNum == 8) { //Если кнопка - 8 
            if(lightpos == 0) { //То в зависимости от ранее нажатой кнопки 
              EEPROM.write(1, 5); //Ставим либо 5, 10 или 20 секунд
              lcd.clear(); //Очищаем дисплей
              lcd.setCursor(0,0);
              lcd.print("Пoдcвeткa бyдeт"); //Отображаем информацию
              lcd.setCursor(0,1);
              lcd.print("рaбoтaть 5 ceк"); //Отображаем информацию
              lcd.setCursor(0,0);
              delay(3000); //Ждем, пока юзер прочитает 
              break; //Выходим из цикла 
            }
            if(lightpos == 1) { //Тут все идентично
              EEPROM.write(1, 10); 
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Пoдcвeткa бyдeт"); //Отображаем информацию
              lcd.setCursor(0,1);
              lcd.print("рaбoтaть 10 ceк"); //Отображаем информацию
              lcd.setCursor(0,0);
              delay(3000);
              break;  
            }
            if(lightpos == 2) { //Тут все идентично
              EEPROM.write(1, 20);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Пoдcвeткa бyдeт"); //Отображаем информацию
              lcd.setCursor(0,1);
              lcd.print("рaбoтaть 20 ceк"); //Отображаем информацию
              lcd.setCursor(0,0);
              delay(3000);
              break;
            }
          }
        }
        lcd.setCursor(0,1);
        lcd.print("Время работы подсветки?"); //Отображаем информацию
        lcd.setCursor(0,1); //Выставляем курсор
        if(lightpos == 0) { lcd.print(" [5] 10  20 "); } //Рисуем [] в зависимости от нажатой кнопки  
        if(lightpos == 1) { lcd.print("  5 [10] 20 "); }
        if(lightpos == 2) { lcd.print("  5  10 [20]"); }
        delay(100); //Ждем
        lcd.clear(); //Очищаем дисплей
    }
    lcd.clear(); //Очищаем дисплей
    lcd.print("calibration end,"); //Отображаем информацию
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print("you can restart"); //Отображаем информацию
    pressfornext(5); //см. строку 373
    lcd.setCursor(0,0); //Выставляем курсор
    lcd.print("you can restart"); //Отображаем информацию
    lcd.setCursor(0,1); //Выставляем курсор
    lcd.print("it from settings"); //Отображаем информацию
    pressfornext(5); //см. строку 373
    lcd.setCursor(0,0); //Выставляем курсор
    lcd.clear(); //Очищаем дисплей 
    lcd.print("in future"); //Отображаем информацию
    pressfornext(5); //см. строку 373
    EEPROM.write(5, false); //Опускаем флажок настройки
    digitalWrite(Backlight_pin,LOW); //Выключаем подсветку
    //Далее юзера перекидывает в 1 меню
  } 
  /*
  "Делает дэб"
  */

  
