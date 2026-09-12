/**
 * @file board.c
 * @brief Board support package implementation: clocks, peripherals and power.
 */
#include "board.h"
#include "app_config.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_spi.h"
#include "stm8l15x_tim4.h"
#include "stm8l15x_exti.h"
#include "cc1101.h"
#ifdef DEBUG
#include "stm8l15x_usart.h"
#endif

/** @brief Configure the system clock to 16 MHz from the internal HSI. */
static void clock_init(void)
{
  /* 16 MHz (HSI with divider 1) */
  CLK_HSICmd(ENABLE);
  while (CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == RESET);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
}

/**
 * @brief Configure all GPIOs used on the board.
 *
 * Unused pins are pulled up to minimise leakage; peripheral pins are set to
 * the modes required by SPI, USART, and the EXTI inputs.
 */
static void gpio_init(void)
{
  /* TODO: Review unused pins configure low output GPIO_Mode_Out_PP_Low_Slow and Init at 0*/
  /* Unused pins as input pull-up to minimise leakage. */
  GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_In_FL_No_IT);

  /* SPI: PB4 NSS, PB5 SCK, PB6 MOSI, PB7 MISO. */
  GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);

  /* USART: PC5 TX, PC6 RX. */
  GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_FL_No_IT);

  /* LED */
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);

  GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

/** @brief Configure EXTI edge sensitivity for CC1101, button and reed inputs. */
static void exti_init(void)
{
  disableInterrupts();
  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_In_FL_IT);
  GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_FL_IT);
  GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_FL_IT);
  EXTI_SetPinSensitivity(GPIO_Pin_2, EXTI_Trigger_Falling);
  EXTI_SetPinSensitivity(GPIO_Pin_3, EXTI_Trigger_Rising_Falling);
  EXTI_SetPinSensitivity(GPIO_Pin_1, EXTI_Trigger_Rising_Falling);
  //enableInterrupts();
}

/** @brief Millisecond tick counter, incremented by @ref board_systick_irq. */
static volatile uint16_t s_tick_ms = 0;

/** @brief Configure TIM4 to generate a 100 Hz periodic update interrupt. */
static void systick_init(void)
{
  /* 16 MHz / 1024 = 15625 Hz; period 156 -> ~10 ms tick (8-bit reload). */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  TIM4_DeInit();
  TIM4_TimeBaseInit(TIM4_Prescaler_1024, 156U - 1U);
  TIM4_ClearITPendingBit(TIM4_IT_Update);
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);
  TIM4_Cmd(ENABLE);
}

uint16_t board_get_tick_ms(void)
{
  uint16_t t;
  /* Atomic read: save CC (I0/I1), mask interrupts, restore on exit.
   * Safe to call from both thread and ISR context. */
  __asm__("push cc\n\tsim");
  t = s_tick_ms;
  __asm__("pop cc");
  return t;
}

void board_systick_irq(void)
{
  TIM4_ClearITPendingBit(TIM4_IT_Update);
  s_tick_ms += 10U; /* 10 ms per tick, keep the counter in milliseconds. */
}

/** @brief Configure SPI1 as 8-bit master, mode 0, for the CC1101. */
static void spi_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
  SPI_DeInit(SPI1);
  SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_16, SPI_Mode_Master,
           SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_Direction_2Lines_FullDuplex,
           SPI_NSS_Soft, 7);
  SPI_Cmd(SPI1, ENABLE);
}

/** @brief Configure USART1 for 9600 8N1 debug logging. */
static void uart_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
  USART_DeInit(USART1);
  USART_Init(USART1, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1,
             USART_Parity_No,
             (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
  USART_Cmd(USART1, ENABLE);
}

void board_init(void)
{
  clock_init();
  gpio_init();
  exti_init();
  spi_init();
  uart_init();
  systick_init();

  //enableInterrupts();
  
  //cc1101_config_gfsk_433_rx_fixed(5);
}
