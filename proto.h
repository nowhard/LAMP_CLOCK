#ifndef PROTO_H
#define PROTO_H

#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/pgmspace.h>
#include "timer1.h" 

#include <stdio.h>

#define HI(x) ((x)>>8)
#define LO(x) ((x)&0xFF)


#define ADRESS_DEV 			0x1 //адрес устройства
#define DEV_NAME   			"GENERIC DEVICE"

#define DEV_ID				0x10
#define DEV_SN				0x1
//--------------------------------------------------------------------------------
#define MAX_LENGTH_REC_BUF 	25 //максимальная длина принимаемого кадра
#define MIN_LENGTH_REC_BUF	5 //минимальная длина принимаемого кадра

#define MAX_LENGTH_TR_BUF  	25 //максимальная длина передаваемого кадра
#define CRC_LEN				2 //длина поля CRC
//-------------------------код операции-------------------------------------------
#define GET_DEV_INFO 		17 //получить информацию об устройстве
#define SET_SINGLE_REG		6 //установить значение одного регистра
#define SET_MULTIPLE_REG	16 //установить значение нескольких регистров
#define READ_HOLDING_REG	3 //чтение регистров хранения
#define READ_INPUT_REG		4 //чтение входных регистров
//--------------------------------------------------------------------
#define REG_ADDR_MIN		0//нижний адрес регистров
#define REG_ADDR_MAX		4//верхний адрес регистров
//--------------------------состояния цифрового автомата--------------
#define MD_RESTART		0	//рестарт цикла и обнуление
#define MD_WAIT 		1	//ожидание данных
#define	MD_PLACE_BUF	2	//прием данных в буфер
#define	MD_ADDR_CHECK	3	//проверка адреса
#define	MD_CRC_CHECK	4	//проверка контрольной суммы	
#define	MD_BUF_HANDLING	5	//обработка команды
#define MD_BUF_TRANSFER	6	//передача ответа ведущему
#define	MD_ERR_HANDLING	7	//обработка ошибок
//--------------------------------------------------------------------
unsigned char 	MD_STATE;//счетчик состояний
unsigned char	volatile  buf_count;//счетчик буфера
unsigned char	transf_count;//счетчик передаваемых байтов
unsigned char	buf_len;//длина передаваемого буфера
unsigned char	cnt_15; //счетчик 1.5 периода
unsigned char	cnt_35; //счетчик 3.5 периода
//------------------------флаги ошибок--------------------------------
unsigned char CRC_ERR;	//ошибка crc
unsigned char COMMAND_ERR;//неподдерживаемая команда
unsigned char TIMEOUT_15; //таймаут 1.5
unsigned char TIMEOUT_35; //таймаут 3.5
//--------------------------------------------------------------------
unsigned char  RecieveBuf[MAX_LENGTH_REC_BUF] ; //буфер принимаемых данных
unsigned char  TransferBuf[MAX_LENGTH_TR_BUF] ; //буфер передаваемых данных
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//прототипы
ISR(TIMER1_OVF_vect);
void Protocol_Init(void); //инициализация протокола
void Send_Info(void);     //посылка информации об устройстве
void SetSingleRegister(void);//установить значение одного регистра
void SetMultipleRegisters(unsigned char *regs);//установить значение нескольких регистров
void ReadHoldingReg(void);//чтение регистров хранения
//void ReadInputReg(unsigned char reg);//чтение входных регистров
//unsigned char ErrorMessage(unsigned char Error);//сообщение об ошибке
void ModBusBufHandling(void); //процесс обработки принятого запроса
void ModBusProcess(void);//главный процесс протокола modbus
void Timer3_Interrupt(void);//обрабтчик прерывания таймера
unsigned int CRC16( unsigned char * pucFrame, unsigned int usLen );//расчет CRC16

void stub(void);//заглушка для проверки


//--------------------------------------------------------------------
unsigned int controller_reg[5];
#define REG_0 0
#define REG_1 1
#define REG_2 2
#define REG_3 3
#define REG_4 4
//--------------------------------------------------------------------
#endif
