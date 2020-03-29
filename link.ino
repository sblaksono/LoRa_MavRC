#include "variables.h"

#include "src/mavlink/mavlink.h"

#ifdef SEND_HEARTBEAT
uint32_t lastHeartbeat = 0;
#endif

#ifdef RADIO_ENABLED

#include "src/LoRa/LoRa.h"

#define LORA_MIN_FREQUENCY      915000
#define LORA_MAX_FREQUENCY      917000
#define LORA_FREQUENCY          915000
#define LORA_BANDWIDTH          250
#define LORA_SPREADING_FACTOR   7
#define LORA_CODING_RATE        5
#define LORA_SYNC_WORD          0x77

#define MAX_LORA_PACKET_SIZE    255

bool transmitting = false;
uint32_t lastRSSIUpdate = 0;
#ifdef DEBUG_MODE
uint32_t lastRadioDebug = 0;
#endif
uint16_t rcvd = 0;
uint16_t sent = 0;
int rssi = 0;
int rssiPct = 0;
uint32_t txBegin = 0;
long txBytes = 0;
int txRate = 0;
long freq = LORA_FREQUENCY;
long minFreq = LORA_MIN_FREQUENCY;
long maxFreq = LORA_MAX_FREQUENCY;
long bandwidth = LORA_BANDWIDTH;
int snr = 0;
uint32_t beginDwell = 0;

void radioInit() {
  #ifdef LORA_RSSI_PIN
  pinMode(LORA_RSSI_PIN, OUTPUT);
  #endif
  LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_INT_PIN);  
  LoRa.onTxDone(onTxDone);
  LoRa.onReceive(onReceive);
  
  #ifdef AIR_MODULE
  RX_LED_INIT;
  RX_LED_OFF;
  #endif
}

void radioBegin() {
  LoRa.begin(freq * 1000);
  LoRa.setSignalBandwidth(bandwidth * 1000);
  LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR); 
  LoRa.setCodingRate4(LORA_CODING_RATE); 
  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.receive();
  beginDwell = millis();

  #ifdef AIR_MODULE
  RX_LED_ON;
  #endif
}

void radioEnd() {
  LoRa.end();
  beginDwell = 0;

  #ifdef AIR_MODULE
  RX_LED_OFF;
  #endif
}

void freqUp() {
  if (freq + bandwidth <= maxFreq) {
    radioEnd();
    freq = freq + bandwidth;
    radioBegin();
  }
}

void freqDown() {
  if (freq - bandwidth >= minFreq) {
    radioEnd();
    freq = freq - bandwidth;
    radioBegin();
  }
}

void onTxDone() {
  transmitting = false;
  txRate = txBytes * 8000 / (millis() - txBegin);
  LoRa.receive();

  #ifdef AIR_MODULE
  RX_LED_ON;
  #endif
}

void radioSend(const uint8_t *buf, uint16_t len) {
  if (len <= MAX_LORA_PACKET_SIZE) {
    while (LoRa.beginPacket() == 0) { }
    txBytes = len;
    txBegin = millis();
    transmitting = true;
    LoRa.write(buf, len);
    LoRa.endPacket(true);
  }
  
  #ifdef AIR_MODULE
  RX_LED_OFF;
  #endif  
}

void linkSend() {
  mavlink_message_t msg;
  mavlink_status_t status;
  char buf[MAVLINK_MAX_PACKET_LEN];

  #ifdef SERIAL_PORT
  while(SERIAL_PORT.available() > 0) {
    uint8_t c = SERIAL_PORT.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      #if defined(DEBUG_MODE)
      debugMavlink(true, &msg);
      #endif

      uint16_t len = mavlink_msg_to_send_buffer((uint8_t*)buf, &msg);
      radioSend(buf, len);
      sent++;
      break;
    }
  }
  #endif
}

void onReceive(int packetSize) {  
  mavlink_message_t msg;
  mavlink_status_t status;
  char buf[MAVLINK_MAX_PACKET_LEN];

  #ifdef AIR_MODULE
  RX_LED_OFF;
  #endif

  for (int i = 0; i < packetSize; i++) {
    uint8_t c = LoRa.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      #if defined(DEBUG_MODE)
      debugMavlink(false, &msg);
      #endif

      uint16_t len = mavlink_msg_to_send_buffer((uint8_t*)buf, &msg);
      #ifdef SERIAL_PORT
      SERIAL_PORT.write(buf, len);
      #endif
      rcvd++;
      break;
    }
  }

  #ifdef AIR_MODULE
  RX_LED_ON;
  #endif
}

int rssiPercentage() {
  float x = -(rssi + 50);
  return constrain(100 - ((exp(x / 100) - 1) * 58), 0, 100);    
}

#endif


#ifdef SERIAL_PASSTHRU
void serialPassthru() {
  mavlink_message_t msg;
  mavlink_status_t status;
  char buf[MAVLINK_MAX_PACKET_LEN];

  #ifdef SERIAL_PORT
  while(SERIAL_PORT.available() > 0) {
    uint8_t c = SERIAL_PORT.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      #if defined(DEBUG_MODE)
      debugMavlink(true, &msg);
      #endif

      uint16_t len = mavlink_msg_to_send_buffer((uint8_t*)buf, &msg);
      SERIAL_PASSTHRU.write(buf, len);
      break;
    }
  }
  #endif

  while(SERIAL_PASSTHRU.available() > 0) {
    uint8_t c = SERIAL_PASSTHRU.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      #if defined(DEBUG_MODE)
      debugMavlink(false, &msg);
      #endif

      uint16_t len = mavlink_msg_to_send_buffer((uint8_t*)buf, &msg);
      #ifdef SERIAL_PORT
      SERIAL_PORT.write(buffer, size);
      #endif
      break;
    }
  }

}
#endif

void linkLoop() {

  #ifdef SERIAL_PASSTHRU
  serialPassthru();
  #endif

  #ifdef RADIO_ENABLED

  if (beginDwell == 0) {
    
  }
  else {
    #ifdef SEND_HEARTBEAT
    if (!transmitting) {
      if (millis() - lastHeartbeat > 1000) {
        uint8_t buf[MAVLINK_MAX_PACKET_LEN];
        uint16_t len = heartbeat(buf);
        radioSend(buf, len);        
        sent++;
        lastHeartbeat = millis(); 
      }
    }  
    #endif
    if (!transmitting) {
      linkSend();
    }
    if (millis() - lastRSSIUpdate > 200) {
      snr = LoRa.packetSnr() * 100;
      rssi = LoRa.packetRssi();
      rssiPct = rssiPercentage();
      #ifdef LORA_RSSI_PIN
      analogWrite(LORA_RSSI_PIN, (rssiPct * 255) / 100); // 0 - 255    
      #endif
      lastRSSIUpdate = millis();
    }
    #ifdef DEBUG_MODE
    if (millis() - lastRadioDebug > 1000) {
      char buf1[200];
      snprintf(buf1, 200, "FREQ: %ld, BW: %ld, RSSI: %d %d%s, SNR: %d", freq, bandwidth, rssi, rssiPct, "%", snr);
      DEBUG_PORT.println(buf1);
      lastRadioDebug = millis();
    }
    #endif
  }

  #endif
}
