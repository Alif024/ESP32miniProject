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

// =================================================================================

#include "PinDefinitionsAndMore.h"
#if !defined(IR_SEND_PIN)
#define IR_SEND_PIN 3
#endif

#define DECODE_DISTANCE_WIDTH

#if !defined(RAW_BUFFER_LENGTH)
#if RAMEND <= 0x4FF || RAMSIZE < 0x4FF
#define RAW_BUFFER_LENGTH 120
#elif RAMEND <= 0xAFF || RAMSIZE < 0xAFF
#define RAW_BUFFER_LENGTH 400
#else
#define RAW_BUFFER_LENGTH 750
#endif
#endif

#include <IRremote.hpp>

#define SEND_BUTTON_PIN APPLICATION_PIN

#define DELAY_BETWEEN_REPEATS_MILLIS 70

IRRawDataType sDecodedRawDataArray[RAW_DATA_ARRAY_SIZE] = { 0x7B34ED12 };
DistanceWidthTimingInfoStruct sDistanceWidthTimingInfo = { 9000, 4500, 560, 1690, 560, 560 };
uint8_t sNumberOfBits = 32;

bool sSendButtonWasActive;




// ฟังก์ชันสำหรับแสดงค่าใน sDistanceWidthTimingInfo
void printDistanceWidthTimingInfo() {
  Serial.print("HeaderMarkMicros: ");
  Serial.println(sDistanceWidthTimingInfo.HeaderMarkMicros);
  Serial.print("HeaderSpaceMicros: ");
  Serial.println(sDistanceWidthTimingInfo.HeaderSpaceMicros);
  Serial.print("OneMarkMicros: ");
  Serial.println(sDistanceWidthTimingInfo.OneMarkMicros);
  Serial.print("OneSpaceMicros: ");
  Serial.println(sDistanceWidthTimingInfo.OneSpaceMicros);
  Serial.print("ZeroMarkMicros: ");
  Serial.println(sDistanceWidthTimingInfo.ZeroMarkMicros);
  Serial.print("ZeroSpaceMicros: ");
  Serial.println(sDistanceWidthTimingInfo.ZeroSpaceMicros);
}

// ฟังก์ชันสำหรับแสดงค่าใน sDistanceWidthTimingInfo
void sendDistanceWidthTimingInfoFB(String path) {
  FirebaseJsonArray sendDistanceWidthTiming;
  sendDistanceWidthTiming.add(sDistanceWidthTimingInfo.HeaderMarkMicros);
  sendDistanceWidthTiming.add(sDistanceWidthTimingInfo.HeaderSpaceMicros);
  sendDistanceWidthTiming.add(sDistanceWidthTimingInfo.OneMarkMicros);
  sendDistanceWidthTiming.add(sDistanceWidthTimingInfo.OneSpaceMicros);
  sendDistanceWidthTiming.add(sDistanceWidthTimingInfo.ZeroMarkMicros);
  sendDistanceWidthTiming.add(sDistanceWidthTimingInfo.ZeroSpaceMicros);
  Firebase.RTDB.setArray(&fbdo, path.c_str(), &sendDistanceWidthTiming);
}

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

  // =======================================================================================

  pinMode(SEND_BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/ || defined(USBCON) /*STM32_stm32*/ || defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
  delay(4000);  // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println(F("Ready to receive pulse distance/width coded IR signals at pin " STR(IR_RECEIVE_PIN)));

  IrSender.begin();  // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
  Serial.print(F("Ready to send IR signals at pin " STR(IR_SEND_PIN) " on press of button at pin "));
  Serial.println(SEND_BUTTON_PIN);
}

void loop() {
  bool tSendButtonIsActive = (digitalRead(SEND_BUTTON_PIN) == LOW);

  if (tSendButtonIsActive) {
    if (!sSendButtonWasActive) {
      Serial.println(F("Stop receiving"));
      IrReceiver.stop();
    }
    Serial.print(F("Button pressed, now sending "));
    Serial.print(sNumberOfBits);
    Serial.print(F(" bits 0x"));
    Serial.print(sDecodedRawDataArray[0], HEX);
    Serial.print(F(" with sendPulseDistanceWidthFromArray timing="));
    IrReceiver.printDistanceWidthTimingInfo(&Serial, &sDistanceWidthTimingInfo);
    Serial.println();
    Serial.flush();  // To avoid disturbing the software PWM generation by serial output interrupts

    IrSender.sendPulseDistanceWidthFromArray(38, &sDistanceWidthTimingInfo, &sDecodedRawDataArray[0], sNumberOfBits,
#if defined(USE_MSB_DECODING_FOR_DISTANCE_DECODER)
                                             PROTOCOL_IS_MSB_FIRST
#else
                                             PROTOCOL_IS_LSB_FIRST
#endif
                                             ,
                                             100, 0);

    delay(DELAY_BETWEEN_REPEATS_MILLIS);  // Wait a bit between retransmissions

  } else if (sSendButtonWasActive) {
    /*
         * Button is just released -> activate receiving
         */
    // Restart receiver
    Serial.println(F("Button released -> start receiving"));
    IrReceiver.start();

  } else if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
      IrReceiver.printIRSendUsage(&Serial);

      if (memcmp(&sDistanceWidthTimingInfo, &IrReceiver.decodedIRData.DistanceWidthTimingInfo, sizeof(sDistanceWidthTimingInfo)) != 0) {
        Serial.print(F("Store new timing info data="));
        IrReceiver.printDistanceWidthTimingInfo(&Serial, &IrReceiver.decodedIRData.DistanceWidthTimingInfo);
        Serial.println();
        sDistanceWidthTimingInfo = IrReceiver.decodedIRData.DistanceWidthTimingInfo;  // copy content here
        // Serial.println(" ");
        // printDistanceWidthTimingInfo();
        // Serial.println(" ");
      } else {
        Serial.print(F("Timing did not change, so we can reuse already stored timing info."));
      }
      if (sNumberOfBits != IrReceiver.decodedIRData.numberOfBits) {
        Serial.print(F("Store new numberOfBits="));
        sNumberOfBits = IrReceiver.decodedIRData.numberOfBits;
        Serial.println(IrReceiver.decodedIRData.numberOfBits);
      }
      if (sDecodedRawDataArray[0] != IrReceiver.decodedIRData.decodedRawDataArray[0]) {
        *sDecodedRawDataArray = *IrReceiver.decodedIRData.decodedRawDataArray;  // copy content here
        Serial.print(F("Store new sDecodedRawDataArray[0]=0x"));
        Serial.println(sDecodedRawDataArray[0], HEX);
      }

      String rawDataString = String(sDecodedRawDataArray[0], HEX);
      rawDataString.toUpperCase();
      String path = "/test/IR-Signal/0x" + rawDataString;
      String pathIRRawData = path + "/IRRawData";
      String pathNumberOfBits = path + "/NumberOfBits";
      String pathDistanceWidthTimingInfo = path + "/DistanceWidthTimingInfo";
      if (Firebase.ready()) {
        Firebase.RTDB.setInt(&fbdo, pathIRRawData.c_str(), sDecodedRawDataArray[0]);
        Firebase.RTDB.setInt(&fbdo, pathNumberOfBits.c_str(), sNumberOfBits);
        sendDistanceWidthTimingInfoFB(pathDistanceWidthTimingInfo);
        delay(5000);
      }
    }
    IrReceiver.resume();  // resume receiver
  }

  sSendButtonWasActive = tSendButtonIsActive;
  delay(100);
  // if (Firebase.ready()) {
  //   // ใช้ getString() เพื่อรับค่าเป็นสตริง
  //   if (Firebase.RTDB.getString(&fbdo, "/IR-Signal-Choose/IRRawData")) {
  //     String valueStr = fbdo.stringData();
  //     // แปลงสตริงเป็น unsigned long long
  //     IRRawDataFB = strtoull(valueStr.c_str(), NULL, 10);
  //     Serial.print("Value: ");
  //     Serial.println(IRRawDataFB);
  //   } else {
  //     // แสดงข้อความผิดพลาดหากการอ่านข้อมูลล้มเหลว
  //     Serial.println("Failed to read data from Firebase");
  //     Serial.println(fbdo.errorReason());
  //   }

  //   // ดึง NumberOfBits จาก database
  //   if (Firebase.RTDB.getInt(&fbdo, F("/IR-Signal-Choose/NumberOfBits"), &NumberOfBitsFB)) {
  //     Serial.print("NumberOfBitsFB: ");
  //     Serial.println(NumberOfBitsFB);
  //   }

  //   // ดึงข้อมูลสัญญาณ array จาก database เก็บในตัวแปร arrTimingInfoFB
  //   if (Firebase.RTDB.getArray(&fbdo, "/IR-Signal-Choose/DistanceWidthTimingInfo")) {
  //     TimingInfoJsonStrFB = String("{\"DistanceWidthTimingInfo\":") + fbdo.to<FirebaseJsonArray>().raw() + "}";
  //     // สร้าง object DynamicJsonDocument
  //     DynamicJsonDocument doc(1024);
  //     // แปลงสตริง JSON เข้าสู่ DynamicJsonDocument
  //     DeserializationError error = deserializeJson(doc, TimingInfoJsonStrFB);
  //     // ตรวจสอบว่ามีข้อผิดพลาดหรือไม่
  //     if (error) {
  //       Serial.print(F("deserializeJson() failed: "));
  //       Serial.println(error.f_str());
  //       return;
  //     }
  //     // ดึงข้อมูล DistanceWidthTimingInfo
  //     JsonArray array = doc["DistanceWidthTimingInfo"];
  //     arraySizeFB = array.size();

  //     // คัดลอกข้อมูลจาก JsonArray เข้าสู่อาร์เรย์ C++
  //     for (size_t i = 0; i < arraySizeFB; i++) {
  //       arrTimingInfoFB[i] = array[i];
  //     }
  //     Serial.println(" ");
  //     // แสดงผลข้อมูลในอาร์เรย์
  //     // for (size_t i = 0; i < arraySizeFB; i++) {
  //     //   Serial.println(arrTimingInfoFB[i]);
  //     // }
  //   } else {
  //     Serial.printf("%s\n", fbdo.errorReason().c_str());
  //   }
  // }
}
