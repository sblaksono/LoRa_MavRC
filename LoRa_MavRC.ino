#include "variables.h"

#if defined(AIR_MODULE)

#elif defined(GND_MODULE)

#include "display.h"
#include "tactile.h"

volatile uint16_t rcValues[RC_CHANNELS_COUNT] = {0};

#ifdef DISPLAY_ENABLED
Display disp;
#endif

Tactile button0(BUTTON_0_PIN);
Tactile button1(BUTTON_1_PIN);
Tactile button2(BUTTON_2_PIN);

uint8_t mode = MODE_IDLE;
uint8_t lastMode = -1;

#endif

void setup() {
  SERIAL_PORT.begin(57600);

  #if defined(DEBUG_MODE)
  Serial.begin(57600);
  #endif
  
  linkInit();

  #if defined(AIR_MODULE)

  linkBegin();

  #elif defined(GND_MODULE)

  #ifdef DISPLAY_ENABLED
  disp.init();
  disp.loop();
  #endif
  
  #ifdef PPM_ENABLED
  initPpm();
  #endif
  
  #ifdef JOYSTICK_ENABLED
  button0.start();
  button1.start();
  button2.start();
  joystickInit();
  #endif
  
  #endif

}

void loop() {

  #if defined(AIR_MODULE)

  linkLoop();

  #elif defined(GND_MODULE)

  button0.loop();
  button1.loop();
  button2.loop(); 
  if (button0.getState() == TACTILE_STATE_LONG_PRESS) {
    mode = MODE_TELEM;
  }
  if (button1.getState() == TACTILE_STATE_LONG_PRESS) {
    mode = MODE_RC;
  }
  if (button2.getState() == TACTILE_STATE_LONG_PRESS) {
    mode = MODE_SIM;
  }

  #ifdef DISPLAY_ENABLED
  disp.loop();
  #endif
  
  #ifdef JOYSTICK_ENABLED

  if (lastMode != mode) {
    if (mode == MODE_SIM) {
      SERIAL_PORT.end();
      joystickBegin();
    }
    else if (mode == MODE_TELEM) {
      linkBegin();
    }

    if (lastMode == MODE_SIM) {
      joystickEnd();
      SERIAL_PORT.begin(57600);
    }
    else if (lastMode == MODE_TELEM) {
      linkEnd();
    }
  }
  if (mode == MODE_SIM) {
    joystickSend();
  }
  else if (mode == MODE_TELEM) {
    linkLoop();
  }
  
  #endif
  
  lastMode = mode;

  #endif
}
