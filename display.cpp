#include "display.h"

#ifdef DISPLAY_ENABLED

#define DISPLAY_COL_COUNT 16

char *modeTexts[] = {"Idle", "Telemetry", "Remote Control", "Joystick / Sim"};

Display::Display(void) {
  U8X8_SSD1306_128X64_NONAME_HW_I2C _display(U8X8_PIN_NONE);
}

void Display::init() {
  _display.begin();
  _display.setFont(u8x8_font_pxplustandynewtv_f);
  _display.clear();
}

void Display::loop() {
  if (!forceUpdate && lastUpdate > 0 && millis() - lastUpdate < 200) {
    return;
  }

  if (modeSelector == 1) {
    renderModeSelector();
  }
  else {
    switch (mode) {
      case MODE_IDLE:
        renderPageIdle();
        break;
      case MODE_TELEM:
        renderPageTelem();
        break;
      case MODE_RC:
        renderPageRC();
        break;
      case MODE_SIM:
        renderPageSim();
        break;
    }
  }
  lastUpdate = millis();
  forceUpdate = false;
}

void Display::renderModeSelector() {
  char buf[DISPLAY_COL_COUNT];

  if (forceUpdate) {
    _display.clear();
    _display.draw1x2String(0, 0, "Select Mode");
  
    uint8_t y = 3;
    for (int i = 0; i <= MODE_MAX; i ++) {
      if (i == selectedMode) {
        snprintf(buf, DISPLAY_COL_COUNT, ">%s", modeTexts[i]);
      }
      else {
        snprintf(buf, DISPLAY_COL_COUNT, " %s", modeTexts[i]);
      }
      _display.drawString(0, y, buf);
      y++;
    }
  }
}

void Display::renderPageIdle() {
    if (forceUpdate) {
      _display.clear();
      _display.draw1x2String(5, 2, "LoRa");
      _display.draw1x2String(5, 4, "MavRC");
    }
}

void Display::renderTitle() {
  if (forceUpdate) {
    _display.clear();
    _display.draw1x2String(0, 0, modeTexts[mode]);
  }
}

void Display::renderPageTelem() {
  char buf[DISPLAY_COL_COUNT];

  renderTitle();

  #ifdef RADIO_ENABLED
  if (freqSelector) {
    snprintf(buf, DISPLAY_COL_COUNT, "Freq: >>%7ld", freq);
  }
  else {
    snprintf(buf, DISPLAY_COL_COUNT, "Freq:%10ld", freq);
  }
  _display.drawString(0, 2, buf);
  snprintf(buf, DISPLAY_COL_COUNT, "Rate:%10d", txRate);
  _display.drawString(0, 3, buf);
  snprintf(buf, DISPLAY_COL_COUNT, "Rcvd:%10u", rcvd);
  _display.drawString(0, 4, buf);
  snprintf(buf, DISPLAY_COL_COUNT, "Sent:%10u", sent);
  _display.drawString(0, 5, buf);
  snprintf(buf, DISPLAY_COL_COUNT, "RSSI:%4d%s %4d", rssiPct, "%", rssi);
  _display.drawString(0, 6, buf);
  snprintf(buf, DISPLAY_COL_COUNT, "SNR :%10d", snr);
  _display.drawString(0, 7, buf);
  #endif
}

void Display::renderPageRC() {
  renderTitle();
}

void Display::renderPageSim() {
  char buf[DISPLAY_COL_COUNT];

  renderTitle();
  #ifdef RC_ENABLED
  uint8_t x = 0;
  uint8_t y = 0;
  for (uint8_t i = 0; i < RC_CHANNELS_COUNT; i ++) {
    snprintf(buf, DISPLAY_COL_COUNT, "%d: %04d", i + 1, rcValues[i]);
    _display.drawString(x, y + 3, buf);
    y ++;
    if (y >= RC_CHANNELS_COUNT / 2) {
      x = DISPLAY_COL_COUNT / 2;
      y = 0;
    }
  }
  #endif
}

#endif
