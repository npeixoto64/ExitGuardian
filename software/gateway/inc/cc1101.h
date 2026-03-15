#ifndef CC1101_H
#define CC1101_H

#include "stm8l15x.h"

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

#define CC1101_FREQ2      0x0D
#define CC1101_FREQ1      0x0E
#define CC1101_FREQ0      0x0F

#define CC1101_MDMCFG4    0x10
#define CC1101_MDMCFG3    0x11
#define CC1101_MDMCFG2    0x12
#define CC1101_MDMCFG1    0x13
#define CC1101_MDMCFG0    0x14

#define CC1101_REG_MCSM1  0x17

#define CC1101_PKTCTRL1   0x07
#define CC1101_PKTCTRL0   0x08

#define CC1101_IOCFG2     0x00
#define CC1101_IOCFG1     0x01
#define CC1101_IOCFG0     0x02

#define CC1101_SYNC1      0x04
#define CC1101_SYNC0      0x05
#define CC1101_PKTLEN     0x06
#define CC1101_DEVIATN    0x15
#define CC1101_FREND0     0x22
#define CC1101_PKTSTATUS  0x38   // status reg (read with B=1)

#define CC1101_SCAL       0x33

#define CC1101_PATABLE    0x3E
#define CC1101_RXFIFO     0x3F   // read => RX FIFO
#define CC1101_TXFIFO     0x3F   // write => TX FIFO

// Status regs (read with B=1 + R=1)
#define CC1101_TXBYTES    0x3A
#define CC1101_RXBYTES    0x3B   // status reg (read with B=1)

#define CC1101_CSN_PORT   GPIOB
#define CC1101_CSN_PIN    GPIO_Pin_4
#define CC1101_MISO_PORT  GPIOB
#define CC1101_MISO_PIN   GPIO_Pin_7

void cc1101_config_gfsk_433_tx_fixed(uint8_t pkt_size);
void cc1101_config_gfsk_433_rx_fixed(uint8_t pkt_size);
void cc1101_send_msg(const uint8_t status, const uint32_t chip_id);
void cc1101_recv_msg(uint32_t *chip_id, uint8_t *status);

#endif
