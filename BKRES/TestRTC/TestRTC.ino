#include <Wire.h>  // IIC protocol

const uint8_t DS1307_addr = 0x68;  // Addr i2c
const uint8_t NumberOfFields = 7;  // number of uint8_ts to read
uint8_t second, minute, hour, day, wday, month;
uint16_t year;
char time [25];

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  // Time RTC init
  // setTime(15, 59, 40, 2, 14, 11, 22);  // 14:21:15 SUN 13/11/22

  // delay for RTC setup done
  delay(3000);
}
void loop() {
  Serial.print("Hello ");
  // put your main code here, to run repeatedly:
  sprintf(time, "%02d-%02d-%04d  %02d:%02d:%02d",
                  day, month, year, hour, minute, second);
  readDS1307();
  Serial.println(String(time));
  delay(1000);
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