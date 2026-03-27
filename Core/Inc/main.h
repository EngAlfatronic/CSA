/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto: CSA
// Autor: IURI DE LIMA
// Versao: V1.0
// Date: 01/04/2025
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

void Error_Handler(void);
/* Private defines -----------------------------------------------------------*/
#define rot_1_Pin GPIO_PIN_0
#define rot_1_GPIO_Port GPIOA
#define rot_2_Pin GPIO_PIN_1
#define rot_2_GPIO_Port GPIOA
#define ad_bateria_Pin GPIO_PIN_2
#define ad_bateria_GPIO_Port GPIOA
#define entrada_manual_Pin GPIO_PIN_3
#define entrada_manual_GPIO_Port GPIOA
#define saida_alimenta_Pin GPIO_PIN_4
#define saida_alimenta_GPIO_Port GPIOA
#define entrada_carregador_Pin GPIO_PIN_5
#define entrada_carregador_GPIO_Port GPIOA
#define latch_Pin GPIO_PIN_7
#define latch_GPIO_Port GPIOA
#define clk_Pin GPIO_PIN_0
#define clk_GPIO_Port GPIOB
#define sdi_Pin GPIO_PIN_1
#define sdi_GPIO_Port GPIOB
#define buzzer_Pin GPIO_PIN_8
#define buzzer_GPIO_Port GPIOA
#define in_profet_1_Pin                                    GPIO_PIN_6
#define in_profet_1_GPIO_Port                              GPIOA

#define in_profet_2_Pin                                    GPIO_PIN_5
#define in_profet_2_GPIO_Port                              GPIOB
#define hab_prof_1_Pin GPIO_PIN_15
#define hab_prof_1_GPIO_Port GPIOA
#define hab_prof_2_Pin GPIO_PIN_3
#define hab_prof_2_GPIO_Port GPIOB
#define rele_Pin GPIO_PIN_4
#define rele_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#ifndef DEFINES_ALFA
	#define DEFINES_ALFA
	
	#define limite_menos_vm       		-50
	#define	limite_menos_am       		-30
	#define	limite_mais_vm         		50
	#define	limite_mais_am         		30
	#define limite_mais_pwm        		10
	#define limite_menos_pwm      		-10
	#define atuador_retorno						TIM1->CCR3
	#define atuador_avanco  					TIM1->CCR2
	#define tensao_maxima         		25.0f
	#define tensao_min_manual					19.0f
	#define tensao_min_automa					21.0f
	#define pwm_atuador_max						3200
	#define pwm_atuador_min						320
	#define pwm_atuador_man_ini				400
	#define angulo_acionamento_pid 		1.5f
	#define histerese_acionamento_pid 0.5f
	
	#define PONTO_LENTO     					800   
	#define PONTO_MEDIO     					2000   
	#define PONTO_RAPIDO    					2500   

	#define PASSO_RAMPA_LENTO   			5
	#define PASSO_RAMPA_MEDIO   			10
	#define PASSO_RAMPA_RAPIDO  			30
	
	#define tempo_acionamento 				500
	
	#define FIM_CURSO_CARR						25000
	#define FIM_CURSO_DESCARR					35000
	
	#define QUATRO_HORAS							14400000
	#define MILIS_PARA_SEGUNDOS   		1000.0f
	#define TEMPO_INVERSAO_ATUADOR		500
	
	#define LEITURAS_BATERIA					10
	
	#define IS_THRESHOLD_FALHA				3900
	#define IS_CONT_FALHA							3
#endif

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
