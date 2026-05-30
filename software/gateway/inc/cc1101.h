/**
 * @file cc1101.h
 * @brief Minimal driver for the TI CC1101 sub-GHz transceiver.
 *
 * Provides SPI register access helpers, GFSK 433.92 MHz configurations for
 * fixed-length TX/RX, and a small message protocol carrying a 32-bit chip ID
 * together with a status byte.
 */
#ifndef CC1101_H
#define CC1101_H

#include "stm8l15x.h"

/** @name SPI header bits
 *  CC1101 header bits: R=0x80, B=0x40. For status regs (0x30-0x3D), set B=1
 *  to read status space.
 *  @{ */
#define CC1101_READ       0x80   /**< Read bit in the SPI header byte. */
#define CC1101_BURST      0x40   /**< Burst/status-space bit in the SPI header. */
/** @} */

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

/**
 * @brief Configure CC1101 for fixed-length GFSK TX on 433.92 MHz.
 *
 * @param pkt_size Fixed payload length in bytes (loaded into @c PKTLEN).
 */
void cc1101_config_gfsk_433_tx_fixed(uint8_t pkt_size);

/**
 * @brief Configure CC1101 for fixed-length GFSK RX on 433.92 MHz.
 *
 * After configuration the chip is strobed into RX and stays in RX after a
 * packet is received (RXOFF_MODE = stay in RX).
 *
 * @param pkt_size Fixed payload length in bytes (loaded into @c PKTLEN).
 */
void cc1101_config_gfsk_433_rx_fixed(uint8_t pkt_size);

/**
 * @brief Transmit a 5-byte status/chip-id packet.
 *
 * Payload layout: `[status, chip_id[31:24], chip_id[23:16], chip_id[15:8],
 * chip_id[7:0]]`.
 *
 * @param status  Sensor status byte (battery / button / reed bits).
 * @param chip_id 32-bit sender chip ID.
 */
void cc1101_send_msg(const uint8_t status, const uint32_t chip_id);

/**
 * @brief Read a 5-byte status/chip-id packet from the RX FIFO.
 *
 * Reads as many bytes as currently reported by @c RXBYTES and decodes the
 * first 5 into @p status and @p chip_id when present.
 *
 * @param[out] chip_id 32-bit sender chip ID (unchanged if FIFO is empty).
 * @param[out] status  Sensor status byte (unchanged if FIFO is empty).
 */
void cc1101_recv_msg(uint32_t *chip_id, uint8_t *status);

#endif
