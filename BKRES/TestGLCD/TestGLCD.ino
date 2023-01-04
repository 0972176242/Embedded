#include <U8g2lib.h>

#define E_PINGLCD 13
#define RW_PIN_GLCD 11
#define REG_PIN_GLCD 10
#define RS_PIN_GLCD 8

#define NEXT_BTN 3
#define DOWN_BTN 6
#define SELECT_BTN 4
#define BACK_BTN 5

String dayOfWeek[8] = {"", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
char timeDisplay[25];

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, E_PINGLCD, RW_PIN_GLCD, REG_PIN_GLCD, RS_PIN_GLCD); // 8 NC
uint8_t up_value;
uint8_t down_value;
uint8_t menu_value;
uint8_t back_value;

uint8_t reference_value = 1;
static int total = 0;
static int total_count = 0;
static int back_count = 0;
static int select_down = 0;

void displayVal_page(uint8_t wday, uint8_t day, uint8_t month, uint8_t year, uint8_t hour, uint8_t minute, uint8_t second, float temperature, float pH_val, float DO_val, float SALT_val)
{
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);
    sprintf(timeDisplay, "%02d-%02d-%04d  %02d:%02d:%02d",
            day, month, year, hour, minute, second);
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(0, 10, (dayOfWeek[wday] + String(timeDisplay)).c_str());
    u8g2.drawStr(0, 23, "Temp");
    u8g2.drawStr(50, 23, ":");
    u8g2.drawStr(95, 23, "C");
    u8g2.drawStr(0, 34, "pH");
    u8g2.drawStr(50, 34, ":");
    u8g2.drawStr(90, 34, "  ");
    u8g2.drawStr(0, 48, "DO ");
    u8g2.drawStr(50, 48, ":");
    u8g2.drawStr(90, 48, "mg/L");
    u8g2.drawStr(50, 61, ":");
    u8g2.drawStr(0, 61, "EC");
    u8g2.drawStr(90, 61, "uS/cm");
    u8g2.drawStr(60, 23, String(temperature).c_str());
    u8g2.drawStr(60, 34, String(pH_val).c_str());
    u8g2.drawStr(60, 48, String(DO_val).c_str());
    u8g2.drawStr(60, 61, String(SALT_val).c_str());
    u8g2.sendBuffer();
}

void pHSensorDispay_page(float temperature, float pH_val, float pH_ADCval, float pH4Calib_ADCval, float pH7Calib_ADCval, float pH9Calib_ADCval)
{
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

void DOSensorDisplay_page(float temperature, float DO_ADCval, float DO0_ADCval, float DO100_ADCval)
{
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

void TDSSensorDisplay_page(float temperature, float SALT_val, float SALT08_val, float SALT09_val)
{
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

void Acception()
{
    u8g2.clearDisplay();
    // u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tf);
    u8g2.drawStr(10, 32, "Accept calibration");
    u8g2.drawStr(40, 44, " Value ");
    u8g2.sendBuffer();
}

void menu_sum()
{
    if (total == 0)
    {
        total_count = 1;
        displayVal_page(2, 12, 11, 2022, 13, 18, 05, 25.76, 2.21, 1.23, 11);
    }
    else if (total == 1)
    {
        pHSensorDispay_page(25, 12.12, 1234.12, 3452.12, 3321.12, 4321.12);
    }
    else if (total == 2)
    {
        DOSensorDisplay_page(21.32, 2345.12, 1234.23, 1254.21);
    }
    else if (total == 3)
    {
        TDSSensorDisplay_page(21.32, 12.32, 23.21, 34.21);
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(NEXT_BTN, INPUT_PULLUP);
    pinMode(DOWN_BTN, INPUT_PULLUP);
    pinMode(SELECT_BTN, INPUT_PULLUP);
    pinMode(BACK_BTN, INPUT_PULLUP);
    u8g2.initDisplay();
    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);
    menu_sum();
}

void loop()
{
    down_value = digitalRead(DOWN_BTN);
    up_value = digitalRead(NEXT_BTN);
    menu_value = digitalRead(SELECT_BTN);
    back_value = digitalRead(BACK_BTN);

    if (up_value != reference_value) // up button
    {
        if (up_value == 0)
        {
            if (total_count == 2 && (total == 1 or total == 2 or total == 3))
            {
                total_count = 1;
            }
            if (total_count == 1)
            {
                if (total >= 3)
                {
                    total = 0;
                    select_down = 0;
                }
                else
                {
                    total++;
                    select_down = 0;
                }
                menu_sum();
            }
            delay(100);
        }
        reference_value = up_value;
    }

    if (down_value != reference_value)
    { //  down button

        if (down_value == 0)
        {
            if (total_count == 1)
            {
                if (select_down >= 3)
                {
                    select_down = 1;
                }
                else
                {
                    select_down++;
                }
            }
            if ((total_count == 1 or total_count == 2) && (total == 2 or total == 3) && select_down == 3)
            {
                select_down = 1;
            }
            if (total_count == 1 && total == 1 && select_down == 1)
            {
                back_count = 0;
                pHSensorDispay_page(25, 12.12, 1234.12, 3452.12, 3321.12, 4321.12);
                u8g2.drawBox(0, 32, 128, 11);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 1 && select_down == 2)
            {
                back_count = 0;
                pHSensorDispay_page(25, 12.12, 1234.12, 3452.12, 3321.12, 4321.12);
                u8g2.drawBox(0, 42, 128, 11);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 1 && select_down == 3)
            {
                back_count = 0;
                pHSensorDispay_page(25, 12.12, 1234.12, 3452.12, 3321.12, 4321.12);
                u8g2.drawBox(0, 52, 128, 11);
                u8g2.sendBuffer(); 
            }

            if (total_count == 1 && total == 2 && select_down == 1)
            {
                back_count = 0;
                DOSensorDisplay_page(21.32, 2345.12, 1234.23, 1254.21);
                u8g2.drawBox(0, 38, 128, 12);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 2 && select_down == 2)
            {
                back_count = 0;
                DOSensorDisplay_page(21.32, 2345.12, 1234.23, 1254.21);
                u8g2.drawBox(0, 50, 128, 12);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 3 && select_down == 1)
            {
                back_count = 0;
                TDSSensorDisplay_page(23.32, 23.23, 23.32, 23.32);
                u8g2.drawBox(0, 38, 128, 12);
                u8g2.sendBuffer();
            }
            if (total_count == 1 && total == 3 && select_down == 2)
            {
                back_count = 0;
                TDSSensorDisplay_page(23.32, 23.23, 23.32, 23.32);
                u8g2.drawBox(0, 50, 128, 12);
                u8g2.sendBuffer();
            }
            delay(100);
        }
        reference_value = down_value;
    }

    if (menu_value != reference_value) // selection button
    {
        if (menu_value == 0)
        {
            total_count++;
            if (total_count == 1)
            {
                back_count = 0;
                menu_sum();
            }
            else if (total_count > 2)
            {
                total_count = 2;
                back_count = 0;
            }

            if (total_count == 2 && total == 1 && select_down == 1)
            {
                back_count = 0;
                total_count = total_count - 1;
                Acception();
            }
            if (total_count == 2 && total == 1 && select_down == 2)
            {
                back_count = 0;
                total_count = total_count - 1;
                Acception();
            }
            if (total_count == 2 && total == 1 && select_down == 3)
            {
                back_count = 0;
                total_count = total_count - 1;
                Acception();
            }

            if (total_count == 2 && total == 2 && select_down == 1)
            {
                back_count = 0;
                total_count = total_count - 1;
                Acception();
            }
            if (total_count == 2 && total == 2 && select_down == 2)
            {
                back_count = 0;
                total_count = total_count - 1;
                Acception();
                select_down = 0;
            }
            if (total_count == 2 && total == 3 && select_down == 1)
            {
                back_count = 0;
                total_count = total_count - 1;
                Acception();
            }
            if (total_count == 2 && total == 3 && select_down == 2)
            {

                back_count = 0;
                total_count = total_count - 1;
                Acception();
                select_down = 0;
            }
            if (total_count == 2 && (total == 0) && (select_down == 1 or select_down == 2 or select_down == 3 or select_down == 0))
            {
                total_count = 1;
                back_count = 0;
                menu_sum();
            }
            delay(100);
        }
        reference_value = menu_value;
    }

    if (back_value != reference_value) // back button
    {
        if (back_value == 0)
        {
            back_count++;
            if (back_count == 1)
            {
                if (total_count == 2 && (total == 0 or total == 1 or total == 2 or total == 3 or select_down == 1 or select_down == 2 or select_down == 3))
                {
                    total_count = total_count - 1;
                    back_count = 0;
                    total = 0;
                }
                else if (total_count == 1 && (total == 0 or total == 1 or total == 2 or total == 3 or select_down == 1 or select_down == 2 or select_down == 3))
                {
                    back_count = 0;
                    total = 0;
                    select_down = 0;
                    menu_sum();
                }
            }
            else

            {
                back_count = 0;
            }
            delay(100);
        }
        reference_value = back_value;
    }
    // Serial.print("Dem tong: "); Serial.print(total_count); Serial.print("    ");
    // Serial.print("+/- tong: "); Serial.print(total); Serial.print("    ");
    // Serial.print("Dem back: "); Serial.print(back_count); Serial.println("    ");
    // Serial.print("Dem xuong: "); Serial.print(select_down); Serial.println("    ");
    delay(100);
}
