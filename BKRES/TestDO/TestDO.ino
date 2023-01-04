#include <EEPROM.h>
#include <myDO_Lib.h>


#define DO_PIN A1

#define VREF 5.0    //VREF (V))
#define ADC_RES 1024 //ADC Resolution

MyDO myDO;

float temperature = 25;
float ADC_Raw;
float ADC_Voltage;
float DO_val;

void setup()
{
  Serial.begin(9600);
  Serial.println();

  // myDO.DO_0_calibration(DO_PIN, VREF, ADC_RES);
  // myDO.DO_100_calibration(DO_PIN, VREF, ADC_RES);
  // myDO.DO_calibration(TWO_POINT_CALIB, DO_PIN, VREF, ADC_RES);
  myDO.begin(DO_PIN, VREF, ADC_RES);
  myDO.printCalibPara();
  myDO.printExpression();

}

void loop()
{
  ADC_Raw = analogRead(DO_PIN);
  ADC_Voltage = ADC_Raw * (float)(VREF) / ADC_RES;

  DO_val = myDO.convertVol_mgL(ADC_Voltage, temperature);
  Serial.println("DO ADC raw: " + String(ADC_Raw) + " -- DO ADC Voltage: " + String(ADC_Voltage) + " -- DO value: " + String(DO_val) + "mg/L");
  delay(1000);
}