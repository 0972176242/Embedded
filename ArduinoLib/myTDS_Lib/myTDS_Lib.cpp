/*!
 * @file myTDS_Lib.cpp
 * @brief Bui Tuan Anh - TDS sensor
 *
 * @url https://github.com/
 */

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <EEPROM.h>

#include "myTDS_Lib.h"

#define EEPROM_write(address, p)              \
    {                                         \
        int i = 0;                            \
        byte *pp = (byte *)&(p);              \
        for (; i < sizeof(p); i++)            \
            EEPROM.write(address + i, pp[i]); \
    }
#define EEPROM_read(address, p)               \
    {                                         \
        int i = 0;                            \
        byte *pp = (byte *)&(p);              \
        for (; i < sizeof(p); i++)            \
            pp[i] = EEPROM.read(address + i); \
    }

#define K_VALUEADDR 0x20  // The K value stored in EPROM of Arduino
#define TDS_BUFFER_SOLUTION 0x24

MyTDS::MyTDS() {
    this->_kValue = 0;
    this->_TDS_buffer_solution_val = 0;
}

MyTDS::~MyTDS() {
}

void MyTDS::begin(uint8_t pin, float Vref, float ADC_resolution) {
    EEPROM_read(K_VALUEADDR, this->_kValue);  // load the K value from the EEPROM
    EEPROM_read(TDS_BUFFER_SOLUTION, this->_TDS_buffer_solution_val);
    if (EEPROM.read(K_VALUEADDR) == 0xFF && EEPROM.read(K_VALUEADDR + 1) == 0xFF && EEPROM.read(K_VALUEADDR + 2) == 0xFF && EEPROM.read(K_VALUEADDR + 3) == 0xFF) {
        TDS_calibration(pin, Vref, ADC_resolution);
    }
}

double MyTDS::theFunction(float ADC_Voltage) {
    return 133.42 * pow(ADC_Voltage, 3) + 255.86 * pow(ADC_Voltage, 2) + 857.39 * ADC_Voltage;
}

float MyTDS::convertVol_ECValue(float ADC_voltage, float temperature) {
    float EC_val = theFunction(ADC_voltage) * this->getKValue();
    this->setECval(EC_val / (1 + 0.02 * (temperature - 25.0)));
    return this->getECval();
}

float MyTDS::convertEC_TDSValue(float tdsFactor) {
    return this->getECval() * tdsFactor;
}

float MyTDS::toSalinity() {
    return this->getECval() * 0.001 * 640;  // SALT = EC(dS/m) * 640;
}

void MyTDS::TDS_calibration(uint8_t pin, float Vref, float ADC_resolution) {
    Serial.println("* >>>>> Calibrate for TDS with known TDS (ppm) (TDS = EC * TDS Factor) <<<<< *");
    Serial.println("- Put the probe to buffer solution. And wait a second until Voltage of sensor is stable");

    // read EC solution value
    Serial.println("TDS value of buffer solution (ppm) ? ");
    while (Serial.available() == 0)
        ;
    this->_TDS_buffer_solution_val = Serial.parseFloat();
    Serial.println("Take value " + String(this->getTDSSolution()));

    Serial.println("-----> Enter 1 to start reading <-----");
    uint8_t method = 0;
    float ADC_Voltage = 0;
    while (method - 48 != 1) {
        if (Serial.available() > 0) {
            method = Serial.read();
            switch (method - 48)  // convert from ASCII to number
            {
                case 1:
                    Serial.println("Starting.....Please wait.....");
                    /* Take 50 vol value in 5s then take average as standard calib vol*/
                    uint8_t i = 50;           // lower power consumption
                    for (i = 50; i > 0; i--)  //

                    {
                        ADC_Voltage += (analogRead(pin) * Vref) / ADC_resolution;
                        delay(100);
                    }
                    ADC_Voltage = ADC_Voltage / 50;
                    this->_kValue = (this->getTDSSolution() / 2) / (float)this->theFunction(ADC_Voltage);
                    EEPROM_write(K_VALUEADDR, this->_kValue);
                    break;
                default:
                    Serial.println("Please enter 1 to start !!");
                    break;
            }
        }
    }
    Serial.println("-------> With TDS = " + String(this->getTDSSolution()) + " --> K value = " + String(this->getKValue()) + " ----- End calibration!");
    Serial.println();
}

void MyTDS::setECval(float ecValue) {
    this->EC_val = ecValue;
}

void MyTDS::setKvalue(float K_val) {
    this->_kValue = K_val;
}

float MyTDS::getECval() {
    return this->EC_val;
}

float MyTDS::getKValue() {
    return this->_kValue;
}

float MyTDS::getTDSSolution() {
    return this->_TDS_buffer_solution_val;
}

void MyTDS::printParam() {
    Serial.println();
    Serial.println("***** Calib parameter of TDS sensor ");
    Serial.println("TDS buffer solution = " + String(this->getTDSSolution()) + " -----> K value " + String(this->getKValue()));
    Serial.println();
}
