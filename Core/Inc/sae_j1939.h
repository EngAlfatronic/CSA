/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto Unidade de transferencia
// Autor  Paulo Palaoro
// Versaon V1.0
// Date 05/02/2013
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define J1939_MSG_LENGTH				5
#define J1939_DATA_LENGTH				8  

//Pagina utilizada
#define pagina_0    						0x00

#define ID_transmissor 			  	10    //0x0A
#define ID_receptor     				11    //0x0B
#define ID_inclinometro  				12    //0x0C
#define ID_cesto			  				13    //0x0D
#define ID_caixa_2		  				14    //0x0E
#define ID_momento		  				15    //0x0F
#define ID_saida_aux	  				16    //0x10
#define ID_manipulador_poste  	17    //0x11
#define ID_j7_j8						  	18    //0x12
#define ID_momento_flyjib		  	19    //0x13
#define ID_mso          		  	20    //0x14
//Prioridades utilizada
#define prioridade_0  					0x00
#define prioridade_1  					0x01
#define prioridade_2  					0x02
#define prioridade_3  					0x03
#define prioridade_4  					0x04
#define prioridade_5  					0x05
#define prioridade_6  					0x06
#define prioridade_7  					0x07 

//Prototipos
unsigned int monta_pgn(unsigned char pdu_format);
unsigned int monta_pgn_envio(unsigned char pdu_format, unsigned char ID_tx, unsigned char ID_rx);
void envia_pacote_can_carregado(unsigned char pdu_format);










