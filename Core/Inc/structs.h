/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto: CSA
// Autor: IURI DE LIMA
// Versao: V1.0
// Date: 01/04/2025
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "main.h"
#include "PID.h"

extern unsigned int val_bat_ad;
extern unsigned char led_1;
extern unsigned char led_2;
extern unsigned char led_3;
extern unsigned char led_4;
extern unsigned char led_5;
extern unsigned char led_6;
extern unsigned char led_7;
extern unsigned char led_8;
extern unsigned char led_9;
extern unsigned char led_10;
extern unsigned char led_11;
extern unsigned char led_12;
extern unsigned char led_13;
extern unsigned char led_14;
extern unsigned char led_15;
extern unsigned char led_16;
extern float valor_tensao;
extern unsigned int startAddress;
extern unsigned char salva_eeprom;
extern unsigned char bit_mpu_falsa;
extern unsigned char bit_erro_MPU;
extern unsigned char cont_pisca_led_ok;
extern signed short int valor_incl_x;
extern signed short int valor_incl_y;
extern unsigned short int cont_buzzer;
extern unsigned char divisor_systick;
extern unsigned short int pwm_value;
extern float eixo_final;

extern signed int busca_eixo_val_1;
extern signed int busca_eixo_val_2;
extern signed int busca_eixo_val_3;

extern signed char valor_anterior_eixo;

extern float RateRoll, RatePitch, RateYaw;
extern int RateCalibrationNumber;
extern float AccX, AccY, AccZ;
extern float AngleRoll, AnglePitch;
extern uint32_t LoopTimer;
extern float KalmanAngleRoll, KalmanUncertaintyAngleRoll;
extern float KalmanAnglePitch, KalmanUncertaintyAnglePitch;
extern float roll, pitch;
extern int valor_eixo_x, valor_eixo_y;
extern float Kalman1DOutput[];
extern uint32_t val_filtro_leit;
extern float valor; 
extern float RollCalibration;
extern float PitchCalibration;

extern struct 
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

extern union union_leds_my_x
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

extern union union_bits
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

extern union union_bits_2
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

extern struct flags_1
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

extern PID_Controller pid;

extern uint16_t is_val_prof_1;
extern unsigned char cont_falha_prof_1;
extern unsigned char cont_falha_prof_2;
