#include "variables.h"

#ifdef RC_ENABLED

volatile uint16_t rcValues[RC_CHANNELS_COUNT] = {0};
volatile bool rcUpdated = false;

#endif

#if defined(GND_MODULE)

#ifdef DISPLAY_ENABLED

#include "display.h"
#include "tactile.h"
Display disp;

Tactile button0(BUTTON_0_PIN);
Tactile button1(BUTTON_1_PIN);
Tactile button2(BUTTON_2_PIN);

#endif

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
  DEBUG_PORT.begin(57600);
  #endif
  
  #ifdef RADIO_ENABLED
  radioInit();
  #endif

  #ifdef SERIAL_PORT
  SERIAL_PORT.begin(57600);
  #endif
  
  #ifdef SERIAL_PASSTHRU
  SERIAL_PASSTHRU.begin(57600);
  #endif

  #if defined(AIR_MODULE) ///////////////////////////////////////////////////////////////

  #ifdef RADIO_ENABLED
  radioBegin();
  #endif

  #elif defined(GND_MODULE) //////////////////////////////////////////////////////////////

  #ifdef DISPLAY_ENABLED
  button0.start();
  button1.start();
  button2.start();
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

  linkLoop();

  #if defined(AIR_MODULE) /////////////////////////////////////////////////////////////


  #elif defined(GND_MODULE) //////////////////////////////////////////////////////////////

  #ifdef DISPLAY_ENABLED
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
    #ifdef RADIO_ENABLED
    else if (freqSelector) {
      freqUp();
    }
    #endif
  }
  if (button1.getState() == TACTILE_STATE_SHORT_PRESS) {
    if (modeSelector) {
      selectedMode ++;
      if (selectedMode > MODE_MAX) {
        selectedMode = MODE_IDLE;
      }
      disp.forceUpdate = true;
    }
    #ifdef RADIO_ENABLED
    else if (freqSelector) {
      freqDown();
    }
    #endif
  }
  if (button2.getState() == TACTILE_STATE_SHORT_PRESS) {
    if (modeSelector) {
      modeSelector = false;
      mode = selectedMode;
      disp.forceUpdate = true;
    }
    #ifdef RADIO_ENABLED
    else if (freqSelector) {
      freqSelector = false;
    }
    else if (mode == MODE_TELEM || mode == MODE_RC) {
      freqSelector = true;
    }
    #endif
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
  #endif
  
  if (lastMode != mode) {
    if (lastMode == MODE_TELEM || lastMode == MODE_RC) {
      if (mode != MODE_TELEM && mode != MODE_RC) {
        #ifdef RADIO_ENABLED
        radioEnd();
        #endif
      }
    }
    else {
      if (mode == MODE_TELEM || mode == MODE_RC) {
        #ifdef RADIO_ENABLED
        radioBegin();
        #endif
      }
    }

    #ifdef JOYSTICK_ENABLED
    if (lastMode == MODE_SIM) {
      joystickEnd();            
    }
    if (mode == MODE_SIM) {
      joystickBegin();      
    }
    #endif

    #ifdef DISPLAY_ENABLED
    disp.forceUpdate = true;
    #endif
    lastMode = mode;
  }
  
  #ifdef DISPLAY_ENABLED
  disp.loop();
  #endif

  #ifdef RC_ENABLED
  if (rcUpdated) {

    #ifdef JOYSTICK_ENABLED
    joystickSend();
    #endif

    rcUpdated = false;
  }
  #endif
  
  #endif ////////////////////////////////////////////////////////////////////////
}
