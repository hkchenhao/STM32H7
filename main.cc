#include "cpu.h"
#include "lib_mem.h"
#include "os.h"
#include "digitalout.h"
#include "SEGGER_SYSVIEW.h"

void Error_Handler() {

}

/* Private function prototypes -----------------------------------------------*/
void SystemClockConfig() {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) {}
  /**Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                                | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
}

OS_MUTEX mutex;
OS_TCB tcb_task1, tcb_task2;
CPU_STK stack_task1[128] = {0};
CPU_STK stack_task2[128] = {0};

void Task1(void* p_led) {
  auto led = (DigitalOut*)p_led;
  OS_ERR os_errnum = OS_ERR_NONE;
  while (true) {
    OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TASK_NONE, &os_errnum);
    OSMutexPend(&mutex, 0, OS_OPT_TASK_NONE, nullptr, &os_errnum);
    led->Write(GPIO_PIN_SET);
    SEGGER_SYSVIEW_PrintfTarget("hello %d", 126);
    OSMutexPost(&mutex, OS_OPT_TASK_NONE, &os_errnum);
//    led->Write(GPIO_PIN_RESET);
//    OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TASK_NONE, &os_errnum);
//    led->Write(GPIO_PIN_SET);
//    OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TASK_NONE, &os_errnum);
  }
}

void Task2(void* p_led) {
  auto led = (DigitalOut*)p_led;
  OS_ERR os_errnum = OS_ERR_NONE;
  while (true) {
    OSMutexPend(&mutex, 0, OS_OPT_TASK_NONE, nullptr, &os_errnum);
    led->Write(GPIO_PIN_SET);
    OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TASK_NONE, &os_errnum);
    OSMutexPost(&mutex, OS_OPT_TASK_NONE, &os_errnum);
//    led->Write(GPIO_PIN_RESET);
//    OSTimeDlyHMSM(0, 0, 0, 300, OS_OPT_TASK_NONE, &os_errnum);
//    led->Write(GPIO_PIN_SET);
//    OSTimeDlyHMSM(0, 0, 0, 300, OS_OPT_TASK_NONE, &os_errnum);
  }
}

int main() {
  HAL_Init();
  SystemClockConfig();
  SystemCoreClockUpdate();
  SEGGER_SYSVIEW_Conf();
  DigitalOut led_red(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  DigitalOut led_blue(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  OS_ERR os_errnum = OS_ERR_NONE;
  OS_CPU_SysTickInitFreq(SystemCoreClock);
  OSInit(&os_errnum);
  OSMutexCreate(&mutex, "mutex", &os_errnum);
  OSTaskCreate(&tcb_task1, "Task1", Task1, (void*)&led_red, 1, &stack_task1[0], 16, 128, 0, 0, nullptr, OS_OPT_TASK_NONE, &os_errnum);
  OSTaskCreate(&tcb_task2, "Task2", Task2, (void*)&led_blue, 2, &stack_task2[0], 16, 128, 0, 0, nullptr, OS_OPT_TASK_NONE, &os_errnum);
  OSStart(&os_errnum);
  while (true) {
    led_red.Write(GPIO_PIN_RESET);
    HAL_Delay(1000);
    led_red.Write(GPIO_PIN_SET);
    HAL_Delay(1000);
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif