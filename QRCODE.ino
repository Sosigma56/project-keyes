#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6kH094D4l"
#define BLYNK_TEMPLATE_NAME "sensorultrasonic"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "esp_camera.h"
#include "ESP32QRCodeReader.h"
#include "soc/soc.h" // For disabling brownout detector
#include "soc/rtc_cntl_reg.h"

// --- 1. Blynk & WiFi Credentials ---
char auth[] = "ynZiXLNg3Jgs9--A4YhFQqFq4aoTYkDy";    // <-- ใส่ Auth Token ของคุณ
char ssid[] = "Redmi 9T";            // <-- ใส่ชื่อ WiFi ของคุณ
char pass[] = "12345678";        // <-- ใส่รหัสผ่าน WiFi ของคุณ

// --- 2. Blynk Virtual Pins ---
#define VIRTUAL_PIN_COMMAND V9
#define VIRTUAL_PIN_STATUS  V8

// --- 3. Camera Pin Definitions for ESP32-CAM AI-THINKER ---
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// --- 4. Global Objects ---
// เปลี่ยนการสร้าง object ให้ไม่มี argument
ESP32QRCodeReader reader;

// --- 5. Camera Setup Function ---
void camera_setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM; config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM; config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM; config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM; config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    Blynk.virtualWrite(VIRTUAL_PIN_STATUS, "ERROR: Camera init failed!");
    return;
  }
}

// --- 6. QR Code Scanning Function ---
// **ฟังก์ชันนี้ถูกแก้ไขให้เข้ากับไลบรารีเวอร์ชันเก่าของคุณ**
String scanQRCode() {
  Serial.println("Capturing frame for QR code scanning...");
  
  // ใช้ struct ชื่อ QRCodeData ตามไลบรารีเวอร์ชันเก่า
  QRCodeData qrCodeData;
  
  // ใช้ฟังก์ชัน receiveQRCode() ที่จะคืนค่าเป็น bool
  // และเก็บข้อมูลใน struct
  bool success = reader.receiveQrCode(&qrCodeData, 5000); // 5 วินาที timeout
  
  if (success) {
    Serial.println("QR Code Found!");
    // ข้อมูลที่ได้อยู่ใน payload ของ struct
    String decoded_data = String((char*)qrCodeData.payload); 
    return decoded_data;
  } else {
    Serial.println("No QR Code detected or timeout.");
    return "";
  }
}

// --- 7. Blynk Virtual Pin Handler (Main Logic) ---
// ฟังก์ชันนี้จะถูกเรียกใช้เมื่อ Blynk ส่งข้อมูลมาที่ V9
BLYNK_WRITE(VIRTUAL_PIN_COMMAND) {
  String command = param.asStr();
  Serial.print("Received command: ");
  Serial.println(command);
  
  // ส่งสถานะกลับไปที่ Blynk Terminal
  Blynk.virtualWrite(VIRTUAL_PIN_STATUS, "Received command: " + command);

  // ตรวจสอบคำสั่งและเริ่มทำงาน
  if (command == "scan_qr") {
    Blynk.virtualWrite(VIRTUAL_PIN_STATUS, "Task started: Scanning for QR code...");
    
    // Step 1: Scan for the QR code
    String scanned_data = scanQRCode();

    // Step 2: Check the scanned data and report back to Blynk
    if (scanned_data.length() > 0) {
      // QR Code was found
      Blynk.virtualWrite(VIRTUAL_PIN_STATUS, "SCAN SUCCESS: Scanned data: " + scanned_data);
    } else {
      // No QR Code was found
      Blynk.virtualWrite(VIRTUAL_PIN_STATUS, "SCAN FAILED: No QR code detected.");
    }
  } else {
    // If the command is not recognized
    Blynk.virtualWrite(VIRTUAL_PIN_STATUS, "Command not recognized. Please use 'scan_qr'.");
  }
}

// --- 8. Setup & Loop ---
void setup() {
  Serial.begin(115200);
  
  // Initialize the camera and QR code reader
  camera_setup();
  reader.setDebug(true); // แก้ไขเป็น reader.setDebug(true) เพราะฟังก์ชันรับ bool

  // Connect to WiFi and Blynk
  Blynk.begin(auth, ssid, pass);
  
  // Send a welcome message to the Blynk Terminal upon connection
  Blynk.virtualWrite(VIRTUAL_PIN_STATUS, "System is ready! Awaiting commands...");
}

void loop() {
  Blynk.run();
}