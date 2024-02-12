#include <ESP32Firebase.h>

#define _SSID "ONE_2.4GHz"          // Your WiFi SSID
#define _PASSWORD "91919191"      // Your WiFi Password
#define REFERENCE_URL "https://ir-signal-default-rtdb.asia-southeast1.firebasedatabase.app/"  // Your Firebase project reference url

Firebase firebase(REFERENCE_URL);

void setup() {
  Serial.begin(115200);
  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  // digitalWrite(LED_BUILTIN, HIGH);

//================================================================//
//================================================================//

  unsigned long long hexValue = 0x6000000521C; // กำหนดค่าในฐาน 16
  unsigned long long decimalValue = hexValue; // จัดเก็บค่าที่ได้ในตัวแปรใหม่
  // Examples of setting String, integer and float values.
  firebase.setString("Example/setString", "It's Working");
  firebase.setInt("Example/setInt", decimalValue);
  
}

void loop() {
  // Nothing
}
