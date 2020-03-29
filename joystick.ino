#include "variables.h"

#ifdef JOYSTICK_ENABLED

#include "src/ArduinoJoystickLibrary/Joystick.h"

#ifdef RC_ENABLED
#define JOYSTICK_CHANNELS_COUNT   RC_CHANNELS_COUNT - 4
#else
#define JOYSTICK_CHANNELS_COUNT   0
#endif

boolean joystickActive = false;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, JOYSTICK_CHANNELS_COUNT, 0,
  true, true, true, true, false, false,
  false, false, false, false, false);

void joystickInit() {  
  #ifdef RC_ENABLED
  Joystick.setXAxisRange(RC_MIN_VALUE, RC_MAX_VALUE);
  Joystick.setYAxisRange(RC_MIN_VALUE, RC_MAX_VALUE);
  Joystick.setZAxisRange(RC_MIN_VALUE, RC_MAX_VALUE);
  Joystick.setRxAxisRange(RC_MIN_VALUE, RC_MAX_VALUE);
  #endif
}

void joystickSend() {
  #ifdef RC_ENABLED
  if (joystickActive) {
    Joystick.setXAxis(rcValues[ROLL_CHANNEL - 1]);
    Joystick.setYAxis(rcValues[PITCH_CHANNEL - 1]);
    Joystick.setZAxis(rcValues[THROTTLE_CHANNEL - 1]);
    Joystick.setRxAxis(rcValues[YAW_CHANNEL - 1]);
    for (int i = 0; i < JOYSTICK_CHANNELS_COUNT; i ++) {
      Joystick.setButton(i, rcValues[i + 4] < 1500 ? 0 : 1);
    }
    Joystick.sendState();
  }
  #endif
}

void joystickBegin() {
  Joystick.begin();  
  joystickActive = true;
}

void joystickEnd() {
  Joystick.end();  
  joystickActive = false;
}

#endif
