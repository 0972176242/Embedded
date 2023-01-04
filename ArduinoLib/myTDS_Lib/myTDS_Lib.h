/*!
 * @file myTDS_Lib.h
 * @brief Arduino library for all type TDS using analog signal
 *
 * @date  2022-11-29
 * @url https://github.com/anh-hust/Embedded/tree/main/ArduinoLib
 */

#ifndef _MYTDS_LIB_H_
#define _MYTDS_LIB_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class MyTDS {
   private:
    float _kValue;
    float _TDS_buffer_solution_val;
    float EC_val;

   public:
    /**
     * @fn Construct a new MyTDS object
     * @brief init all to zero
     *
     */
    MyTDS();
    ~MyTDS();

    /**
     * @brief begin, load K value from EPROM
     * @param Vref Voltage reference in Vol (V)
     *
     */
    void begin(uint8_t pin, float Vref, float ADC_resolution);

    /**
     * @fn mesureTDS using filterData to filter noise data, temperature compenstation
     * @param ADC_voltage in Vol (V)
     *
     * @unit: EC_val in uS/cm
     *        TDS    in ppm
     *        SALT   in ppt
     */
    double theFunction(float ADC_Voltage);
    float convertVol_ECValue(float ADC_Voltage, float temperature);
    float convertEC_TDSValue(float TDS_factor);
    float toSalinity();

    /**
     * @fn calibration --> K value
     * @brief filterData using median filtering algorithm to filter noise then calculate the medium value
     *
     */
    void TDS_calibration(uint8_t pin, float Vref, float ADC_resolution);

    /**
     * @brief Set the Kvalue object
     *
     * @param K_val is K factor to convert from EC to TDS
     */
    void setKvalue(float K_val);
    void setECval(float ecValue);

    /**
     * @fn getter and setter
     */
    float getECval();
    float getKValue();
    float getTDSSolution();

    /**
     * @fn print TDS sensor parameter through Serial
     */
    void printParam();
};

#endif /* _MYTDS_LIB_H_ */