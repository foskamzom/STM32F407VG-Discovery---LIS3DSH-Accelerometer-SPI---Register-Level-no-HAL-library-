/******************************************************************************************************

Project : STM32F407VG Discovery Board - LIS3DSH Accelerometer SPI with No HAL library (Register Level)

Device  : STM32F407VG

Author  : Firat Ozkan

NOTE : I developed this project with help of Hemant Nile's GitHub repository. Shout out to Hemant Nile
for providing this project with in-depth explanation.
URL : https://github.com/hemantnile/stm32f4-discovery-SPI
******************************************************************************************************/

#include <stm32f407xx.h>
#include "Clock_Config.h"

void GPIO_Config(void);
void SPI_Config(void);
void Spi_Tx (unsigned char address, unsigned char data);
unsigned char Spi_Rx (unsigned char address);

static signed char x,y,z;

int main(void)
{
  Clock_Config();
  GPIO_Config();
  SPI_Config();
  
  // Output data rate 100 Hz, Continuous Update, X enable, Y enable Z enable
  Spi_Tx(0x20U, 0x67U);
  
  // Full-scale : 101
  Spi_Tx(0x24U, 0x48U);
  
  while (1)
  {
    // Read OUT_X_H
    x = (signed char)Spi_Rx(0x29);
    // Read OUT_Y_H
    y = (signed char)Spi_Rx(0x2B);
    // Read OUT_Z_H
    z = (signed char)Spi_Rx(0x2D);
    
    if(x < -20)
      GPIOD->ODR |= (1 << 12);
    else
      GPIOD->ODR &= ~(1U << 12);
    if(x > 20)
      GPIOD->ODR |= (1 << 14);
    else
      GPIOD->ODR &= ~(1U << 14);
    if(y < -20)
      GPIOD->ODR |= (1 << 15);
    else
      GPIOD->ODR &= ~(1U << 15);
    if(y > 20)
      GPIOD->ODR |= (1 << 13);
    else
      GPIOD->ODR &= ~(1U << 13);
  }
}

void GPIO_Config(void)
{
  // Peripheral clock enable for GPIOD
  RCC->AHB1ENR |= (1 << 3);
  
  // Output mode for GPIOD pins 12, 13, 14 and 15
  GPIOD->MODER = (1 << 24) | (1 << 26) | (1 << 28) | (1 << 30);
  
  // High speed mode for GPIOD pins 12, 13, 14 and 15
  GPIOD->OSPEEDR = (3U << 24) | (3U << 26) | (3U << 28) | (3U << 30);
}

void SPI_Config(void)
{
  // Peripheral clock enable for GPIOA and GPIOE
  RCC->AHB1ENR |= 1 | (1 << 4);
  
  // Alternate mode for GPIOA pins 5, 6 and 7
  GPIOA->MODER |= (1 << 11) | (1 << 13) | (1 << 15);
  
  // High speed mode for GPIOA pins 5, 6 and 7
  GPIOA->OSPEEDR |= (3U << 10) | (3U << 12) | (3U << 14);
  
  // Pull-up selected for GPIOA pins 5, 6 and 7
  GPIOA->PUPDR = (1 << 10) | (1 << 12) | (1 << 14);
  
  // Alternate mode 5 selected for pins 5, 6 and 7 (SPI pins)
  GPIOA->AFR[0] = (5U << 20) | (5U << 24) | (5U << 28);
  
  // Output mode for GPIOE pin 3
  GPIOE->MODER |= (1 << 6);
  
  // High speed mode for GPIOE pin 3
  GPIOE->OSPEEDR = (3U << 6);
  
  // Pull-up selected for GPIOE pin 3
  GPIOE->PUPDR = (1 << 6);
  
  // Peripheral clock enable for SPI1
  RCC->APB2ENR |= (1 << 12);
  
  // Spi configurations
  SPI1->CR1 = SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_BR_1 | SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
  
  // Drive GPIOE pin 3 HIGH
  GPIOE->ODR = (1 << 3);
  
  // Enable SPI
  SPI1->CR1 |= SPI_CR1_SPE;
}

void Spi_Tx (unsigned char address, unsigned char data)
{
  // Temporary data for useless reads
  unsigned char temp;
  
  // Drive GPIOE pin 3 LOW
  GPIOE->ODR = 0;
  
  // Wait for TX buffer to be emptied
  while(!(SPI1->SR & (1 << 1)));
  
  // Send write address
  SPI1->DR = address;
  
  // Wait for RX buffer to be buffed
  while(!(SPI1->SR & (1 << 0)));
  
  // Temp read
  temp = (unsigned char)SPI1->DR;
  
  // Wait for TX buffer to be emptied
  while(!(SPI1->SR & (1 << 1)));
  
  // Write data
  SPI1->DR = data;
  
  // Wait for RX buffer to be buffed
  while(!(SPI1->SR & (1 << 0)));
  
  // Temp read
  temp = (unsigned char)SPI1->DR;
  
  // Wait for SPI communication to end
  while(SPI1->SR & (1 << 7));
  
  // Drive GPIOE pin 3 HIGH
  GPIOE->ODR = (1 << 3);
}

unsigned char Spi_Rx (unsigned char address)
{
  unsigned char data;
  
  // First bit 1 for read
  address |= 0x80;
  
  // Drive GPIOE pin 3 LOW
  GPIOE->ODR = 0;
  
  // Wait for TX buffer to be emptied
  while(!(SPI1->SR & (1 << 1)));
  
  // Send read address
  SPI1->DR = address;
  
  // Wait for RX buffer to be buffed
  while(!(SPI1->SR & (1 << 0)));
  
  // Temp read
  data = (unsigned char)SPI1->DR;
  
  // Wait for TX buffer to be emptied
  while(!(SPI1->SR & (1 << 1)));
  SPI1->DR = 0x00U;
  
  // Wait for RX buffer to be buffed
  while(!(SPI1->SR & (1 << 0)));
  
  // Read data
  data = (unsigned char)SPI1->DR;
  
  // Wait for SPI communication to end
  while(SPI1->SR & (1 << 7));
  
  // Drive GPIOE pin 3 HIGH
  GPIOE->ODR = (1 << 3);
  
  return data;
}
