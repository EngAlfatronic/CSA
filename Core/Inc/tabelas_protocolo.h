//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto Controle Remoto Alfatronic TRM-MAX
// Autor  Paulo Palaoro
// Versao V1.0
// Data 20/07/2023
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Informa fim do pacote
#define fim_pac                     ';'
//Ativa desativa
#define desativa                    0
#define ativa                       1

//device_id tipo do device          ID
#define id_pac_conf      						0  
#define id_joy_mono   							1
#define id_joy_bid									2
#define id_perf_chaves			        3
#define id_perf_teclas			        4
#define id_per_mom                  5
#define id_per_fly                  6
#define id_per_info    		          7
#define id_alertas     		          8
#define id_emergencias		          9
#define id_cal_corr                 10
#define id_cal_rampa                11
#define id_parametros_radio         12
#define id_diagnostico   	          13
#define id_manutencao   	          14
#define id_comandos                 15
#define id_mod_par_nr12             16
#define id_pac_lora_can             17
#define id_wabtec                   18
#define id_mod_par_fly              19
#define id_mod_par_radio            20
#define id_nav_par_radio            21
#define id_nav_par_incli            22
#define id_nav_par_nr12             23
#define id_nav_par_fly              24
#define id_mod_par_time             25
#define id_nav_par_time             26
#define id_mod_par_incli            27
#define id_datalloger               28
#define id_mod_par_usuario          29
#define id_terradril                30
#define id_mod_par_app              31

#define id_nav_par_corr             32
#define id_nav_par_rampa            33
#define id_mod_par_corr             34
#define id_mod_par_rampa            35
#define id_info_trans_data          36
//ID:0 Configuracao              sub_ID
#define sub_brilho                  1

//ID:1 joystick alfatronic         sub_ID
#define joy_mono_1                  1
#define joy_mono_2                  2
#define joy_mono_3                  3
#define joy_mono_4                  4
#define joy_mono_5                  5
#define joy_mono_6                  6
#define joy_mono_7                  7
#define joy_mono_8                  8
#define joy_mono_9                  9
#define joy_mono_10                 10
#define optos_sticks								11
#define joy_mono_11                 12
#define joy_mono_12                 13
#define joy_mono_13                 14
#define joy_mono_14                 15
#define joy_mono_15                 16
#define joy_mono_16                 17
#define joy_mono_17                 18
#define joy_mono_18                 19
#define joy_mono_19                 20
#define joy_mono_20                 21
#define joy_mono_21                 22

#define potenciometro_1             23
#define potenciometro_2             24
#define potenciometro_3             25
#define potenciometro_4             26
#define potenciometro_5             27
#define potenciometro_6             28
#define potenciometro_7             29
#define potenciometro_8             30
#define potenciometro_9             31
#define potenciometro_10            32
#define potenciometro_11            33
#define potenciometro_12            34

//ID:2 joystick bidirecional       sub_ID
#define joy_bid_1_x                 1
#define joy_bid_1_y                 2
#define joy_bid_1_z                 3
#define joy_bid_1_tec               4
#define joy_bid_pac_1               5 //TRM6 max bomba
#define joy_bid_2_x                 6
#define joy_bid_2_y                 7
#define joy_bid_2_z                 8
#define joy_bid_2_tec               9
#define joy_bid_pac_2               10 //TRM6 bomba
#define joy_bid_3_x                 11
#define joy_bid_3_y                 12
#define joy_bid_3_z                 13
#define joy_bid_3_tec               14
#define joy_bid_pac_3               15
#define joy_bid_pac_4               16
#define joy_bid_pac_5               17

//Sub chaves dois bits para cada chave exp. chave_1.b0 e chave_1.b1 cada pac comporta 8 chaves de duas posicoes 
//ID:3 Sub ID chaves                sub_ID  informa bits
#define perf_chaves_1                1
#define tab_ch_reset								        0x0001
#define tab_ch_buzina								        0x0002
#define tab_ch_motor_on							        0x0004
#define tab_ch_motor_off						        0x0008
#define tab_ch_rpm_mais							        0x0010
#define tab_ch_rpm_menos						        0x0020
#define tab_ch_auto_rpm_on					        0x0040
#define tab_ch_auto_rpm_off					        0x0080
#define tab_ch_coelho								        0x0100
#define tab_ch_tartaruga						        0x0200
#define tab_ch_vel_mais							        0x1000
#define tab_ch_vel_menos						        0x2000
#define tab_ch_aux_1_mais						        0x0800
#define tab_ch_aux_1_menos					        0x0400
#define tab_ch_aux_2_mais						        0x4000
#define tab_ch_aux_2_menos  				        0x8000 

//ID:5  Sub momento nr12            sub_ID
#define sub_pressao_elev             	1
#define sub_contra_elev            	 	2
#define sub_mom_elev               	 	3
#define sub_pressao_incl       				4
#define sub_contra_incl              	5
#define sub_mom_incl               	 	6
#define sub_angulo_x                	7
#define sub_angulo_y                	8
#define sub_angulo_lanca              9

//ID:6 Sub nr12 da fly              sub_ID
#define sub_fly_pressao               1
#define sub_fly_contra_press          2
#define sub_fly_momento               3
#define sub_fly_angulo             	  4
#define sub_fly_angulo_z              5

//ID:7   per_info                  sub_ID
#define sub_per_bat          					1
#define sub_sinal_com  								2
#define sub_nr12_ativo								3
#define sub_flygib_conectada					4
#define sub_freq_rf										5
#define sub_canal_rf									6
#define sub_tempo_ativo 							7
#define sub_pot_sinal_db  						8
#define sub_tempo_hp            	   	9
#define sub_per_tempo_cronometro	 	  10	
#define sub_tensao_bat_caminhao 	 	  11	
#define sub_tela_atual								12
#define sub_textos_sticks 	        	13
#define sub_alterando_parametro				14
#define sub_hora_unix_1						  	15	
#define sub_hora_unix_2					    	16	
#define sub_rpm_1 							    	17	
#define sub_temp_1							    	18	
#define sub_vazao_bomb					    	19	
#define sub_modo_coelho_tart		    	20	
#define sub_status_schwing  		    	21	
#define sub_ini_schwing					    	22	
#define sub_press_oleo_caminhao				23	
#define sub_bat_caminhao	            24
#define sub_sondeq_nivel_combustivel  25
#define sub_sondeq_posicionamento	    26
#define sub_sondeq_rotacao_cabecote	  27
#define sub_sondeq_press_rot_cabecote	28
#define sub_sondeq_avanco_cabecote	  29
#define sub_sondeq_press_avan_cabecote 30
#define sub_sondeq_rotacao_bomba_lama	31
#define sub_sondeq_press_bomba_lama	  32
#define sub_sondeq_acion_spt          33
#define sub_sondeq_contador_spt       34
#define sub_sondeq_profund            35
#define sub_sondeq_rpm_coroa          36
#define sub_ativa_wifi                37
#define sub_linguagem                 38
#define sub_id_1                      39
#define sub_id_2                      40
#define sub_salva_id                  41
#define sub_logo_atual                42
#define sub_lib_par_app               43

//Bits do parametros ID:8
#define val_tela_sticks								0
#define val_tela_cal_sticks		  			1
#define val_tela_cal_rampa            2
#define val_tela_cal_corr							3
#define val_tela_menu	                4
#define val_tela_momento              5
#define val_tela_inclinometro					6
#define val_tela_diagnostico					7
#define val_tela_manutencao						8
#define val_tela_relogio   						9
#define val_tela_fly      						10
#define val_tela_salvando 						11
#define val_tela_nr12 						    12
#define val_tela_bomba 						    13
#define val_tela_bomba_lanca   			  14
#define val_tela_sondeq_esteira 			15
#define val_tela_sondeq_posici		    16
#define val_tela_sondeq_perf  		    17
#define val_tela_wabtec_principal	    18
#define val_tela_wabtec_grampos		    19
#define val_tela_terradril						20

//ID:8 bits alertas amarelos        sub_ID     bit
#define sub_alerta_1_am		             1	
#define max_movimentos												0x0001 //0
#define alerta_sensor_pressao_elev						0x0002 //1
#define alerta_sensor_pressao_incl						0x0004 //2
#define alerta_sensor_incl_lanca							0x0008 //3
#define pac_recebido_erro											0x0010 //4
#define pressurize_inclinacao									0x0020 //5
#define parametros_bloqueados									0x0040 //6
#define alerta_fora inclinacao								0x0080 //7
#define alerta_modo_manual										0x0100 //8
#define alerta_bat_baixa  										0x0200 //9
#define alerta_ative_homem										0x0400 //10
#define alerta_start      										0x0800 //11

//ID:9  bits alertas vermelhos       sub_ID   bit
#define alerta_1_vm		                 1	
#define emergencia_ver_flash       		        0x0001
#define emergencia_erro_tranceiver		        0x0002
#define emergencia_radio							        0x0004
#define emergencia_caminhao                   0x0008
#define emerergencia_nr12_incomp              0x0010
#define emergencia_rec_incomp                 0x0020
#define emergencia_desligando                 0x0040
#define emergencia_bat_nivel_critico          0x0080
#define emergencia_rtc                        0x0100
#define emergencia_chave_ativa                0x0200
#define emergencia_stick_ativo                0x0400
#define emergencia_opto_ativo                 0x0800
#define emergencia_stick_sem_cal              0x1000
#define emergencia_sensor_elev                0x2000
#define emergencia_sensor_incl                0x4000
#define emergencia_sensor_lanca               0x8000

//Envia um bit indicando qual stick esta ativo

//ID:10                           sub_ID     bit
#define sub_id_cal_corr             1
#define cal_corr_mais_max						2
#define cal_corr_mais_min						3
#define cal_corr_menos_min					4
#define cal_corr_menos_max					5
#define cal_val_stick	              6
	
//ID:11                           sub_ID     bit
#define sub_id_cal_rampa            1
#define rampa_aceleracao 						2
#define rampa_desacelaracao 				3
#define rampa_val_stick	            4

//ID:12                          sub_ID
#define sub_parametro_atual					1
#define sub_valor_atual							2
#define sub_valor_original	        3
#define sub_valor_maximo						4
#define sub_valor_minimo						5

//ID:13 tela diagnostico         sub_ID
#define sub_diag_stick_1            1
#define sub_diag_stick_2            2
#define sub_diag_stick_3            3
#define sub_diag_stick_4            4
#define sub_diag_stick_5            5
#define sub_diag_stick_6            6
#define sub_diag_stick_7            7
#define sub_diag_stick_8            8
#define sub_diag_stick_9            9
#define sub_diag_stick_10           10
#define sub_diag_opto               11
#define sub_diag_ch                 12

//ID:14 tela manutencao         sub_ID
#define sub_manutecao              1
#define man_caixa_1										   0x0001						
#define man_caixa_2										   0x0002
#define man_berco 										   0x0004
#define man_beletora									   0x0008
#define man_cabine										   0x0010
#define man_transporte								   0x0020
#define man_feed_J1_mais							   0x0040
#define man_feed_J2_mais							   0x0080
#define man_feed_J3_mais							   0x0100
#define man_feed_J4_mais							   0x0200
#define man_feed_J1_menos					   	   0x0400
#define man_feed_J2_menos							   0x0800
#define man_feed_J3_menos							   0x1000
#define man_feed_J4_menos							   0x2000	

//ID:15 tela                    sub_ID   Valor
#define sub_id_salvar		  		      1		
#define sub_id_envia_par 		        2
#define sub_id_zeramento 		        3
#define sub_id_par_fabrica          4
#define sub_id_envia_todos_par      5  
#define sub_envia_par_caixa         6
//Valor da 15.2
#define val_bt_usuario                     1
#define val_bt_trans                       2
#define val_bt_incl                        3
#define val_bt_lmc                         4
#define val_bt_fly                         5
//Valor 15.3
#define val_zera_incl                      1
#define val_zera_lanca                     2
#define val_cal_trans                      3
#define val_zera_fly                       4
//Valores para envio comandos

//ID:17 responde tela         sub_ID
#define sub_muda_canal               1 
#define sub_resp_tela_radio          2
#define sub_resp_tela_momento        3
#define sub_resp_tela_incl           4
#define sub_resp_tela_relog          5
#define sub_resp_tela_fly            6
#define sub_resp_tela_man            7
#define sub_resp_tela_corr           8
#define sub_resp_tela_ram            9
#define sub_resp_tela_nr12          10
#define sub_resp_tela_10            11
#define sub_resp_tela_11            12
#define sub_resp_tela_12            13
///trata pacotes separados em rela??o as telas
#define sub_envia_cal_time          14
#define sub_envia_time              15
#define sub_envia_zera_incl         16
#define sub_envia_zera_lanc         17
#define sub_envia_cal_trans         18
#define sub_envia_leds_caixa        19
#define sub_envia_pac_geral         20
#define sub_envia_pac_sensor        21
#define sub_envia_pac_cx2           22
#define sub_envia_pac_cor_ram       23
#define sub_envia_pac_par_nr12      24
#define sub_envia_pac_alt_id        25
#define sub_envia_pac_fly           26
#define sub_envia_pac_mso           27
#define sub_envia_pac_mom           28
#define sub_envia_pac_incl          29
#define sub_envia_pac_schwing       30
#define sub_envia_pac_busca_id      31
#define sub_envia_pac_salva_id      32
#define sub_envia_pac_geral_1       33
#define sub_envia_pac_geral_2       34
#define sub_envia_pac_geral_3       35
#define sub_pac_salva_param         36
#define sub_pac_salva_rampa         37
#define sub_pac_salva_corr          38
#define sub_envia_pac_par_incl      39
#define sub_envia_pac_par_fly       40
#define sub_envia_pac_par_incl_nav  41
#define sub_envia_pac_par_nr12_nav  42
#define sub_envia_pac_par_fly_nav   43
#define sub_envia_cal_time_nav      44
#define sub_envia_pac_par_time      45
#define sub_valida_id               46

//ID:18 Wabtec        sub_ID             val
#define sub_grampo_passo             1   
#define sub_modo_viagem              2 
#define sub_sel_ferramenta           3
#define sub_funcao_ferramenta        4
#define sub_funcao_freio             5
#define sub_indicador_modo           6
#define sub_ponto_mira               7
#define sub_alarme                   8
#define sub_status_operacao          9
#define sub_grampo_calibracao       10
#define sub_selecao_ferramenta      11
#define sub_grampo_pos_ec1          12
#define sub_grampo_pos_ec2          13
#define sub_grampo_pos_ec3          14
#define sub_grampo_pos_ec4          15
#define sub_grampo_pos_fce          16
#define sub_grampo_pos_fcd          17
#define sub_mira_esq                18
#define sub_mira_dir                19
#define sub_mira_cor                20
#define sub_modo_cal                21
#define sub_flex_1                  22
#define sub_flex_2                  23
#define sub_flex_3                  24
#define sub_flex_4                  25
#define sub_flex_5                  26
#define sub_flex_6                  27

///Comunica??o trans/receptor
#define info_salva_par          251
#define info_altera_id_1        250
#define info_responde_id        249
#define info_chaves             248
#define info_momento            247
#define info_inf_sen            246
#define info_inclino            245
#define info_busca_hora         244
#define info_paramet            243
#define info_acerta_hora        242
#define info_pisca_led          241
#define info_zer_inc_cam        240
#define info_zer_inc_lan        239
#define info_cal_inc_tra        238
#define info_can_geral          237
#define info_can_cor_ram        236
#define info_flyjib             235
#define info_cx2                234
#define info_envia_sens         233
#define info_rec_bps            232
#define info_pac_1              231
#define info_pac_2              230
#define info_can_feed           229
#define info_cal_corr           228
#define info_cal_rampa          227
#define info_momento_2          226
#define info_paramet_incl       225
#define info_paramet_nr12       224
#define info_paramet_fly        223
#define info_param_fabrica      222
#define info_paramet_incl_app   221
#define info_paramet_mom_app    220
#define info_paramet_fly_app    219
#define info_altera_id_2        218
#define info_can_essencial      217
#define info_filtro_datalog_1   216
#define info_filtro_datalog_2   215
#define info_filtro_datalog_3   214
#define info_filtro_datalog_4   213
#define info_filtro_datalog_5   212
#define info_pac_ine_data       211
#define info_pac_falha_lei_data 210
#define info_pac_lib_data       209
#define info_pac_erro_sticks    208
#define info_pac_erros          207
#define info_fim_pac_data       206
#define info_pac_invalido       205
#define info_zera_fly           204
//Para qualquer pacote de envio que use stick bit 16
#define J1_bit_16                  		       	0x0001
#define J2_bit_16                         		0x0002
#define J3_bit_16                         		0x0004
#define J4_bit_16                         		0x0008
#define J5_bit_16                         		0x0010
#define J6_bit_16                         		0x0020
#define J7_bit_16                         		0x0040
#define J8_bit_16                         		0x0080
#define J9_bit_16                         		0x0100
#define J10_bit_16                        		0x0200
#define J11_bit_16                        		0x0400	
#define J12_bit_16                        		0x0800
#define J13_bit_16                        		0x1000
#define J14_bit_16                        		0x2000
#define J15_bit_16                        		0x4000
#define J16_bit_16                        		0x8000	

////sub_envia_pac_busca_id, tabela codificada para radios
#define radio_max_padrao                      1
#define radio_max_nr12                        2
#define radio_bomba_plug_play                 3 
#define radio_bomba_lanca                     4 
#define radio_bomba_putz                      5
#define radio_max_pvg_padrao                  6
#define radio_max_pvg_nr12                    7
#define radio_bombinha												8
#define radio_lancinha                        9 
#define radio_trm6_padrao                     10
#define radio_max_nr12_fly                    11
#define radio_max_pvg_fly                     12
#define radio_sondeq                          13
#define radio_wabtec                          14
#define radio_terradril                       15

//Sub ID:28 datalloger                        sub_id      valor
#define sub_pacote_data                       1
#define sub_requisita_pac                     2
#define sub_data_fim                          3
#define sub_pac_invalido                      4
//Sub ID 28.3
#define pacote_ok                                           0
#define val_pacote_inexistente                              1
#define val_falha_comunicao                                 2
#define val_falha_leitura                                   3
#define sub_data_parametros                   5
// SUB ID 28.5
#define val_unix_inicial_1                                  0
#define val_unix_inicial_2                                  1
#define val_unix_inicial_3                                  2
#define val_unix_inicial_4                                  3
#define val_unix_final_1                                    4
#define val_unix_final_2                                    5
#define val_unix_final_3                                    6
#define val_unix_final_4                                    7
#define val_filto                                           8
#define sub_data_mom                          6
#define sub_data_incl                         7
#define sub_data_fly                          8
#define sub_data_ok                           9
#define sub_data_liga                         10
//Sub ID:30 terradrill
#define sub_id_press_bomba_1                 1
#define sub_id_press_bomba_2                 2
#define sub_id_press_avanco                  3
#define sub_id_rotacao                       4
#define sub_id_cabecote                      5
#define sub_id_modo_trab                     6

#define sub_id_envia_pac_can_1               7
#define sub_id_recebe_pac_can_1              8
#define sub_id_pac_lora_1                    9
#define info_pac_sticks                      251
#define info_pac_chaves                      250
#define info_pac_pressao                     249
#define info_pac_rpm                         248
#define info_pac_par                         247
#define info_pac_rampa_1                     246
#define info_pac_corr_1                      245

//Sub ID:31 parametros app
#define sub_id_busca_par_corr                1
#define sub_id_busca_par_rampa               2
#define sub_id_busca_par_incl                3
#define sub_id_busca_par_mom                 4
#define sub_id_busca_par_fly                 5

//Sub ID:36 informa??o trans data
#define sub_id_pacote_erros                  1


