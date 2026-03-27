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
#include "PID.h"

extern ADC_HandleTypeDef hadc;

unsigned int val_bat_ad;
unsigned char led_1;
unsigned char led_2;
unsigned char led_3;
unsigned char led_4;
unsigned char led_5;
unsigned char led_6;
unsigned char led_7;
unsigned char led_8;
unsigned char led_9;
unsigned char led_10;
unsigned char led_11;
unsigned char led_12;
unsigned char led_13;
unsigned char led_14;
unsigned char led_15;
unsigned char led_16;
float valor_tensao;
unsigned int startAddress = 0x08007CC0;
unsigned char salva_eeprom;
unsigned char bit_mpu_falsa;
unsigned char bit_erro_MPU;
unsigned char cont_pisca_led_ok;
signed short int valor_incl_x;
signed short int valor_incl_y;
unsigned short int cont_buzzer;
unsigned char divisor_systick;
unsigned short int pwm_value;
float eixo_final;

signed int busca_eixo_val_1;
signed int busca_eixo_val_2;
signed int busca_eixo_val_3;

signed char valor_anterior_eixo;

float RateRoll, RatePitch, RateYaw;
int RateCalibrationNumber;
float AccX, AccY, AccZ;
float AngleRoll, AnglePitch;
uint32_t LoopTimer;
float KalmanAngleRoll=0, KalmanUncertaintyAngleRoll=2*2;
float KalmanAnglePitch=0, KalmanUncertaintyAnglePitch=2*2;
float roll, pitch;
int valor_eixo_x, valor_eixo_y;
float Kalman1DOutput[]={0,0};
uint32_t val_filtro_leit = 0;
float valor; 
float RollCalibration;
float PitchCalibration;

struct 
{
	unsigned int val_1_struct;//1
//Busca pontos de inicio e fim do curso dos stick_1
	float eeprom_zero_eixo_y;              //2
	float eeprom_zero_eixo_x;              //3
//Zero da lanca
	//Int com bytes usados independentes
	struct
	{
		unsigned char valor_1;
		signed char valor_2;
		unsigned char valor_3;
		unsigned char valor_4;			
	}struct_quatro_bytes_1;//5
}struct_zeramento;
//definicoes de zeramento 
#define     ini_struct_zera										struct_zeramento.val_1_struct
#define     zero_eixo_y												struct_zeramento.eeprom_zero_eixo_y
#define     zero_eixo_x												struct_zeramento.eeprom_zero_eixo_x
#define     val_eixo                  				struct_zeramento.struct_quatro_bytes_1.valor_1
#define     eixo_sel                  				struct_zeramento.struct_quatro_bytes_1.valor_3
#define     posicao_mpu												struct_zeramento.struct_quatro_bytes_1.valor_4

union union_leds_my_x
{
	struct
	{
		char b0:1;
		char b1:1;
		char b2:1;
		char b3:1;
		char b4:1;
		char b5:1;
		char b6:1;
		char b7:1;
    char b8:1;
		char b9:1;
		char b10:1;
		char b11:1;
		char b12:1;
		char b13:1;
		char b14:1;
		char b15:1;
	}struct_leds_my;
	unsigned short int byte_my;		
}union_leds_my;
#define byte_leds_my      	  	 union_leds_my.byte_my
#define led_my_1  			      	 union_leds_my.struct_leds_my.b0
#define led_my_2  			      	 union_leds_my.struct_leds_my.b1
#define led_my_3  			      	 union_leds_my.struct_leds_my.b2
#define led_my_4  			      	 union_leds_my.struct_leds_my.b3
#define led_my_5  			      	 union_leds_my.struct_leds_my.b4
#define led_my_6  			      	 union_leds_my.struct_leds_my.b5
#define led_my_7  			      	 union_leds_my.struct_leds_my.b6
#define led_my_8  			      	 union_leds_my.struct_leds_my.b7
#define led_my_9  			      	 union_leds_my.struct_leds_my.b8
#define led_my_10  			      	 union_leds_my.struct_leds_my.b9
#define led_my_11  			      	 union_leds_my.struct_leds_my.b10
#define led_my_12  			      	 union_leds_my.struct_leds_my.b11
#define led_my_13  			      	 union_leds_my.struct_leds_my.b12
#define led_my_14  			      	 union_leds_my.struct_leds_my.b13
#define led_my_15  			      	 union_leds_my.struct_leds_my.b14
#define led_my_16  			      	 union_leds_my.struct_leds_my.b15

union union_bits
{
	struct
	{
		char b0:1;
		char b1:1;
		char b2:1;
		char b3:1;
		char b4:1;
		char b5:1;
		char b6:1;
		char b7:1;
	}struct_bits;
	unsigned char bits;		
}union_bits;
#define byte_union      	  	   union_bits.bits
#define bit_ch_manual  			     union_bits.struct_bits.b0
#define bit_carregando  			   union_bits.struct_bits.b1
#define bit_ch_avanca  			     union_bits.struct_bits.b2
#define bit_ch_recua  			     union_bits.struct_bits.b3
#define bit_calibra              union_bits.struct_bits.b4
#define status_mpu               union_bits.struct_bits.b5
#define fim_de_curso	  				 union_bits.struct_bits.b6
#define habil_pid								 union_bits.struct_bits.b7

union union_bits_2
{
	struct
	{
		char b0:1;
		char b1:1;
		char b2:1;
		char b3:1;
		char b4:1;
		char b5:1;
		char b6:1;
		char b7:1;
	}struct_bits_2;
	unsigned char bits_2;		
}union_bits_2;
#define byte_union_2      	  	 union_bits_2.bits_2
#define bit_calibra_leds  			 union_bits_2.struct_bits_2.b0
#define bit_calibra_eixo  			 union_bits_2.struct_bits_2.b1
#define bit_calibra_angulo  		 union_bits_2.struct_bits_2.b2
#define bit_calibra_MPU  		     union_bits_2.struct_bits_2.b3
#define bit_tensao_baixa				 union_bits_2.struct_bits_2.b4
#define bit_falha_prof_1				 union_bits_2.struct_bits_2.b5
#define bit_falha_prof_2				 union_bits_2.struct_bits_2.b6

struct flags_1
{
	char b0:1;
	char b1:1;
	char b2:1;
	char b3:1;
	char b4:1;
	char b5:1;
	char b6:1;
	char b7:1;
}struct_1;
#define     beep_rapido_bateria struct_1.b0
#define     beep_lento_bateria  struct_1.b1
#define     beep_rapido_inclin  struct_1.b2
#define     beep_lento_inclin   struct_1.b3
#define     beep_fim_de_curso		struct_1.b4
#define     bit_ativa_beep			struct_1.b5

PID_Controller pid;

uint16_t is_val_prof_1;
unsigned char cont_falha_prof_1;
unsigned char cont_falha_prof_2;

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_ADC_Init(void);
void MX_CAN_Init(void);
void MX_I2C1_Init(void);
void MX_TIM1_Init(void);
void MX_TIM14_Init(void);
void trata_rele(void);
void le_bateria(void);
void le_entradas(void);
void envia_leds_my(unsigned short int val_leds);
void update_leds(float tensao);
void trata_leds_incl(signed short int val_incl);
void trata_leds_carr(float tensao);
void pisca_leds(void);
void trata_beep(void);
void trata_pwm_manual(void);
void testa_inverte_eixo(void);
void verifica_inverte_atuador(void);


