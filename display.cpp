#include "display.h"

#ifdef DISPLAY_ENABLED

#define DISPLAY_COL_COUNT 16

Display::Display(void) {
    U8X8_SSD1306_128X64_NONAME_HW_I2C _display(U8X8_PIN_NONE);
}

void Display::init() {
    _display.begin();
    _display.setFont(u8x8_font_pxplustandynewtv_f);
    _display.clear();
}

void Display::loop() {
    if (lastMode == mode && lastUpdate > 0 && millis() - lastUpdate < 200) {
        return;
    }

    switch (mode) {
        case MODE_IDLE:
            renderPageIdle();
            break;
        case MODE_SIM:
            renderPageSim();
            break;
        case MODE_TELEM:
            renderPageTelem();
            break;
        case MODE_RC:
            renderPageRC();
            break;
    }
    lastUpdate = millis();
}

void Display::renderPageIdle() {
    if (lastMode != mode) {
      _display.clear();
      _display.draw1x2String(5, 2, "LoRa");
      _display.draw1x2String(5, 4, "MavRC");
    }
}

void Display::renderPageSim() {
    char buf[DISPLAY_COL_COUNT];

    if (lastMode != mode) {
      _display.clear();
      _display.draw1x2String(0, 0, "Joystick / Sim");
    }

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
}

void Display::renderPageTelem() {
    char buf[DISPLAY_COL_COUNT];

    if (lastMode != mode) {
      _display.clear();
      _display.draw1x2String(0, 0, "Telemetry");
    }
    snprintf(buf, DISPLAY_COL_COUNT, "Rcvd:%10d", rcvd);
    _display.drawString(0, 3, buf);
    snprintf(buf, DISPLAY_COL_COUNT, "Sent:%10d", sent);
    _display.drawString(0, 4, buf);
    snprintf(buf, DISPLAY_COL_COUNT, "RSSI:%4d%s %4d", rssiPct, "%", rssi);
    _display.drawString(0, 6, buf);
}

void Display::renderPageRC() {
    if (lastMode != mode) {
      _display.clear();
      _display.drawString(0, 0, "Remote Control");
    }
}

#endif
