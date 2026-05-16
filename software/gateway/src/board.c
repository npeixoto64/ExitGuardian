#include "board.h"

#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_adc.h"
#include "stm8l15x_i2c.h"
#include "stm8l15x_spi.h"
#include "stm8l15x_usart.h"
#include "stm8l15x_tim1.h"
#include "stm8l15x_tim4.h"
#include "stm8l15x_exti.h"
#include "cc1101.h"

static void clock_init(void)
{
  /* 16 MHz (HSI with divider 1) */
  CLK_HSICmd(ENABLE);
  while (CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == RESET);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
}

static void gpio_init(void)
{
  /* Unused pins as input pull-up to minimise leakage. */
  GPIO_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);

  /* PA6 ADC input (floating). */
  GPIO_Init(GPIOA, GPIO_Pin_6, GPIO_Mode_In_FL_No_IT);

  /* I2C: PC0 SDA, PC1 SCL (open-drain). */
  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_OD_HiZ_Fast);
  GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Fast);

  /* SPI: PB4 NSS, PB5 SCK, PB6 MOSI, PB7 MISO. */
  GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);

  /* USART: PC2 TX, PC3 RX. */
  GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_In_FL_No_IT);

  /* TIM1 differential PWM: PD7 CH1, PD2 CH1N. */
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast);

  /* External interrupt inputs. */
  GPIO_Init(IRQ_CC1101_PORT, IRQ_CC1101_PIN, GPIO_Mode_In_FL_IT);
  GPIO_Init(PUSH_BTN_PORT,   PUSH_BTN_PIN,   GPIO_Mode_In_FL_IT);
  GPIO_Init(REED_DOOR_PORT,  REED_DOOR_PIN,  GPIO_Mode_In_FL_IT);

  /* LEDs + spare PC4 output. */
  GPIO_Init(LED_B_PORT, LED_B_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(LED_Y_PORT, LED_Y_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(LED_R_PORT, LED_R_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(BUZZER_EN_PORT, BUZZER_EN_PIN, GPIO_Mode_Out_PP_Low_Fast);

  GPIO_ResetBits(LED_B_PORT, LED_B_PIN);
  GPIO_ResetBits(LED_Y_PORT, LED_Y_PIN);
  GPIO_ResetBits(LED_R_PORT, LED_R_PIN);
  GPIO_ResetBits(BUZZER_EN_PORT, BUZZER_EN_PIN);
}

static void exti_init(void)
{
  EXTI_SetPinSensitivity(IRQ_CC1101_EXTI, EXTI_Trigger_Falling);
  EXTI_SetPinSensitivity(PUSH_BTN_EXTI,   EXTI_Trigger_Rising_Falling);
  EXTI_SetPinSensitivity(REED_DOOR_EXTI,  EXTI_Trigger_Rising_Falling);
}

static volatile uint16_t s_tick_ms = 0;

static void systick_init(void)
{
  /* 16 MHz / 128 = 125 kHz; period 125 -> 1 ms tick. */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  TIM4_DeInit();
  TIM4_TimeBaseInit(TIM4_Prescaler_128, 125U - 1U);
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
  s_tick_ms++;
}

static void adc_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  ADC_DeInit(ADC1);
  ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_2);
  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);
  ADC_Cmd(ADC1, ENABLE);
  ADC_ChannelCmd(ADC1, ADC_Channel_6, ENABLE);
}

static void i2c_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);
  I2C_DeInit(I2C1);
  I2C_Init(I2C1, 400000, 0x00, I2C_Mode_I2C, I2C_DutyCycle_2,
           I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
  I2C_Cmd(I2C1, ENABLE);
}

static void spi_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
  SPI_DeInit(SPI1);
  SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_16, SPI_Mode_Master,
           SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_Direction_2Lines_FullDuplex,
           SPI_NSS_Soft, 7);
  SPI_Cmd(SPI1, ENABLE);
}

static void uart_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
  USART_DeInit(USART1);
  USART_Init(USART1, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1,
             USART_Parity_No,
             (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
  USART_Cmd(USART1, ENABLE);
}

static void pwm_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
  TIM1_DeInit();
  TIM1_TimeBaseInit(7U, TIM1_CounterMode_Up, 1000U, 0U);
  TIM1_OC1Init(TIM1_OCMode_PWM1,
               TIM1_OutputState_Enable,
               TIM1_OutputNState_Enable,
               500U,
               TIM1_OCPolarity_High,
               TIM1_OCNPolarity_High,
               TIM1_OCIdleState_Reset,
               TIM1_OCNIdleState_Reset);
  TIM1_OC1PreloadConfig(ENABLE);
  TIM1_BDTRConfig(TIM1_OSSIState_Disable,
                  TIM1_LockLevel_Off,
                  0x10U,
                  TIM1_BreakState_Disable,
                  TIM1_BreakPolarity_Low,
                  TIM1_AutomaticOutput_Disable);
  TIM1_CtrlPWMOutputs(ENABLE);
}

void board_init(void)
{
  clock_init();
  gpio_init();
  exti_init();
  adc_init();
  i2c_init();
  spi_init();
  uart_init();
  pwm_init();
  systick_init();

  enableInterrupts();
  
  cc1101_config_gfsk_433_rx_fixed(5);
}

void board_buzzer(uint8_t on)
{
  TIM1_Cmd(on ? ENABLE : DISABLE);
}

void board_enter_deep_sleep(void)
{
  /* Reduce current draw before HALT; wake source is EXTI on PD0/PD4/PD5. */
  ADC_Cmd(ADC1, DISABLE);
  I2C_Cmd(I2C1, DISABLE);
  SPI_Cmd(SPI1, DISABLE);
  USART_Cmd(USART1, DISABLE);

  enableInterrupts();
  halt();

  /* Restore peripherals after wake-up. */
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);

  ADC_Cmd(ADC1, ENABLE);
  I2C_Cmd(I2C1, ENABLE);
  SPI_Cmd(SPI1, ENABLE);
  USART_Cmd(USART1, ENABLE);
}
