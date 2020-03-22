#include "variables.h"

#if defined(AIR_MODULE)

#elif defined(GND_MODULE)

#include "display.h"
#include "tactile.h"

volatile uint16_t rcValues[RC_CHANNELS_COUNT] = {0};
volatile bool rcUpdated = false;

#ifdef DISPLAY_ENABLED
Display disp;
#endif

Tactile button0(BUTTON_0_PIN);
Tactile button1(BUTTON_1_PIN);
Tactile button2(BUTTON_2_PIN);

uint8_t mode = MODE_IDLE;
uint8_t lastMode = -1;
bool modeSelector = false;
bool lastModeSelector = false;
int selectedMode = mode;
int lastSelectedMode = -1;
bool freqSelector = false;

#endif

void setup() {

  #if defined(DEBUG_MODE)
  Serial.begin(57600);
  #endif
  
  #ifdef LINK_ENABLED
  linkInit();
  #endif

  #if defined(AIR_MODULE) ///////////////////////////////////////////////////////////////

  #ifdef LINK_ENABLED
  SERIAL_PORT.begin(57600);
  linkBegin();
  #endif

  #elif defined(GND_MODULE) //////////////////////////////////////////////////////////////

  button0.start();
  button1.start();
  button2.start();

  #ifdef DISPLAY_ENABLED
  disp.init();
  disp.loop();
  #endif
  
  #ifdef PPM_ENABLED
  initPpm();
  #endif
  
  #ifdef JOYSTICK_ENABLED
  joystickInit();
  #endif
  
  #endif ////////////////////////////////////////////////////////////////////////////////

}

void loop() {

  #if defined(AIR_MODULE) /////////////////////////////////////////////////////////////

  #ifdef LINK_ENABLED
  linkLoop();
  #endif

  #elif defined(GND_MODULE) //////////////////////////////////////////////////////////////

  button0.loop();
  button1.loop();
  button2.loop(); 

  if (button0.getState() == TACTILE_STATE_SHORT_PRESS) {
    if (modeSelector) {
      selectedMode --;
      if (selectedMode < MODE_IDLE) {
        selectedMode = MODE_MAX;
      }
      disp.forceUpdate = true;
    }
    else if (freqSelector) {
      #ifdef LINK_ENABLED
      freqUp();
      #endif
    }
  }
  if (button1.getState() == TACTILE_STATE_SHORT_PRESS) {
    if (modeSelector) {
      selectedMode ++;
      if (selectedMode > MODE_MAX) {
        selectedMode = MODE_IDLE;
      }
      disp.forceUpdate = true;
    }
    else if (freqSelector) {
      #ifdef LINK_ENABLED
      freqDown();
      #endif
    }
  }
  if (button2.getState() == TACTILE_STATE_SHORT_PRESS) {
    if (modeSelector) {
      modeSelector = false;
      mode = selectedMode;
      disp.forceUpdate = true;
    }
    else if (freqSelector) {
      freqSelector = false;
    }
    else if (mode == MODE_TELEM || mode == MODE_RC) {
      freqSelector = true;
    }
  }
  else if (button2.getState() == TACTILE_STATE_LONG_PRESS) {
    if (!modeSelector) {
      lastSelectedMode = -1;
      selectedMode = mode;
      modeSelector = true;
      disp.forceUpdate = true;
    }
  }

  if (lastModeSelector != modeSelector) {
    lastModeSelector = modeSelector;
    disp.forceUpdate = true;
  }
  
  if (lastMode != mode) {
    if (lastMode == MODE_SIM) {
      #ifdef JOYSTICK_ENABLED
      joystickEnd();
      #endif
    }
    if (lastMode == MODE_TELEM || lastMode == MODE_RC) {
      if (mode != MODE_TELEM && mode != MODE_RC) {
        #ifdef LINK_ENABLED
        linkEnd();
        SERIAL_PORT.end();
        #endif
      }
    }
    else {
      if (mode == MODE_TELEM || mode == MODE_RC) {
        #ifdef LINK_ENABLED
        SERIAL_PORT.begin(57600);
        linkBegin();
        #endif
      }
    }
    if (mode == MODE_SIM) {
      #ifdef JOYSTICK_ENABLED
      joystickBegin();
      #endif
    }

    disp.forceUpdate = true;
    lastMode = mode;
  }
  
  if (rcUpdated) {
    if (mode == MODE_SIM) {
      #ifdef JOYSTICK_ENABLED
      joystickSend();
      #endif
    }
    rcUpdated = false;
  }
  
  if (mode == MODE_TELEM || mode == MODE_RC) {
    #ifdef LINK_ENABLED
    linkLoop();
    #endif
  }

  #ifdef DISPLAY_ENABLED
  disp.loop();
  #endif

  #endif ////////////////////////////////////////////////////////////////////////
}
