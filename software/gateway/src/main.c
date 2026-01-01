// Minimal main.c for STM8L151K4T6 + SDCC

static void delay(void)
{
    // crude delay; adjust as needed
    volatile unsigned long i = 0;
		for (i = 0; i < 2000UL; i++) {
    }
}

int main(void)
{
    // PA_DDR (0x5002): output
    *(volatile unsigned char*)0x5002 |= (1u << 2);

    // PA_CR1 (0x5003): push-pull
    *(volatile unsigned char*)0x5003 |= (1u << 2);

    // PA_CR2 (0x5004): slow mode (clear bit)
    *(volatile unsigned char*)0x5004 &= (unsigned char)~(1u << 2);

    // Start from a known state: drive low first (covers active-low LED case)
    *(volatile unsigned char*)0x5000 &= (unsigned char)~(1u << 2);
		
		while(1)
		{
        *(volatile unsigned char*)0x5000 ^= (1u << 2); // toggle PA2 (ODR)
        delay();
    }
}
