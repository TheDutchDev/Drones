#include "stm32f4xx_hal.h"

extern void SerialLogger_OnTxComplete(UART_HandleTypeDef* huart);

static DMA_HandleTypeDef hdma_usart1_tx;

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance == USART1) {
        /* 1. Enable Peripheral Clocks */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_DMA2_CLK_ENABLE();

        /* 2. Configure GPIO Pins (PA9 = TX, PA10 = RX) */
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* 3. Configure DMA for USART1 TX (DMA2 Stream7 Channel4) */
        hdma_usart1_tx.Instance = DMA2_Stream7;
        hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        HAL_DMA_Init(&hdma_usart1_tx);

        __HAL_LINKDMA(huart, hdmatx, hdma_usart1_tx);

        /* 4. DMA interrupt init */
        HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
    }
}

void HAL_DMA_MspInit(DMA_HandleTypeDef* hdma)
{
    __HAL_RCC_DMA2_CLK_ENABLE();
}

void DMA2_Stream7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    SerialLogger_OnTxComplete(huart);
}
