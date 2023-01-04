/*!
 * @file myDO_Lib.cpp.cpp
 * @brief Bui Tuan Anh - DO sensor
 *
 * @url https://github.com/
 */

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <EEPROM.h>

#include "myDO_Lib.h"

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

#define ZERO_DO_VALUEADDR 0x12        // the start address of the DO calibration parameters stored in the EEPROM
#define ONEHUNDRED_DO_VALUEADDR 0x16  // the start address of the DO calibration parameters stored in the EEPROM

MyDO::MyDO() {
    this->_v0 = 0;
    this->_v100 = 0;

    this->a = 0;
    this->b = 0;
}

MyDO::~MyDO() {
}

void MyDO::begin(uint8_t pin, float Vref, float ADC_resolution)  // put it in setup()
{
    EEPROM_read(ONEHUNDRED_DO_VALUEADDR, this->_v100);                                                                                                                                                               // load the (DO = 100) voltage of the DO sensor from the EEPROM
    if (EEPROM.read(ONEHUNDRED_DO_VALUEADDR) == 0xFF && EEPROM.read(ONEHUNDRED_DO_VALUEADDR + 1) == 0xFF && EEPROM.read(ONEHUNDRED_DO_VALUEADDR + 2) == 0xFF && EEPROM.read(ONEHUNDRED_DO_VALUEADDR + 3) == 0xFF) {  // float data type --> 4 bytes

        Serial.println("* >>>>> Choose method to calib <<<<< *");
        Serial.println(" -----> 1 for One point calibration method");
        Serial.println(" -----> 2 for Two point calibration method");
        uint8_t method = 0;
        while (method - 48 != 1 || method - 48 != 2) {
            if (Serial.available() > 0) {
                method = Serial.read();
                switch (method - 48)  // convert from ASCII to number
                {
                    case 1:
                        DO_calibration(ONE_POINT_CALIB, pin, Vref, ADC_resolution);
                        break;
                    case 2:
                        DO_calibration(TWO_POINT_CALIB, pin, Vref, ADC_resolution);
                        break;
                    default:
                        Serial.println("Please choose the right method\n1 --> One point calibration\n2 --> Two point calibration");
                        break;
                }
            }
        }
    }

    /* After get calib value --> find Vol-% dissolved oxygen characteristic */
    DO_function();
}

void MyDO::DO_0_calibration(uint8_t pin, float Vref, float ADC_resolution) {
    // Serial.println("* >>>>> Calibrate for DO = 0 point <<<<< *");
    // Serial.println("- Insert pH probe into 0 percent dissolved oxygen water. And wait a second until Voltage of sensor is stable");
    // Serial.println("-----> Enter 1 to start reading <-----");
    // uint8_t method = 0;
    // while (method - 48 != 1) {
    //     if (Serial.available() > 0) {
    //         method = Serial.read();
    //         switch (method - 48)  // convert from ASCII to number
    //         {
    //             case 1:
    //                 Serial.println("Starting.....Please wait.....");
    /* Take 50 vol value in 5s then take average as standard calib vol*/
    uint8_t i = 50;           // lower power consumption
    for (i = 50; i > 0; i--)  //

    {
        this->_v0 += (analogRead(pin) * Vref) / ADC_resolution;
        delay(100);
    }
    this->_v0 = this->_v0 / 50;
    EEPROM_write(ZERO_DO_VALUEADDR, this->_v0);
    //                 break;
    //             default:
    //                 Serial.println("Please enter 1 to start !!");
    //                 break;
    //         }
    //     }
    // }
    // Serial.println("-------> DO = 0 percent Calib Vol: " + String(this->_v0) + " ----- End calibration!");
    // Serial.println();
}

void MyDO::DO_100_calibration(uint8_t pin, float Vref, float ADC_resolution) {
    // Serial.println("* >>>>> Calibrate for DO = 100 point (Air) <<<<< *");
    // Serial.println("- Expose the probe to the air and maintain proper air flow. And wait a second until Voltage of sensor is stable");
    // Serial.println("-----> Enter 1 to start reading <-----");
    // uint8_t method = 0;
    // while (method - 48 != 1) {
    //     if (Serial.available() > 0) {
    //         method = Serial.read();
    //         switch (method - 48)  // convert from ASCII to number
    //         {
    //             case 1:
    //                 Serial.println("Starting.....Please wait.....");
    /* Take 50 vol value in 5s then take average as standard calib vol*/
    uint8_t i = 50;           // lower power consumption
    for (i = 50; i > 0; i--)  //

    {
        this->_v100 += (analogRead(pin) * Vref) / ADC_resolution;
        delay(100);
    }
    this->_v100 = this->_v100 / 50;
    EEPROM_write(ONEHUNDRED_DO_VALUEADDR, this->_v100);
    //                 break;
    //             default:
    //                 Serial.println("Please enter 1 to start !!");
    //                 break;
    //         }
    //     }
    // }
    // Serial.println("-------> DO = 100 percent Calib Vol: " + String(this->_v100) + " ----- End calibration!");
    // Serial.println();
}

void MyDO::DO_calibration(uint8_t method, uint8_t pin, float Vref, float ADC_resolution) {
    if (method == ONE_POINT_CALIB) {
        this->_v0 = 0;
        DO_100_calibration(pin, Vref, ADC_resolution);
    } else if (method == TWO_POINT_CALIB) {
        DO_0_calibration(pin, Vref, ADC_resolution);
        DO_100_calibration(pin, Vref, ADC_resolution);
    } else {
        Serial.println("Please choose ONE POINT or TWO POINT method !!!");
    }
}

void MyDO::DO_function() {
    this->a = 1 / (this->getv100() - this->getv0());
    this->b = -this->getv0() / (this->getv100() - this->getv0());
}

float MyDO::getv0() {
    return this->_v0;
}
float MyDO::getv100() {
    return this->_v100;
}
double MyDO::getA() {
    return this->a;
}
double MyDO::getB() {
    return this->b;
}
float MyDO::getStandardDO(float temperature) {
    return (this->DO_Table[(int)temperature] * temperature) / (int)temperature;
}

float MyDO::convertVol_mgL(float ADC_voltage, float temperature_c) {
    float percentDO = this->getA() * ADC_voltage + this->getB();
    return (percentDO * this->getStandardDO(temperature_c)) / 1000.0;
}

void MyDO::printCalibPara() {
    Serial.println("***** DO Voltage at");
    Serial.println("DO = 0% " + String(this->getv0()));
    Serial.println("DO = 100% " + String(this->getv100()));
    Serial.println();
}

void MyDO::printExpression() {
    Serial.println();
    Serial.println("***** Calib function: (Vol-percentDO characteristic) ");
    Serial.println("Percent DO in water = " + String(this->getA()) + "*(ADC_VoltageCurrent) + " + String(this->getB()));
    Serial.println("Convert to mg/L: PercentDO * StandardDOTable / 1 (100%)");
    Serial.println();
}