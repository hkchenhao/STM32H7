#ifndef STM32H7_DRIVER_BSP_DIGITALOUT_H
#define STM32H7_DRIVER_BSP_DIGITALOUT_H

#include "stm32h7xx_hal.h"

class DigitalOut {
 public:
  DigitalOut(GPIO_TypeDef* const& gpio_port, uint16_t gpio_pin, GPIO_PinState gpio_pinstate = GPIO_PIN_RESET,
             uint32_t gpio_pull = GPIO_NOPULL, uint32_t gpio_speed = GPIO_SPEED_FREQ_LOW) : gpio_port_(gpio_port), gpio_pin_(gpio_pin) {
    HAL_GPIO_DeInit(gpio_port_, gpio_pin_);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = gpio_pin_;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = gpio_pull;
    GPIO_InitStruct.Speed = gpio_speed;
    HAL_GPIO_Init(gpio_port_, &GPIO_InitStruct);
    HAL_GPIO_WritePin(gpio_port_, gpio_pin_, gpio_pinstate);
  }

  void Write(GPIO_PinState gpio_pinstate) {
    HAL_GPIO_WritePin(gpio_port_, gpio_pin_, gpio_pinstate);
  }

  GPIO_PinState Read() {
    return HAL_GPIO_ReadPin(gpio_port_, gpio_pin_);
  }

  DigitalOut& operator=(GPIO_PinState gpio_pinstate) {
    HAL_GPIO_WritePin(gpio_port_, gpio_pin_, gpio_pinstate);
    return *this;
  }

 private:
  GPIO_TypeDef* gpio_port_;
  uint16_t gpio_pin_;
};

#endif
