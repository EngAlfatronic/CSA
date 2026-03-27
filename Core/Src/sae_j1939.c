/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto Caixa 1 NR12
// Autor  Paulo Palaoro
// Versaon V1.0
// Date 18/04/2024
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "main.h"
#include "structs.h"
#include "perifericos.h"
#include "sae_j1939.h"


//Union utilizada para envio da mensagem  
union J1939_mensagem_tx_union_1
{
	struct
	{
		unsigned char	source_address;			 //Endereco de quem vai enviar
		unsigned char	PDU_specific;				 //Endereco de que vai receber
		unsigned char	PDU_format;					 //0 a 239 mensagem especifica 240 a 255 brodcast 
		unsigned char	data_page				:2;	 //Pagina o a 3
		unsigned char	priority				:3;	 //Prioridade 0 a 7
	}struct_J1939;
	unsigned int PGN_envio;		
}J1939_mensagem_tx;
#define source_address_tx    				J1939_mensagem_tx.struct_J1939.source_address
#define PDU_specific_tx    					J1939_mensagem_tx.struct_J1939.PDU_specific
#define PDU_format_tx    						J1939_mensagem_tx.struct_J1939.PDU_format
#define data_page_tx    						J1939_mensagem_tx.struct_J1939.data_page
#define priority_tx    							J1939_mensagem_tx.struct_J1939.priority
#define mensage_tx_1    						J1939_mensagem_tx.PGN_envio

//Union utilizada para recebimento das mensagens  
union J1939_mensagem_rx_union_1
{
	struct
	{				
		unsigned char	source_address;			 //Endereco de quem enviou
		unsigned char	PDU_specific;				 //Endereco de que vai receber
		unsigned char	PDU_format;					 //0 a 239 mensagem especifica 240 a 255 brodcast 
		unsigned char	data_page				:2;	 //Pagina o a 3
		unsigned char	priority				:3;	 //Prioridade 0 a 7
	}struct_J1939;
	unsigned int PGN_envio;		
}J1939_mensagem_rx_1;
#define source_address_rx    				J1939_mensagem_rx_1.struct_J1939.source_address
#define PDU_specific_rx    					J1939_mensagem_rx_1.struct_J1939.PDU_specific
#define PDU_format_rx    						J1939_mensagem_rx_1.struct_J1939.PDU_format
#define data_page_rx    						J1939_mensagem_rx_1.struct_J1939.data_page
#define priority_rx    							J1939_mensagem_rx_1.struct_J1939.priority
#define mensage_rx_1    						J1939_mensagem_rx_1.PGN_envio

extern CAN_HandleTypeDef hcan;

//Funcao para enviar uma mensagem
unsigned int monta_pgn(unsigned char pdu_format)
{
//Carrega valores de PGN de envio
	source_address_tx = ID_receptor;
//Responde a quem solicitou
	PDU_specific_tx	= ID_caixa_2;
	PDU_format_tx = pdu_format;
	data_page_tx	= pagina_0;
	priority_tx	= prioridade_3;
	return mensage_tx_1;
}

//Funcao para enviar uma mensagem
unsigned int monta_pgn_envio(unsigned char pdu_format, unsigned char ID_tx, unsigned char ID_rx)
{
//Carrega valores de PGN de envio
	source_address_tx = ID_rx;
//Responde a quem solicitou
	PDU_specific_tx	= ID_tx;
	PDU_format_tx = pdu_format;
	data_page_tx	= pagina_0;
	priority_tx	= prioridade_3;
	return mensage_tx_1;
}

//Funcao para enviar ascii do lcd
void envia_pacote_can_carregado(unsigned char pdu_format)
{
	static unsigned int val_milis;
	if(uwTick < (val_milis + 100))return;
	val_milis = uwTick; 
	uint8_t TxData[8];
	uint32_t mb;
	CAN_TxHeaderTypeDef TxMessage;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_EXT;
	TxMessage.DLC = 2;
//PDU_format, quem ta enviando, quem vai receber
	TxMessage.ExtId =  monta_pgn_envio(pdu_format, ID_transmissor, ID_cesto);
//for que conta quantidade de pacotes a enviar
	TxData[0] = 1;
	TxData[1] = (uint8_t)(eixo_final * 10);
	if(HAL_CAN_AddTxMessage(&hcan, &TxMessage, TxData, &mb) != HAL_OK)
	{
		HAL_Delay(1);
	}
}
