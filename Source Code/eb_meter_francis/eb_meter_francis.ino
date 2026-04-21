#define BLYNK_TEMPLATE_ID "TMPL3eraibvP4"
#define BLYNK_TEMPLATE_NAME "IoT based Energy Metering"
#define BLYNK_AUTH_TOKEN "McuvD1Cklo9Bx4Xu3TXUJY8gcVaF4-EA"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
WiFiClient client;

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ZMPT101B.h>
#include "EmonLib.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SENSITIVITY 510.0f

ZMPT101B voltageSensor(35, 50.0);
EnergyMonitor emon1;

char auth[] = "McuvD1Cklo9Bx4Xu3TXUJY8gcVaF4-EA";
char ssid[] = "Realme GT 7";
char pass[] = "yfnz2629";
float voltage;
double Irms=0;
double power;
double kwh = 0;

float tariffRate = 6.0;
float billAmount = 0;

unsigned long lastMillis = 0;

void setup()
{  
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Wifi Connecting ..");
 voltageSensor.setSensitivity(SENSITIVITY);
 emon1.current(32, 90.9);   // CT sensor pin & calibration

  Blynk.begin(auth,ssid,pass);
 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Energy Meter");
 
  lastMillis = millis();
  delay(1000);
}

void loop()
{
  Blynk.run();
  voltage = voltageSensor.getRmsVoltage();
  Irms = emon1.calcIrms(2000);

  power = voltage * Irms;   // Realistic power calculation
  power = power/1000; 
  // ---- kWh Calculation ----
  
  if( Irms>10 ){
    
    unsigned long currentMillis = millis();
    double timeHours = (currentMillis - lastMillis) / 3600000.0;
    kwh += power * timeHours;
    lastMillis = currentMillis;
  }

  billAmount = kwh * tariffRate;
  
  Serial.print("Voltage: "); Serial.println(voltage);
  Serial.print("Current: "); Serial.println(Irms);
  Serial.print("Power: "); Serial.println(power);
  Serial.print("Energy (kWh): "); Serial.println(kwh);
  
  if(kwh>1.0){ // unit alert
      Blynk.logEvent("Alert");
    }
    
  displayData();
  Blynk.virtualWrite(V0,voltage);
  Blynk.virtualWrite(V1,Irms);
  Blynk.virtualWrite(V2,power);
  Blynk.virtualWrite(V3,kwh);
  Blynk.virtualWrite(V4,billAmount);
}

void displayData()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("V:");
  lcd.print(voltage,1);
  lcd.print(" I:");
  lcd.print(Irms,2);

  lcd.setCursor(0,1);
  lcd.print("P:");
  lcd.print(power,1);
  lcd.print("W");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Energy:");
  lcd.setCursor(0,1);
  lcd.print(kwh,4);
  lcd.print(" kWh");
  delay(1000);
}
