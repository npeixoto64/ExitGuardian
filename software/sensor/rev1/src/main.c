/**
 * @file main.c
 * @brief Application entry point and top-level interrupt handlers.
 */
#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "device_id.h"
#include "app_config.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_exti.h"
#include "stm8l15x_tim4.h"
#include "cc1101.h"
#include "log.h"

// ============================================================================
// Constants and Enums
// ============================================================================

// Status byte format:
// Bits 7-4: Battery capacity (high nibble)
// Bits 2-1: Button action (2 bits): 0=none, 1=force update, 2=pair, 3=unpair
// Bit 0:    Reed switch state
#define STATUS_REED_SW_BIT      0
#define STATUS_BTN_SHIFT        1
#define STATUS_BTN_MASK         0x03
#define STATUS_BATT_SHIFT       4
#define STATUS_BATT_MASK        0x0F

#define STATUS_SET_BATT(batt)   (((batt) & STATUS_BATT_MASK) << STATUS_BATT_SHIFT)
#define STATUS_SET_BTN(btn)     (((btn) & STATUS_BTN_MASK) << STATUS_BTN_SHIFT)
#define STATUS_SET_REED(reed)   (((reed) ? 1 : 0) << STATUS_REED_SW_BIT)

typedef enum {
    BTN_ACTION_NONE       = 0,
    BTN_ACTION_FORCE_UPD  = 1,
    BTN_ACTION_PAIR       = 2,
    BTN_ACTION_UNPAIR     = 3,
} button_action_t;

// Timing constants (in TIM2 ticks, 10ms per tick)
#define DEBOUNCE_TICKS              3    // 30ms debounce period
#define BTN_DOUBLE_PRESS_WINDOW     40   // 400ms max gap between clicks
#define BTN_LONG_PRESS_TICKS        80   // 800ms hold duration
#define LED_ON_HOLD_TICKS           50   // 500ms before auto-off
#define LED_ON_STARTUP_HOLD_TICKS   100  // 1000ms startup hold
#define LED_BLINK_HALF_PERIOD       20   // 200ms half-period for blink
#define LED_BLINK_COUNT             4    // 4 on-off cycles for low battery
#define LOW_BATTERY_THRESHOLD       3    // 0-15 scale
#define WWDG_COUNTER_MAX            0x7F // WWDG 7-bit counter max value (T[6:0])
#define WWDG_REFRESH_THRESHOLD      0x40 // Latest safe refresh point before watchdog reset (counter must stay >= 0x40)

// ============================================================================
// Global System State
// ============================================================================

static volatile uint8_t g_tim2_ticks = 0xFF;

// Button state variables
static struct {
    volatile uint8_t irq_flag;
    volatile uint8_t last_irq_tick;
    uint8_t is_pressed;
    uint8_t press_start_tick;
    uint8_t click_count;
    uint8_t first_click_tick;
    uint8_t long_press_reported;
} g_button = {0};

// LED state variables
static struct {
    uint8_t is_on;
    uint8_t off_flag;
    uint8_t on_start_tick;
    uint8_t blink_remaining;
    uint8_t blink_last_toggle_tick;
} g_led = {0};

// Reed switch state variables
static struct {
    volatile uint8_t irq_flag;
    volatile uint8_t last_irq_tick;
    uint8_t current_state;
} g_reed = {0};

// Power management state
static struct {
    volatile uint8_t gdo0_tx_complete_flag;
} g_power = {0};

uint32_t chip_id_hash = 0;

// ============================================================================
// Utility Functions
// ============================================================================

static inline uint8_t ticks_elapsed_since(uint8_t tick_start)
{
    return (uint8_t)(tick_start - g_tim2_ticks);
}

// ============================================================================
// LED Control
// ============================================================================

static inline void led_turn_on(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    g_led.is_on = 1;
    g_led.on_start_tick = g_tim2_ticks;
    g_led.off_flag = 0;
}

static inline void led_turn_off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    g_led.is_on = 0;
    g_led.off_flag = 1;
}

static void led_hold_on_for_duration(uint8_t duration_ticks)
{
    led_turn_on();
    while (ticks_elapsed_since(g_led.on_start_tick) < duration_ticks) {}
}

static void led_update(uint8_t battery_capacity)
{
    if (battery_capacity > LOW_BATTERY_THRESHOLD)
    {
        // Normal battery: single on/off control
        if (g_led.is_on && (ticks_elapsed_since(g_led.on_start_tick) >= LED_ON_HOLD_TICKS))
        {
            led_turn_off();
        }
    }
}

// ============================================================================
// Sensor Communication
// ============================================================================

#ifdef DEBUG
static void log_sensor_status(uint8_t battery_capacity, button_action_t btn_action, uint8_t reed_state)
{
    send_string("\r\n[batt:");
    send_uint8_t(battery_capacity);
    send_string(" btn:");
    send_uint8_t(btn_action);
    send_string(" reed:");
    send_uint8_t(reed_state);
    send_string("] ");
}
#else
#define log_sensor_status(batt, btn, reed)
#define log_battery_level(raw, level)
#endif

static void transmit_sensor_status(uint32_t value)
{
    led_turn_on();
    cc1101_send_msg(value);
    send_string("\r\nSend packet: 0x");
    send_hex_byte((value >> 24) & 0xFF);
    send_hex_byte((value >> 16) & 0xFF);
    send_hex_byte((value >> 8) & 0xFF);
    send_hex_byte(value & 0xFF);
}

static volatile uint8_t  g_irq_cc1101_flag = 0;

// /* CC1101 GDO0 falling-edge interrupt (IRQ vector 8, PD0).
//  * Set when the CC1101 asserts its interrupt line (active-low),
//  * signalling that a packet has been received. Processed in main loop. */
// INTERRUPT_HANDLER(EXTI0_IRQHandler, 9)
// {
//   if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1) == RESET)
//   {
//     g_irq_cc1101_flag = 1;
//   }
//   EXTI_ClearITPendingBit(EXTI_IT_Pin1);
// }

// /* Push-button both-edges interrupt (IRQ vector 12, PD4).
//  * ISR kept in main.c so the linker always includes button.rel via button_isr(). */
// INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
// {
//   button_isr();
//   EXTI_ClearITPendingBit(EXTI_IT_Pin4);
// }

// /* Reed switch (door sensor) both-edges interrupt (IRQ vector 13, PD5).
//  * ISR kept in main.c so the linker always includes reed.rel via reed_isr(). */
// INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
// {
//   reed_isr();
//   EXTI_ClearITPendingBit(EXTI_IT_Pin5);
// }

/* TIM4 update/overflow interrupt (IRQ vector 25).
 * Fires at 1 kHz; delegates tick increment and flag clear to board driver.
 * 1 ms tick */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  g_tim2_ticks--;
  TIM4_ClearITPendingBit(TIM4_IT_Update);
}

static void system_initialize(void)
{
    mcu_init();

#ifdef TRANSMITTER
    send_string("\r\n\r\n\r\n**** start [TX MODE] ****");
    //board_pre_init_tx();
// #else
//     send_string("\r\nRX MODE\r\n");
//     board_pre_init_rx();
#endif
    
    // LED startup sequence
    //led_hold_on_for_duration(LED_ON_STARTUP_HOLD_TICKS);
    //led_turn_off();
    led_turn_on();
    send_string("\r\nled_turn_on()");

    // Initialize system state
    //chip_id_hash = device_id_get_hash();
    //g_reed.current_state = (GPIO_ReadInputPin(GPIOA, GPIO_Pin_2) != 0) ? 1 : 0;
    
    // // Complete board initialization (enables EXTI and interrupts)
    // board_init();
}

/**
 * @brief Application entry point.
 *
 * Brings up the board and the mode manager, then runs the main super-loop:
 * watchdog refresh, reed/button handling, CC1101 packet dispatch, event
 * forwarding to the mode manager and LED/buzzer tick.
 *
 * @return Never returns.
 */
int main(void)
{
    system_initialize();

    enableInterrupts();

    uint32_t counter = 0;

    send_string("\r\nbefore loop");

    while (1)
    {
        uint8_t battery_capacity = 10;
        uint8_t timer = g_tim2_ticks;

        // Process button input
#ifdef TRANSMITTER
        //transmit_sensor_status(counter++);
#endif

        // Handle radio transmission completion
        if (g_power.gdo0_tx_complete_flag)
        {
            g_power.gdo0_tx_complete_flag = 0;
#ifndef TRANSMITTER
            cc1101_recv_msg(&counter);
            led_turn_on();
            send_string("\r\nSend packet: 0x");
            send_hex_byte((counter >> 24) & 0xFF);
            send_hex_byte((counter >> 16) & 0xFF);
            send_hex_byte((counter >> 8) & 0xFF);
            send_hex_byte(counter & 0xFF);
#endif
            led_turn_off();
        }

        GPIO_ToggleBits(GPIOB, GPIO_Pin_0);

        send_string("\r\nbefore time count");

        timer = g_tim2_ticks;
        while (ticks_elapsed_since(timer) < LED_ON_STARTUP_HOLD_TICKS) {}
    }
}
