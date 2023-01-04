#include <BufferedPrint.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <RingBuf.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>

// On the Ethernet Shield, CS is pin 4. SdFat handles setting SS
const int chipSelect = 53;
/*
 SD card read/write
  
 This example shows how to read and write data to and from an SD card file 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created   Nov 2010
 by David A. Mellis
 updated 2 Dec 2010
 by Tom Igoe
 modified by Bill Greiman 11 Apr 2011
 This example code is in the public domain.
 	 
 */
#include <Wire.h>  // IIC protocol
#include <SPI.h>

#define VREF 5.0
#define ADC_RES 1023

#define SD_SS_PIN 53

SdFat sd;
SdFile myFile;

const uint8_t DS1307_addr = 0x68;  // Addr i2c
const uint8_t NumberOfFields = 7;  // number of uint8_ts to read
uint8_t second, minute, hour, day, wday, month;
uint16_t year;
String fileName;
char c[20];


float temperature;

void setup() {
  // I2C init
  Wire.begin();
  
  sprintf(c, "Hello 1 2 3");

  Serial.begin(9600);
  while (!Serial)
    ;          // wait for Leonardo
  delay(400);  // catch Due reset problem

  // setTime(21, 13, 30, 1, 13, 11, 22);

  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(SD_SS_PIN, SPI_FULL_SPEED))
    Serial.println("SD card is removed ! Plug it in to write data !!");
  
  // readDS1307();
  // fileName = String(day) + "-" + String(month) + "-" + String(year) + "_" + String(hour) + "h" + String(minute) + "m.txt";


  // re-open the file for reading:
  // if (!myFile.open("test.txt", O_READ)) {
  //   sd.errorHalt("opening test.txt for read failed");
  // }
  // Serial.println("test.txt:");

  // read from the file until there's nothing else in it:
  // int data;
  // while ((data = myFile.read()) >= 0) Serial.write(data);
  // // close the file:
  // myFile.close();
}

void loop() {
  readDS1307();
  digitalClockDisplay();
  Serial.println();
  // open the file for write at end like the Native SD library
  if (myFile.open(fileName.c_str(), O_RDWR | O_CREAT | O_AT_END)) { // convert String to const char * using c_str()
    // if the file opened okay, write to it:
    Serial.print("Writing to test.txt...");
    myFile.write(c);

    // close the file:
    myFile.close();
    myFile.sync();
    Serial.println("done.");
  } else {
      if (sd.begin(SD_SS_PIN, SPI_FULL_SPEED)){
        fileName = String(day) + "-" + String(month) + "-" + String(year) + "_" + String(hour) + "h" + String(minute) + "m.txt";
        Serial.print("New file is created: " + fileName);
      }
      else     
        Serial.println("SD card is removed ! Plug it in to write data !!");
     
  }
  delay(2000);
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