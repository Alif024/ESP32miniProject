#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <ArduinoJson.h>

// กำหนดค่าสำหรับการเชื่อมต่อ WiFi และ Firebase
#define WIFI_SSID "ONE_2.4GHz"
#define WIFI_PASSWORD "91919191"
#define API_KEY "AIzaSyB5NElLq9i-qJJgqsYzyEaVlyLZaN0_RLo"
#define DATABASE_URL "https://ir-signal-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "aleefrock12345@gmail.com"
#define USER_PASSWORD "123456"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
// unsigned long sendDataPrevMillis = 0;

unsigned long long IRRawDataFB = 0;
int arrTimingInfoFB[10];  // กำหนดขนาดอาร์เรย์เป็น 10
size_t arraySizeFB = 0;
uint8_t NumberOfBitsFB;
String TimingInfoJsonStrFB;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Connected to Wi-Fi");

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready()) {
    // ใช้ getString() เพื่อรับค่าเป็นสตริง
    if (Firebase.RTDB.getString(&fbdo, "/IR-Signal-Choose/IRRawData")) {
      String valueStr = fbdo.stringData();
      // แปลงสตริงเป็น unsigned long long
      IRRawDataFB = strtoull(valueStr.c_str(), NULL, 10);
      Serial.print("Value: ");
      Serial.println(IRRawDataFB);
      if (IRRawDataFB == 0x6000000521C) {
        Serial.println("yes");
      }
    } else {
      // แสดงข้อความผิดพลาดหากการอ่านข้อมูลล้มเหลว
      Serial.println("Failed to read data from Firebase");
      Serial.println(fbdo.errorReason());
    }

    // ดึง NumberOfBits จาก database
    if (Firebase.RTDB.getInt(&fbdo, F("/IR-Signal-Choose/NumberOfBits"), &NumberOfBitsFB)) {
      Serial.print("NumberOfBitsFB: ");
      Serial.println(NumberOfBitsFB);
    }

    // ดึงข้อมูลสัญญาณ array จาก database เก็บในตัวแปร arrTimingInfoFB
    if (Firebase.RTDB.getArray(&fbdo, "/IR-Signal-Choose/DistanceWidthTimingInfo")) {
      TimingInfoJsonStrFB = String("{\"DistanceWidthTimingInfo\":") + fbdo.to<FirebaseJsonArray>().raw() + "}";
      // สร้าง object DynamicJsonDocument
      DynamicJsonDocument doc(1024);
      // แปลงสตริง JSON เข้าสู่ DynamicJsonDocument
      DeserializationError error = deserializeJson(doc, TimingInfoJsonStrFB);
      // ตรวจสอบว่ามีข้อผิดพลาดหรือไม่
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      // ดึงข้อมูล DistanceWidthTimingInfo
      JsonArray array = doc["DistanceWidthTimingInfo"];
      arraySizeFB = array.size();
      
      // คัดลอกข้อมูลจาก JsonArray เข้าสู่อาร์เรย์ C++
      for (size_t i = 0; i < arraySizeFB; i++) {
        arrTimingInfoFB[i] = array[i];
      }
      Serial.println(" ");
      // แสดงผลข้อมูลในอาร์เรย์
      for (size_t i = 0; i < arraySizeFB; i++) {
        Serial.println(arrTimingInfoFB[i]);
      }
    } else {
      Serial.printf("%s\n", fbdo.errorReason().c_str());
    }
    Serial.println(" ");
    delay(5000);  // รอ 5 วินาทีก่อนทำการดึงข้อมูลอีกครั้ง
  }
}
