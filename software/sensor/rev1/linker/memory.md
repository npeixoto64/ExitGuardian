# STM8L151K4T6 memory layout (for SDCC/aslink):
FLASH:
  0x8000–0x807F  vectors (128 bytes)
  0x8080–0xBFFF  code (16 KB total flash on K4 variants)

RAM:
  0x0000–0x07FF  data/stack (2 KB)

# Implemented via Makefile flags:
--code-loc 0x8080
--data-loc 0x0000
-Wl-bVECTORS=0x8000
