/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* DUAL_CORE_BOOT_SYNC_SEQUENCE: Define for dual core boot synchronization    */
/*                             demonstration code based on hardware semaphore */
/* This define is present in both CM7/CM4 projects                            */
/* To comment when developping/debugging on a single core                     */
#define DUAL_CORE_BOOT_SYNC_SEQUENCE

#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */

#define MPU_DEMO_REGION_NUMBER       MPU_REGION_NUMBER15
#define MPU_DEMO_REGION_SIZE         MPU_REGION_SIZE_32B
#define MPU_DEMO_BUFFER_WORDS        8U
#define MPU_DEMO_TRIGGER_DELAY_MS    2000U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static volatile uint32_t g_mpuDemoBuffer[MPU_DEMO_BUFFER_WORDS] __attribute__((aligned(32)));
static volatile uint32_t g_mpuDemoTargetAddress = 0U; /* 记录目标地址 */
static volatile uint32_t g_mpuDemoFaultAddress = 0U;  /* 记录故障地址 */
static volatile uint32_t g_mpuDemoCfsr = 0U;          /* 记录CFSR */
static volatile uint8_t g_mpuDemoFaultSeen = 0U;      /* 标记是否已打印 */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE BEGIN 0 */

static void MPU_Demo_PrintChar(char ch)
{
    while ((huart1.Instance->ISR & USART_ISR_TXE_TXFNF) == 0U)
    {
    }

    huart1.Instance->TDR = (uint8_t)ch;
}

static void MPU_Demo_PrintString(const char *text)
{
    while (*text != '\0')
    {
        MPU_Demo_PrintChar(*text);
        text++;
    }

    while ((huart1.Instance->ISR & USART_ISR_TC) == 0U)
    {
    }
}

static void MPU_Demo_PrintLine(const char *text)
{
    MPU_Demo_PrintString(text);
    MPU_Demo_PrintString("\r\n");
}

static void MPU_Demo_PrintHex32(uint32_t value)
{
    static const char hex_table[] = "0123456789ABCDEF";
    uint32_t shift = 28U;

    MPU_Demo_PrintString("0x");

    while (1)
    {
        MPU_Demo_PrintChar(hex_table[(value >> shift) & 0x0FU]);

        if (shift == 0U)
        {
            break;
        }

        shift -= 4U;
    }
}

static void MPU_Demo_PrintLabelAndHex32(const char *label, uint32_t value)
{
    MPU_Demo_PrintString(label);
    MPU_Demo_PrintHex32(value);
    MPU_Demo_PrintString("\r\n");
}


/* MPU 只读区域配置 */
static void MPU_Demo_ConfigureReadOnlyRegion(void)
{
    MPU_Region_InitTypeDef mpu_init = {0};

    HAL_MPU_Disable();

    mpu_init.Enable = MPU_REGION_ENABLE;
    mpu_init.Number = MPU_DEMO_REGION_NUMBER;
    mpu_init.BaseAddress = (uint32_t)g_mpuDemoBuffer;
    mpu_init.Size = MPU_DEMO_REGION_SIZE;
    mpu_init.SubRegionDisable = 0x00;
    mpu_init.TypeExtField = MPU_TEX_LEVEL0;
    mpu_init.AccessPermission = MPU_REGION_PRIV_RO;
    mpu_init.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    mpu_init.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    mpu_init.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    mpu_init.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&mpu_init);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


/* MPU 实验测试 */
static void MPU_Demo_Run(void)
{
    volatile uint32_t read_value;

    MPU_Demo_PrintLine("");
    MPU_Demo_PrintLine("===== STM32H7 MPU demo =====");
    MPU_Demo_PrintLine("Demo object: a 32-byte aligned SRAM buffer.");
    MPU_Demo_PrintLabelAndHex32("Protected buffer base = ",
                               (uint32_t)g_mpuDemoBuffer);

    /* 第一次正常写入 */
    g_mpuDemoBuffer[0] = 0x12345678U;

    MPU_Demo_PrintLine("1) Before read-only protection: first write succeeds.");
    MPU_Demo_PrintLabelAndHex32("Buffer[0] = ", g_mpuDemoBuffer[0]);

    /* 配置 MPU 只读保护 */
    MPU_Demo_ConfigureReadOnlyRegion();

    MPU_Demo_PrintLine("2) MPU enabled: region15 is now privileged read-only.");
    MPU_Demo_PrintLine("   Region size = 32 bytes, XN = 1.");

    /* 验证读取权限 */
    read_value = g_mpuDemoBuffer[0];

    MPU_Demo_PrintLine("3) Read access is still allowed.");
    MPU_Demo_PrintLabelAndHex32("Read back = ", read_value);

    /* 延时后故意写入只读区域 */
    MPU_Demo_PrintLine("4) After 2 seconds, code will write the same buffer again.");
    MPU_Demo_PrintLine("   Expected result: enter MemManage_Handler and print fault info.");

    HAL_Delay(MPU_DEMO_TRIGGER_DELAY_MS);

    g_mpuDemoTargetAddress = (uint32_t)&g_mpuDemoBuffer[0];
    g_mpuDemoBuffer[0] = 0xA5A5A5A5U;

    MPU_Demo_PrintLine("Unexpected: write succeeded. MPU did not trigger.");
}


/* MPU 异常信息处理 */
void App_MemManageHandler(void)
{
    if (huart1.Instance == NULL)
    {
        return;
    }

    if (g_mpuDemoFaultSeen == 0U)
    {
        g_mpuDemoFaultSeen = 1U;
        g_mpuDemoCfsr = SCB->CFSR;
        g_mpuDemoFaultAddress = SCB->MMFAR;

        MPU_Demo_PrintLine("");
        MPU_Demo_PrintLine(">>> MemManage fault captured <<<");

        if ((g_mpuDemoCfsr & SCB_CFSR_DACCVIOL_Msk) != 0U)
        {
            MPU_Demo_PrintLine("Reason = data access violation");
        }

        if ((g_mpuDemoCfsr & SCB_CFSR_MMARVALID_Msk) != 0U)
        {
            MPU_Demo_PrintLabelAndHex32("Fault address = ",
                                       g_mpuDemoFaultAddress);
        }

        MPU_Demo_PrintLabelAndHex32("Target address = ",
                                   g_mpuDemoTargetAddress);

        MPU_Demo_PrintLabelAndHex32("SCB->CFSR = ",
                                   g_mpuDemoCfsr);

        MPU_Demo_PrintLine("MPU blocked the write. CPU stays in MemManage_Handler.");
    }
}

/* USER CODE END 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  int32_t timeout;
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_0 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
timeout = 0xFFFF;
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
if ( timeout < 0 )
{
Error_Handler();
}
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  MPU_Demo_Run();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
 
    /* USER CODE BEGIN 3 */
    printf("[CM7] System Running...\r\n");
    HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
