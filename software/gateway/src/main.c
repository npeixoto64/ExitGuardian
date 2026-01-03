#include <stdint.h>

#define PA_ODR (*(volatile uint8_t*)0x5000)
#define PA_DDR (*(volatile uint8_t*)0x5002)
#define PA_CR1 (*(volatile uint8_t*)0x5003)
#define PA_CR2 (*(volatile uint8_t*)0x5004)
#define PA2 (1u << 2)
#define PA3 (1u << 3)
#define PA4 (1u << 4)

volatile uint32_t counter = 5;
volatile uint32_t zero = 0;

static void delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 20000UL; i++) {}
}

int main(void)
{
  counter = 5;
  zero = 0;

    PA_DDR |= PA2;
    PA_CR1 |= PA2;
    PA_CR2 &= (uint8_t)~PA2;

    PA_DDR |= PA3;
    PA_CR1 |= PA3;
    PA_CR2 &= (uint8_t)~PA3;

    PA_DDR |= PA4;
    PA_CR1 |= PA4;
    PA_CR2 &= (uint8_t)~PA4;

    if (counter == 5)
      PA_ODR |= PA3;

    if (zero == 0)
      PA_ODR |= PA4;

    while (1) {
        PA_ODR ^= PA2;
        delay();
    }
}
