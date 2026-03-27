/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto CSA
// Autor  Iuri Lima
// Versaon V1.0
// Date 11/07/2025
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "mpu_6050.h"
#include "perifericos.h"
#include "flash.h"
#include "structs.h"
#include "calibracao.h"

#include <math.h>

void calibracao(void)
{
	static unsigned short int cont_calibra;
	while((HAL_GPIO_ReadPin(entrada_manual_GPIO_Port,entrada_manual_Pin) == 0)&(HAL_GPIO_ReadPin(rot_1_GPIO_Port,rot_1_Pin) == 1)&(bit_calibra == 0))
	{
		cont_calibra++;
		if(cont_calibra >= 500)
		{
			cont_calibra = 0;
			bit_calibra = 1;
			zera_inclinometro();
		}
	}
	
	while((HAL_GPIO_ReadPin(entrada_manual_GPIO_Port,entrada_manual_Pin) == 0)&(HAL_GPIO_ReadPin(rot_2_GPIO_Port,rot_2_Pin) == 1)&(bit_calibra == 0))
	{
		cont_calibra++;
		if(cont_calibra >= 500)
		{
			cont_calibra = 0;
			bit_calibra = 1;
			calibracao_eixo();
		}
	}
	
	if(bit_calibra == 1)
	{
		salva_eeprom = 1;
		write_flash();
	}
		
	bit_calibra = 0;
}

void leds_calibracao(void)
{
	static unsigned char contador_leds, acionamento_leds, inverte_acionamento;
	
	contador_leds++;
	if(contador_leds >= 20)
	{
		contador_leds = 0;
	
		if(inverte_acionamento == 0)
		{
			acionamento_leds++;
			if(acionamento_leds > 5)inverte_acionamento = 1;
		}
		else
		{
			acionamento_leds--;
			if(acionamento_leds <= 0)inverte_acionamento = 0;
		}
		
		if(acionamento_leds >= 1) led_6 = 1;
		else										  led_6 = 0;
		if(acionamento_leds >= 2) led_7 = 1;
		else											led_7 = 0;
		if(acionamento_leds >= 3) led_3 = 1;
		else											led_3 = 0;
		if(acionamento_leds >= 4) led_8 = 1;
		else											led_8 = 0;
		if(acionamento_leds >= 5) led_9 = 1;
		else											led_9 = 0;
	}
}

void calibracao_eixo(void)
{
	static int cont;
	
	zera_inclinometro();
	 
	// Leituras de estabilizacao da MPU
	for(cont = 0; cont < 3000; cont++)
	{
		le_mpu();
	}
	
	cont = 0;
	
	// Verifica se o inclinometro esta zerado
	if(
		((valor_eixo_x > 0)&(valor_eixo_x >= limite_mais_am))|
		((valor_eixo_x < 0)&(valor_eixo_x <= limite_menos_am))|
		((valor_eixo_y > 0)&(valor_eixo_y >= limite_mais_am))|
		((valor_eixo_y < 0)&(valor_eixo_y <= limite_menos_am))
	)
	{
		while(1)
		{
			HAL_Delay(1);
		}
	}
	
	// Aciona o atuador por no max. 2.5s
	// Verifica qual dos eixos passou do valor Amarelo, e verifica se estava no eixo negativo ou positivo
	// Assim determina o eixo primario e a direção de funcionamento do atuador
	while(cont <= FIM_CURSO_DESCARR)
	{
		HAL_Delay(2);
		le_mpu();
		cont++;
		prof_2_high();
		prof_1_high();
		atuador_avanco = (pwm_atuador_max / 2); 
		atuador_retorno = 0;      
		rele_high();
		
		// Verifica se passou do eixo X positivo, seleciona ele como eixo primario
		if((valor_eixo_x > 0)&(valor_eixo_x >= limite_mais_am))
		{
			eixo_sel = 0;
			val_eixo = 0;
			break;
		}
		
		// Se o eixo X variou para o negativo, inverte o funcionamento do atuador e seta o eixo como principal
		if((valor_eixo_x < 0)&(valor_eixo_x <= limite_menos_am))
		{
			eixo_sel = 0;
			val_eixo = 1;
			break;
		}
		
		// Verifica se passou do eixo Y positivo, seleciona ele como eixo primario
		if((valor_eixo_y > 0)&(valor_eixo_y >= limite_mais_am))
		{
			eixo_sel = 1;
			val_eixo = 0;
			break;
		}
		// Se o eixo Y variou para o negativo, inverte o funcionamento do atuador e seta o eixo como principal
		if((valor_eixo_y < 0)&(valor_eixo_y <= limite_menos_am))
		{
			eixo_sel = 1;
			val_eixo = 1;
			break;
		}
		
		leds_calibracao();
	}
	
	// Desliga o atuado
	prof_2_high();
	prof_1_high();
	atuador_avanco = 0; 
	atuador_retorno = 0;      
	rele_high();

	//Retorna para o eixo inicial
	while(cont > 0)
	{
		HAL_Delay(2);
		 le_mpu();
		 cont--;
		 prof_2_high();
		 prof_1_high();
		 atuador_avanco = 0; 
		 atuador_retorno = (pwm_atuador_max / 2);      
		 rele_high();
		 leds_calibracao();
	}
	
	prof_2_high();
	prof_1_high();
	atuador_avanco = 0; 
	atuador_retorno = 0;      
	rele_high();
	bit_calibra_eixo = 0;
}
