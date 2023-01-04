/*!
 * @file myDO_Lib.h
 * @brief Arduino library for all type DO using analog signal
 *
 * @date  2022-11-13
 * @url https://github.com/
 */

#ifndef _MYDO_LIB_H_
#define _MYDO_LIB_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define ONE_POINT_CALIB 0
#define TWO_POINT_CALIB 1

class MyDO {
   private:
    float _v0;
    float _v100;

    /* Standard mg/L of DO at 0->41 celcius */
    const uint16_t DO_Table[41] = {
        14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
        11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
        9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
        7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

    /**
     * @brief percent Oxygen in water (y) = a*(current voltage in DO sensor (x)) + b
     *
     */
    double a;
    double b;

   public:
    /**
     * @fn Construct a new MyDO object
     * @brief init all to zero
     *
     */
    MyDO();
    ~MyDO();

    /**
     * @fn begin
     * @brief Initialization all to zero
     *
     * @param pin            : Pin in Arduino connect to Analog out from sensor
     * @param Vref           : Voltage supply for DO sensor (V)
     * @param ADC_resolution : ADC resolution of Arduino
     */
    void begin(uint8_t pin, float Vref, float ADC_resolution);

    /**
     * @fn find ADC voltage - % dissolved oxygen characteristic
     * @brief From v0 and v100 --> function to map ADC voltage to % dissolved oxygen in water
     *
     */
    void DO_function();

    /**
     * @brief Calib DO value at 0% dissolved oxygen and 100% (in air) dissolved oxygen
     *
     * @param choiceCalibMethod :ONE_POINT or TWO_POINT
     * @param Vref              : Voltage supply foe DO sensor
     * @param ADC_resolution    : ADC resolution of Arduino
     */
    void DO_0_calibration(uint8_t pin, float Vref, float ADC_resolution);
    void DO_100_calibration(uint8_t pin, float Vref, float ADC_resolution);
    void DO_calibration(uint8_t method, uint8_t pin, float Vref, float ADC_resolution);

    /**
     * @brief after find the V-% characteristic, we find % from current voltage then convert to mg/L
     *
     * @param ADC_voltage       :current voltage
     * @param temperature_c     :temperature
     */
    float convertVol_mgL(float ADC_voltage, float temperature_c);

    /**
     * @brief get method
     *
     * @return float
     */
    float getv0();
    float getv100();
    float getTemperature();

    double getA();
    double getB();
    float getStandardDO(float temperature);

    /**
     * @brief Print the destinated expression after calibration through Serial
     *
     */
    void printCalibPara();
    void printExpression();
};

#endif