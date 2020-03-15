#include "variables.h"

#ifdef PPM_ENABLED

#include <avr/interrupt.h>

#define MIN_PULSE_WIDTH (RC_MIN_VALUE - 15)
#define MAX_PULSE_WIDTH (RC_MAX_VALUE + 15)

#define PPM_CAPTURE_PIN       13
#define NEWFRAME_PULSE_WIDTH  3000
#define TIMER_COUNT_DIVIDER   2

void initTimer(void) {
  TCCR3A = 0;
  TCCR3B = (0 << ICNC3) | (1 << ICES3) | (1 << CS31);
  TCCR3C = 0;

  TIFR3 = (1 << ICF3);  // clear pending
  TIMSK3 = (1 << ICIE3);  // and enable
}

uint16_t adjust(uint16_t diff, uint8_t chan) {
  // Convert to microseconds (because of timer prescaler usage)
  return diff / TIMER_COUNT_DIVIDER;
}

void initPpm() {
  pinMode(PPM_CAPTURE_PIN, INPUT);  // Set-up the input capture pin
  digitalWrite(PPM_CAPTURE_PIN, HIGH);  // Enable the internal pullup-resistor
  initTimer();
}

ISR(TIMER3_CAPT_vect) {
  union twoBytes {
    uint16_t word;
    uint8_t  byte[2];
  } timeValue;

  uint16_t now, diff;
  static uint16_t last = 0;
  static uint8_t chan = 0;

  timeValue.byte[0] = ICR3L;  // Grab captured timer value (low byte)
  timeValue.byte[1] = ICR3H;  // Grab captured timer value (high byte)

  now = timeValue.word;
  diff = now - last;
  last = now;

  // All numbers are microseconds multiplied by TIMER_COUNT_DIVIDER (as prescaler is set to 1/8 of 16 MHz)
  if (diff > (NEWFRAME_PULSE_WIDTH * TIMER_COUNT_DIVIDER)) {
    chan = 0;  // New data frame detected, start again
  }
  else {
    if (diff > (MIN_PULSE_WIDTH * TIMER_COUNT_DIVIDER - RC_THRESHOLD)
        && diff < (MAX_PULSE_WIDTH * TIMER_COUNT_DIVIDER + RC_THRESHOLD)
        && chan < RC_CHANNELS_COUNT) {
      rcValues[chan] = adjust(diff, chan);  // Store detected value
    }
    chan++;  // No value detected within expected range, move to next channel
  }
}

#endif
