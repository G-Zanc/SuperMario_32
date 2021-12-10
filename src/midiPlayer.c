#include "stm32f0xx.h"
#include <math.h>
#include <stdint.h>
#include "midi.h"
#include "step.h"
#include <stdio.h>
#include <time.h>

short int wavetable[N];
extern uint8_t flag[];
extern uint8_t overworld3[];
extern uint8_t death[];
int marioDeathMusic = 0;
MIDI_Player *music;
extern int gameOverMusic;
extern int flagWinMusic;
extern int musicFlip;
extern int musicFlipWin;

void __attribute__((optimize("O0"))) delay_cycles(uint32_t cyc) {
  uint32_t d_i;
  for (d_i = 0; d_i < cyc; ++d_i) {
    asm("NOP");
  }
}

void init_wavetable(void)
{
    for(int i=0; i < N; i++)
    {
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
    }
}


void enableDAC1Ports()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 0x00000300;

    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0x0000000f;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_1 | GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR5_1;
}


void setup_DAC()
{
    //SET RCC CLOCK
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    //ENABLE TRIGGER FOR CHANNEL AND CHANNEL
    DAC->CR |= DAC_CR_TEN1 | DAC_CR_TSEL1;
    DAC->CR |= DAC_CR_EN1;
}

void init_tim6()
{
    //TIMER ENABLE AND RATE
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 0;
    TIM6->ARR = (48000000 / (RATE*(TIM6->PSC + 1)) - 1);

    TIM6->DIER |= TIM_DIER_UIE;

    //TIMER ENABLE
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM6_DAC_IRQn;
    NVIC_SetPriority(TIM6_DAC_IRQn, 1);
}

void TIM6_DAC_IRQHandler(void)
{
    TIM6->SR &= ~TIM_SR_UIF;
    DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;

    int sample = 0;

    for(int i=0; i < sizeof voice / sizeof voice[0]; i++)
    {
        if(voice[i].step != 0)
        {
            sample += (wavetable[voice[i].offset>>16] * voice[i].volume);
            voice[i].offset += voice[i].step;
            if ((voice[i].offset >> 16) >= sizeof wavetable / sizeof wavetable[0])
                voice[i].offset -= (sizeof wavetable / sizeof wavetable[0]) << 16;
        }
    }

    sample = (sample >> 16) + 2048;
    if (sample > 4095) sample = 4065;
    DAC->DHR12R1 = sample;
}



void init_tim2()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 48 - 1;
    TIM2->ARR = RATE - 1;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM2_IRQn;
    NVIC_SetPriority(TIM2_IRQn, 1);
}



void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;
    midi_play();

    if(gameOverMusic != 0)
    {
        //music = midi_init(overworld3);
        music = midi_init(death);
        //delay_cycles(2000000);
    }
    if(flagWinMusic != 0)
        {
            //music = midi_init(overworld3);
            music = midi_init(flag);
            //delay_cycles(2000000);
        }

    if (music->nexttick >= MAXTICKS && musicFlip == 0 && musicFlipWin == 0)
        midi_init(overworld3);
}




void set_tempo(int time, int value, const MIDI_Header *hdr)
{
    TIM2->ARR = value/hdr->divisions - 1;
}

void note_on(int time, int chan, int key, int velo)
{
  if(velo == 0)
  {
      note_off(time, chan, key, velo);
      return;
  }

  for(int i=0; i < sizeof voice / sizeof voice[0]; i++)
  {
      if (voice[i].step == 0)
      {
          // configure this voice to have the right step and volume
          voice[i].step = step[key];
          voice[i].note = key;
          voice[i].volume = velo;
          break;
      }
  }
}

void note_off(int time, int chan, int key, int velo)
{
  for(int i=0; i < sizeof voice / sizeof voice[0]; i++)
  {
    if (voice[i].step != 0 && voice[i].note == key)
    {
      // turn off this voice
      voice[i].step = 0;
      voice[i].note = 0;
      voice[i].volume = 0;
      break;
    }
  }
}


void note_off2(int time, int chan, int key, int velo)
{
  for(int i=0; i < sizeof voice2 / sizeof voice2[0]; i++)
  {
    if (voice2[i].step != 0 && voice2[i].note == key)
    {
      // turn off this voice
      voice2[i].step = 0;
      voice2[i].note = 0;
      voice2[i].volume = 0;
      break;
    }
  }
}


int midiPlayerMusic(void)
{
   music = midi_init(overworld3);


    /*for(;;)
        asm("wfi");*/

}
