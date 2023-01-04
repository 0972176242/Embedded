#include <myDO_Lib.h>
#include <myPH_Lib.h>
#include <myTDS_Lib.h>

// SD card Lib
#include <SdFat.h>
#include <Wire.h>  // IIC protocol

// GLCD
#include <SPI.h>
#include <U8g2lib.h>

#define VREF 5000     // VREF (mV)
#define ADC_RES 1023  // ADC Resolution

/* SD card pin */
#define SD_SS_PIN 53  // Arduino Mega

/* NTC temperature sensor */
#define TEM_PIN A0    // NTC PIN
#define RT0 100000.0  // NTC 100k
#define B 3977.0      // from datasheet of NTC
#define R 10000.0     // R=10KΩ
#define T0 (25 + 273.15)

/* pH pin */
#define PH_PIN A2  // PH analog pin

/* Set up for DO sensor */
#define DO_PIN A1  // DO analog pin

#define TDS_PIN A3      // TDS sensor analog pin
#define TDS_FACTOR 0.5  // TDS = EC * 0.5

float R_therNTC;

/* RTC DS1307 declaration */
const uint8_t DS1307_addr = 0x68;  // Addr i2c
const uint8_t NumberOfFields = 7;  // number of uint8_ts to read
uint8_t second, minute, hour, day, wday, month;
uint16_t year;

/* LCD 2004A init */
// LiquidCrystal_I2C lcd(0x27, 20, 4);

/* GLCD declaration */
#define E_PIN_GLCD 13
#define RW_PIN_GLCD 11
#define REG_PIN_GLCD 10
#define RS_PIN_GLCD 8

#define NEXT_BTN 3
#define DOWN_BTN 6
#define SELECT_BTN 4
#define BACK_BTN 5

MyDO myDO;
MyPH myPH;
MyTDS myTDS;

/* Set up variables for GLCD */
String dayOfWeek[8] = {"", "SU", "MO", "TU", "WE", "TH", "FR", "SA"};
char timeDisplay[25];

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, E_PIN_GLCD, RW_PIN_GLCD, REG_PIN_GLCD, RS_PIN_GLCD);  // 8 NC
uint8_t next_value;
uint8_t down_value;
uint8_t select_value;
uint8_t back_value;

uint8_t reference_value = 1;
static int total = 0;
static int total_count = 0;
static int back_count = 0;
static int select_down = 0;

/* SD card */
SdFat sd;
SdFile myFile;

// char fileName[21];
String fileName;
char data[45];
char header[55];
bool needHeader;  // new file created need new header

/* Global variables hold sensor's value*/
float temperature;
float DO_val;
float pH_val;
float EC_val;
float TDS_val;
float SALT_val;

float pH_count;
float DO_count;
float EC_count;
float SALT_count;

float tem_ADCval;
float pH_ADCval;
float DO_ADCval;
float EC_ADCval;

uint8_t writeToSD;
uint8_t count;

/**
                    main
*/
void setup() {
    /* I2C init */
    Wire.begin();
    sd.begin(SD_SS_PIN, SPI_FULL_SPEED);

    /* Serial init */
    // Serial.begin(9600);
    // while (!Serial)
    //   ;           // wait Serial
    delay(1000);  // catch Due reset problem

    /* LCD 2004 init */
    // lcd.begin();
    // lcd.backlight();

    /* pH */
    // myPH.calibration(PH_PIN, VREF, ADC_RES);
    // myPH.pH_4_Calibration(PH_PIN, VREF, ADC_RES);
    // myPH.pH_6_86_Calibration(PH_PIN, VREF, ADC_RES);
    // myPH.pH_9_Calibration(PH_PIN, VREF, ADC_RES);
    myPH.begin(PH_PIN, VREF, ADC_RES);
    // myPH.printCalibPara();
    // myPH.printExpression();

    /* DO */
    // myDO.DO_0_calibration(DO_PIN, VREF, ADC_RES);
    // myDO.DO_100_calibration(DO_PIN, VREF, ADC_RES);
    // myDO.DO_calibration(ONE_POINT_CALIB, DO_PIN, VREF / 1000.0, ADC_RES);
    myDO.begin(DO_PIN, VREF / 1000.0, ADC_RES);
    // myDO.printCalibPara();
    // myDO.printExpression();

    /* TDS */
    // myTDS.TDS_calibration(TDS_PIN, VREF, ADC_RES);
    myTDS.begin(TDS_PIN, VREF, ADC_RES);
    myTDS.setKvalue(0.67);
    // myTDS.printParam();

    /* Time RTC init */
    // setTime(14, 12, 30, 7, 10, 12, 22);  // 14:12:30 SAT 10/12/2022

    pinMode(NEXT_BTN, INPUT_PULLUP);
    pinMode(DOWN_BTN, INPUT_PULLUP);
    pinMode(SELECT_BTN, INPUT_PULLUP);
    pinMode(BACK_BTN, INPUT_PULLUP);
    u8g2.initDisplay();
    u8g2.clearDisplay();
    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);
    
    writeToSD = 150;
    count = 150;
    needHeader = true;
    
    pH_count = 0;
    DO_count = 0;
    EC_count = 0;
    SALT_count = 0;

    pH_ADCval = 0;
    DO_ADCval = 0;

    pH_val = 0;
    DO_val = 0;
    EC_val = 0;
    SALT_val = 0;   

    readDS1307();
    menu_sum(day, month, year, wday, hour, minute, second, pH_ADCval, DO_ADCval, temperature, pH_val, DO_val, EC_val, SALT_val);
}

void loop() {
    //@NOTE: must read temp first for DO & pH sensor
    /**
     * @fn read sensor phase, temp read 20 values in 2s then take average
     *     read pH, DO, SALT value 3s, 30 values then take average
     *     read time at last
     */
    temperature = readTem();

    if (count > 100) { // 5s for pH sensor
        pH_count += readPH(temperature);
    }

    else if (count > 50) { // 5s for DO sensor
        DO_count += readDO(temperature);
    }

    else if (count > 0) { // 5s for TDS sensor
        EC_count += readEC(temperature);
        SALT_count += readSalinity(temperature);
    }
    else { // 15s --> average them all and set new cycle
      pH_val = pH_count / 50.0;
      DO_val = DO_count / 50.0;
      EC_val = EC_count / 50.0;
      SALT_val = SALT_count / 50.0;

      pH_count = 0;
      DO_count = 0;
      EC_count = 0;
      SALT_count = 0;

      count = 150;
    }

    if(count % 5 == 0){
      readDS1307();
      displayVal_page(wday, day, month, year, hour, minute, second, temperature, pH_val, DO_val, EC_val);      
    }

    // displayAll();
    // displayToLCD();    
    // displayGLCD(day, month, year, wday, hour, minute, second, pH_ADCval, DO_ADCval, temperature, pH_val, DO_val, EC_val, SALT_val);

    count--;
    writeToSD--;
    /**
     *  @fn: Write to SD card
     *  @brief: If file open --> write --> clearCache
     *          Else when card is removed --> check if card is inserted (begin == 1)
     *                                             then update fileName (.csv) based on time from RTC --> clearCache
     *                                    --> if not inserted yet
     *                                             then do nothing
     */
    if (writeToSD == 0) {
        if (myFile.open(fileName.c_str(), O_RDWR | O_CREAT | O_AT_END)) {  // convert String to const char * using c_str()
            // Serial.print("Writing ..... ");
            if (needHeader) {
                sprintf(header, "Date,Time,Temperature(Celsius),pH,DO(mg/L),SALT(ppt),\n");
                // Serial.println(header);
                myFile.write(header);
                needHeader = false;
            }
            sprintf(data, "%02d-%02d-%04d,%02d:%02d:%02d,%02d.%02d,%02d.%02d,%02d.%02d,%02d.%02d,\n", day, month, year, hour, minute, second,
                    (int)temperature, (int)((temperature - (int)temperature) * 100),
                    (int)pH_val, (int)((pH_val - (int)pH_val) * 100),
                    (int)DO_val, (int)((DO_val - (int)DO_val) * 100),
                    (int)SALT_val, (int)((SALT_val - (int)SALT_val) * 100));
            myFile.write(data);
            // Serial.print(String(data));

            // close the file:
            while (!myFile.sync())
                ;  // wait until all data writed on SD
            while (!myFile.close())
                ;
            sd.cacheClear();
            // Serial.println(" ..... Done !!");/.
        } else {
            if (sd.begin(SD_SS_PIN, SPI_HALF_SPEED)) {
                sd.cacheClear();
                fileName = String(day) + "-" + String(month) + "-" + String(year) + "_" + String(hour) + "h" + String(minute) + "m.csv";
                if (!myFile.exists(fileName.c_str()))
                    needHeader = true;  // new file new header
                                        // Serial.println("Write to new file -----> " + String(fileName));
            } else {
                sd.cacheClear();
                // Serial.println("**-----> NO Available Card *****!!!");
            }
        }
        writeToSD = 150;
    }
    delay(100);
}

/*
======================= Temperature =================================
*/
float readTem() {
    tem_ADCval = analogRead(TEM_PIN) * float(VREF / 1000) / ADC_RES;  // ADC convert to mV
    R_therNTC = tem_ADCval / ((VREF / 1000.0 - tem_ADCval) / R);
    return (1.0 / ((log(R_therNTC / RT0) / B) + (1 / T0))) - 273.15;  // expression to calculate temp
}

void displayTem() {
    Serial.print(" Temperature: ");
    Serial.print(temperature);
}

/**
======================= DO sensor ================================
*/
float readDO(float temperature_c) {
    DO_ADCval = analogRead(DO_PIN) * (float)(VREF / 1000.0) / ADC_RES;
    return myDO.convertVol_mgL(DO_ADCval, temperature);
}

void displayDO() {
    Serial.print("  DO: ");
    Serial.print(DO_val, 2);
    Serial.print("ug/L");
}

/**
========================= pH sensor ================================
*/
float readPH(float temperature_c) {
    pH_ADCval = analogRead(PH_PIN) * (float)VREF / ADC_RES;
    return myPH.convertVol_PH(pH_ADCval, temperature_c);
}

void displayPH() {
    Serial.print("  pH: ");
    Serial.print(pH_val, 2);
}

/**
=========================== TDS sensor =====================================
*/
float readEC(float temperature_c) {
    EC_ADCval = analogRead(TDS_PIN) * (float)(VREF / 1000.0) / ADC_RES;
    return myTDS.convertVol_ECValue(EC_ADCval, temperature_c);
}

float readTDS(float temperature_c) {
    return myTDS.convertEC_TDSValue(TDS_FACTOR);
}

float readSalinity(float temperature_c) {
    return myTDS.toSalinity();
}

void displayECandTDS() {
    Serial.print("  EC: ");
    Serial.print(EC_val, 2);
    Serial.print("uS/cm  TDS: ");
    Serial.print(TDS_val, 2);
    Serial.print("ppm");
}

/**
========================= Display =====================================
*/
void displayAll() {
    digitalClockDisplay();
    displayTem();
    displayDO();
    displayPH();
    displayECandTDS();
    Serial.println();
}

// setCursor(row, col);
// void displayToLCD() {
//   sprintf(currentTime, "%02d-%02d-%02d  %02d:%02d:%02d", day, month, year, hour, minute, second);
//   lcd.setCursor(0, 0);
//   lcd.print(currentTime);
//   lcd.setCursor(4, 1);
//   lcd.print("Sensor Value");
//   lcd.setCursor(0, 2);
//   lcd.print("Tem:" + String(temperature));
//   lcd.setCursor(10, 2);
//   lcd.print("pH :" + String(pH_val));
//   lcd.setCursor(0, 3);
//   lcd.print("DO :" + String(DO_val));
// }

void menu_sum(uint8_t day_c, uint8_t month_c, uint16_t year_c, uint8_t wday_c, uint8_t hour_c, uint8_t minute_c, uint8_t second_c,
                    float pH_ADC_c, float DO_ADC_c, float temperature_c, float pH_val_c, float DO_val_c, float EC_val_c, float SALT_val_c) {
    if (total == 0) {
        total_count = 1;
        displayVal_page(wday_c, day_c, month_c, year_c, hour_c, minute_c, second_c, temperature_c, pH_val_c, DO_val_c, SALT_val_c);
    } else if (total == 1) {
        pHSensorDisplay_page(temperature_c, pH_val_c, pH_ADC_c, myPH.getPH_4_Voltage(), myPH.getPH_6_86_Voltage(), myPH.getPH_9_Voltage());
    } else if (total == 2) {
        DOSensorDisplay_page(temperature_c, DO_ADC_c, myDO.getv0(), myDO.getv100());
    } else if (total == 3) {
        TDSSensorDisplay_page(temperature_c, SALT_val_c, 2.32, 2.32);
    }
}

void displayGLCD(uint8_t day_c, uint8_t month_c, uint16_t year_c, uint8_t wday_c, uint8_t hour_c, uint8_t minute_c, uint8_t second_c,
                    float pH_ADC_c, float DO_ADC_c, float temperature_c, float pH_val_c, float DO_val_c, float EC_val_c, float SALT_val_c) {
    
    down_value = digitalRead(DOWN_BTN);
    next_value = digitalRead(NEXT_BTN);
    select_value = digitalRead(SELECT_BTN);
    back_value = digitalRead(BACK_BTN);

    if (next_value != reference_value)  // next button
    {
        if (next_value == 0) {
            if (total_count == 2 && (total == 1 or total == 2 or total == 3)) {
                total_count = 1;
            }
            if (total_count == 1) {
                if (total >= 3) {
                    total = 0;
                    select_down = 0;
                } else {
                    total++;
                    select_down = 0;
                }
                menu_sum(day_c, month_c, year_c, wday_c, hour_c, minute_c, second_c, pH_ADC_c, DO_ADC_c, temperature_c, pH_val_c, DO_val_c, EC_val_c, SALT_val_c);
            }
            // delay(100);
        }
        reference_value = next_value;
    }

    if (down_value != reference_value) {  //  down button

        if (down_value == 0) {
            if (total_count == 1) {
                if (select_down >= 3) {
                    select_down = 1;
                } else {
                    select_down++;
                }
            }
            if ((total_count == 1 or total_count == 2) && (total == 2 or total == 3) && select_down == 3) {
                select_down = 1;
            }
            if (total_count == 1 && total == 1 && select_down == 1) {
                back_count = 0;
                pHSensorDisplay_page(temperature_c, pH_val_c, pH_ADC_c, myPH.getPH_4_Voltage(), myPH.getPH_6_86_Voltage(), myPH.getPH_9_Voltage());
                u8g2.drawBox(0, 32, 128, 11);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 1 && select_down == 2) {
                back_count = 0;
                pHSensorDisplay_page(temperature_c, pH_val_c, pH_ADC_c, myPH.getPH_4_Voltage(), myPH.getPH_6_86_Voltage(), myPH.getPH_9_Voltage());
                u8g2.drawBox(0, 42, 128, 11);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 1 && select_down == 3) {
                back_count = 0;
                pHSensorDisplay_page(temperature_c, pH_val_c, pH_ADC_c, myPH.getPH_4_Voltage(), myPH.getPH_6_86_Voltage(), myPH.getPH_9_Voltage());
                u8g2.drawBox(0, 52, 128, 11);
                u8g2.sendBuffer();
            }

            if (total_count == 1 && total == 2 && select_down == 1) {
                back_count = 0;
                DOSensorDisplay_page(temperature_c, DO_ADC_c, myDO.getv0(), myDO.getv100());
                u8g2.drawBox(0, 38, 128, 12);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 2 && select_down == 2) {
                back_count = 0;
                DOSensorDisplay_page(temperature_c, DO_ADC_c, myDO.getv0(), myDO.getv100());
                u8g2.drawBox(0, 50, 128, 12);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 3 && select_down == 1) {
                back_count = 0;
                TDSSensorDisplay_page(temperature_c, SALT_val_c, 123, 123);
                u8g2.drawBox(0, 38, 128, 12);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 3 && select_down == 2) {
                back_count = 0;
                TDSSensorDisplay_page(temperature_c, SALT_val_c, 123, 123);
                u8g2.drawBox(0, 50, 128, 12);
                u8g2.sendBuffer();
            }
            // delay(100);
        }
        reference_value = down_value;
    }

    if (select_value != reference_value)  // select button
    {
        if (select_value == 0) {
            total_count++;
            if (total_count == 1) {
                back_count = 0;
                menu_sum(day_c, month_c, year_c, wday_c, hour_c, minute_c, second_c, pH_ADC_c, DO_ADC_c, temperature_c, pH_val_c, DO_val_c, EC_val_c, SALT_val_c);
            } else if (total_count > 2) {
                total_count = 2;
                back_count = 0;
            }

            if (total_count == 2 && total == 1 && select_down == 1) {
                back_count = 0;
                total_count = total_count - 1;
                calibpH4();
            }
            if (total_count == 2 && total == 1 && select_down == 2) {
                back_count = 0;
                total_count = total_count - 1;
                calibpH7();
            }
            if (total_count == 2 && total == 1 && select_down == 3) {
                back_count = 0;
                total_count = total_count - 1;
                calibpH9();
            }

            if (total_count == 2 && total == 2 && select_down == 1) {
                back_count = 0;
                total_count = total_count - 1;
                calibDO0();
            }
            if (total_count == 2 && total == 2 && select_down == 2) {
                back_count = 0;
                total_count = total_count - 1;
                calibDO100();
                select_down = 0;
            }
            if (total_count == 2 && total == 3 && select_down == 1) {
                back_count = 0;
                total_count = total_count - 1;
                Acception();
            }
            if (total_count == 2 && total == 3 && select_down == 2) {
                back_count = 0;
                total_count = total_count - 1;
                Acception();
                select_down = 0;
            }
            if (total_count == 2 && (total == 0) && (select_down == 1 or select_down == 2 or select_down == 3 or select_down == 0)) {
                total_count = 1;
                back_count = 0;
                menu_sum(day_c, month_c, year_c, wday_c, hour_c, minute_c, second_c, pH_ADC_c, DO_ADC_c, temperature_c, pH_val_c, DO_val_c, EC_val_c, SALT_val_c);
            }
            // delay(100);
        }
        reference_value = select_value;
    }

    if (back_value != reference_value)  // back button
    {
        if (back_value == 0) {
            back_count++;
            if (back_count == 1) {
                if (total_count == 2 && (total == 0 or total == 1 or total == 2 or total == 3 or select_down == 1 or select_down == 2 or select_down == 3)) {
                    total_count = total_count - 1;
                    back_count = 0;
                    total = 0;
                } else if (total_count == 1 && (total == 0 or total == 1 or total == 2 or total == 3 or select_down == 1 or select_down == 2 or select_down == 3)) {
                    back_count = 0;
                    total = 0;
                    select_down = 0;
                    menu_sum(day_c, month_c, year_c, wday_c, hour_c, minute_c, second_c, pH_ADC_c, DO_ADC_c, temperature_c, pH_val_c, DO_val_c, EC_val_c, SALT_val_c);
                }
            } else

            {
                back_count = 0;
            }
        }
        reference_value = back_value;
    }
}

void displayVal_page(uint8_t wday, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, float temperature, float pH_val, float DO_val, float EC_val) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);
    sprintf(timeDisplay, " %02d-%02d-%04d %02d:%02d:%02d",
            day, month, year, hour, minute, second);
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(0, 10, (dayOfWeek[wday] + String(timeDisplay)).c_str());
    u8g2.drawStr(0, 23, "Temp");
    u8g2.drawStr(40, 23, ":");
    u8g2.drawStr(95, 23, (String((char)248) + "C").c_str());
    u8g2.drawStr(0, 34, "pH");
    u8g2.drawStr(40, 34, ":");
    u8g2.drawStr(90, 34, "  ");
    u8g2.drawStr(0, 48, "DO ");
    u8g2.drawStr(40, 48, ":");
    u8g2.drawStr(90, 48, "mg/L");
    u8g2.drawStr(40, 61, ":");
    u8g2.drawStr(0, 61, "EC");
    u8g2.drawStr(90, 61, "uS/cm");
    u8g2.drawStr(45, 23, String(temperature).c_str());
    u8g2.drawStr(45, 34, String(pH_val).c_str());
    u8g2.drawStr(45, 48, String(DO_val).c_str());
    u8g2.drawStr(45, 61, String(EC_val).c_str());
    u8g2.sendBuffer();
}

void pHSensorDisplay_page(float temperature, float pH_val, float pH_ADCval, float pH4Calib_ADCval, float pH7Calib_ADCval, float pH9Calib_ADCval) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(30, 10, "pH Sensor");
    u8g2.drawStr(0, 21, "Temp");
    u8g2.drawStr(53, 21, ":");
    u8g2.drawStr(100, 21, "C");
    u8g2.drawStr(0, 32, "pH =");
    u8g2.drawStr(53, 32, ":");
    u8g2.drawStr(110, 32, "mV");
    u8g2.drawStr(0, 42, "pH = 4.01");
    u8g2.drawStr(53, 42, ":");
    u8g2.drawStr(110, 42, "mV");
    u8g2.drawStr(0, 52, "pH = 6.86");
    u8g2.drawStr(53, 52, ":");
    u8g2.drawStr(110, 52, "mV");
    u8g2.drawStr(0, 62, "pH = 9.18");
    u8g2.drawStr(53, 62, ":");
    u8g2.drawStr(110, 62, "mV");
    u8g2.drawStr(25, 32, String(pH_val).c_str());
    u8g2.drawStr(65, 21, String(temperature).c_str());
    u8g2.drawStr(65, 32, String(pH_ADCval).c_str());
    u8g2.drawStr(65, 42, String(pH4Calib_ADCval).c_str());
    u8g2.drawStr(65, 52, String(pH7Calib_ADCval).c_str());
    u8g2.drawStr(65, 62, String(pH9Calib_ADCval).c_str());
    u8g2.sendBuffer();
}

void DOSensorDisplay_page(float temperature, float DO_ADCval, float DO0_ADCval, float DO100_ADCval) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(30, 10, "DO Sensor");
    u8g2.drawStr(0, 23, "Temp");
    u8g2.drawStr(50, 23, ":");
    u8g2.drawStr(95, 23, "C");
    u8g2.drawStr(0, 36, "DO");
    u8g2.drawStr(50, 36, ":");
    u8g2.drawStr(106, 36, "mV");
    u8g2.drawStr(0, 48, "D0-0%");
    u8g2.drawStr(50, 48, ":");
    u8g2.drawStr(106, 48, "mV");
    u8g2.drawStr(0, 60, "D0-100%");
    u8g2.drawStr(50, 60, ":");
    u8g2.drawStr(106, 60, "mV");
    u8g2.drawStr(60, 23, String(temperature).c_str());
    u8g2.drawStr(60, 36, String(DO_ADCval).c_str());
    u8g2.drawStr(60, 48, String(DO0_ADCval).c_str());
    u8g2.drawStr(60, 60, String(DO100_ADCval).c_str());
    u8g2.sendBuffer();
}

void TDSSensorDisplay_page(float temperature, float SALT_val, float SALT08_val, float SALT09_val) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(30, 10, "TDS Sensor");
    u8g2.drawStr(0, 23, "Temp");
    u8g2.drawStr(60, 23, ":");
    u8g2.drawStr(100, 23, "C");
    u8g2.drawStr(0, 36, "SALT");
    u8g2.drawStr(60, 36, ":");
    u8g2.drawStr(100, 36, "ohm");
    u8g2.drawStr(0, 48, "SALT-0.8%");
    u8g2.drawStr(60, 48, ":");
    u8g2.drawStr(100, 48, "ohm");
    u8g2.drawStr(0, 60, "SALT-0.9%");
    u8g2.drawStr(60, 60, ":");
    u8g2.drawStr(100, 60, "ohm");
    u8g2.drawStr(68, 23, String(temperature).c_str());
    u8g2.drawStr(68, 36, String(SALT_val).c_str());
    u8g2.drawStr(68, 48, String(SALT08_val).c_str());
    u8g2.drawStr(68, 60, String(SALT09_val).c_str());
    u8g2.sendBuffer();
}

void calibpH4() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(10, 12, "pH = 4 solution");
    u8g2.drawStr(0, 24, "Wait probe is stable");
    u8g2.drawStr(0, 48, "Push NEXT to calib");
    u8g2.sendBuffer();
    uint8_t push = digitalRead(NEXT_BTN);

    while (push != 0 || digitalRead(BACK_BTN) != 0)
        push = digitalRead(NEXT_BTN);  // wait until push button NEXT or BACK
    if (push == 0) {                   // NEXT BUTTON pushed --> calib
        waitingScreen();
        myPH.pH_4_Calibration(PH_PIN, VREF, ADC_RES);
        u8g2.drawStr(20, 22, "Done !!!");
        u8g2.drawStr(0, 44, "pH = 4 --> ");
        u8g2.drawStr(60, 44, (String(myPH.getPH_4_Voltage()) + "mV").c_str());
        u8g2.sendBuffer();
    }
    delay(1000);
}

void calibpH7() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(10, 12, "pH = 6.86 solution");
    u8g2.drawStr(0, 24, "Wait probe is stable");
    u8g2.drawStr(0, 48, "Push NEXT to calib");
    u8g2.sendBuffer();
    uint8_t push = digitalRead(NEXT_BTN);

    while (push != 0 || digitalRead(BACK_BTN) != 0)
        push = digitalRead(NEXT_BTN);  // wait until push button NEXT or BACK
    if (push == 0) {                   // NEXT BUTTON pushed --> calib
        waitingScreen();
        myPH.pH_4_Calibration(PH_PIN, VREF, ADC_RES);
        u8g2.drawStr(20, 22, "Done !!!");
        u8g2.drawStr(0, 44, "pH = 6.86 --> ");
        u8g2.drawStr(60, 44, (String(myPH.getPH_6_86_Voltage()) + "mV").c_str());
        u8g2.sendBuffer();
    }
    delay(1000);
}

void calibpH9() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(10, 12, "pH = 9.18 solution");
    u8g2.drawStr(0, 24, "Wait probe is stable");
    u8g2.drawStr(0, 48, "Push NEXT to calib");
    u8g2.sendBuffer();
    uint8_t push = digitalRead(NEXT_BTN);

    while (push != 0 || digitalRead(BACK_BTN) != 0)
        push = digitalRead(NEXT_BTN);  // wait until push button NEXT or BACK
    if (push == 0) {                   // NEXT BUTTON pushed --> calib
        waitingScreen();
        myPH.pH_4_Calibration(PH_PIN, VREF, ADC_RES);
        u8g2.drawStr(20, 22, "Done !!!");
        u8g2.drawStr(0, 44, "pH = 9.18 --> ");
        u8g2.drawStr(60, 44, (String(myPH.getPH_9_Voltage()) + "mV").c_str());
        u8g2.sendBuffer();
    }
    delay(1000);
}

void calibDO0() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(0, 0, "Enter the probe to");
    u8g2.drawStr(0, 12, "Zero percent oxygen solution");
    u8g2.drawStr(0, 24, "And wait until");
    u8g2.drawStr(0, 36, "the probe is stable");
    u8g2.drawStr(0, 48, "Push SELECT to begin calib");
    u8g2.sendBuffer();
    uint8_t push = digitalRead(NEXT_BTN);

    while (push != 0 || digitalRead(BACK_BTN) != 0)
        push = digitalRead(NEXT_BTN);  // wait until push button NEXT or BACK
    if (push == 0) {                   // NEXT BUTTON pushed --> calib
        waitingScreen();
        myDO.DO_0_calibration(DO_PIN, VREF / 1000.0, ADC_RES);
        u8g2.drawStr(20, 22, "Done !!!");
        u8g2.drawStr(0, 44, "DO = 0% --> ");
        u8g2.drawStr(60, 44, (String(myDO.getv0()) + "mV").c_str());
        u8g2.sendBuffer();
    }
    delay(1000);
}

void calibDO100() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(0, 0, "Enter the probe to");
    u8g2.drawStr(0, 12, "Air and maintain proper air flow");
    u8g2.drawStr(0, 24, "And wait until");
    u8g2.drawStr(0, 36, "the probe is stable");
    u8g2.drawStr(0, 48, "Push SELECT to begin calib");
    u8g2.sendBuffer();
    uint8_t push = digitalRead(NEXT_BTN);
    while (push != 0 || digitalRead(BACK_BTN) != 0)
        push = digitalRead(NEXT_BTN);  // wait until push button NEXT or BACK
    if (push == 0) {                   // NEXT BUTTON pushed --> calib
        waitingScreen();
        myDO.DO_100_calibration(DO_PIN, VREF / 1000.0, ADC_RES);
        u8g2.drawStr(20, 22, "Done !!!");
        u8g2.drawStr(0, 44, "DO = 100% --> ");
        u8g2.drawStr(60, 44, (String(myDO.getv100()) + "mV").c_str());
        u8g2.sendBuffer();
    }
    delay(1000);
}

void Acception() {
    u8g2.clearDisplay();
    // u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(10, 32, "Accept calibration");
    u8g2.drawStr(40, 44, " Value ");
    u8g2.sendBuffer();
}

void waitingScreen() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(0, 10, "Take it easy. Just a second");
    u8g2.sendBuffer();
}

/**
========================= RTC DS 1307 ================================
*/
/* read value from DS 1307 RTC */
void readDS1307() {
    Wire.beginTransmission(DS1307_addr);
    Wire.write((uint8_t)0x00);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_addr, NumberOfFields);

    second = bcd2dec(Wire.read() & 0x7f);
    minute = bcd2dec(Wire.read());
    hour = bcd2dec(Wire.read() & 0x3f);  // 24h format
    wday = bcd2dec(Wire.read());
    day = bcd2dec(Wire.read());
    month = bcd2dec(Wire.read());
    year = bcd2dec(Wire.read());
    year += 2000;
}

/* Convert BCD to DEC */
int bcd2dec(uint8_t num) {
    return ((num / 16 * 10) + (num % 16));
}

/* Convert DEC to BCD */
int dec2bcd(uint8_t num) {
    return ((num / 10 * 16) + (num % 10));
}

/* Print RTC DS1307 to monitor */
void digitalClockDisplay() {
    // digital clock display of the time
    Serial.print(day);
    Serial.print("-");
    Serial.print(month);
    Serial.print("-");
    Serial.print(year);
    Serial.print("  ");
    Serial.print(hour);
    printDigits(minute);
    printDigits(second);
    Serial.print(" --> ");
}

void printDigits(uint16_t digits) {
    Serial.print(":");

    if (digits < 10)
        Serial.print('0');
    Serial.print(digits);
}

/* Set time */
void setTime(uint8_t hr, uint8_t min, uint8_t sec, uint8_t wd, uint8_t d, uint8_t mth, uint8_t yr) {
    Wire.beginTransmission(DS1307_addr);
    Wire.write(uint8_t(0x00));  // reset pointer
    Wire.write(dec2bcd(sec));
    Wire.write(dec2bcd(min));
    Wire.write(dec2bcd(hr));
    Wire.write(dec2bcd(wd));  // day of week: Sunday = 1, Saturday = 7
    Wire.write(dec2bcd(d));
    Wire.write(dec2bcd(mth));
    Wire.write(dec2bcd(yr));
    Wire.endTransmission();
}

/* ====================================================================== */
