/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto: CSA
// Autor: IURI DE LIMA
// Versao: V1.0
// Date: 01/04/2025
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "perifericos.h"
#include "sae_j1939.h"
#include "mpu_6050.h"
#include "variaveis.h"
#include "flash.h"
#include "PID.h"
#include "calibracao.h"

#include <stdlib.h>
#include <math.h>

short int trata_PID(void);
short int trata_manual(void);
void trata_saida(void);
void le_is_profets(void);

// #define MODO_TESTE_HARDWARE

int main(void)
{
	float pid_kp, pid_ki, pid_kd, pid_int_limit, pid_out_limit;
	
	// Inicializa perifericos
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
	saida_alimenta_high();
  MX_ADC_Init();
  MX_CAN_Init();
	MX_TIM1_Init();
	//MX_TIM14_Init();
	MX_I2C1_Init();
	HAL_Delay(100);
	mpu_inicializa();
	HAL_Delay(100);
	
	// Carrega os valores iniciais da flash
	valores_iniciais_flash();
	
	// Zera MPU, determina eixo prinicpal e calibra o atuador
	calibracao();
	
	//pid_kp = 500;
	//pid_ki = 10;
	//pid_kd = 50;
	pid_kp = 1500;
	pid_ki = 2;
	pid_kd = 1;
	pid_int_limit = pwm_atuador_max;
	pid_out_limit = pwm_atuador_max;
	
	PID_Init(&pid, pid_kp, pid_ki, pid_kd, pid_int_limit, pid_out_limit);
	
	pisca_leds();
	
  while (1)
  {
		le_bateria();
		le_entradas();
		le_mpu();
		le_is_profets();
		
		verifica_fim_de_curso();
		verifica_tempo_ativo();
		verifica_alterna_eixo();
		
		trata_saida();
		
		trata_leds_incl(eixo_final);
		trata_leds_carr(valor_tensao);
		
		envia_pacote_can_carregado(235);
  }
}

void verifica_tempo_ativo()
{
	static int tick_ini = 0;
	
	if(
			tick_ini == 0 ||
			atuador_retorno != 0 ||
			atuador_retorno != 0
	)tick_ini = uwTick;
	
	if(uwTick - tick_ini >= QUATRO_HORAS)
	{
		prof_2_low();
	  prof_1_low();
	  atuador_avanco = 0;
	  atuador_retorno = 0;
		bit_ativa_beep = 1;
		HAL_Delay(2000);
		saida_alimenta_low();
	}
}

void verifica_fim_de_curso()
{
	static float angulo_corte;
	static int contador;
	unsigned int valor_fim_curso;
	
	valor_fim_curso = interpolacao(valor_tensao, bit_ch_manual == 1 ? tensao_min_manual : tensao_min_automa, tensao_maxima, FIM_CURSO_DESCARR, FIM_CURSO_CARR);
	
	if(
		(bit_ch_manual == 0 && ( angulo_corte != 0 && (eixo_final - angulo_corte > 1.0f || eixo_final - angulo_corte < -1.0f))) ||
		(bit_ch_manual == 1 && ( bit_ch_avanca == 0 && bit_ch_recua == 0))
	)
	{
		contador = 0;
		beep_fim_de_curso = 0;
		fim_de_curso = 0;
		angulo_corte = 0.0f;
	}

	if(atuador_avanco == pwm_atuador_max || atuador_retorno == pwm_atuador_max)
	{
		contador++;
	}

	if(contador >= valor_fim_curso)
	{
		if(angulo_corte == 0.0f)angulo_corte = eixo_final;
		beep_fim_de_curso = 1;
		fim_de_curso = 1;
		contador = valor_fim_curso;
		return;
	}
	
	fim_de_curso = 0;
}

void verifica_alterna_eixo(void)
{
	if(eixo_sel == 0)eixo_final = roll;
	if(eixo_sel == 1)eixo_final = pitch;
}

short int trata_PID(void)
{
  static unsigned int val_milis, val_milis_prev;
	static float dt;

  val_milis = uwTick;
	
	dt = (val_milis - val_milis_prev) / MILIS_PARA_SEGUNDOS;
	
	val_milis_prev = val_milis;

  return PID_Update(&pid, 0.0f, (float)eixo_final, dt);
}

short int trata_manual(void)
{
  if(bit_ch_avanca == 1)return pwm_atuador_max;
  else if(bit_ch_recua == 1)return -pwm_atuador_max;
  else return 0;
}

void trata_saida(void)
{
  static int saida_rampa;
  static int saida;
  static int saida_prev;
  static unsigned char histerese_pid;
  static unsigned char des_modo;
	static int contador_rele;
  static unsigned char habil_rampa;
	
  unsigned int pwm_saida;

  // Se calibrando, nao permite acionar a saida
  if(bit_calibra == 1)return;

  // Se em tensao baixa ou chegou no fim de curso, desativa a saida diretamente
  if(bit_tensao_baixa == 1 || fim_de_curso == 1)
  {
    atuador_avanco = 0;
    atuador_retorno = 0;

    rele_low();

    return;
  }

  // Aciona os profets
  // SOMENTE deve acionar aqui, ou desacionar quando desliga por tempo
  prof_2_high();
  prof_1_high();
	
  // Se em modo automatico
  if(bit_ch_manual == 0)
  {
		#ifdef MODO_TESTE_HARDWARE
			static int tempo_teste;
			static unsigned char direcao_teste;
		
			if(uwTick > (tempo_teste + 5000))
			{
				tempo_teste = uwTick;
				if(direcao_teste == 0)direcao_teste = 1;
				else direcao_teste = 0;
				
				if(direcao_teste == 1)saida = pwm_atuador_max;
				else									saida = -pwm_atuador_max;
			}
		#endif
		#ifndef MODO_TESTE_HARDWARE
			saida = trata_PID();

			// Se esta vindo do modo manual, e teve muita alteracao de angulo, parte da ultima saida para suavizar
			if(abs(saida - saida_prev) > 500 && des_modo == 1)saida = saida_prev;
		
			if(abs(saida) < pwm_atuador_min)
			{
					// Se o valor de saida do PID esta baixo, e o angulo esta dentro dos limites, desabilita o PID
					if(eixo_final < histerese_acionamento_pid && eixo_final > -histerese_acionamento_pid)
					{
							histerese_pid = 0;
							habil_pid = 0;
							saida = 0;
					}
					// Se nao, joga o valor minimo para a saida
					else
					{
							if(abs(saida) > 0) 
							{
									if(saida < 0) saida = -fmaxf(abs(saida), pwm_atuador_min);
									else          saida = fmaxf(saida, pwm_atuador_min);
							}
							else
							{
									saida = 0;
							}
					}
			}
			
			// Se a leitura de angulo passou dos limites do PID, aciona o pid
			if(eixo_final > angulo_acionamento_pid || eixo_final < -angulo_acionamento_pid)
			{
					histerese_pid = 1;
					habil_pid = 1;
			}
			
			// Se est� dentro dos limites do PID, nao libera a saida
			if(histerese_pid == 0) saida = 0;
		#endif
  }
  // Se em modo manual
  else
  {
		habil_pid = 0;
    saida = trata_manual();
  }

  des_modo = bit_ch_manual;

  // Se a diferenca entre a saida_rampa e saida for muito grande, gera uma rampa entre os valores para evitar tranco
  // Se um dos valores for positivo e o outro negativo, tamb�m gera rampa
	if((abs(saida_rampa - saida) > 500 || ((saida * saida_rampa) < 0)) && /*des_modo*/ 1 == 1)habil_rampa = 1;
	
  if(habil_rampa == 1)
  {
    int diferenca = saida - saida_rampa;
		unsigned char passo_rampa;
		
		int valor_abs = abs(saida_rampa);

		// Rampa de 3 pontos com interpolacao entre eles para fazer a rampa mais lenta quando chega proximo de 0
		// E acelera cada vez mais quanto mais longe de 0
		if(valor_abs <= PONTO_LENTO) 
		{
				passo_rampa = PASSO_RAMPA_LENTO;
		}
		else if(valor_abs <= PONTO_MEDIO) 
		{
				passo_rampa = interpolacao(valor_abs, PONTO_LENTO, PONTO_MEDIO, PASSO_RAMPA_LENTO, PASSO_RAMPA_MEDIO);
		}
		else if(valor_abs <= PONTO_RAPIDO) 
		{
				passo_rampa = interpolacao(valor_abs, PONTO_MEDIO, PONTO_RAPIDO,PASSO_RAMPA_MEDIO, PASSO_RAMPA_RAPIDO);
		}
		else 
		{
				passo_rampa = PASSO_RAMPA_RAPIDO;
		}
		
		if(abs(saida_rampa - saida) < 10)
		{
			habil_rampa = 0;
		}
    
    if(abs(diferenca) > passo_rampa)
    {
        saida_rampa += (diferenca > 0) ? passo_rampa : -passo_rampa;
    }
		else
		{
			saida_rampa = saida;
		}
  }
  else
  {
      saida_rampa = saida; // Se a diferen�a n�o for muito grande, joga o valor direto
  }

	saida_prev = saida;
  pwm_saida = abs(saida_rampa);
	if(pwm_saida < pwm_atuador_min)pwm_saida = 0;
  
	// Se estiver no modo automatico, considera a histerese para evitar compensacoes minusculas
  if(
		(pwm_saida < pwm_atuador_min) &
		(
			(des_modo == 0 && histerese_pid == 0)|
			(des_modo == 1)
		)
	)
  {
    atuador_avanco = 0;
    atuador_retorno = 0;

		// Evita que o rele fique acoplando e desacoplando em mudancas de direcao rapidas
		contador_rele++;
		if(contador_rele >= 5000)rele_low();
		
    return;
  }
	
	contador_rele = 0;
	
  if(saida_rampa > 0)
  {
    atuador_avanco = pwm_saida * (val_eixo ^ 1);  // 0 se val_eixo==1, pwm_saida se val_eixo==0
    atuador_retorno = pwm_saida * val_eixo;       // pwm_saida se val_eixo==1, 0 se val_eixo==0

    rele_high();
  }
  else if(saida_rampa < 0)
  {
    atuador_avanco = pwm_saida * val_eixo;        // pwm_saida se val_eixo==1, 0 se val_eixo==0
    atuador_retorno = pwm_saida * (val_eixo ^ 1); // 0 se val_eixo==1, pwm_saida se val_eixo==0

    rele_high();
  }
	else
	{
		atuador_avanco = 0;
		atuador_retorno = 0;
	}
}

//Funcao para tratar beep
void trata_beep(void)
{
	#ifdef MODO_TESTE_HARDWARE
	return;
	#endif
	if(bit_ativa_beep == 1)
	{
		buzzer_high();
		return;
	}
//Trata beep rapido	
	if((beep_rapido_bateria == 1)|(beep_rapido_inclin == 1))
	{
		if(cont_buzzer > 30)cont_buzzer = 0;
		if(cont_buzzer > 5)buzzer_low();
		else
		{
			buzzer_high();
		}
		return;
	}
//Trata beep lento	
	if((beep_lento_bateria == 1)|(beep_lento_inclin == 1)|(beep_fim_de_curso == 1))
	{
		if(cont_buzzer > 90)cont_buzzer = 0;
		if(cont_buzzer > 5)buzzer_low();
		else
		{
			buzzer_high();
		}			
	}
	if((beep_rapido_bateria == 0)&(beep_rapido_inclin == 0)&(beep_lento_bateria == 0)&(beep_lento_inclin == 0)&(bit_ativa_beep == 0)&(beep_fim_de_curso == 0))buzzer_low();	
}

//Funcao para piscar leds para inicializar
void pisca_leds(void)
{
////////////////////////////////////////////////////////////////
//                   LED1                   LED14
//                   LED2                   LED13
//LED16  LED6  LED7  LED3  LED8  LED9       LED12
//LED15              LED4                   LED11
//                   LED5                   LED10
	
	unsigned char cont_pisca;	
//Liga bargraf da bateria		
	led_12 = 1;
	led_3 = 1;
	HAL_Delay(200);
	led_13 = 1;
	led_11 = 1;
	led_7 = 1;
	led_8 = 1;
	HAL_Delay(200);
	led_14 = 1;
	led_10 = 1;
	led_6 = 1;
	led_9 = 1;
	HAL_Delay(200);
	for(cont_pisca = 0;cont_pisca < 3;cont_pisca++)
	{			
		led_9 = 1;
		led_8 = 1;
		led_7 = 1;
		led_3 = 1;	
		led_6 = 1;
		led_13 = 1;
	  led_11 = 1;
		led_12 = 1;
		led_14 = 1;
	  led_10 = 1;
		bit_ativa_beep = 1;
		HAL_Delay(200);
		led_9 = 0;
		led_8 = 0;
		led_7 = 0;
		led_3 = 0;	
		led_6 = 0;
		led_13 = 0;
	  led_11 = 0;
		led_12 = 0;
		led_14 = 0;
	  led_10 = 0;
		bit_ativa_beep = 0;
		HAL_Delay(200);
	}
	led_10 = 0;
	led_11 = 0;
	led_12 = 0;
	led_13 = 0;
	led_14 = 0;
}

void trata_leds_incl(signed short int val_incl)
{
	val_incl = val_incl * 10;
	
	if((val_incl < limite_mais_am)&(val_incl > limite_menos_am))cont_pisca_led_ok = 3;
  if(val_incl > 0)
	{
	 if((val_incl < limite_mais_vm)&(val_incl >= limite_mais_am))cont_pisca_led_ok = 4;
	 if(val_incl >= limite_mais_vm)cont_pisca_led_ok = 5;
	}
	if(val_incl < 0)
	{
	 if((val_incl > limite_menos_vm)&(val_incl <= limite_menos_am))cont_pisca_led_ok = 2;
	 if(val_incl <= limite_menos_vm)cont_pisca_led_ok = 1;
	}
		
//Ativa leds de acordo com posicao
	if(bit_tensao_baixa == 1)
	{
		led_3 = 0;
		led_6 = 0;
		led_7 = 0;
		led_8 = 0;
		led_9 = 0;
		beep_rapido_inclin = 0;
		beep_lento_inclin = 0;
	}
	else
	{
		if(cont_pisca_led_ok == 5)led_9 = 1,beep_rapido_inclin = 1,beep_lento_inclin = 0;
		else led_9 = 0;
		if(cont_pisca_led_ok == 4)led_8 = 1,beep_rapido_inclin = 0,beep_lento_inclin = 1;
		else led_8 = 0;
		if(cont_pisca_led_ok == 3)led_3 = 1,beep_rapido_inclin = 0,beep_lento_inclin = 0;
		else led_3 = 0;
		if(cont_pisca_led_ok == 2)led_7 = 1,beep_rapido_inclin = 0,beep_lento_inclin = 1;
		else led_7 = 0;
		if(cont_pisca_led_ok == 1)led_6 = 1,beep_rapido_inclin = 1,beep_lento_inclin = 0;
		else led_6 = 0;	
	}
}
void trata_leds_carr(float tensao) 
{
	static unsigned char cont_pisca_led_bateria;
//Aciona os LEDs conforme a tens�o com histerese
	if(bit_carregando == 1)tensao -= 1.0f;
	if(bit_tensao_baixa == 1)
	{
		cont_pisca_led_bateria++;
		
		led_10 = 0;
		led_11 = 0;
		led_12 = 0;
		led_13 = 0;
		if(cont_pisca_led_bateria >= 200)
		{
			led_14 = !led_14;
			cont_pisca_led_bateria = 0;
		}
		beep_rapido_bateria = 1;
		beep_lento_bateria = 0;
		return;
	}
  if(tensao >= 25.3f)
	{
		led_10 = 1;
		led_11 = bit_carregando; // Se estiver carregando, acende barrinha como bargraf
		led_12 = bit_carregando;
		led_13 = bit_carregando;
		led_14 = bit_carregando;
		beep_lento_bateria = 0;
		beep_rapido_bateria = 0; // Verde (100%)
	}
  else if(tensao >= 24.8f)
	{
		led_10 = 0;
		led_11 = 1;
		led_12 = bit_carregando;
		led_13 = bit_carregando;
		led_14 = bit_carregando;
		beep_lento_bateria = 0;
		beep_rapido_bateria = 0; // Verde (75%)
	}
  else if(tensao >= 24.3f)
	{
		led_10 = 0;
		led_11 = 0;
		led_12 = 1;
		led_13 = bit_carregando;
		led_14 = bit_carregando;
		beep_lento_bateria = 0;
		beep_rapido_bateria = 0; // Verde (50%)
	}
  else if (tensao >= 24.0f)
	{
		led_10 = 0;
		led_11 = 0;
		led_12 = 0;
		led_13 = 1;
		led_14 = bit_carregando;
		beep_lento_bateria = 1;
		beep_rapido_bateria = 0; // Amarelo (25%)
	}
  else if (tensao >= 21.0f)
	{
		led_10 = 0;
		led_11 = 0;
		led_12 = 0;
		led_13 = 0;
		led_14 = 1;
		beep_rapido_bateria = 1;
		beep_lento_bateria = 0; // Vermelho (<25%)
	}
}

void envia_leds_my(unsigned short int val_leds)
{
	////////////////////////////////////////////////////////////////
	//                   LED1                   LED14
	//                   LED2                   LED13
	//LED16  LED6  LED7  LED3  LED8  LED9       LED12
	//LED15              LED4                   LED11
	//                   LED5                   LED10
    // Loop para enviar os dados serialmente
    for(char i = 0; i < 16; i++)
    {
        if(val_leds & 0x8000) sdi_high();
        else sdi_low(); 
        val_leds = val_leds << 1;
        clk_low();
        clk_high();
    }
    
    latch_high();
    latch_low();
    
    // Em vez de 16 atribui��es individuais, use a opera��o bitwise!
    // Assumindo que os leds (led_1, led_2, etc) tamb�m est�o em uma estrutura similar
    unsigned short int leds_value = 0;
    
    // Constr�i o valor de 16 bits baseado nos LEDs individuais
    if(led_1) leds_value |= (1 << 0);
    if(led_2) leds_value |= (1 << 1);
    if(led_3) leds_value |= (1 << 2);
    if(led_4) leds_value |= (1 << 3);
    if(led_5) leds_value |= (1 << 4);
    if(led_6) leds_value |= (1 << 5);
    if(led_7) leds_value |= (1 << 6);
    if(led_8) leds_value |= (1 << 7);
    if(led_9) leds_value |= (1 << 8);
    if(led_10) leds_value |= (1 << 9);
    if(led_11) leds_value |= (1 << 10);
    if(led_12) leds_value |= (1 << 11);
    if(led_13) leds_value |= (1 << 12);
    if(led_14) leds_value |= (1 << 13);
    if(led_15) leds_value |= (1 << 14);
    if(led_16) leds_value |= (1 << 15);
    
    // Agora, uma �nica atribui��o!
    byte_leds_my = leds_value;
}

void le_entradas(void)
{
	if(HAL_GPIO_ReadPin(entrada_manual_GPIO_Port,entrada_manual_Pin) == 1)bit_ch_manual = 1;
	else bit_ch_manual = 0;
	
	if(HAL_GPIO_ReadPin(entrada_carrega_bat_GPIO_Port,entrada_carrega_bat_Pin) == 1)bit_carregando = 1;
	else bit_carregando = 0;
	
	if(HAL_GPIO_ReadPin(rot_1_GPIO_Port,rot_1_Pin) == 1)
	{
		bit_ch_avanca = 1;
	}
	else 
	{
		bit_ch_avanca = 0;
	}
	
	if(HAL_GPIO_ReadPin(rot_2_GPIO_Port,rot_2_Pin) == 1)
	{
		bit_ch_recua = 1;
	}
	else 
	{
		bit_ch_recua = 0;
	}
}

/* - Le um canal do ADC de forma generica, trocando o canal diretamente
     no registrador CHSELR do STM32F0.
   - Inputs: canal (numero do canal ADC, ex: 2 para bateria, 6 para IS).
     Globais: hadc.
   - Outputs: retorna valor ADC lido (0 a 4095).
*/
uint16_t le_adc_canal(uint32_t canal)
{
	ADC1->CHSELR = (1 << canal);
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 10);
	uint16_t val = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	return val;
}

/* - Le os pinos IS dos dois BTN8982 para detectar falha nos profets.
     Profet 1 (PA6 / ADC_IN6): leitura analogica. Se ADC satura (>IS_THRESHOLD_FALHA),
     indica overcurrent, overtemperature ou curto-circuito.
     Profet 2 (PB5 / digital): leitura digital. Verifica apenas quando as saidas
     deveriam estar desligadas, para evitar falso positivo (PB5 nao possui ADC).
   - Inputs: hadc (global), in_profet_2_GPIO_Port/Pin (defines).
     Globais lidas: atuador_avanco, atuador_retorno.
   - Outputs: is_val_prof_1, bit_falha_prof_1, bit_falha_prof_2,
     cont_falha_prof_1, cont_falha_prof_2.
*/
void le_is_profets(void)
{
	is_val_prof_1 = le_adc_canal(6);

	if(is_val_prof_1 >= IS_THRESHOLD_FALHA)
	{
		cont_falha_prof_1++;
		if(cont_falha_prof_1 >= IS_CONT_FALHA)
		{
			bit_falha_prof_1 = 1;
			cont_falha_prof_1 = IS_CONT_FALHA;
		}
	}
	else
	{
		cont_falha_prof_1 = 0;
	}

	if(atuador_avanco == 0 && atuador_retorno == 0)
	{
		if(HAL_GPIO_ReadPin(in_profet_2_GPIO_Port, in_profet_2_Pin) == GPIO_PIN_SET)
		{
			cont_falha_prof_2++;
			if(cont_falha_prof_2 >= IS_CONT_FALHA)
			{
				bit_falha_prof_2 = 1;
				cont_falha_prof_2 = IS_CONT_FALHA;
			}
		}
		else
		{
			cont_falha_prof_2 = 0;
		}
	}
}

void le_bateria(void)
{
	static unsigned char cont_bateria = 0;
	static unsigned char ultima_ch_manual;
	static float media_bateria = 0;
	static float ultima_tensao;
	static float leituras_bateria[10] = {22, 22, 22, 22, 22, 22, 22, 22, 22, 22};
	static unsigned int val_milis;
	unsigned char i;
	
	if(uwTick < (val_milis + 250))return;
	val_milis = uwTick;
	
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc,100);
	val_bat_ad = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	leituras_bateria[cont_bateria] = ((val_bat_ad * 3.3f) / 4095.0f) / 0.1060f;
	
	// Ignora leituras absurdas (normalmente as primeiras sao valores absurdos, dps estabiliza)
	if(leituras_bateria[cont_bateria] < 14.0f)return;
	cont_bateria++;
	
	for(i = 0; i < LEITURAS_BATERIA; i++)media_bateria += leituras_bateria[i];
	
	valor_tensao = media_bateria / LEITURAS_BATERIA;
	
	media_bateria = 0;
	
	if(
		(bit_ch_manual == 1 && valor_tensao < tensao_min_manual) ||
		(bit_ch_manual == 0 && valor_tensao < tensao_min_automa)
	)
	{
		bit_tensao_baixa = 1;
		bit_ativa_beep = 1;
		ultima_tensao = valor_tensao;
		ultima_ch_manual = bit_ch_manual;
	}
	else if(
		(valor_tensao - ultima_tensao >= 2.0f && bit_ch_manual == 0) ||
		(valor_tensao - ultima_tensao >= 4.0f && bit_ch_manual == 1) ||
		(ultima_ch_manual != bit_ch_manual) 
	)
	{
		bit_tensao_baixa = 0;
		bit_ativa_beep = 0;
	}
	
	if(cont_bateria == LEITURAS_BATERIA)cont_bateria = 0;
}

unsigned int interpolacao(int x, int x1, int x2, int y1, int y2)
{
	if(x2 == x1) return y1;
	
	if(x <= x1) return y1;
	if(x >= x2) return y2;
	
	// Formula: y = y1 + (x - x1) * (y2 - y1) / (x2 - x1)
	long numerador = (long)(x - x1) * (y2 - y1);
	int denominador = (x2 - x1);
	
	return y1 + (int)(numerador / denominador);
}

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
