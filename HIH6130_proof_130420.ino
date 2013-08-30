// HIH_6130_1  - Arduino
// 
// Arduino                HIH-6130
// SCL (Analog 5) ------- SCL (term 3)
// SDA (Analog 4) ------- SDA (term 4)
//
// Note 2.2K pullups to 5 VDC on both SDA and SCL
//
// Pin4 ----------------- Vdd (term 8) 
//
// Illustrates how to measure relative humidity and temperature.
//
// copyright, Peter H Anderson, Baltimore, MD, Nov, '11
// You may use it, but please give credit.  
    
#include <Wire.h> //I2C library

byte fetch_humidity_temperature(unsigned int *p_Humidity, unsigned int *p_Temperature);
void print_float(float f, int num_digits);

#define TRUE 1
#define FALSE 0

uint32_t dtime;  // delay time in msec

void setup(void)
{
   Serial.begin(115200);
   Wire.begin();
   pinMode(4, OUTPUT);
   digitalWrite(4, HIGH); // this turns on the HIH3610
   delay(3000);
   Serial.print("HIH6130 Proof, Sample interval ");  // just to be sure things are working
   dtime = 10000;  //msec between readings
   Serial.print(dtime/1000);
   Serial.println(" sec");
   
   Serial.println("Time Stat  RH%   TempC");
   
}
    
void loop(void)
{
   byte _status;
   unsigned int H_dat, T_dat;
   float RH, T_C;
   
   
   while(1)
   {
      _status = fetch_humidity_temperature(&H_dat, &T_dat);
      Serial.print(millis()/1000);
      
      switch(_status)
      {
          case 0:  Serial.print(" Norm ");
                   break;
          case 1:  Serial.print(" Stale Data! ");
                   break;
          case 2:  Serial.print(" In command mode! ");
                   break;
          default: Serial.print(" Diagnostic! "); 
                   break; 
      }       
    
      RH = (float) H_dat * 6.10e-3;
      T_C = (float) T_dat * 1.007e-2 - 40.0;

//    Serial.print("RH: ");
      print_float(RH, 2);
      Serial.print(" % ");
      print_float(T_C, 2);
      Serial.println(" C");
      delay(dtime);
   }
}

byte fetch_humidity_temperature(unsigned int *p_H_dat, unsigned int *p_T_dat)
{
      byte address, Hum_H, Hum_L, Temp_H, Temp_L, _status;
      unsigned int H_dat, T_dat;
      address = 0x27;;
      Wire.beginTransmission(address); 
      Wire.endTransmission();
      delay(100);
      
      Wire.requestFrom((int)address, (int) 4);
      Hum_H = Wire.receive();
      Hum_L = Wire.receive();
      Temp_H = Wire.receive();
      Temp_L = Wire.receive();
      Wire.endTransmission();
      
      _status = (Hum_H >> 6) & 0x03;
      Hum_H = Hum_H & 0x3f;
      H_dat = (((unsigned int)Hum_H) << 8) | Hum_L;
      T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
      T_dat = T_dat / 4;
      *p_H_dat = H_dat;
      *p_T_dat = T_dat;
      return(_status);
}
   
void print_float(float f, int num_digits)
{
    int f_int;
    int pows_of_ten[4] = {1, 10, 100, 1000};
    int multiplier, whole, fract, d, n;

    multiplier = pows_of_ten[num_digits];
    if (f < 0.0)
    {
        f = -f;
        Serial.print("-");
    }
    whole = (int) f;
    fract = (int) (multiplier * (f - (float)whole));

    Serial.print(whole);
    Serial.print(".");

    for (n=num_digits-1; n>=0; n--) // print each digit with no leading zero suppression
    {
         d = fract / pows_of_ten[n];
         Serial.print(d);
         fract = fract % pows_of_ten[n];
    }
}      

