#include "board.h"
#include "reed.h"
#include "log.h"

#define REED_DEBOUNCE_MS        20U     /* ignore edges shorter than this */

static volatile uint8_t  g_reed_close_evt       = 0;
static volatile uint8_t  g_reed_open_evt        = 0;
static volatile uint16_t g_reed_debounce_ms     = 0;
static volatile uint8_t  reed_handle_debounce   = 0;

static volatile reed_event_t g_reed_pending_evt = REED_EVT_NONE;

/* Reed switch (door sensor) both-edges ISR body (IRQ vector 13, PD5).
 * Called from EXTI5_IRQHandler in main.c, which the linker always pulls in.
 * Arms the debounce timer on the first edge; subsequent bounces are ignored. */
void reed_isr(void)
{
  if (reed_handle_debounce == 0)
  {
    reed_handle_debounce = 1;
    g_reed_debounce_ms   = board_get_tick_ms();
  }
}

void reed_handle(void)
{
    if (reed_handle_debounce)
    {
      if ((uint16_t)(board_get_tick_ms() - g_reed_debounce_ms) > REED_DEBOUNCE_MS)
      {
          reed_handle_debounce = 0;
          if (GPIO_ReadInputDataBit(REED_DOOR_PORT, REED_DOOR_PIN) == RESET)
          {
              g_reed_close_evt = 1;
              g_reed_pending_evt = REED_EVT_CLOSED;
              // send_string("\r\ng_reed_close_evt set");
          }
          else
          {
              g_reed_open_evt = 1;
              g_reed_pending_evt = REED_EVT_OPENED;
              // send_string("\r\ng_reed_open_evt set");
          }
      }
    }
}

reed_event_t reed_take_event(void)
{
  reed_event_t evt = g_reed_pending_evt;
  g_reed_pending_evt = REED_EVT_NONE;
  return evt;
}