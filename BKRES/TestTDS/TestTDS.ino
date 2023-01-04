#include <EEPROM.h>
#include <myTDS_Lib.h>

#define TDS_PIN A3

#define VREF 5.0    //VREF (V)
#define ADC_RES 1024 //ADC Resolution

MyTDS myTDS;
float tdsValue;
float ecValue;
float ADC_voltage;
float SALT_val;
float temperature = 25;

void setup() {
  Serial.begin(9600);
  Serial.println();
  // myTDS.TDS_calibration(TDS_PIN, VREF, ADC_RES);
  myTDS.begin(TDS_PIN, VREF, ADC_RES);
  myTDS.setKvalue(0.67);
  myTDS.printParam();
}

void loop() {
  ADC_voltage = analogRead(TDS_PIN) * (float)VREF / ADC_RES;
  ecValue = myTDS.convertVol_ECValue(ADC_voltage, temperature);
  tdsValue = myTDS.convertEC_TDSValue(0.5);
  SALT_val = myTDS.toSalinity();
  Serial.println("ADC_Voltage: " + String(ADC_voltage) + " -----> TDS Value: " + String(tdsValue) + " -----> EC value: " + String(ecValue) + " -----> SALT: " + String(SALT_val));
  Serial.println();
  delay(2000);
}
