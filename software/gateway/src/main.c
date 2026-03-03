#include <stdint.h>
#include <stdio.h>
#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_adc.h"
#include "stm8l15x_i2c.h"
#include "stm8l15x_spi.h"
#include "stm8l15x_usart.h"
#include "stm8l15x_tim1.h"
#include "stm8l15x_exti.h"
#include "cc1101.h"
#include "log.h"

static volatile uint8_t g_pd2_went_low_flag = 0;

// INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
// {
//    g_pd2_went_low_flag = 1;
// }

int main(void)
{
    //Configure clock to 16 MHz (HSI with divider 1)
    CLK_HSICmd(ENABLE);
    while (CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == RESET);
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

    // Configure not used pins as input pins with pull-up
    // GPIO_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
    // GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);
    // GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_PU_No_IT);
    // GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);
    // GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
    // GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
    // GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);
    // GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);

    // Configure PA6 as ADC input (floating input)
    // GPIO_Init(GPIOA, GPIO_Pin_6, GPIO_Mode_In_FL_No_IT);

    // // Configure PC0 as SDA and PC1 as SCL for I2C (open-drain)
    // GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_OD_HiZ_Fast);
    // GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Fast);

    // Configure PB4-PB7 as SPI (PB4 NSS, PB5 SCK, PB6 MISO, PB7 MOSI)
    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);   // NSS
    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);   // SCK
    GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);   // MOSI
    GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);        // MISO

    // Configure PC2 and PC3 as UART (PC2 TX, PC3 RX)
    GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); // TX
    GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_In_FL_No_IT);      // RX

    // Configure PD7 and PD2 as differential PWM (TIM1 CH1 and CH1N)
    // GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast); // TIM1_CH1
    // GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); // TIM1_CH1N

    // Configure PD0, PD4 and PD5 as inputs floating with interrupt
    GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_In_FL_IT);
    //GPIO_Init(GPIOD, GPIO_Pin_5, GPIO_Mode_In_FL_IT);
    //GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_In_FL_IT);

    // Configure EXTI for PD0 (falling edge trigger)
    //EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Falling);

    // Configure PA2, PA3, PA4, PC4 as push-pull outputs
    GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);
    // GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);

    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    // GPIO_ResetBits(GPIOC, GPIO_Pin_4);

    // Configure ADC for PA6 (Channel 6)
    // CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
    // ADC_DeInit(ADC1);
    // ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_2);
    // ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);
    // ADC_Cmd(ADC1, ENABLE);
    // ADC_ChannelCmd(ADC1, ADC_Channel_6, ENABLE);

    // Configure I2C for PC0 (SDA) and PC1 (SCL)
    // CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);
    // I2C_DeInit(I2C1);
    // I2C_Init(I2C1, 100000, 0x00, I2C_Mode_I2C, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
    // I2C_Cmd(I2C1, ENABLE);

    // Configure SPI for PB4-PB7
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
    SPI_DeInit(SPI1);
    SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_16, SPI_Mode_Master,
            SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_Direction_2Lines_FullDuplex,
            SPI_NSS_Soft, 7);
    SPI_Cmd(SPI1, ENABLE);

    // Configure USART for PC2 (TX) and PC3 (RX)
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
    USART_DeInit(USART1);
    USART_Init(USART1, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1,
                USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
    USART_Cmd(USART1, ENABLE);

    // // Configure TIM1 for differential PWM on PD7 (CH4) and PD2 (CH3N)
    // CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
    // TIM1_DeInit();
    // TIM1->PSCRH = 0x00;
    // TIM1->PSCRL = 0x07;
    // TIM1->ARRH = 0x03;
    // TIM1->ARRL = 0xE8;
    // TIM1->CCMR1 =
    // (6 << 4) |   // OC1M = 110 → PWM mode 1
    // (1 << 3);    // OC1PE = preload enable
    // TIM1->CCR1H = 0x01;
    // TIM1->CCR1L = 0xF4;   // 500 / 1000 = 50%

    // TIM1->CCER1 |=  (1<<0) | (1<<2);   // CH1 + CH1N
    // TIM1->CCER1 &= ~((1<<1) | (1<<3)); // active high

    // TIM1->DTR = 0x10;  // example value; tune as needed

  //TIM1->BKR |= (1 << 7);   // MOE = 1
  //TIM1->CR1 |= (1 << 0);   // CEN = 1

  // TIM1_TimeBaseInit(159, TIM1_CounterMode_Up, 49, 0); // 16MHz / 160 = 100kHz, period 50 -> 2kHz
  // TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Enable,
  //              25, TIM1_OCPolarity_High, TIM1_OCNPolarity_High,
  //              TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
  // TIM1_SelectOCxM(TIM1_Channel_4, TIM1_OCMode_PWM1);
  // TIM1_SetCompare4(25);
  // TIM1_CCxCmd(TIM1_Channel_4, ENABLE);
  // TIM1_Cmd(ENABLE);
  // TIM1_CtrlPWMOutputs(ENABLE);

    // Configure CC1101 radio for RX mode
    cc1101_config_gfsk_433_rx_fixed();

    // Enable global interrupts
    //enableInterrupts();

    static uint32_t counter = 0;
    // char buffer[20];

    // sprintf(buffer, "Initial Counter: %lu\r", counter);
    // send_string(buffer);

    GPIO_SetBits(GPIOA, GPIO_Pin_3);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    while (1) {
        GPIO_ToggleBits(GPIOA, GPIO_Pin_2);
        // GPIO_ToggleBits(GPIOA, GPIO_Pin_3);
        // GPIO_ToggleBits(GPIOA, GPIO_Pin_4);

        // if (g_pd2_went_low_flag)
        // {
        //     // Reception concluded
        //     g_pd2_went_low_flag = 0;

             cc1101_recv_u32(&counter);

        //     //GPIO_ToggleBits(GPIOA, GPIO_Pin_2);

        //     // Send UART message with counter
        //     //sprintf(buffer, "Counter: %lu\r", counter++);
        //     // sprintf(buffer, "\r\nCounter: %lu", counter++);
        //     // send_string(buffer);
        // }
    }
}
