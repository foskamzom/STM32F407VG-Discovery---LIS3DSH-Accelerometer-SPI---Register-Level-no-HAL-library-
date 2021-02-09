#include <stm32f407xx.h>
#include "Clock_Config.h"

void Clock_Config(void)
{
  SCB->VTOR = FLASH_BASE;
  
  // Enable HSE and wait for it to activate
  RCC->CR |= RCC_CR_HSEON;
  while(!(RCC->CR & RCC_CR_HSERDY));
  
  // Set the power enable clock and voltage regulator
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR |= PWR_CR_VOS;
  
  // Configure the flash prefetch and the latency related settings
  FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;
  
  // Configure the prescalers HCLK, PCLK1 and PCLK2
  // AHB PR
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
  
  // APB PR
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
  
  // APB2 PR
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
  
  // Configure the main PLL
  RCC->PLLCFGR |= (4 << 0) | (128 << 6) | (0 << 16) | RCC_PLLCFGR_PLLSRC_HSE | (4 << 24);
  
  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY));
  
  // Select the clock source and wait for it to be set
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
