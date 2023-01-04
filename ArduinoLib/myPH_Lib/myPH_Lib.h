/*!
 * @file DFRobot_PH_mine.h
 * @brief Arduino library for all type pH sensor using analog signal
 *
 * @date  2022-11-13
 * @url https://github.com/
 */

#ifndef _PH_MINE_H_
#define _PH_MINE_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class MyPH
{
private:
    float _pH_4_Voltage;
    float _pH_6_86_Voltage;
    float _pH_9_Voltage;

    // for linear funtion 6.86 pH point and 4 pH point y = a.(ADC_voltage) + b
    double a_6_86_4;
    double b_6_86_4;

    // for linear function 9 pH point and 6.86 point y = a.(ADC_voltage) + b
    double a_9_6_86;
    double b_9_6_86;

public:
    /**
     * @fn constructor
     * @brief Initialize all to zero
     *
     */
    MyPH();
    ~MyPH();

    /**
     * @fn begin
     * @brief Initialization
     *
     * @param pin            : Pin in Arduino connect to Analog out from sensor
     * @param Vref           : Voltage supply for pH sensor
     * @param ADC_resolution : ADC resolution of Arduino
     */
    void begin(uint8_t pin, float Vref, float ADC_resolution);

    /**
     * @fn find a and b comperands for linear expression for 2 point pH 6.86 & 4 and 9 & 6.86
     * @brief set a & b of those function
     *
     */
    void pHfunction_6_86_4();
    void pHfunction_9_6_86();

    /**
     * @fn independent calibration for each pH value
     * @brief Calibrate the calibration data then write at EPROM, can use indepent with begin()
     *
     * @param pin            : Pin in Arduino connect to Analog out from sensor
     * @param Vref           : Voltage supply for pH sensor
     * @param ADC_resolution : ADC resolution of Arduino
     */
    void pH_4_Calibration(uint8_t pin, float Vref, float ADC_resolution);
    void pH_6_86_Calibration(uint8_t pin, float Vref, float ADC_resolution);
    void pH_9_Calibration(uint8_t pin, float Vref, float ADC_resolution);
    void calibration(uint8_t pin, float Vref, float ADC_resolution);

    /**
     * @brief Get a, b value of linear express between 2 point pH && Calib Voltage at 3 pH point
     *
     */
    double getA_6_86_4();
    double getA_9_6_86();

    double getB_6_86_4();
    double getB_9_6_86();

    float getPH_4_Voltage();
    float getPH_6_86_Voltage();
    float getPH_9_Voltage();

    /**
     * @brief Print the destinated expression after calibration through Serial
     *
     */
    void printCalibPara();
    void printExpression();

    /**
     * @fn convertVol_PH
     * @brief Convert voltage to PH with temperature compensation
     * @note voltage to pH value, with temperature compensation
     *
     * @param ADC_voltage     : Voltage value be converted from ADC raw from Arduino pin
     * @param temperature : Temperature of water env
     * @return The PH value
     */
    float convertVol_PH(float ADC_voltage, float temperature);
};
#endif
