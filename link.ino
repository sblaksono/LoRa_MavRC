#include "variables.h"

#ifdef LINK_ENABLED

#include "src/LoRa/LoRa.h"

#ifdef USE_MAVLINK
  #include "src/mavlink/mavlink.h"
#endif

#define LORA_MIN_FREQUENCY      915000
#define LORA_MAX_FREQUENCY      917000
#define LORA_FREQUENCY          915000
#define LORA_BANDWIDTH          250
#define LORA_SPREADING_FACTOR   7
#define LORA_CODING_RATE        5
#define LORA_SYNC_WORD          0x77

#define MAX_BUF_SIZE            255

bool transmitting = false;
bool incoming = false;
uint32_t lastRSSIUpdate = 0;
uint16_t rcvd = 0;
uint16_t sent = 0;
int rssi = 0;
int rssiPct = 0;
long freq = LORA_FREQUENCY;
long minFreq = LORA_MIN_FREQUENCY;
long maxFreq = LORA_MAX_FREQUENCY;
long bandwidth = LORA_BANDWIDTH;
int snr = 0;
uint32_t beginDwell = 0;

void linkInit() {
  pinMode(LORA_RSSI_PIN, OUTPUT);
  LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_INT_PIN);  
  LoRa.onTxDone(onTxDone);
  LoRa.onReceive(onReceive);
  
  #if defined(AIR_MODULE)
  RX_LED_INIT;
  RX_LED_OFF;
  #endif
}

void linkBegin() {
  LoRa.begin(freq * 1000);
  LoRa.setSignalBandwidth(bandwidth * 1000);
  LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR); 
  LoRa.setCodingRate4(LORA_CODING_RATE); 
  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.receive();
  beginDwell = millis();

  #if defined(AIR_MODULE)
  RX_LED_ON;
  #endif
}

void linkEnd() {
  LoRa.end();
  beginDwell = 0;

  #if defined(AIR_MODULE)
  RX_LED_OFF;
  #endif
}

void freqUp() {
  if (freq + bandwidth <= maxFreq) {
    linkEnd();
    freq = freq + bandwidth;
    linkBegin();
  }
}

void freqDown() {
  if (freq - bandwidth >= minFreq) {
    linkEnd();
    freq = freq - bandwidth;
    linkBegin();
  }
}

void onTxDone() {
  transmitting = false;
  LoRa.receive();

  #if defined(AIR_MODULE)
  RX_LED_ON;
  #endif
}

void loraSend(const uint8_t *buffer, size_t size) {
  while (LoRa.beginPacket() == 0) { }
  LoRa.write(buffer, size);
  LoRa.endPacket(true);
  transmitting = true;
  sent++;

  #if defined(AIR_MODULE)
  RX_LED_OFF;
  #endif  
}

void linkSend() {
  mavlink_message_t msg;
  mavlink_status_t status;
  char buf[MAX_BUF_SIZE];
  int n = 0;
  
  while(SERIAL_PORT.available() > 0) {
    uint8_t c = SERIAL_PORT.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      if (n < MAX_BUF_SIZE) {
        unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buf, &msg);

        #if defined(DEBUG_TX)
        char buf1[200];
        snprintf(buf1, 200, "SEND ID: %d, SEQ: %d, COMP: %d, SYS: %d, LEN: %d", msg.msgid, msg.seq, msg.compid, msg.sysid, len);
        Serial.println(buf1);
        #endif
            
        loraSend(buf, len);
      }
      n = 0;
    }
    else {
      n ++;
    }
  }
}

void writeSerial(const uint8_t *buffer, size_t size) {
  SERIAL_PORT.write(buffer, size);
  rcvd++;
}

void onReceive(int packetSize) {
  mavlink_message_t msg;
  mavlink_status_t status;
  char buf[MAX_BUF_SIZE];

  #if defined(DEBUG_RX)
  char buf1[200];
  snprintf(buf1, 200, "LORA PACKET: %d", packetSize);
  Serial.println(buf1);
  #endif

  #if defined(AIR_MODULE)
  RX_LED_OFF;
  #endif
  
  for (int i = 0; i < packetSize; i++) {
    uint8_t c = LoRa.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buf, &msg);

      #if defined(DEBUG_RX)
      char buf1[200];
      snprintf(buf1, 200, "RECV ID: %d, SEQ: %d, COMP: %d, SYS: %d, LEN: %d", msg.msgid, msg.seq, msg.compid, msg.sysid, len);
      Serial.println(buf1);
      #endif
            
      if (len < MAX_BUF_SIZE) {
        writeSerial(buf, len);
      }
      rcvd++;
      break;
    }
  }

  #if defined(AIR_MODULE)
  RX_LED_ON;
  #endif
}

int rssiPercentage() {
  float x = -(rssi + 40);
  return constrain(100 - ((exp(x / 100) - 1) * 58), 0, 100);    
}

void linkLoop() {
  if (beginDwell == 0) {
    
  }
  else {
    if (!transmitting) {
      linkSend();
    }
    if (millis() - lastRSSIUpdate > 500) {
      rssi = LoRa.packetRssi();
      rssiPct = rssiPercentage();
      analogWrite(LORA_RSSI_PIN, (rssiPct * 255) / 100); // 0 - 255    
      lastRSSIUpdate = millis();

      #if defined(DEBUG_RX)
      char buf1[200];
      snprintf(buf1, 200, "RSSI: %d %d%s", rssi, rssiPct, "%");
      Serial.println(buf1);
      #endif
    }
  }
}

#endif
