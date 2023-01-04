/*!
 * @file DFRobot_PH_mine.cpp
 * @brief Bui Tuan Anh - Arrduino Lib for PH sensor Meter ProKit V2
 *
 * @url https://github.com/
 */

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <EEPROM.h>

#include "myPH_Lib.h"

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

#define PH_4_VALUEADDR 0x00     // the start address of the pH calibration parameters stored in the EEPROM
#define PH_6_86_VALUEADDR 0x04  // the start address of the pH calibration parameters stored in the EEPROM
#define PH_9_VALUEADDR 0x08     // the start address of the pH calibration parameters stored in the EEPROM

MyPH::MyPH() {
    this->_pH_4_Voltage = 0;
    this->_pH_6_86_Voltage = 0;
    this->_pH_9_Voltage = 0;
    this->a_6_86_4 = 0;
    this->a_9_6_86 = 0;
    this->b_6_86_4 = 0;
    this->b_9_6_86 = 0;
}

MyPH::~MyPH() {
}

void MyPH::begin(uint8_t pin, float Vref, float ADC_resolution)  // put it in setup()
{
    EEPROM_read(PH_4_VALUEADDR, this->_pH_4_Voltage);                                                                                                                            // load the (pH = 4.0) voltage of the pH board from the EEPROM
    if (EEPROM.read(PH_4_VALUEADDR) == 0xFF && EEPROM.read(PH_4_VALUEADDR + 1) == 0xFF && EEPROM.read(PH_4_VALUEADDR + 2) == 0xFF && EEPROM.read(PH_4_VALUEADDR + 3) == 0xFF) {  // float data type --> 4 bytes
        pH_4_Calibration(pin, Vref, ADC_resolution);                                                                                                                             // this function will calib and assign this->_pH_4_Voltage
    }

    /* Load 6.86 pH Voltage value from EPROM */
    EEPROM_read(PH_6_86_VALUEADDR, this->_pH_6_86_Voltage);  // load the (pH = 6.86) voltage of the pH board from the EEPROM
    if (EEPROM.read(PH_6_86_VALUEADDR) == 0xFF && EEPROM.read(PH_6_86_VALUEADDR + 1) == 0xFF && EEPROM.read(PH_6_86_VALUEADDR + 2) == 0xFF && EEPROM.read(PH_6_86_VALUEADDR + 3) == 0xFF) {
        pH_6_86_Calibration(pin, Vref, ADC_resolution);  // this function will calib and assign this->_pH_6_86Voltage
    }

    /* Load 9 pH Voltage value from EPROM */
    EEPROM_read(PH_9_VALUEADDR, this->_pH_9_Voltage);  // load the (pH = 9) voltage of the pH board from the EEPROM
    if (EEPROM.read(PH_9_VALUEADDR) == 0xFF && EEPROM.read(PH_9_VALUEADDR + 1) == 0xFF && EEPROM.read(PH_9_VALUEADDR + 2) == 0xFF && EEPROM.read(PH_9_VALUEADDR + 3) == 0xFF) {
        pH_9_Calibration(pin, Vref, ADC_resolution);  // this function will calib and assign this->_pH_9_Voltage
    }

    /* find the linear expression for 2 point pH */
    pHfunction_6_86_4();
    pHfunction_9_6_86();
}

void MyPH::pH_4_Calibration(uint8_t pin, float Vref, float ADC_resolution) {
    // Serial.println("* >>>>> Calibrate for pH = 4 <<<<< *");
    // Serial.println("- Insert pH probe into 4-pH water. And wait a second until Voltage of sensor is stable");
    // Serial.println("-----> Enter 1 to start reading <-----");
    // uint8_t isStart = 0;
    // while (isStart - 48 != 1) // convert ASCII to number
    // {
    //     if (Serial.available() > 0)
    //     {
    //         isStart = Serial.read();
    //         switch (isStart - 48) // convert from ASCII to number
    //         {
    //         case 1:
    //             Serial.println("Starting.....Please wait.....");
    /* Take 50 vol value in 5s then take average as standard calib vol*/
    uint8_t i = 50;           // lower power consumption
    for (i = 50; i > 0; i--)  //
    {
        this->_pH_4_Voltage += (analogRead(pin) * Vref) / ADC_resolution;
        delay(100);
    }
    this->_pH_4_Voltage = this->_pH_4_Voltage / 50;
    EEPROM_write(PH_4_VALUEADDR, this->_pH_4_Voltage);
    //             break;
    //         default:
    //             Serial.println("Please enter 1 to start !!");
    //             break;
    //         }
    //     }
    // }
    // Serial.println("-------> pH 4 Calib Vol: " + String(this->_pH_4_Voltage) + " ----- End calibration!");
    // Serial.println();
}

void MyPH::pH_6_86_Calibration(uint8_t pin, float Vref, float ADC_resolution) {
    // Serial.println("* >>>>> Calibrate for pH = 6.86 <<<<< *");
    // Serial.println("- Insert pH probe into 6.86-pH water. And wait a second until Voltage of sensor is stable");
    // Serial.println("-----> Enter 1 to start reading <-----");
    // uint8_t isStart = 0;
    // while (isStart - 48 != 1) {
    //     if (Serial.available() > 0) {
    //         isStart = Serial.read();
    //         switch (isStart - 48)  // convert from ASCII to number
    //         {
    //             case 1:
    //                 Serial.println("Starting.....Please wait.....");
    /* Take 50 vol value in 5s then take average as standard calib vol*/
    uint8_t i = 50;           // lower power consumption
    for (i = 50; i > 0; i--)  //

    {
        this->_pH_6_86_Voltage += (analogRead(pin) * Vref) / ADC_resolution;
        delay(100);
    }
    this->_pH_6_86_Voltage = this->_pH_6_86_Voltage / 50;
    EEPROM_write(PH_6_86_VALUEADDR, this->_pH_6_86_Voltage);
    //                 break;
    //             default:
    //                 Serial.println("Please enter 1 to start !!");
    //                 break;
    //         }
    //     }
    // }
    // Serial.println("-------> pH 6.86 Calib Vol: " + String(this->_pH_6_86_Voltage) + " ----- End calibration!");
    // Serial.println();
}

void MyPH::pH_9_Calibration(uint8_t pin, float Vref, float ADC_resolution) {
    // Serial.println("* >>>>> Calibrate for pH = 9 <<<<< *");
    // Serial.println("- Insert pH probe into 9-pH water. And wait a second until Voltage of sensor is stable");
    // Serial.println("-----> Enter 1 to start reading <-----");
    // uint8_t isStart = 0;
    // while (isStart - 48 != 1) {
    //     if (Serial.available() > 0) {
    //         isStart = Serial.read();
    //         switch (isStart - 48)  // convert from ASCII to number
    //         {
    //             case 1:
    //                 Serial.println("Starting.....Please wait.....");
    /* Take 50 vol value in 5s then take average as standard calib vol*/
    uint8_t i = 50;           // lower power consumption
    for (i = 50; i > 0; i--)  //

    {
        this->_pH_9_Voltage += (analogRead(pin) * Vref) / ADC_resolution;
        delay(100);
    }
    this->_pH_9_Voltage = this->_pH_9_Voltage / 50;
    EEPROM_write(PH_9_VALUEADDR, this->_pH_9_Voltage);
    //                 break;
    //             default:
    //                 Serial.println("Please enter 1 to start !!");
    //                 break;
    //         }
    //     }
    // }
    // Serial.println("-------> pH 9 Calib Vol: " + String(this->_pH_9_Voltage) + " ----- End calibration!");
    // Serial.println();
}

void MyPH::calibration(uint8_t pin, float Vref, float ADC_resolution) {
    this->pH_4_Calibration(pin, Vref, ADC_resolution);
    this->pH_6_86_Calibration(pin, Vref, ADC_resolution);
    this->pH_9_Calibration(pin, Vref, ADC_resolution);
}

void MyPH::pHfunction_6_86_4() {
    this->a_6_86_4 = (6.86 - 4) / (this->_pH_6_86_Voltage - this->_pH_4_Voltage);
    this->b_6_86_4 = 6.86 - (this->a_6_86_4 * this->_pH_6_86_Voltage);
}

void MyPH::pHfunction_9_6_86() {
    this->a_9_6_86 = (9 - 6.86) / (this->_pH_9_Voltage - this->_pH_6_86_Voltage);
    this->b_9_6_86 = 6.86 - (this->a_9_6_86 * this->_pH_6_86_Voltage);
}

double MyPH::getA_6_86_4() {
    return this->a_6_86_4;
}

double MyPH::getA_9_6_86() {
    return this->a_9_6_86;
}

double MyPH::getB_6_86_4() {
    return this->b_6_86_4;
}

double MyPH::getB_9_6_86() {
    return this->b_9_6_86;
}

float MyPH::getPH_4_Voltage() {
    return this->_pH_4_Voltage;
}

float MyPH::getPH_6_86_Voltage() {
    return this->_pH_6_86_Voltage;
}

float MyPH::getPH_9_Voltage() {
    return this->_pH_9_Voltage;
}

float MyPH::convertVol_PH(float ADC_voltage, float temperature) {
    return (ADC_voltage > this->getPH_6_86_Voltage()) ? this->getA_6_86_4() * ADC_voltage + this->getB_6_86_4()
                                                      : this->getA_9_6_86() * ADC_voltage + this->getB_9_6_86();
}

void MyPH::printExpression() {
    Serial.println();
    Serial.println("***** Calib function: ");
    Serial.println("Between 6.86 and 4 pH point: pHCurrent = " + String(this->getA_6_86_4()) + "*(ADC_VoltageCurrent) + " + String(this->getB_6_86_4()));
    Serial.println("Between 9 and 6.86 pH point: pHCurrent = " + String(this->getB_9_6_86()) + "*(ADC_VoltageCurrent) + " + String(this->getB_9_6_86()));
    Serial.println();
}

void MyPH::printCalibPara() {
    Serial.println("***** pH Voltage at");
    Serial.println("pH = 4: " + String(this->getPH_4_Voltage()));
    Serial.println("pH = 6.86: " + String(this->getPH_6_86_Voltage()));
    Serial.println("pH = 9: " + String(this->getPH_9_Voltage()));
    Serial.println();
}