#include <myPH_Lib.h>

#define PH_PIN A2
#define VREF 5000  // mV
#define ADC_RES 1023

float temperature = 25;
float ADC_voltage, phValue;
MyPH myPH;

String cmd;

void setup() {
  Serial.begin(9600);
  Serial.println();
  // myPH.calibration(PH_PIN, VREF, ADC_RES);
  // myPH.pH_4_Calibration(PH_PIN, VREF, ADC_RES);
  // myPH.pH_6_86_Calibration(PH_PIN, VREF, ADC_RES);
  // myPH.pH_9_Calibration(PH_PIN, VREF, ADC_RES);
  myPH.begin(PH_PIN, VREF, ADC_RES);
  myPH.printCalibPara();
  myPH.printExpression();
}

void loop() {
  ADC_voltage = analogRead(PH_PIN) * (float)VREF / ADC_RES;
  phValue = myPH.convertVol_PH(ADC_voltage, temperature);
  Serial.print("ADC_Voltage (mV): " + String(ADC_voltage));
  Serial.print("-- pH Value: " + String(phValue));
  Serial.println();
  delay(3000);
}