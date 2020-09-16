#include <Arduino.h>
// Чтение тока и напряжения
#include <Wire.h>
#include <Adafruit_ADS1015.h>

 Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

//==
int16_t ADCres;      // переменная для чтения результата 16 бит
float Voltage = 0.0; // переменная для расчета напряжения


float curr = 0.0; // переменная для расчета напряжения
#define zeroamp 2.482
#define oneamp 2.612

#define x1BIT 0.125      // значение 1 бита
// Настройки фильтра 
#define NUM_READINGS 10 
// Настройки фильтра

// Чтение тока и напряжения

#define PIN_TRANSITOR_MODULE_Charge 32 
#define PIN_TRANSITOR_MODULE_DiCharge 18

bool ShargeState = false;   // Переменный буль состояния открытости-закрытости транзистора открывающего цепь заряда
bool DiShargeState = false; // Переменный буль состояния открытости-закрытости транзистора закрывающего цепь разряда

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void SerialInput(); //Прототип
void setup(void)
{
  Serial.begin(115200);
  Serial.println("Hello!");

  // Чтение тока и напряжения
    Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
    Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
    
    // The ADC input range (or gain) can be changed via the following
    // functions, but be careful never to exceed VDD +0.3V max, or to
    // exceed the upper and lower limits if you adjust the input range!
    // Setting these values incorrectly may destroy your ADC!
    //                                                                ADS1015  ADS1115
    //                                                                -------  -------
    ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
    // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
    // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
    // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
    
    ads.begin();
  // Чтение тока и напряжения

  pinMode(PIN_TRANSITOR_MODULE_Charge,OUTPUT); //Транзистор модуль ответственный за зарядку
  digitalWrite(PIN_TRANSITOR_MODULE_Charge,ShargeState); //Включаем его

  pinMode(PIN_TRANSITOR_MODULE_DiCharge,OUTPUT); //Транзистор модуль ответственный за РАЗрядку
  digitalWrite(PIN_TRANSITOR_MODULE_DiCharge,DiShargeState); //Включаем его
}

void loop(void)
{
 // /*
 // Чтение  напряжения
  int16_t results; // для чтения напряжения
  float multiplier = 0.1875F; // ADS1115  @ +/- 6.144V gain (16-bit results) 0.1875F
  results = ads.readADC_Differential_0_1();  

//  фильтр значений тока
int averageAmp; //результат усреднения
long sum = 0;                                  // локальная переменная sum
  for (int i = 0; i < NUM_READINGS; i++) {     // согласно количеству усреднений
    sum += ads.readADC_SingleEnded(2);         // суммируем значения с любого датчика в переменную sum
  }
  averageAmp = sum / NUM_READINGS;                  // находим среднее арифметическое, разделив сумму на число измерений
//  фильтр значений тока

// Печать результата
  Serial.print(results * multiplier); Serial.print("mV)");
  Serial.print("  amp: "); 
  Serial.print( float(13300 - averageAmp)/731 ,3); //13300 Примерное значение при нулевой нагрузке на ACS712. Чем больше нагрузка тем ниже напряжение приходит. Отнимаем разницу.Делим на подобраное число 
// Печать результата

  Serial.println();
  /*
  Serial.print("Voltage: ");
  Serial.print(Voltage, 4);
  Serial.print("\t\tCurr: ");
  Serial.println(curr, 4);
  */
  //8760 - 0
  //8743 - 0.047
  //8587 - 0.441
  //
  // 13299 -2.501 - 0 A
  // 13243 - 2.494 - 0.073 A
  // 12607 - 2.374 - 0.947 A
  
  delay(50);

    
  SerialInput();
}



















void SerialInput() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  
  if (stringComplete) {
    Serial.println(inputString);
    if(inputString.substring(0,1) == "c"){ // Начать Charge
      Serial.println("I receive Charge");
    }
    if(inputString.substring(0,1) == "d"){ // Начать DiCharge (Разрядка)
      Serial.println("I receive DiCharge");
      DiShargeState=!DiShargeState;
      digitalWrite(18, DiShargeState); //Включаем его
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

}

