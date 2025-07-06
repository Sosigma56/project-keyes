/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-stepper-motor-28byj-48-uln2003/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based on Stepper Motor Control - one revolution by Tom Igoe
*/
#include <Stepper.h>
#include <ESP32Servo.h> // ไลบรารีสำหรับ Servo บน ESP32

const int stepsPerRevolution = 2048;  // จำนวนสเต็ปต่อการหมุน 1 รอบสำหรับ 28BYJ-48

// ULN2003 Motor Driver Pins สำหรับ Stepper 1 (INA)
#define INA1 19
#define INA2 18
#define INA3 17
#define INA4 16

// ULN2003 Motor Driver Pins สำหรับ Stepper 2 (INB)
#define INB1 13
#define INB2 12
#define INB3 14
#define INB4 27


int roundupstep1 = 9;
int roundupstep2 = 9; // ตัวแปรนี้ยังไม่ได้ใช้ในโค้ดปัจจุบัน แต่เก็บไว้เผื่ออนาคต
int pos = 0;          // ตัวแปรสำหรับเก็บตำแหน่ง Servo

// กำหนด Object ของ Stepper Motor
Stepper myStepper1(stepsPerRevolution, INA1, INA3, INA2, INA4);
Stepper myStepper2(stepsPerRevolution, INB1, INB3, INB2, INB4);

Servo myservo;
Servo myservo2;       // สร้าง Object Servo สำหรับควบคุม Servo ตัวที่ 2 (ใหม่)        // สร้าง Object Servo สำหรับควบคุม Servo

void setup() {
  Serial.begin(115200); // เริ่มต้น Serial Port
  Serial.println("--- เริ่มต้นโปรแกรม ---");

  // กำหนดความเร็วของ Stepper Motor
  myStepper1.setSpeed(15);
  myStepper2.setSpeed(15);

  // ผูก Servo Object กับขา GPIO 5
  myservo.attach(5);
  myservo2.attach(4); // ผูก Servo ตัวที่ 2
  Serial.println("Servo ถูกผูกกับขา GPIO 5");

  
   // *** สำคัญ: ตั้งค่า Servo ไปที่ 90 องศาก่อนเริ่มกวาด ***
  myservo.write(90);
  myservo2.write(90); // กำหนดตำแหน่งเริ่มต้นของ Servo 2

  delay(500); // หน่วงเวลาเล็กน้อยเพื่อให้ Servo ไปถึงตำแหน่งเริ่มต้น


  // --- เริ่มต้นการทำงานของ Stepper Motor ---

   //Stepper Motor 1 หมุนตามเข็มนาฬิกา roundupstep1 รอบ
  Serial.println("Stepper 1 กำลังหมุนตามเข็มนาฬิกา...");
  for (int x = 0; x < roundupstep1; x++) {
   myStepper1.step(stepsPerRevolution);
    // ไม่ควรมี delay() มากเกินไปใน loop นี้ เพราะจะทำให้ Stepper 2 ต้องรอนานมาก
  }
  Serial.println("Stepper 1 หมุนครบแล้ว");
  // --- Servo Motor 2 ทำงาน (กวาดมุม 90-180-90) ---
  Serial.println("Servo 2 กำลังทำงาน (กวาดมุม 90-180-90)..."); // อัปเดตข้อความ
  // ลูปแรก: หมุนจาก 90 องศา ขึ้นไป 180 องศา
  for (pos = 90; pos <= 180; pos += 1) { // เปลี่ยนจุดสิ้นสุดเป็น 180
    myservo2.write(pos);
    delay(15);
 }
  Serial.println("Servo 2 ไปที่ 180 องศาแล้ว"); // อัปเดตข้อความ
  delay(500);

  // ลูปที่สอง: หมุนจาก 180 องศา ลงไป 90 องศา
  for (pos = 180; pos >= 90; pos -= 1) { // เปลี่ยนจุดเริ่มต้นเป็น 180 และจุดสิ้นสุดเป็น 90
    myservo2.write(pos);
    delay(15);
  }
  Serial.println("Servo 2 กลับมาที่ 90 องศาแล้ว"); // อัปเดตข้อความ
  delay(500); // หน่วงเวลาหลังจาก Servo 2 ทำงานเสร็จ

  // --- Stepper Motor 2 ทำงาน ---
  Serial.println("Stepper 2 กำลังหมุนตามเข็มนาฬิกา...");
  for (int x = 0; x < roundupstep2; x++) {
    myStepper2.step(-stepsPerRevolution);
  }
  Serial.println("Stepper 2 หมุนครบแล้ว");
  
  delay(1000); // หน่วงเวลาหลังจาก Stepper 1 หมุนเสร็จ

  // Stepper Motor 2 หมุนตามเข็มนาฬิกา roundupstep2 รอบ
  // ในโค้ดเดิมคุณใช้ roundupstep1 แต่เรามี roundupstep2 ก็ควรใช้ตัวนี้
  Serial.println("Stepper 2 กำลังหมุนตามเข็มนาฬิกา...");
  for (int x = 0; x < roundupstep2; x++) { // ใช้ roundupstep2
    myStepper2.step(-stepsPerRevolution);
  }
  Serial.println("Stepper 2 หมุนครบแล้ว");
  delay(1000); // หน่วงเวลาหลังจาก Stepper 2 หมุนเสร็จ

  // --- Servo Motor ทำงานเพียงครั้งเดียวหลังจาก Stepper หมุนเสร็จ ---
  // Servo Motor ทำงานเพียงครั้งเดียวหลังจาก Stepper หมุนเสร็จ
  Serial.println("Servo กำลังทำงาน (กวาดมุม 90-0-90)...");

  

  // ลูปแรก: กวาด Servo จาก 90 องศา ลงไป 0 องศา
  for (pos = 90; pos >= 0; pos -= 1) { // หมุนจาก 90 องศา ลงไป 0 องศา
    myservo.write(pos);
    delay(15); // หน่วงเวลาเพื่อให้ Servo ไปถึงตำแหน่ง
  }
  Serial.println("Servo ไปที่ 0 องศาแล้ว");
  delay(500); // หน่วงเวลาที่ 0 องศา

  // ลูปที่สอง: กวาด Servo จาก 0 องศา ขึ้นไป 90 องศา
  for (pos = 0; pos <= 90; pos += 1) { // หมุนจาก 0 องศา ขึ้นไป 90 องศา
    myservo.write(pos);
    delay(15); // หน่วงเวลาเพื่อให้ Servo ไปถึงตำแหน่ง
  }
  Serial.println("Servo กลับมาที่ 90 องศาแล้ว");
  delay(500); // หน่วงเวลาหลังจาก Servo ทำงานเสร็จ

  Serial.println("--- โปรแกรมใน Setup ทำงานเสร็จสิ้น ---");
} // ปิดฟังก์ชัน setup()
void loop() {
  // ฟังก์ชัน loop() จะว่างเปล่า เพราะเราต้องการให้ Stepper และ Servo ทำงานแค่ครั้งเดียวใน Setup
  // ถ้าต้องการให้ทำอะไรซ้ำๆ ที่นี่ก็ใส่ได้
}