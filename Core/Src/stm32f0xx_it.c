/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto: CSA
// Autor: IURI DE LIMA
// Versao: V1.0
// Date: 01/04/2025
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "stm32f0xx_it.h"
#include "perifericos.h"
#include "tabelas_protocolo.h"
#include "structs.h"
#include "mpu_6050.h"

#include <math.h>

extern TIM_HandleTypeDef htim14;
extern CAN_HandleTypeDef hcan;
extern void trata_beep(void);
void Control_PWM(void);


void NMI_Handler(void)
{
  
  while (1)
  {
  }

}


void HardFault_Handler(void)
{
  
  while (1)
  {
    
  }
}


void SVC_Handler(void)
{
  
}


void PendSV_Handler(void)
{
  
}


void SysTick_Handler(void)
{
  HAL_IncTick();
	divisor_systick++;
	if(divisor_systick > 50)
	{
		divisor_systick = 0;
		cont_buzzer++;	
	}	
	trata_beep();
	envia_leds_my(byte_leds_my);
}

void TIM14_IRQHandler(void) 
{
  HAL_TIM_IRQHandler(&htim14);
}

void CEC_CAN_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan);
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	uint32_t id;
  CAN_RxHeaderTypeDef RxHeader;
  uint8_t RxData[8];
  id = RxHeader.ExtId;
	
	if(HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxHeader,RxData) != HAL_OK)Error_Handler();
  if(id == 0x0CE60101)		
	{
		return;
  }
}
