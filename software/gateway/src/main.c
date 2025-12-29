// Minimal main.c for STM8L151K4T6 + SDCC

int main(void)
{
    *(volatile unsigned char*)0x500C |=  (1u << 4);              // PC_DDR: output
    *(volatile unsigned char*)0x500D |=  (1u << 4);              // PC_CR1: push-pull
    *(volatile unsigned char*)0x500E |= (unsigned char)~(1u<<4); // PC_CR2: slow
    *(volatile unsigned char*)0x500A |=  (1u << 4);              // PC_ODR: high

    for (;;)
        ;

    return 0;
}
