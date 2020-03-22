#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>

// -- Select one module type ----------------------------------------------------------------------

//#define AIR_MODULE  
#define GND_MODULE    

#define USE_MAVLINK

//#define DEBUG_MODE


// -- ATMega32U4 ----------------------------------------------------------------------------------

#if defined(ARDUINO_AVR_PROMICRO) || defined(ARDUINO_AVR_LEONARDO) || defined(ARDUINO_AVR_MICRO)

  #define LORA_SS_PIN     8
  #define LORA_RST_PIN    4   
  #define LORA_INT_PIN    7   
  #define LORA_RSSI_PIN   9

// -- ATMega328P -----------------------------------------------------------------------------------

#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO)

  #define LORA_SS_PIN     10
  #define LORA_RST_PIN    4   
  #define LORA_INT_PIN    3   
  #define LORA_RSSI_PIN   9

#endif


// -- Air module related ---------------------------------------------------------------------------

#if defined(AIR_MODULE) ////////////////////////////////////////////////////////////////////////////

#define LINK_ENABLED

// Lora + ATMega32U4, Lora32U4

#if defined(ARDUINO_AVR_PROMICRO) || defined(ARDUINO_AVR_LEONARDO) || defined(ARDUINO_AVR_MICRO)
  #define RX_LED_PIN      3
  #define RX_BUTTON_PIN    2
  #define SERIAL_PORT   Serial1

// Lora + ATMega328P

#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO)
  #define RX_LED_PIN      7
  #define RX_BUTTON_PIN    2
  #define SERIAL_PORT     Serial

#endif

#define RX_LED_INIT     pinMode(RX_LED_PIN, OUTPUT)
#define RX_LED_ON       digitalWrite(RX_LED_PIN, HIGH)
#define RX_LED_OFF      digitalWrite(RX_LED_PIN, LOW)


// -- Ground module related -----------------------------------------------------------------------

#elif defined(GND_MODULE) /////////////////////////////////////////////////////////////////////////

#define DISPLAY_ENABLED
#define SERIAL_PORT     Serial

#ifndef DEBUG_MODE
  #define LINK_ENABLED
#endif

// Button pins
#define BUTTON_0_PIN      12
#define BUTTON_1_PIN      11
#define BUTTON_2_PIN      10

// Joystick only on ATMega32U4
#if defined(ARDUINO_AVR_PROMICRO) || defined(ARDUINO_AVR_LEONARDO) || defined(ARDUINO_AVR_MICRO)
  #define JOYSTICK_ENABLED
#endif

// -- RC ------------------------------------------------------
#define RC_CHANNELS_COUNT 8
#define ROLL_CHANNEL      1
#define PITCH_CHANNEL     2
#define THROTTLE_CHANNEL  3
#define YAW_CHANNEL       4
#define RC_MIN_VALUE      1000
#define RC_MAX_VALUE      2000
#define RC_THRESHOLD      100

extern volatile uint16_t rcValues[RC_CHANNELS_COUNT];
extern volatile bool rcUpdated;

// PPM Input
#define PPM_ENABLED

// -- Modes -------------------------------------------------
#define MODE_IDLE   0
#define MODE_TELEM  1
#define MODE_RC     2
#define MODE_SIM    3
#define MODE_MAX    MODE_SIM

extern uint8_t mode;
extern bool modeSelector;
extern int selectedMode;

// -- link related --------------------------------------------

extern bool freqSelector;
extern uint16_t rcvd;
extern uint16_t sent;
extern int rssi;
extern int rssiPct;
extern long freq;
extern long minFreq;
extern long maxFreq;
extern long bandwidth;
extern int snr;

#endif ////////////////////////////////////////////////////////////////////////////////////////////

#endif
