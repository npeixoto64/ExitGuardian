#include "buzzer.h"
#include "board.h"

#define BUZZER_LONG_MS         2000U  /* one-shot tone duration */
#define BUZZER_BEEP_PERIOD_MS  1000U  /* periodic beep period */
#define BUZZER_BEEP_ON_MS       100U  /* periodic beep on-time */

typedef struct {
  uint16_t last_tick;
  uint8_t  on;        /* current physical state */
  uint8_t  long_done; /* LONG one-shot already finished */
  uint8_t  prev_mode;
} buzzer_state_t;

static buzzer_state_t s_st = {0, 0, 0, BUZZER_MODE_OFF};

static void buzzer_drive(uint8_t on)
{
  if (s_st.on != on) {
    board_buzzer(on);
    s_st.on = on;
  }
}

void buzzer_handle(uint16_t now, buzzer_mode_t mode)
{
  /* Mode transition: reset per-mode bookkeeping. */
  if (mode != s_st.prev_mode) {
    s_st.prev_mode = mode;
    s_st.last_tick = now;
    s_st.long_done = 0;

    switch (mode) {
    case BUZZER_MODE_OFF:
      buzzer_drive(0);
      return;
    case BUZZER_MODE_LONG:
      buzzer_drive(1);
      return;
    case BUZZER_MODE_BEEP:
    default:
      buzzer_drive(1);
      break;
    }
  }

  switch (mode) {
  case BUZZER_MODE_OFF:
    /* nothing to do */
    return;

  case BUZZER_MODE_LONG:
    if (!s_st.long_done &&
        (uint16_t)(now - s_st.last_tick) >= BUZZER_LONG_MS) {
      buzzer_drive(0);
      s_st.long_done = 1;
    }
    return;

  case BUZZER_MODE_BEEP:
  default:
    if (!s_st.on &&
        (uint16_t)(now - s_st.last_tick) >= BUZZER_BEEP_PERIOD_MS) {
      buzzer_drive(1);
      s_st.last_tick = now;
    } else if (s_st.on &&
               (uint16_t)(now - s_st.last_tick) >= BUZZER_BEEP_ON_MS) {
      buzzer_drive(0);
      /* keep last_tick to measure full period from the rising edge */
    }
    return;
  }
}
