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
  * Autor: David Carranza
  * Carnet: 23979
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// Frecuencias exactas en Hz
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_G4S 415 // Sol#
#define NOTE_A4  440
#define NOTE_AS4 466 // Sib
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define REST     0

// Definición de tiempos para controlar la velocidad (ms)
// Tiempos específicos para el estilo "Opening" (Tempo: 110 BPM)
#define SP_L   700   // Larga (para el inicio lento)
#define SP_N   450   // Negra (velocidad normal del resto)
#define SP_C   225   // Corchea
#define SP_S   112   // Semicorchea
#define SIL    150   // Silencio de articulación

// Silencios para separar frases
#define SIL_C 273   // Silencio de corchea
#define SIL_N 545   // Silencio de negra

// Para Oogway (Lento/Solemne)
#define OG_N 800   // Negra
#define OG_C 400   // Corchea
#define OG_B 1600  // Blanca

// Tiempos para Heidi (Tempo 110 BPM)
#define H_S   136  // Semicorchea
#define H_C   273  // Corchea
#define H_CP  409  // Corchea con puntillo (1.5 * Corchea)
#define H_N   545  // Negra
#define H_B   1090 // Blanca
#define H_BP  1635 // Blanca con puntillo (1.5 * Blanca)
#define H_R   2180 // Redonda

typedef struct {
    uint16_t frequency;
    uint16_t duration_ms;
} Note;

// Cancion 1: Spider-Man (Ajustada al tutorial)
// Estructura de la canción mejorada
Note cancion_spiderman[] = {
    // --- INICIO LENTO Y SOLEMNE ---
    // MI SOL SI (Notas largas y separadas)
    {NOTE_E4, SP_L}, {REST, SIL}, {NOTE_G4, SP_L}, {REST, SIL}, {NOTE_B4, 1000}, {REST, 500},

    // LA SOL MI MI (Acento en el último MI)
    {NOTE_A4, SP_N}, {NOTE_G4, SP_C}, {NOTE_E4, SP_N}, {NOTE_E5, SP_L}, {REST, 500},

    // --- ENTRA EL RITMO CONSTANTE (Más rápido) ---
    // MI SOL SI LA SOL MI (Fluido)
    {NOTE_E4, SP_N}, {NOTE_G4, SP_C}, {NOTE_B4, SP_N}, {NOTE_A4, SP_C}, {NOTE_G4, SP_C}, {NOTE_E4, SP_N}, {REST, SIL},

    // MI SOL SI (Marcado)
    {NOTE_E4, SP_C}, {NOTE_G4, SP_C}, {NOTE_B4, SP_N}, {REST, SIL},

    // DO` SI LA SOL MI (Descenso rápido)
    {NOTE_C5, SP_S}, {NOTE_B4, SP_S}, {NOTE_A4, SP_S}, {NOTE_G4, SP_S}, {NOTE_E4, SP_C}, {REST, SIL},

    // LA DO` MI` RE` DO` LA (Clímax - notas muy picadas)
    {NOTE_A4, SP_C}, {NOTE_C5, SP_C}, {NOTE_E5, SP_N}, {NOTE_D5, SP_S}, {NOTE_C5, SP_S}, {NOTE_A4, SP_C}, {REST, SIL},

    // MI SOL SI (Retorno)
    {NOTE_E4, SP_C}, {NOTE_G4, SP_C}, {NOTE_B4, SP_N}, {REST, SIL},

    // DO` SI LA SOL MI (Velocidad normal)
    {NOTE_C5, SP_C}, {NOTE_B4, SP_C}, {NOTE_A4, SP_C}, {NOTE_G4, SP_C}, {NOTE_E4, SP_N}, {REST, SIL},

    // DO` SI (Final dramático)
    {NOTE_C5, SP_N}, {NOTE_B4, 1200}, {REST, 300},

    // LA LA SOL LA SOL MI
    {NOTE_A4, SP_C}, {NOTE_A4, SP_C}, {NOTE_G4, SP_C}, {NOTE_A4, SP_C}, {NOTE_G4, SP_C}, {NOTE_E4, 1500},

    {REST, 0}
};

// Cancion 2: Oogway Ascends (Ajustada al tempo solemne)
Note cancion_oogway[] = {
    {NOTE_D4, OG_N}, {NOTE_F4, OG_N}, {NOTE_D5, OG_N}, {NOTE_C5, OG_N}, {NOTE_A4, OG_B}, // Re Fa Re' Do' La
    {REST, OG_N},
    {NOTE_G4, OG_N}, {NOTE_A4, OG_N}, {NOTE_G4, OG_N}, {NOTE_F4, OG_N}, {NOTE_D4, OG_B}, // Sol La Sol Fa Re
    {REST, OG_N},
    {NOTE_F4, OG_N}, {NOTE_A4, OG_N}, {NOTE_F5, OG_N}, {NOTE_E5, OG_N}, {NOTE_C5, OG_B}, // Fa La Fa' Mi' Do'
    {NOTE_A4, OG_C}, {NOTE_G4, OG_C}, {NOTE_A4, OG_C}, {NOTE_C5, OG_C}, {NOTE_A4, OG_C}, {NOTE_G4, OG_B}, // La Sol La Do' La Sol
    {NOTE_A4, OG_C}, {NOTE_C5, OG_C}, {NOTE_D5, OG_N}, {NOTE_D5, OG_N}, {NOTE_C5, OG_C}, {NOTE_C5, OG_B}, // La Do' Re' Re' Do' Do'
    {NOTE_A4, OG_C}, {NOTE_C5, OG_C}, {NOTE_D5, OG_N}, {NOTE_AS4, OG_N}, {NOTE_A4, OG_N}, {NOTE_A4, OG_B}, // La Do' Re' Sib La La
    {REST, OG_N},
    {NOTE_A4, OG_C}, {NOTE_C5, OG_C}, {NOTE_D5, OG_N}, {NOTE_F5, OG_N}, {NOTE_D5, OG_N}, {NOTE_C5, OG_B}, // La Do' Re' Fa' Re' Do'
    {NOTE_A4, OG_C}, {NOTE_C5, OG_C}, {NOTE_G4, OG_N}, {NOTE_F4, OG_B}, // La Do' Sol Fa
    {NOTE_D4, OG_N}, {NOTE_F4, OG_N}, {NOTE_D4, OG_N}, {NOTE_C5, OG_B}, // Re Fa Re Do'
    {NOTE_A4, OG_C}, {NOTE_C5, OG_C}, {NOTE_G4, OG_N}, {NOTE_F4, OG_N}, {NOTE_D4, OG_B}, // La Do' Sol Fa Re
    {REST, 0}
};

// Cancion 3: Abulito dime tu (Heidy)
Note cancion_heidi[] = {
    {NOTE_E4, H_S}, {NOTE_F4, H_S},                         // MI FA (semi corcheas)
    {NOTE_G4, H_C}, {NOTE_G4, H_C}, {NOTE_G4, H_C}, {NOTE_E5, H_C}, // SOL SOL SOL MI' (Corcheas)
    {NOTE_C5, H_N}, {REST, H_C},                            // DO' (negra) + silencio corchea

    {NOTE_E4, H_S}, {NOTE_F4, H_S},                         // MI FA (semicorcheas)
    {NOTE_G4, H_C}, {NOTE_G4, H_C},                         // SOL SOL (Corchas)
    {NOTE_G4, H_CP}, {NOTE_G4, H_S},                        // SOL con puntillo, Sol semicorchea
    {NOTE_A4, H_C}, {NOTE_G4, H_C}, {NOTE_F4, H_C},         // LA SOL FA (corchea)

    {NOTE_D4, H_S}, {NOTE_E4, H_S},                         // RE MI (Semicorchea)
    {NOTE_F4, H_C}, {NOTE_F4, H_C}, {NOTE_F4, H_C}, {NOTE_D5, H_C}, // FA FA FA RE' (corchea)
    {NOTE_B4, H_N}, {REST, H_C},                            // Si (negra) + silencio corchea

    {NOTE_A4, H_C}, {NOTE_G4, H_N},                         // La (corchea), Sol negra
    {NOTE_G4, H_C}, {NOTE_D5, H_C}, {NOTE_E5, H_C}, {NOTE_D5, H_C}, // SOL RE' MI' RE' (corchea)
    {NOTE_C5, H_N},                                         // Do' negra

    {NOTE_F5, H_S}, {NOTE_F5, H_N},                         // Fa' (semicorchea), FA' Negra
    {NOTE_F5, H_S}, {NOTE_F5, H_N},                         // Fa' (semicorchea), FA' Negra
    {NOTE_A4, H_C}, {NOTE_A4, H_C}, {NOTE_E5, H_N},         // La La (corchea), MI' negra
    {NOTE_E5, H_C}, {NOTE_E5, (H_C + H_B)},                 // MI' corchea, MI' (corchea + blanca)

    {NOTE_D5, H_C}, {NOTE_D5, H_N},                         // RE' (corchea), RE' negra
    {NOTE_D5, H_C}, {NOTE_D5, H_N},                         // RE' (corchea), RE' negra
    {NOTE_D5, H_C}, {NOTE_F5, H_C}, {NOTE_E5, H_N},         // RE' FA' (corchea), MI' Negra
    {NOTE_D5, H_S}, {NOTE_C5, (H_S + H_N)},                 // RE' (semicorchea) DO' (semicorchea + negra)

    {NOTE_C5, H_S}, {NOTE_C5, H_S},                         // DO' DO' (Semicorchea)
    {NOTE_F5, H_S}, {NOTE_F5, (H_S + H_BP)},                // FA' (Semicorchea) FA' (semicorchea + blanca con puntillo)
    {REST, H_N},                                            // silencio de negra

    {NOTE_A4, H_N}, {NOTE_C5, H_N}, {NOTE_F5, H_N}, {NOTE_E5, H_N},
    {NOTE_E5, H_N}, {NOTE_D5, H_N}, {NOTE_D5, H_N},         // La do FA' MI' MI' RE' RE' (negra)
    {NOTE_C5, H_B}, {REST, H_N},                            // Do' blanca, silencio de negra

    // Sección de estribillo con alteraciones
    {NOTE_G4, H_CP}, {NOTE_G4S, H_S},                       // Sol (corchea con puntillo), Sol# (semicorechea)
    {NOTE_A4, H_C}, {NOTE_F5, H_C}, {NOTE_A4, H_C}, {NOTE_F5, H_C}, // LA FA' LA FA' (Corchea)
    {NOTE_A4, H_N},                                         // La negra

    {NOTE_A4, H_CP}, {NOTE_G4S, H_S},                       // La (corchea con puntillo), Sol# (semicorechea)
    {NOTE_G4, H_S}, {NOTE_E5, H_S}, {NOTE_G4, H_S}, {NOTE_E5, H_S}, // SOL MI' SOL MI' (semicorchea)
    {NOTE_G4, H_N},                                         // sol negra

    {NOTE_G4, H_CP}, {NOTE_G4S, H_S},                       // Sol (corchea con puntillo), Sol# (semicorechea)
    {NOTE_F4, H_N}, {NOTE_D5, H_N}, {NOTE_C5, H_N}, {NOTE_B4, H_N},
    {NOTE_C5, H_N}, {NOTE_D5, H_N}, {NOTE_E5, H_N},         // FA RE' DO' SI DO' RE' MI' (negra)

    {NOTE_G4, H_CP}, {NOTE_G4S, H_S},                       // Sol (corchea con puntillo), Sol# (semicorechea)
    {NOTE_A4, H_S}, {NOTE_F5, H_S}, {NOTE_A4, H_S}, {NOTE_F5, H_S}, // LA FA' LA FA' (Semicorchea)
    {NOTE_A4, H_N},                                         // La negra

    {NOTE_A4, H_CP}, {NOTE_G4S, H_S},                       // La (corchea con puntillo), Sol# (semicorechea)
    {NOTE_G4, H_S}, {NOTE_E5, H_S}, {NOTE_G4, H_S}, {NOTE_E5, H_S}, // SOL MI ' SOL MI' (semicorchea)
    {NOTE_G4, H_N},                                         // sol negra

    {NOTE_G4, H_CP}, {NOTE_G4S, H_S},                       // Sol (corchea con puntillo), Sol# (semicorechea)
    {NOTE_F4, H_N}, {NOTE_D5, H_N}, {NOTE_C5, H_N}, {NOTE_B4, H_N}, // Fa RE' DO' SI (negra)
    {NOTE_C5, H_R},                                         // Do' (redonda)
    {REST, 0}
};

char msg_menu[] = "\r\n--- Reproductor UVG ---\r\n1. Spider-Man (PWM)\r\n2. Oogway Ascends (PWM)\r\n3. Abuelito Dime Tú (PWM)\r\nSeleccion: ";
uint8_t rx_data;

// Tabla senoidal de 8 bits (0-255)
const uint8_t sine_table[32] = {
    128, 153, 177, 199, 219, 234, 246, 253, 255, 253, 246, 234, 219, 199, 177, 153,
    128, 103, 79, 57, 37, 22, 10, 3, 0, 3, 10, 22, 37, 57, 79, 103
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
void Play_Tone(uint16_t frequency, uint16_t duration);
void Play_Song(Note song[]);
void Play_Oogway_DAC(Note *cancion);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_DAC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Enviar el menú a la terminal
      HAL_UART_Transmit(&huart2, (uint8_t*)msg_menu, strlen(msg_menu), 100);

      // Bloquear hasta recibir un carácter
      HAL_UART_Receive(&huart2, &rx_data, 1, HAL_MAX_DELAY);

      if (rx_data == '1') {
          HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nReproduciendo Spider-Man...\r\n", 20, 100);
          HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
          Play_Song(cancion_spiderman);
          HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
      }
      else if (rx_data == '2') {
          HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nReproduciendo Oogway Ascends...\r\n", 17, 100);
          HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
          Play_Song(cancion_oogway);
          HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
      }
      else if (rx_data == '3') {
    	  HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nReproduciendo Abuelito Dime Tú (Heidy)...\r\n", 16, 100);
    	  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    	  Play_Song(cancion_heidi);
    	  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
      }
      else if (rx_data == '4') {
    	  HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nOogway DAC (Seno)...\r\n", 24, 100);
    	  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    	  Play_Oogway_DAC(cancion_oogway);
    	  HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
      }
      else {
          HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nOpcion no valida.\r\n", 13, 100);
      }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 83;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 83;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 124;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void Play_Tone(uint16_t frequency, uint16_t duration) {
    if (frequency == REST) {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    } else {
        // Como el timer corre a 1MHz (84MHz / 84), el ARR es 1,000,000 / frecuencia
        uint32_t arr_value = (1000000 / frequency) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim3, arr_value);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, arr_value / 2); // 50% Duty Cycle
    }
    HAL_Delay(duration);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0); // Silencio breve entre notas
    HAL_Delay(25);
}

void Play_Song(Note song[]) {
    int i = 0;
    while (song[i].duration_ms != 0) {
        Play_Tone(song[i].frequency, song[i].duration_ms);
        i++;
    }
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0); // Apagar al finalizar
}

void Play_Oogway_DAC(Note *cancion) {
    for (int i = 0; cancion[i].duration_ms != 0; i++) {
        uint32_t freq = cancion[i].frequency;
        uint32_t dur = cancion[i].duration_ms;

        if (freq == REST) {
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_8B_R, 0);
            HAL_Delay(dur);
        } else {
            uint32_t delay_us = (1000000 / (freq * 32));
            uint32_t start_tick = HAL_GetTick();

            while ((HAL_GetTick() - start_tick) < dur) {
                for (int j = 0; j < 32; j++) {
                    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_8B_R, sine_table[j]);
                    for (volatile int wait = 0; wait < delay_us; wait++);
                }
            }
        }
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_8B_R, 0);
        HAL_Delay(20);
    }
}
/* USER CODE END 4 */

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
