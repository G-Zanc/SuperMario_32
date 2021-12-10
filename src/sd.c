#include "stm32f0xx.h"
#include "sd.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include "ff.h"

void init_spi1_slow(void)
{

    // Enable the RCC
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Disable
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // Set the baud rate divisor to the maximum value to make the SPI baud rate as low as possible.
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

    // Set it to Master Mode.
    SPI1->CR1 |= SPI_CR1_MSTR;

    // Set the word size to 8-bit.
    SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;
    SPI1->CR2 &= ~SPI_CR2_DS_3;

    // Configure "Software Slave Management" and "Internal Slave Select".
    SPI1->CR1 |= SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SSI;

    // Set the "FIFO reception threshold" bit in CR2 so that the SPI channel immediately releases a received 8-bit value.
    SPI1->CR2 |= SPI_CR2_FRXTH; //| SPI_CR2_RXNEIE;

    // Try this!
    SPI1->CR2 |= SPI_CR2_NSSP | SPI_CR2_SSOE;
    //SPI_RxFIFOThresholdConfig(SPI1 ,SPI_RxFIFOThreshold_QF);

    // Enable the SPI channel.
    SPI1->CR1 |= SPI_CR1_SPE;
}

void enable_sdcard(void)
{
    GPIOB->BRR = GPIO_ODR_2; // Set PB2 low to enable SD card;
}

void disable_sdcard(void)
{
    GPIOB->BSRR = GPIO_ODR_2; // Set PB2 high to disable SD card;
}

void init_sdcard_io(void)
{
    init_spi1_slow();
    disable_sdcard();
}

void sdcard_io_high_speed()
{
    // Disable
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // Set the baud rate
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_BR_0;
    //SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;
    // Enable the SPI channel.
    SPI1->CR1 |= SPI_CR1_SPE;
}

int read_highscore(const char* file)
{

    FATFS fs;
    FIL F;
    int score = 0;
    char byte[4];
    //char *p = byte;

    // File is found and good to go
    if (f_mount(&fs, "", 0) != FR_OK) return(0);
    if (f_open (&F, file, FA_READ) != FR_OK) return(0);
    //f_read(&F, &score, 4, &bytes);

    f_gets(byte, sizeof byte, &F); // Get the line
    //score = byte[0];
    score = atoi(byte);
    f_close (&F);
    return score;

}

void write_highscore(const char* file, int score)
{
    FATFS fs;
    FIL F;
    char write_score[4];
    char * p = write_score;
    sprintf(write_score, "%d", score);
    // File is found and good to go
    if (f_mount(&fs, "", 0) != FR_OK) return;
    if (f_open (&F, file, FA_WRITE) != FR_OK) return;
    f_puts(p,&F);
    //f_write(&F, &write_score,(sizeof write_score), &bytes);
    f_close (&F);
}

void clear_highscore(const char* file)
{
    FATFS fs;
    FIL F;
    int bytes;
    char write_score[4] = "----";
    char * p = write_score;
    // File is found and good to go
    if (f_mount(&fs, "", 0) != FR_OK) return;
    if (f_open (&F, file, FA_WRITE) != FR_OK) return;
    f_puts(p,&F);
    f_close (&F);
}
