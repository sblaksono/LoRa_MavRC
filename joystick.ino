#include "variables.h"

#ifdef JOYSTICK_ENABLED

#include "src/ArduinoJoystickLibrary/Joystick.h"

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, RC_CHANNELS_COUNT - 4, 0,
  true, true, true, true, false, false,
  false, false, false, false, false);

void joystickInit() {  
  Joystick.setXAxisRange(RC_MIN_VALUE, RC_MAX_VALUE);
  Joystick.setYAxisRange(RC_MIN_VALUE, RC_MAX_VALUE);
  Joystick.setZAxisRange(RC_MIN_VALUE, RC_MAX_VALUE);
  Joystick.setRxAxisRange(RC_MIN_VALUE, RC_MAX_VALUE);
}

void joystickSend() {
  Joystick.setXAxis(rcValues[ROLL_CHANNEL - 1]);
  Joystick.setYAxis(rcValues[PITCH_CHANNEL - 1]);
  Joystick.setZAxis(rcValues[THROTTLE_CHANNEL - 1]);
  Joystick.setRxAxis(rcValues[YAW_CHANNEL - 1]);
  for (int i = 0; i < RC_CHANNELS_COUNT - 4; i ++) {
    Joystick.setButton(i, rcValues[i + 4] < 1500 ? 0 : 1);
  }
  Joystick.sendState();
}

void joystickBegin() {
  Joystick.begin();  
}

void joystickEnd() {
  Joystick.end();  
}

#endif
