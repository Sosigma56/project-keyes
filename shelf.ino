#define BLYNK_PRINT Serial    // Enables serial print for debugging

// ***** สำคัญมาก! กำหนด BLYNK_TEMPLATE_ID และ BLYNK_DEVICE_NAME (หรือ BLYNK_TEMPLATE_NAME)
// ให้เป็นสิ่งแรกๆ ที่ไลบรารี Blynk จะเห็น.
// ข้อผิดพลาด "Please specify your BLYNK_TEMPLATE_ID and BLYNK_TEMPLATE_NAME" เกิดขึ้นเมื่อ
// ไลบรารี Blynk ถูก include ก่อนที่ค่าเหล่านี้จะถูกกำหนดอย่างถูกต้อง.
#define BLYNK_TEMPLATE_ID   "TMPL6kH094D4l"
#define BLYNK_DEVICE_NAME   "sensorultrasonic" // ชื่ออุปกรณ์ของคุณใน Blynk IoT Console
 // เพิ่มบรรทัดนี้เพื่อรองรับไลบรารีที่อาจจะยังหา BLYNK_TEMPLATE_NAME
#define BLYNK_TEMPLATE_ID "TMPL6kH094D4l"
#define BLYNK_TEMPLATE_NAME "sensorultrasonic"
#define BLYNK_FIRMWARE_VERSION "0.1.0"      // กำหนดเวอร์ชันเฟิร์มแวร์ (แนะนำ)

// บางครั้งจำเป็นต้องกำหนด Token เริ่มต้นสำหรับไลบรารี Blynk เวอร์ชันใหม่ๆ
// ถึงแม้ว่าเราจะส่ง auth[] ใน Blynk.begin() ก็ตาม
#define BLYNK_AUTH_TOKEN_DEFAULT "ynZiXLNg3Jgs9--A4YhFQqFq4aoTYkDy"


#include <WiFi.h>             // Library for ESP32 Wi-Fi (เปลี่ยนจาก ESP8266WiFi.h)
#include <BlynkSimpleEsp32.h> // Library for Blynk on ESP32 (เปลี่ยนจาก BlynkSimpleEsp8266.h)

// คุณควรได้รับ Auth Token ใน Blynk App.
// ไปที่ Project Settings (ไอคอนรูปน็อต).
// ตัวแปร auth นี้จะถูกใช้ใน Blynk.begin()
char auth[] = "ynZiXLNg3Jgs9--A4YhFQqFq4aoTYkDy"; // Auth Token ของคุณ

// ข้อมูล Wi-Fi ของคุณ.
// ตั้งค่า pass เป็น "" สำหรับเครือข่ายเปิด.
char ssid[] = "Redmi 9T";     // ชื่อ Wi-Fi ของคุณ
char pass[] = "12345678";     // รหัสผ่าน Wi-Fi ของคุณ

// สำหรับ Blynk Cloud Server
const char BLYNK_HOST[] = "blynk.cloud"; // โฮสต์ของเซิร์ฟเวอร์ Blynk Cloud
const int BLYNK_PORT = 80;               // พอร์ตของเซิร์ฟเวอร์ Blynk Cloud

// Ultrasonic Sensor Pins
// กำหนดขา GPIO ของ ESP32 (ใช้เลข GPIO โดยตรง)
const int trigPin = 33; // GPIO5 (ขา Trig ของ HC-SR04, เทียบเท่า D1 บน NodeMCU)
const int echoPin = 27; // GPIO4 (ขา Echo ของ HC-SR04, เทียบเท่า D2 บน NodeMCU)

// LED Pin - Removed definition and usage for physical LED
// const int ledPin = 16; // GPIO16 (ขาควบคุม LED, เทียบเท่า D0 บน NodeMCU) // Removed

long duration; // Used to store the duration of the ultrasonic pulse travel
int distanceCm; // Used to store the calculated distance in centimeters

// V1 is a Virtual Pin connected to the LED Widget in Blynk
#define VIRTUAL_PIN_LED V1

void setup() {
  // Start Serial Communication for debugging purposes
  Serial.begin(115200);

  // Set the mode for Trig and Echo pins of the Ultrasonic sensor
  pinMode(trigPin, OUTPUT); // Trig Pin as Output (sends sound waves)
  pinMode(echoPin, INPUT);  // Echo Pin as Input (receives reflected sound waves)

  // LED Pin - Removed initialization and initial state setting for physical LED
  // pinMode(ledPin, OUTPUT); // Removed
  // digitalWrite(ledPin, LOW); // Ensure LED is off at startup // Removed

  // Begin Blynk connection
  // Provide Auth Token, SSID, Password, Blynk Host, and Port
  Blynk.begin(auth, ssid, pass, BLYNK_HOST, BLYNK_PORT);

  // Wait for Blynk connection to be established (optional, but helpful for debugging)
  while (Blynk.connect() == false) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to Blynk!");
}

void loop() {
  // Run the Blynk client in the loop to maintain connection and interaction with Blynk
  Blynk.run();

  // Check if Blynk is connected before sending data (optional)
  if (Blynk.connected()) {
    // ***** Ultrasonic Sensor Operation *****
    // Clear the trigPin by setting it LOW for 2 microseconds
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Set the trigPin HIGH for 10 microseconds to send a pulse
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the echoPin, returns the sound wave travel time in microseconds
    // pulseIn() waits for the Echo Pin to go HIGH and then measures the duration until it goes LOW
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance in centimeters
    // Speed of sound in air is approximately 343 meters/second or 0.0343 cm/microsecond
    // Distance = (round-trip time * speed of sound) / 2 (divided by 2 because it's round trip)
    distanceCm = duration * 0.0343 / 2;

    // Print the distance to the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distanceCm);
    Serial.println(" cm");

    // ***** Condition Check and Blynk Control (Physical LED control removed) *****
    // Check if the distance is greater than 15 cm
    if (distanceCm > 15) {
      Serial.println("Distance > 15 cm: Blynk LED ON");
      // digitalWrite(ledPin, HIGH); // Removed: Turn on the physical LED
      Blynk.virtualWrite(VIRTUAL_PIN_LED, 255); // Send 255 (ON) to the Blynk LED Widget
    } else {
      Serial.println("Distance <= 15 cm: Blynk LED OFF");
      // digitalWrite(ledPin, LOW);  // Removed: Turn off the physical LED
      Blynk.virtualWrite(VIRTUAL_PIN_LED, 0);   // Send 0 (OFF) to the Blynk LED Widget
    }
  } else {
    // If Blynk is not connected, do nothing or print a warning message
    Serial.println("Blynk not connected. Waiting...");
  }

  // Add a small delay to prevent overly rapid readings and keep the serial output readable
  delay(500); // Read every 0.5 seconds
}
