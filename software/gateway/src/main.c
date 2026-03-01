#include <stdint.h>
#include <stdio.h>
#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_adc.h"
#include "stm8l15x_i2c.h"
#include "stm8l15x_spi.h"
#include "stm8l15x_usart.h"
#include "stm8l15x_tim1.h"

static void delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 40000UL; i++) {}
}

void send_string(const char* str)
{
    while (*str) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData8(USART1, *str++);
    }
}

void send_register_hex(const char* reg_name, uint8_t value)
{
    const char hex_chars[] = "0123456789ABCDEF";
    
    // Send register name
    send_string(reg_name);
    
    // Send " = 0x"
    send_string(" = 0x");
    
    // Send high nibble
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData8(USART1, hex_chars[(value >> 4) & 0x0F]);
    
    // Send low nibble
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData8(USART1, hex_chars[value & 0x0F]);
}

#define CC1101_CSN_PORT   GPIOB
#define CC1101_CSN_PIN    GPIO_Pin_4
#define CC1101_MISO_PORT  GPIOB
#define CC1101_MISO_PIN   GPIO_Pin_7

// CC1101 header bits: R=0x80, B=0x40. For status regs (0x30-0x3D): use B=1 to read status reg.
#define CC1101_READ       0x80
#define CC1101_BURST      0x40

// Command strobes (0x30-0x3D) when B=0
#define CC1101_SRES       0x30
#define CC1101_SNOP       0x3D

// Status registers (0x30-0x3D) when B=1
#define CC1101_PARTNUM    0x30
#define CC1101_VERSION    0x31

#define CC1101_SRES       0x30
#define CC1101_SRX        0x34
#define CC1101_STX        0x35
#define CC1101_SIDLE      0x36
#define CC1101_SFRX       0x3A
#define CC1101_SFTX       0x3B
#define CC1101_SNOP       0x3D

#define CC1101_FREQ2             0x0D
#define CC1101_FREQ1             0x0E
#define CC1101_FREQ0             0x0F

#define CC1101_MDMCFG4           0x10
#define CC1101_MDMCFG3           0x11
#define CC1101_MDMCFG2           0x12
#define CC1101_MDMCFG1           0x13
#define CC1101_MDMCFG0           0x14

#define CC1101_PKTCTRL1          0x07
#define CC1101_PKTCTRL0          0x08

#define CC1101_IOCFG2            0x00
#define CC1101_IOCFG1            0x01
#define CC1101_IOCFG0            0x02


static inline void csn_hi(void) { GPIO_SetBits(CC1101_CSN_PORT, CC1101_CSN_PIN); }
static inline void csn_lo(void) { GPIO_ResetBits(CC1101_CSN_PORT, CC1101_CSN_PIN); }

static uint8_t spi1_xfer(uint8_t b)
{
    while (SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET) {}
    SPI_SendData(SPI1, b);
    while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET) {}
    return SPI_ReceiveData(SPI1);
}

// Pull CSn low and wait for SO/MISO low (CHIP_RDYn). Returns 1 on ready, 0 on timeout.
static uint8_t cc1101_select(void)
{
    csn_lo();
    uint16_t to = 60000;
    while (GPIO_ReadInputDataBit(CC1101_MISO_PORT, CC1101_MISO_PIN) != RESET) {
        if (--to == 0) { csn_hi(); return 0; }
    }
    return 1;
}

static inline void cc1101_deselect(void) { csn_hi(); }

// Send a command strobe (B=0). Returns the status byte.
static uint8_t cc1101_strobe(uint8_t strobe)
{
    if (!cc1101_select()) return 0xFF;
    uint8_t status = spi1_xfer(strobe);  // status byte shifted out on SO
    cc1101_deselect();
    return status;
}

// Read a status register (0x30-0x3D) one byte. Returns register value, sets *pStatus to status byte.
static uint8_t cc1101_read_status(uint8_t addr, uint8_t *pStatus)
{
    if (!cc1101_select()) { if (pStatus) *pStatus = 0xFF; return 0xFF; }

    // For 0x30-0x3D: use B=1 to select "status reg", and R=1 to read
    uint8_t status = spi1_xfer(CC1101_READ | CC1101_BURST | (addr & 0x3F));
    uint8_t val    = spi1_xfer(0x00); // clock out the register value
    cc1101_deselect();

    if (pStatus) *pStatus = status;
    return val;
}

void cc1101_write_reg(uint8_t addr, uint8_t value)
{
    // Assert CSn and wait for CHIP_RDYn (SO/MISO low)
    cc1101_select();              // pulls CSn low + waits for MISO low

    // Send address byte (write, single)
    spi1_xfer(addr & 0x3F);

    // Send data byte
    spi1_xfer(value);

    // Deassert CSn
    cc1101_deselect();
}

uint8_t cc1101_read_reg(uint8_t addr)
{
    cc1101_select();
    spi1_xfer(0x80 | (addr & 0x3F));   // READ, single
    uint8_t v = spi1_xfer(0x00);
    cc1101_deselect();
    return v;
}


uint8_t cc1101_self_test(uint8_t *partnum, uint8_t *version)
{
    // 1) Reset chip
    cc1101_strobe(CC1101_SRES);

    // Datasheet note: after SRES, you must wait for SO low again before next header
    // (our select() does that on the next transaction). :contentReference[oaicite:4]{index=4}

    // 2) Read PARTNUM and VERSION
    uint8_t st;
    uint8_t pn = cc1101_read_status(CC1101_PARTNUM, &st);
    uint8_t vr = cc1101_read_status(CC1101_VERSION, &st);

    if (partnum)
      *partnum = pn;
    
    if (version)
      *version = vr;

    // Basic sanity:
    // - VERSION reset value is documented as 0x14 (may vary by silicon). :contentReference[oaicite:5]{index=5}
    // - PARTNUM is documented as 0x00 for CC1101. :contentReference[oaicite:6]{index=6}
    if (pn == 0xFF && vr == 0xFF)
      return 0; // typical "no slave / MISO stuck high" symptom
    
    return 1;
}

int main(void)
{
  //Configure clock to 16 MHz (HSI with divider 1)
  CLK_HSICmd(ENABLE);
  while (CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == RESET);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

  // Configure not used pins as input pins with pull-up
  GPIO_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);

  // Configure PA6 as ADC input (floating input)
  GPIO_Init(GPIOA, GPIO_Pin_6, GPIO_Mode_In_FL_No_IT);

  // Configure PC0 as SDA and PC1 as SCL for I2C (open-drain)
  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_OD_HiZ_Fast);
  GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Fast);

  // Configure PB4-PB7 as SPI (PB4 NSS, PB5 SCK, PB6 MISO, PB7 MOSI)
  GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast); // NSS
  GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);  // SCK
  GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);      // MOSI
  GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);  // MISO

  // Configure PC2 and PC3 as UART (PC2 TX, PC3 RX)
  GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); // TX
  GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_In_FL_No_IT);      // RX

  // Configure PD7 and PD2 as differential PWM (TIM1 CH1 and CH1N)
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast); // TIM1_CH1
  GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); // TIM1_CH1N

  // Configure PD0, PD4 and PD5 as inputs floating with interrupt
  GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_In_FL_IT);
  GPIO_Init(GPIOD, GPIO_Pin_5, GPIO_Mode_In_FL_IT);
  GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_In_FL_IT);

  // Configure PA2, PA3, PA4, PC4 as push-pull outputs
  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);

  GPIO_SetBits(GPIOA, GPIO_Pin_2);
  GPIO_ResetBits(GPIOA, GPIO_Pin_3);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  GPIO_SetBits(GPIOC, GPIO_Pin_4);

  // Configure ADC for PA6 (Channel 6)
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  ADC_DeInit(ADC1);
  ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_2);
  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);
  ADC_Cmd(ADC1, ENABLE);
  ADC_ChannelCmd(ADC1, ADC_Channel_6, ENABLE);

  // Configure I2C for PC0 (SDA) and PC1 (SCL)
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);
  I2C_DeInit(I2C1);
  I2C_Init(I2C1, 100000, 0x00, I2C_Mode_I2C, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
  I2C_Cmd(I2C1, ENABLE);

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

  // Configure TIM1 for differential PWM on PD7 (CH4) and PD2 (CH3N)
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
  TIM1_DeInit();
  TIM1->PSCRH = 0x00;
  TIM1->PSCRL = 0x07;
  TIM1->ARRH = 0x03;
  TIM1->ARRL = 0xE8;
  TIM1->CCMR1 =
    (6 << 4) |   // OC1M = 110 → PWM mode 1
    (1 << 3);    // OC1PE = preload enable
  TIM1->CCR1H = 0x01;
  TIM1->CCR1L = 0xF4;   // 500 / 1000 = 50%

  TIM1->CCER1 |=  (1<<0) | (1<<2);   // CH1 + CH1N
  TIM1->CCER1 &= ~((1<<1) | (1<<3)); // active high

  TIM1->DTR = 0x10;  // example value; tune as needed

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

  cc1101_strobe(CC1101_SRES);

  uint8_t partnum, version;
  cc1101_self_test(&partnum, &version);
  send_register_hex("PARTNUM=", partnum);
  send_register_hex(" | VERSION=", version);
  send_string("\r\n");

  cc1101_strobe(CC1101_SIDLE);

  // Carrier Frequency = 433.92 MHz
  cc1101_write_reg(CC1101_FREQ2, 0x10); // FREQ2
  uint8_t result = cc1101_read_reg(CC1101_FREQ2);
  send_register_hex("FREQ2=", result);
  cc1101_write_reg(CC1101_FREQ1, 0xB0); // FREQ1
  result = cc1101_read_reg(CC1101_FREQ1);
  send_register_hex(" | FREQ1=", result);
  cc1101_write_reg(CC1101_FREQ0, 0x71); // FREQ0
  result = cc1101_read_reg(CC1101_FREQ0);
  send_register_hex(" | FREQ0=", result);

  // Bandwidth = 203 kHz, Baudrate = 1.25 kbps
  cc1101_write_reg(CC1101_MDMCFG4, 0xF5); // MDMCFG4
  result = cc1101_read_reg(CC1101_MDMCFG4);
  send_register_hex(" | MDMCFG4=", result);
  cc1101_write_reg(CC1101_MDMCFG3, 0x94); // MDMCFG3
  result = cc1101_read_reg(CC1101_MDMCFG3);
  send_register_hex("\r\nMDMCFG3=", result);

  // Modulation: OOK, no synch, no manchester decoding
  cc1101_write_reg(CC1101_MDMCFG2, 0x30); // MDMCFG2 or 0xB0 (better sensitivity)
  result = cc1101_read_reg(CC1101_MDMCFG2);
  send_register_hex(" | MDMCFG2=", result);

  // Async serial, infinite packet length mode
  cc1101_write_reg(CC1101_PKTCTRL0, 0x32); // PKTCTRL0
  result = cc1101_read_reg(CC1101_PKTCTRL0);
  send_register_hex(" | PKTCTRL0=", result);

  // Serial Data Output on GDO0, or 0x0E carrier sense, if RSSI (Received signal strength indication) is above threshold
  cc1101_write_reg(CC1101_IOCFG0, 0x0D); // IOCFG0
  result = cc1101_read_reg(CC1101_IOCFG0);
  send_register_hex(" | IOCFG0=", result);

  send_string("\r\n\r\n");

  cc1101_strobe(CC1101_SRX);           // enter receive

  while (1) {
    // GPIO_ToggleBits(GPIOA, GPIO_Pin_2);
    // GPIO_ToggleBits(GPIOA, GPIO_Pin_3);
    // GPIO_ToggleBits(GPIOA, GPIO_Pin_4);

    // Send UART message with counter
    // static uint32_t counter = 0;
    // char buffer[20];
    // sprintf(buffer, "Counter: %lu\r", counter++);
    // send_string(buffer);

    delay();
  }
}
