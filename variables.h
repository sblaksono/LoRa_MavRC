#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>

// -- Select module and config --------------------------------------------------------------------

//#define LORA32U4_AIR_MODULE     // use Lora32U4 as "standard"
//#define CUSTOM328P_AIR_MODULE
//#define LORA32U4_GND_MODULE 
#define CUSTOM32U4_GND_MODULE
//#define PROMICRO_SIM_MODULE
//#define OTHER32U4_SIM_MODULE

//#define DEBUG_MODE

#define MAV_SYSTEM_ID   249

// -- Hardware configuration ----------------------------------------------------------------------

#if defined(LORA32U4_AIR_MODULE) || defined(CUSTOM328P_AIR_MODULE)

  #define AIR_MODULE 
  #define RADIO_ENABLED

#elif defined(LORA32U4_GND_MODULE) || defined(CUSTOM32U4_GND_MODULE)

  #define GND_MODULE
  #define DISPLAY_ENABLED
  #define RADIO_ENABLED
  #define SEND_HEARTBEAT

  #if defined(__AVR_ATmega32U4__)
    #define JOYSTICK_ENABLED
    #define PPM_ENABLED
  #endif

#elif defined(PROMICRO_SIM_MODULE) || defined(OTHER32U4_SIM_MODULE)

  #define GND_MODULE
  #define PASSTHRU_ENABLE

  #if defined(__AVR_ATmega32U4__)
    #define JOYSTICK_ENABLED
    #define PPM_ENABLED
  #endif

#endif

#ifdef PPM_ENABLED
  #define RC_ENABLED
#endif

#if defined(CUSTOM32U4_GND_MODULE)

  #ifndef __AVR_ATmega32U4__
    #error Wrong target board selection.
  #endif

  #define LORA_SS_PIN       8
  #define LORA_RST_PIN      5   
  #define LORA_INT_PIN      7   

  #define PPM_CAPTURE_ICP1

  #define BUTTON_0_PIN      20 // A2
  #define BUTTON_1_PIN      19 // A1
  #define BUTTON_2_PIN      18 // A0
  #define BUTTON_3_PIN      21 // A3

#elif defined(CUSTOM328P_AIR_MODULE)

  #ifndef __AVR_ATmega328P__
    #error Wrong target board selection.
  #endif

  #define LORA_SS_PIN       10
  #define LORA_RST_PIN      4   
  #define LORA_INT_PIN      3   

  #define RX_LED_PIN        7
  #define RX_BUTTON_PIN     2
  #define LORA_RSSI_PIN     9

#elif defined(PROMICRO_SIM_MODULE)

  #ifndef __AVR_ATmega32U4__
    #error Wrong target board selection.
  #endif

  #define PPM_CAPTURE_ICP1

#elif defined(OTHER32U4_SIM_MODULE)

  #ifndef __AVR_ATmega32U4__
    #error Wrong target board selection.
  #endif

  #define PPM_CAPTURE_ICP3

#else

  #ifndef __AVR_ATmega32U4__
    #error Wrong target board selection.
  #endif
  
  #ifdef RADIO_ENABLED
    #define LORA_SS_PIN       8
    #define LORA_RST_PIN      4   
    #define LORA_INT_PIN      7
  #endif
  
  #ifdef AIR_MODULE
    #define RX_LED_PIN        3
    #define RX_BUTTON_PIN     2
    #define LORA_RSSI_PIN     9  
  #endif
  
  #ifdef GND_MODULE
    #define BUTTON_0_PIN      20 // A2
    #define BUTTON_1_PIN      19 // A1
    #define BUTTON_2_PIN      18 // A0
    #define BUTTON_3_PIN      21 // A3
  #endif

  #ifdef PPM_ENABLED
    #define PPM_CAPTURE_ICP3
  #endif

#endif


#if defined(__AVR_ATmega32U4__)

  #if defined(AIR_MODULE)         // air module
    #define SERIAL_PORT           Serial1
    #define DEBUG_PORT            Serial
  #endif
  
  #if defined(GND_MODULE)         // gnd module
    #ifdef DEBUG_MODE
      #define DEBUG_PORT          Serial
    #else
      #define SERIAL_PORT         Serial
    #endif
    
    #ifdef PASSTHRU_ENABLE
      #define SERIAL_PASSTHRU     Serial1
    #endif

    #ifdef PPM_ENABLED
      #if defined(PPM_CAPTURE_ICP1)
        #define PPM_CAPTURE_PIN         4
      #elif defined(PPM_CAPTURE_ICP3)
        #define PPM_CAPTURE_PIN         13
      #endif
    #endif        
  #endif
  
#elif defined(__AVR_ATmega328P__)

  #ifdef DEBUG_MODE
    #define DEBUG_PORT            Serial
  #else
    #define SERIAL_PORT           Serial
  #endif    

#endif


// -- Air module related ---------------------------------------------------------------------------

#if defined(AIR_MODULE) ////////////////////////////////////////////////////////////////////////////

#define RX_LED_INIT     pinMode(RX_LED_PIN, OUTPUT)
#define RX_LED_ON       digitalWrite(RX_LED_PIN, HIGH)
#define RX_LED_OFF      digitalWrite(RX_LED_PIN, LOW)


// -- GND module related -----------------------------------------------------------------------

#elif defined(GND_MODULE) /////////////////////////////////////////////////////////////////////////

// -- Modes ------------
#define MODE_IDLE   0
#define MODE_TELEM  1
#define MODE_RC     2
#define MODE_SIM    3
#define MODE_MAX    MODE_SIM

extern uint8_t mode;
extern bool modeSelector;
extern int selectedMode;

#endif ////////////////////////////////////////////////////////////////////////////////////////////


#ifdef RC_ENABLED

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

#endif

#ifdef RADIO_ENABLED

// -- link related --------------------------------------------

extern bool freqSelector;
extern uint16_t rcvd;
extern uint16_t sent;
extern int txRate;
extern int rssi;
extern int rssiPct;
extern long freq;
extern long minFreq;
extern long maxFreq;
extern long bandwidth;
extern int snr;

#endif

#endif
