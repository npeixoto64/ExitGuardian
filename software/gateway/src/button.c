/**
 * @file button.c
 * @brief Push-button debounce and short/long-press timing implementation.
 */
#include "board.h"
#include "button.h"
#include "log.h"
#include "stm8l15x_gpio.h"

#define BTN_DEBOUNCE_MS        20U    /* ignore edges shorter than this  */
#define BTN_SHORT_PRESS_MAX_MS 2000U  /* release <= 2 s -> short press   */
#define BTN_LONG_PRESS_MIN_MS  5000U  /* release >= 5 s -> long press    */

static volatile uint8_t  g_btn_down_evt       = 0;
static volatile uint8_t  g_btn_up_evt         = 0;
static volatile uint16_t g_btn_debounce_ms    = 0;
static volatile uint8_t  btn_handle_debounce  = 0;
static volatile uint16_t g_btn_down_ms        = 0;
static volatile uint8_t  g_btn_long_press_evt = 0;

static volatile button_event_t g_btn_pending_evt = BUTTON_EVT_NONE;

/**
 * @brief Push-button EXTI ISR body.
 *
 * Called from `EXTI4_IRQHandler` in `main.c` on any button edge. Arms the
 * debounce timer on the first edge; subsequent bounces are ignored.
 */
void button_isr(void)
{
  if (btn_handle_debounce == 0)
  {
    btn_handle_debounce = 1;
    g_btn_debounce_ms   = board_get_tick_ms();
  }
}

/**
 * @brief Process debounced button events.
 *
 * Resolves the stable level after the debounce window, then classifies the
 * subsequent release as short, ignored or long press according to the
 * configured timing thresholds.
 */
void button_handle(void)
{
  if (btn_handle_debounce)
  {
    if ((uint16_t)(board_get_tick_ms() - g_btn_debounce_ms) > BTN_DEBOUNCE_MS)
    {
      btn_handle_debounce = 0;
      if (GPIO_ReadInputDataBit(PUSH_BTN_PORT, PUSH_BTN_PIN) == RESET)
      {
        g_btn_down_evt = 1;
        g_btn_down_ms  = board_get_tick_ms();
      }
      else
      {
        g_btn_up_evt = 1;
      }
    }
  }

  if (g_btn_down_evt)
  {
    if ((uint16_t)(board_get_tick_ms() - g_btn_down_ms) > BTN_LONG_PRESS_MIN_MS)
    {
      g_btn_long_press_evt = 1;
      g_btn_down_evt       = 0;
      g_btn_up_evt         = 0;
      g_btn_pending_evt = BUTTON_EVT_LONG_PRESS_DETECTED;
    }
  }

  if (g_btn_up_evt)
  {
    if ((uint16_t)(board_get_tick_ms() - g_btn_down_ms) < BTN_SHORT_PRESS_MAX_MS)
    {
      g_btn_down_evt = 0;
      g_btn_up_evt   = 0;
      g_btn_pending_evt = BUTTON_EVT_SHORT_PRESS;
    }
    else if ((uint16_t)(board_get_tick_ms() - g_btn_down_ms) <= BTN_LONG_PRESS_MIN_MS)
    {
      /* FR_025: release in (2 s, 5 s) is ignored. */
      g_btn_down_evt = 0;
      g_btn_up_evt   = 0;
    }

    if (g_btn_long_press_evt)
    {
      g_btn_long_press_evt = 0;
      g_btn_up_evt         = 0;
      g_btn_pending_evt = BUTTON_EVT_LONG_PRESS_RELEASED;
    }
  }
}

/**
 * @brief Take and clear the latest pending button event.
 *
 * @return The pending event, or @ref BUTTON_EVT_NONE if none is queued.
 */
button_event_t button_take_event(void)
{
  button_event_t evt = g_btn_pending_evt;
  g_btn_pending_evt = BUTTON_EVT_NONE;
  return evt;
}

