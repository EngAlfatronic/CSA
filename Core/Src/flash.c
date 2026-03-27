/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto: CSA
// Autor: IURI DE LIMA
// Versao: V1.0
// Date: 01/04/2025
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "structs.h"
#include "flash.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash_ex.h"
#include <math.h>
 

void write_flash(void)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	
	unsigned char cont_bytes;
	unsigned int *pont_struct;
	unsigned char bytes_struct_nr12;
	bytes_struct_nr12 = (sizeof(struct_zeramento)/4);
//Verifica bit para ver se deve salvar
	if(salva_eeprom == 0)return;
	salva_eeprom = 0;
//Inicia gravacao de dados na flash
	__disable_irq();
	HAL_FLASH_Unlock();
	FLASH->CR &= FLASH_CR_OPTWRE;
	while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) != 0){}
 
		
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = startAddress;
	EraseInitStruct.NbPages  = 1;
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		 /*Error occurred while page erase.*/
		HAL_FLASH_GetError();
	}		
	pont_struct = &ini_struct_zera;
//Entra em loop ate gravar todos os dados da estrutura
	for(cont_bytes = 0;cont_bytes < bytes_struct_nr12;cont_bytes++)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (startAddress + ((cont_bytes)*4)), *pont_struct) != HAL_OK)
		{
			pont_struct = 0;
			return;
		}
		(pont_struct)++;
		while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) != 0) { }
	}
	HAL_FLASH_Lock();
	__enable_irq();
}

void read_flash(void)
{
	unsigned char cont_flash;
	unsigned int *pont_struct;
	unsigned char bytes_struct_nr12;
	bytes_struct_nr12 = (sizeof(struct_zeramento)/4);
//Inicia busca dos dados da flash
	pont_struct = &ini_struct_zera;
//Entra em loop ate buscar todos buscar toda estrutura de dados
	for(cont_flash = 0;cont_flash < bytes_struct_nr12;cont_flash++)
	{
		*pont_struct = *(uint32_t *)(startAddress + (cont_flash) * 4);
		(pont_struct)++;
	}
}

void valores_iniciais_flash(void)
{
	read_flash();
//Se nao gravados pega valores padrao
	if(ini_struct_zera == 0xFFFFFFFF)
	{	
		ini_struct_zera = 1;
		zero_eixo_y = 0;
		zero_eixo_x = 0;
		val_eixo = 0;
		eixo_sel = 0;
		salva_eeprom = 1;
		write_flash();
	}
}

