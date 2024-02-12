#include <Arduino.h>

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

void setup() {
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
    /*
         * Button is not pressed and data available -> store received data and resume
         * DistanceWidthTimingInfo and sNumberOfBits should be constant for all keys of the same IR remote / protocol
         */
    IrReceiver.printIRResultShort(&Serial);
    if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
      IrReceiver.printIRSendUsage(&Serial);

      if (memcmp(&sDistanceWidthTimingInfo, &IrReceiver.decodedIRData.DistanceWidthTimingInfo, sizeof(sDistanceWidthTimingInfo)) != 0) {
        Serial.print(F("Store new timing info data="));
        IrReceiver.printDistanceWidthTimingInfo(&Serial, &IrReceiver.decodedIRData.DistanceWidthTimingInfo);
        Serial.println();
        sDistanceWidthTimingInfo = IrReceiver.decodedIRData.DistanceWidthTimingInfo;  // copy content here
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
        Serial.println(IrReceiver.decodedIRData.decodedRawDataArray[0], HEX);
      }
    }
    IrReceiver.resume();  // resume receiver
  }

  sSendButtonWasActive = tSendButtonIsActive;
  delay(100);
}
