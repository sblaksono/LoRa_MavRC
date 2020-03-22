#ifndef DISPLAY_H
#define DISPLAY_H

#include "variables.h"
#include "tactile.h"

#ifdef DISPLAY_ENABLED

#include "Wire.h"
#include <U8x8lib.h>

class Display {
  public:
    Display(void);
    void init();
    void loop();
    bool forceUpdate = false;
  private:
    U8X8_SSD1306_128X64_NONAME_HW_I2C _display;
    void renderModeSelector();
    void renderPageIdle();
    void renderPageSim();
    void renderPageTelem();
    void renderPageRC();
    uint32_t lastUpdate = 0;
};

#endif

#endif
