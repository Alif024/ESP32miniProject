#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <ArduinoJson.h>

#define WIFI_SSID "ONE_2.4GHz"
#define WIFI_PASSWORD "91919191"
#define API_KEY "AIzaSyB5NElLq9i-qJJgqsYzyEaVlyLZaN0_RLo"
#define DATABASE_URL "https://ir-signal-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "aleefrock12345@gmail.com"
#define USER_PASSWORD "123456"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;

unsigned long long hexValue = 0x6000000521C;
unsigned long long decimalValue = hexValue;
int DistanceWidthTimingInfo[] = { 9050, 4550, 600, 1650, 600, 500 };
int arraySize = sizeof(DistanceWidthTimingInfo) / sizeof(DistanceWidthTimingInfo[0]);
uint8_t sNumberOfBits = 44;
String jsonStr;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  config.timeout.serverResponse = 10 * 1000;

  for (size_t i = 0; i < arraySize; i++) {
    Serial.println(DistanceWidthTimingInfo[i]);
  }
}

void loop() {
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    // Firebase.RTDB.setInt(&fbdo, F("/test/int"), decimalValue);

    // ส่ง sendDistanceWidthTiming ไปใน database
    FirebaseJsonArray sendDistanceWidthTiming;
    for (int i = 0; i < arraySize; i++) {
      sendDistanceWidthTiming.add(DistanceWidthTimingInfo[i]);
    }
    Firebase.RTDB.setArray(&fbdo, "/test/array", &sendDistanceWidthTiming);

    // ดึง IRRawData ที่เป็น command จาก database
    Firebase.RTDB.getInt(&fbdo, F("/IR-Signal-Choose/IRRawData"), &decimalValue);

    // ดึงจำนวน bits จาก database
    Firebase.RTDB.getInt(&fbdo, F("/IR-Signal-Choose/NumberOfBits"), &sNumberOfBits);

    // ดึงข้อมูลสัญญาณ array จาก database เก็บในตัวแปร DistanceWidthTimingInfo
    if (Firebase.RTDB.getArray(&fbdo, "/IR-Signal-Choose/DistanceWidthTimingInfo")) {
      jsonStr = String("{\"DistanceWidthTimingInfo\":") + fbdo.to<FirebaseJsonArray>().raw() + "}";
      // สร้าง object DynamicJsonDocument
      DynamicJsonDocument doc(1024);
      // แปลงสตริง JSON เข้าสู่ DynamicJsonDocument
      DeserializationError error = deserializeJson(doc, jsonStr);
      // ตรวจสอบว่ามีข้อผิดพลาดหรือไม่
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      // ดึงข้อมูล DistanceWidthTimingInfo
      JsonArray array = doc["DistanceWidthTimingInfo"];
      arraySize = array.size();
      // สร้างอาร์เรย์ใน C++ สำหรับเก็บข้อมูล
      DistanceWidthTimingInfo[arraySize];
      // คัดลอกข้อมูลจาก JsonArray เข้าสู่อาร์เรย์ C++
      for (size_t i = 0; i < arraySize; i++) {
        DistanceWidthTimingInfo[i] = array[i];
      }
      // แสดงผลข้อมูลในอาร์เรย์
      for (size_t i = 0; i < arraySize; i++) {
        Serial.println(DistanceWidthTimingInfo[i]);
      }
    } else {
      Serial.printf("%s\n", fbdo.errorReason().c_str());
    }
  }

  // แสดงผลข้อมูลในอาร์เรย์
  for (size_t i = 0; i < arraySize; i++) {
    Serial.println(DistanceWidthTimingInfo[i]);
  }
  Serial.println(decimalValue, HEX);
  Serial.println(sNumberOfBits);
  Serial.println(" ");

  delay(1000);
}




// For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse.ino
// FirebaseJson json;
// for (int i = 0; i < arraySize; i++) {
//   if (i == 0) {
//     json.set(String(i), DistanceWidthTimingInfo[i]);
//     Firebase.RTDB.set(&fbdo, F("/test/DistanceWidthTimingInfo"), &json);
//   } else {
//     json.add(String(i), DistanceWidthTimingInfo[i]);
//     Firebase.RTDB.updateNode(&fbdo, F("/test/DistanceWidthTimingInfo"), &json);
//   }
//   delay(5000);
// }
