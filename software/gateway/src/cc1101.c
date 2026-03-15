#include "cc1101.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_spi.h"
#include "log.h"
#include <stdint.h>

static inline void csn_hi(void) { GPIO_SetBits(CC1101_CSN_PORT, CC1101_CSN_PIN); }
static inline void csn_lo(void) { GPIO_ResetBits(CC1101_CSN_PORT, CC1101_CSN_PIN); }

static volatile uint8_t cc1101_pktlen_shadow;

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

// Read a CC1101 status register (single byte)
// Status regs are at 0x30-0x3D; read = addr | 0xC0 (R + burst select for status space)
/*static*/ uint8_t cc1101_read_status(uint8_t addr)
{
    uint8_t v;
    cc1101_select();
    spi1_xfer(addr | 0xC0);   // read + "status space"
    v = spi1_xfer(0x00);
    cc1101_deselect();
    return v;
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


// uint8_t cc1101_self_test(uint8_t *partnum, uint8_t *version)
// {
//     // 1) Reset chip
//     cc1101_strobe(CC1101_SRES);

//     // Datasheet note: after SRES, you must wait for SO low again before next header
//     // (our select() does that on the next transaction). :contentReference[oaicite:4]{index=4}

//     // 2) Read PARTNUM and VERSION
//     uint8_t st;
//     uint8_t pn = cc1101_read_status(CC1101_PARTNUM, &st);
//     uint8_t vr = cc1101_read_status(CC1101_VERSION, &st);

//     if (partnum)
//       *partnum = pn;
    
//     if (version)
//       *version = vr;

//     // Basic sanity:
//     // - VERSION reset value is documented as 0x14 (may vary by silicon). :contentReference[oaicite:5]{index=5}
//     // - PARTNUM is documented as 0x00 for CC1101. :contentReference[oaicite:6]{index=6}
//     if (pn == 0xFF && vr == 0xFF)
//       return 0; // typical "no slave / MISO stuck high" symptom
    
//     return 1;
// }

// --- Burst write (for FIFO or multi-reg writes) ---
static void cc1101_write_burst(uint8_t addr, const uint8_t *data, uint8_t len)
{
    if (!cc1101_select()) return;

    // Write burst: R=0, B=1
    spi1_xfer((addr & 0x3F) | CC1101_BURST);

    while (len--) {
        spi1_xfer(*data++);
    }

    cc1101_deselect();
}

void cc1101_config_gfsk_433_tx_fixed(uint8_t pkt_size)
{
    cc1101_pktlen_shadow = pkt_size;

    // Force IDLE before changing freq / packet handler fields :contentReference[oaicite:10]{index=10} :contentReference[oaicite:11]{index=11}
    cc1101_strobe(CC1101_SIDLE);
    cc1101_strobe(CC1101_SFTX);
    cc1101_strobe(CC1101_SFRX);

    // GDO0 = TX Active (asserted while in TX)
    cc1101_write_reg(CC1101_IOCFG0, 0x46);

    // ---- Sync word (default is 0xD391; keep it unless you want your own network ID) :contentReference[oaicite:12]{index=12}
    cc1101_write_reg(CC1101_SYNC1, 0xD3); // Confirmed
    cc1101_write_reg(CC1101_SYNC0, 0x91); // Confirmed

    // ---- Packet config: fixed length pkt_size, whitening ON, CRC ON, normal FIFO mode :contentReference[oaicite:13]{index=13}
    cc1101_write_reg(CC1101_PKTLEN, cc1101_pktlen_shadow);
    cc1101_write_reg(CC1101_PKTCTRL1,  0x00);   // no addr check, no append status
    cc1101_write_reg(CC1101_PKTCTRL0,  0x44);   // WHITE_DATA=1, CRC_EN=1, LENGTH_CONFIG=0 (fixed lenght configured in PACKETLEN)

    // ---- Frequency: 433.92 MHz with 26 MHz XOSC
    // f_carrier = FREQ * fXOSC / 2^16  => FREQ = 0x10B071 :contentReference[oaicite:14]{index=14}
    cc1101_write_reg(CC1101_FREQ2, 0x10); // Confirmed
    cc1101_write_reg(CC1101_FREQ1, 0xB0); // Confirmed
    cc1101_write_reg(CC1101_FREQ0, 0x71); // Confirmed

    // ---- Modem: GFSK + 38.383 kBaud + ~102 kHz RX BW
    // MDMCFG4: CHANBW_E=3, CHANBW_M=0, DRATE_E=10  => 0xCA :contentReference[oaicite:15]{index=15} :contentReference[oaicite:16]{index=16}
    // MDMCFG3: DRATE_M=0x83  => ~38.383 kBaud :contentReference[oaicite:17]{index=17}
    // MDMCFG2: DEM_DCFILT_OFF=0, MOD_FORMAT=GFSK(001), SYNC_MODE=3 (30/32, repeated sync) => 0x13 :contentReference[oaicite:18]{index=18}
    // MDMCFG1: use default-ish: 8 preamble bytes, FEC off, CHANSPC_E=2 => 0x42 :contentReference[oaicite:19]{index=19}
    // MDMCFG0: default channel spacing mantissa 0xF8 (~200 kHz with E=2) :contentReference[oaicite:20]{index=20}
    cc1101_write_reg(CC1101_MDMCFG4, 0xCA);
    cc1101_write_reg(CC1101_MDMCFG3, 0x83);
    cc1101_write_reg(CC1101_MDMCFG2, 0x13);
    cc1101_write_reg(CC1101_MDMCFG1, 0x42);
    cc1101_write_reg(CC1101_MDMCFG0, 0xF8);

    // ---- Deviation: ~20.63 kHz (E=3, M=5 => 0x35) :contentReference[oaicite:21]{index=21}
    cc1101_write_reg(CC1101_DEVIATN, 0x35);

    // ---- Output power: PATABLE[0] = 0x60 ≈ 0 dBm @ 433 MHz (multi-layer inductors table) :contentReference[oaicite:22]{index=22}
    //uint8_t pa = 0x60;
    //cc1101_write_burst(CC1101_PATABLE, &pa, 1);
    cc1101_write_reg(CC1101_FREND0, (cc1101_read_reg(CC1101_FREND0) & ~0x07));
    cc1101_write_reg(CC1101_PATABLE, 0xC0);

    // Optional: manual calibrate after changing frequency (safe + simple) :contentReference[oaicite:23]{index=23}
    cc1101_strobe(CC1101_SCAL);
}

void cc1101_send_msg(const uint8_t status, const uint32_t chip_id)
{
    uint8_t payload[5];

    payload[0] = status;
    payload[1] = (uint8_t)(chip_id >> 24);
    payload[2] = (uint8_t)(chip_id >> 16);
    payload[3] = (uint8_t)(chip_id >> 8);
    payload[4] = (uint8_t)(chip_id);

    // Make sure we're in a sane state and TX FIFO is empty
    cc1101_strobe(CC1101_SIDLE);
    cc1101_strobe(CC1101_SFTX);

    // Load payload (fixed length mode => NO length byte)
    cc1101_write_burst(CC1101_TXFIFO, payload, 5);
    //cc1101_write_burst(CC1101_TXFIFO, buffer, len);

    // Start TX (radio will send preamble+sync automatically, then payload, then CRC if enabled) :contentReference[oaicite:25]{index=25}
    cc1101_strobe(CC1101_STX);

    // Depending on MCSM1.TXOFF_MODE (default is usually IDLE), you may already be back in IDLE. :contentReference[oaicite:27]{index=27}
}

// ---- Burst read (for RX FIFO) ----
/*static*/ void cc1101_read_rxfifo(uint8_t addr, uint8_t *data, uint8_t len)
{
    if (!cc1101_select()) return;

    // Read burst: R=1, B=1
    spi1_xfer(CC1101_READ | CC1101_BURST | (addr & 0x3F));

    while (len--) {
        *data++ = spi1_xfer(0x00);
    }

    cc1101_deselect();
}

void cc1101_config_gfsk_433_rx_fixed(uint8_t pkt_size)
{
    cc1101_pktlen_shadow = pkt_size;

    cc1101_strobe(CC1101_SRES);
    cc1101_strobe(CC1101_SIDLE);
    cc1101_strobe(CC1101_SFRX);
    cc1101_strobe(CC1101_SFTX);

    // GDO0 = asserted when a packet is received and CRC is OK.
    cc1101_write_reg(CC1101_IOCFG0, 0x06);

    // Sync word must match TX
    cc1101_write_reg(CC1101_SYNC1, 0xD3);
    cc1101_write_reg(CC1101_SYNC0, 0x91);

    // Fixed length payload = pkt_size
    cc1101_write_reg(CC1101_PKTLEN, cc1101_pktlen_shadow);

    // PKTCTRL1:
    // - CRC_AUTOFLUSH = 1 (recommended when no GDO; CRC fails won't leave garbage)
    // - APPEND_STATUS = 1 (so we can read CRC_OK from FIFO)
    // - ADR_CHK = 0 (no address filtering)
    //
    // Common bit mapping: CRC_AUTOFLUSH bit3, APPEND_STATUS bit2 => 0x0C
    cc1101_write_reg(CC1101_PKTCTRL1, 0x0C);

    // PKTCTRL0: WHITE_DATA=1, CRC_EN=1, LENGTH_CONFIG=0 (fixed length configured in PACKETLEN)
    cc1101_write_reg(CC1101_PKTCTRL0, 0x44);

    // Frequency 433.92 MHz @ 26 MHz XOSC => 0x10B071 (as discussed)
    cc1101_write_reg(CC1101_FREQ2, 0x10);
    cc1101_write_reg(CC1101_FREQ1, 0xB0);
    cc1101_write_reg(CC1101_FREQ0, 0x71);

    // Modem config: GFSK, 38.383 kBaud, ~102 kHz RX BW (same as TX)
    cc1101_write_reg(CC1101_MDMCFG4, 0xCA);
    cc1101_write_reg(CC1101_MDMCFG3, 0x83);
    cc1101_write_reg(CC1101_MDMCFG2, 0x13);
    cc1101_write_reg(CC1101_MDMCFG1, 0x42);
    cc1101_write_reg(CC1101_MDMCFG0, 0xF8);

    cc1101_write_reg(CC1101_DEVIATN, 0x35);

    // Optional: calibrate after changing freq
    cc1101_strobe(CC1101_SCAL);

    uint8_t mcsm1 = cc1101_read_reg(CC1101_REG_MCSM1);
    mcsm1 = (mcsm1 & ~(0x3 << 2)) | (0x3 << 2);   // RXOFF_MODE = 3 (stay in RX)
    cc1101_write_reg(CC1101_REG_MCSM1, mcsm1);

    // Enter RX
    cc1101_strobe(CC1101_SRX);
}

static void cc1101_restart_rx(void)
{
    cc1101_strobe(CC1101_SIDLE);
    cc1101_strobe(CC1101_SFRX);
    cc1101_strobe(CC1101_SRX);
}

void cc1101_recv_msg(uint32_t *chip_id, uint8_t *status)
{
    uint8_t payload[5];
    uint8_t rx_bytes = cc1101_read_status(CC1101_RXBYTES);
    
    if (rx_bytes > 0)
    {
        cc1101_read_rxfifo(CC1101_RXFIFO, payload, rx_bytes);

        *status = payload[0];
        *chip_id = ((uint32_t)payload[1] << 24) |
                    ((uint32_t)payload[2] << 16) |
                    ((uint32_t)payload[3] <<  8) |
                    ((uint32_t)payload[4] <<  0);
    }
    return;
}
