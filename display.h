#ifndef DISPLAY_H
#define DISPLAY_H

#include "variables.h"
#include "tactile.h"

#ifdef DISPLAY_ENABLED

#include "Wire.h"
#include <U8x8lib.h>

#define DISPLAY_PAGE_MAX  1
#define DISPLAY_PAGE_INIT 0
#define DISPLAY_PAGE_STAT 1

class Display {
  public:
    Display(void);
    void init();
    void loop();
  private:
    U8X8_SSD1306_128X64_NONAME_HW_I2C _display;
    void renderPageIdle();
    void renderPageSim();
    void renderPageTelem();
    void renderPageRC();
    uint32_t lastUpdate = 0;
};

#endif

#endif
