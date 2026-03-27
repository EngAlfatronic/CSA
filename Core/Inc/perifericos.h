

#include "main.h"

/* Private defines -----------------------------------------------------------*/
#define rot_1_Pin GPIO_PIN_0
#define rot_1_GPIO_Port GPIOA
#define rot_2_Pin GPIO_PIN_1
#define rot_2_GPIO_Port GPIOA
#define ad_bateria_Pin GPIO_PIN_2
#define ad_bateria_GPIO_Port GPIOA
#define entrada_manual_Pin GPIO_PIN_3
#define entrada_manual_GPIO_Port GPIOA
#define entrada_carrega_bat_Pin GPIO_PIN_5
#define entrada_carrega_bat_GPIO_Port GPIOA
#define saida_alimenta_Pin                                 GPIO_PIN_4
#define saida_alimenta_GPIO_Port                           GPIOA
#define saida_alimenta_high()                              HAL_GPIO_WritePin(saida_alimenta_GPIO_Port, saida_alimenta_Pin,GPIO_PIN_SET)
#define saida_alimenta_low()                               HAL_GPIO_WritePin(saida_alimenta_GPIO_Port, saida_alimenta_Pin,GPIO_PIN_RESET)

#define latch_Pin                                          GPIO_PIN_7
#define latch_GPIO_Port                                    GPIOA
#define latch_high()                                       HAL_GPIO_WritePin(latch_GPIO_Port, latch_Pin,GPIO_PIN_SET)
#define latch_low()                                        HAL_GPIO_WritePin(latch_GPIO_Port, latch_Pin,GPIO_PIN_RESET)

#define clk_Pin                                            GPIO_PIN_0
#define clk_GPIO_Port                                      GPIOB
#define clk_high()                                         HAL_GPIO_WritePin(clk_GPIO_Port, clk_Pin,GPIO_PIN_SET)
#define clk_low()                                          HAL_GPIO_WritePin(clk_GPIO_Port, clk_Pin,GPIO_PIN_RESET)

#define sdi_Pin                                            GPIO_PIN_1
#define sdi_GPIO_Port                                      GPIOB
#define sdi_high()                                         HAL_GPIO_WritePin(sdi_GPIO_Port, sdi_Pin,GPIO_PIN_SET)
#define sdi_low()                                          HAL_GPIO_WritePin(sdi_GPIO_Port, sdi_Pin,GPIO_PIN_RESET)

#define buzzer_Pin                                         GPIO_PIN_8                         
#define buzzer_GPIO_Port                                   GPIOA
#define buzzer_high()                                      HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin,GPIO_PIN_SET)
#define buzzer_low()                                       HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin,GPIO_PIN_RESET)

#define in_profet_1_Pin                                    GPIO_PIN_6
#define in_profet_1_GPIO_Port                              GPIOA

#define in_profet_2_Pin                                    GPIO_PIN_5
#define in_profet_2_GPIO_Port                              GPIOB

#define hab_prof_1_Pin                                     GPIO_PIN_15
#define hab_prof_1_GPIO_Port                               GPIOA
#define prof_1_high()                                      HAL_GPIO_WritePin(hab_prof_1_GPIO_Port, hab_prof_1_Pin,GPIO_PIN_SET)
#define prof_1_low()                                       HAL_GPIO_WritePin(hab_prof_1_GPIO_Port, hab_prof_1_Pin,GPIO_PIN_RESET)

#define hab_prof_2_Pin                                     GPIO_PIN_3
#define hab_prof_2_GPIO_Port                               GPIOB
#define prof_2_high()                                      HAL_GPIO_WritePin(hab_prof_2_GPIO_Port, hab_prof_2_Pin,GPIO_PIN_SET)
#define prof_2_low()                                       HAL_GPIO_WritePin(hab_prof_2_GPIO_Port, hab_prof_2_Pin,GPIO_PIN_RESET)

#define rele_Pin                                           GPIO_PIN_4
#define rele_GPIO_Port                                     GPIOB
#define rele_high()                                        HAL_GPIO_WritePin(rele_GPIO_Port, rele_Pin,GPIO_PIN_SET)
#define rele_low()                                         HAL_GPIO_WritePin(rele_GPIO_Port, rele_Pin,GPIO_PIN_RESET)

void MX_GPIO_Init(void);
void MX_ADC_Init(void);
void MX_CAN_Init(void);
void MX_I2C1_Init(void);
void MX_TIM3_Init(void);
void verifica_alterna_eixo(void);
void verifica_fim_de_curso(void);
void verifica_tempo_ativo(void);
unsigned int interpolacao(int x, int x1, int x2, int y1, int y2);
