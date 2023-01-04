//  // Bai toan : nhan tin dieu khien bat tat bong den
// // Neu noi dung tin nhan la : LAMP_ON       thi bat bong den
// // Neu noi dung tin nhan la : LAMP_OFF      thi tat bong den

// const String myphone = "01689951815";     // Thay so cua ban vao day
// const int PWR_KEY =  9;                          // Chan so 9 arduino uno dung lam chan dieu khien bat tat module sim900A
// const int RELAY =  12;                              // Chan so 12 arduino uno dung lam chan dieu khien dong/cat Relay de On/Off den

// String RxBuff = "";                                    // Khai bao bo dem nhan du lieu
//                                                                 // Tat ca du lieu nhan ve tu module sim deu duoc luu trong day 

// int Index_Lamp_On = -1;                           // vi tri cua chuoi "LAMP_ON"
// int Index_Lamp_Off = -1;                           // vi tri cua chuoi "LAMP_OFF"

// void Gsm_Power_On();                                            // Bat module Sim 900A
// void Gsm_Init();                                                       // Cau hinh Module Sim 900A
// void Gsm_MakeCall(String phone);                            // Ham goi dien
// void Gsm_MakeSMS(String phone,String content);     // Ham nhan tin


// void setup() {
//   Serial.begin(9600);                           // Cau hinh UART de giao tiep module Sim 900A

//   digitalWrite(RELAY, LOW);               // Khai bao chan  de dieu khien dong cat RELAY
//   pinMode(RELAY, OUTPUT);

//   digitalWrite(PWR_KEY, LOW);         // Khai bao chan PWR_KEY de dieu khien bat bat module Sim 900A
//   pinMode(PWR_KEY, OUTPUT);

//   delay(1000);                       
//   Gsm_Power_On();                                      // Bat Module Sim 900A
//   delay(10000);
//   Gsm_Init();                                                 // Cau hinh module Sim 900A
//   Gsm_MakeCall(myphone);                          // Test cuoc goi 
//   Gsm_MakeSMS(myphone,"I'm a test");       // Test tin nhan
// }

// void loop() {
//   delay(1000);                                                                    // tre 1s 
//   Index_Lamp_On = RxBuff.indexOf("LAMP_ON");              // Tim vi tri cua chuoi "LAMP_ON" trong bo dem nhan RxBuff
//   if(Index_Lamp_On >= 0)                                                   // Neu tim thay "LAMP_ON" trong RxBuff
//   {
//     Index_Lamp_On = -1;                                                     // 
//     RxBuff = "";                                                                   // Xoa bo dem
//     digitalWrite(RELAY, HIGH);                                             // Dong Relay de bat den // Bat bong den
//   }
//   else
//   {                                                                                    // Neu khong tim thay "LAMP_ON" thi tiep tuc tim "LAMP_OFF"
//     Index_Lamp_Off = RxBuff.indexOf("LAMP_OFF");          // Tim vi tri cua chuoi "LAMP_OFF" trong bo dem nhan RxBuff  
//     if(Index_Lamp_Off >= 0)                                                // Neu tim thay "LAMP_OFF" trong RxBuff
//     {
//       Index_Lamp_Off = -1;                                                   // 
//       RxBuff = "";                                                                // Xoa bo dem
//       digitalWrite(RELAY, LOW);                                           // Cat Relay de tat den
//     }
//   }
// }

// void serialEvent() {                                          // Chuong trinh ngat nhan du lieu
//   while (Serial.available()) {                               // Doi den khi co du lieu nhan ve
//     // get the new byte:
//     char inChar = (char)Serial.read();                  // Doc mot byte du lieu vua nhan ve
//     RxBuff += inChar;                                       // Ghi byte do vao bo dem nhan RxBuff (ta se xu ly RxBuff trong vong loop()) 
//     if(RxBuff.length()>= 128)                              // Neu bo dem qua dai thi xoa bo dem di
//     {
//       RxBuff = "";
//     }
//   }
// }

// void Gsm_Power_On()
// {
//   digitalWrite(PWR_KEY, HIGH);                // Du chan PWR_KEY len cao it nhat 1s 
//   delay(1500);                                            // o day ta de 1,5s
//   digitalWrite(PWR_KEY, LOW);                  // Du chan PWR_KEY xuong thap
//   delay(100);                                              // cac ban xem trong Hardware designed sim900A de hieu ro hon
// }

// void Gsm_Init()
// {
//   Serial.println("ATE0");                            // Tat che do phan hoi (Echo mode)
//   delay(2000);
//   Serial.println("AT+IPR=9600");              // Dat toc do truyen nhan du lieu 9600 bps
//   delay(2000);
//   Serial.println("AT+CMGF=1");                // Chon che do TEXT Mode
//   delay(2000);
//   Serial.println("AT+CLIP=1");                  // Hien thi thong tin nguoi goi den
//   delay(2000);
//   Serial.println("AT+CNMI=2,2");              // Hien thi truc tiep noi dung tin nhan
//   delay(2000);
// }

// void Gsm_MakeCall(String phone)           
// {
//   Serial.println("ATD" + phone + ";");         // Goi dien 
//   delay(10000);                                        // Sau 10s
//   Serial.println("ATH");                              // Ngat cuoc goi
//   delay(2000);
// }

// void Gsm_MakeSMS(String phone,String content)
// {
//   Serial.println("AT+CMGS=\"" + phone + "\"");     // Lenh gui tin nhan
//   delay(3000);                                                     // Cho ky tu '>' phan hoi ve 
//   Serial.print(content);                                          // Gui noi dung
//   Serial.print((char)26);                                         // Gui Ctrl+Z hay 26 de ket thuc noi dung tin nhan va gui tin di
//   delay(5000);                                                      // delay 5s
// }

#include <SoftwareSerial.h>
SoftwareSerial SIM900A(9, 8);
void setup()
{
  SIM900A.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println ("SIM900A Ready");
  delay(1000);
  call();
}
void loop()
{
  // delay(10000);
  // Serial.println("SIM900A CIPSHUT");
  // SIM900A.print("AT+CIPSHUT<CR><LF>");
  // delay(2000);

  // Serial.println("CIPSTART");
  // SIM900A.print("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"<CR><LF>");
  // delay(7000); 

  // Serial.println("CIPSEND");
  // SIM900A.print("AT+CIPSEND<CR><LF>");
  // delay(2000);
  
  // Serial.println("SEND");
  // SIM900A.print("GET /update?api_key=YS8UIZY9YWSY818L&field1=25 HTTP/1.1<CR><LF>Host: api.thingspeak.com<CR><LF><CR><LF><SUB>");
  }

 void call(){
   Serial.println("SIM900A Make a call");
   delay(1000);
   SIM900A.println("ATD+84972176242;");
   delay(7000);
}