#include "i2c.h"
#include "keyboard.h"

static volatile unsigned char second, minute, hour,day,month,year, read_time_state=0;
volatile struct pt i2c_read, i2c_write;
extern unsigned char I2C_Stop_Flag;
//---------------------I2C_INIT------------------------------------
void INIT_I2C(void)
{
	PT_INIT(&i2c_read);
    PT_INIT(&i2c_write);
	TWBR=0xF;
}
//---------------------I2C_WRITE------------------------------------

PT_THREAD(Write_I2C(struct pt *pt,unsigned char address,unsigned char address_reg, unsigned char data))
{
	PT_BEGIN(pt);

	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//START
	
	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=START)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);
	}

	TWDR=address;//address+write
	
	TWCR=(1<<TWINT)|(1<<TWEN);//transfer address

	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=MT_SLA_ACK)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);
	}
	
	TWDR=(unsigned char)address_reg;//address+write
	
	TWCR=(1<<TWINT)|(1<<TWEN);//transfer address

	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=MT_DATA_ACK)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);
	}

	TWDR=data;//address+write
	
	TWCR=(1<<TWINT)|(1<<TWEN);//transfer address

	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=MT_DATA_ACK)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);
	}

	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP

	PT_END(pt);
}
//---------------------I2C_READ------------------------------------

PT_THREAD( Read_I2C(struct pt *pt,unsigned char address,unsigned char address_reg,unsigned char *buf))
{
	PT_BEGIN(pt);
	

	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//START
	
	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=START)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);
	}

	TWDR=address;//address+write
	
	TWCR=(1<<TWINT)|(1<<TWEN);//transfer address

	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=MT_SLA_ACK)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);
	}
	
	TWDR=address_reg;//
	
	TWCR=(1<<TWINT)|(1<<TWEN);//transfer address

	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=MT_DATA_ACK)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);	
	}


//------------------read---------------------
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//START2
	
	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=START_2)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);
	}

	
	TWDR=address+1;//address+read
	
	TWCR=(1<<TWINT)|(1<<TWEN);//transfer address

	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start

	if((TWSR&0xF8)!=MT_SLA_READ_ACK)//result
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP	
 		PT_EXIT(pt);
	}

    

	TWCR=(1<<TWINT)|(1<<TWEN);//read & nack

	PT_WAIT_WHILE(pt,!(TWCR&(1<<TWINT)));//...wait for start
	
	if((TWSR&0xF8)!=READ_NACK)
	{
		TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
		PT_EXIT(pt);		
	}
	buf[0]=TWDR;
	//buf[0]=0x11;
    
	
	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//STOP
	PT_END(pt);
}
//------------------------------------------------------------
PT_THREAD(ReadTime(struct pt *pt,unsigned char *led_buf))
{	
   PT_BEGIN(pt);

  while(1) 
   {
   		
	  PT_WAIT_WHILE(pt,I2C_Stop_Flag);  			

	   PT_DELAY(pt,50);
	   PT_SPAWN(pt, &i2c_read,Read_I2C(&i2c_read,TMR_ADDR,0x0,&second));
	   PT_DELAY(pt,5);
	   PT_SPAWN(pt, &i2c_read,Read_I2C(&i2c_read,TMR_ADDR,0x1,&minute));
	   PT_DELAY(pt,5);
	   PT_SPAWN(pt, &i2c_read,Read_I2C(&i2c_read,TMR_ADDR,0x2,&hour));
	   PT_DELAY(pt,5);
	   PT_SPAWN(pt, &i2c_read,Read_I2C(&i2c_read,TMR_ADDR,0x4,&day));
	   PT_DELAY(pt,5);
	   PT_SPAWN(pt, &i2c_read,Read_I2C(&i2c_read,TMR_ADDR,0x5,&month));
	   PT_DELAY(pt,5);
	   PT_SPAWN(pt, &i2c_read,Read_I2C(&i2c_read,TMR_ADDR,0x6,&year));


		led_buf[5]=((hour&0xF0)>>4);
		led_buf[4]=(hour&0x0F);

		led_buf[3]=(minute&0xF0)>>4;
		led_buf[2]=(minute&0x0F);

		led_buf[1]=(second&0xF0)>>4;
		led_buf[0]=(second&0x0F);

		led_buf[11]=((day&0xF0)>>4);
		led_buf[10]=(day&0x0F);
		
		led_buf[9]=(month&0xF0)>>4;
		led_buf[8]=(month&0x0F);
		
		led_buf[7]=(year&0xF0)>>4;
		led_buf[6]=(year&0x0F);
	}
	PT_END(pt);
}
//----------------------------------------------------------
PT_THREAD(ClockInit(struct pt *pt))
{	
   PT_BEGIN(pt);
	

	   PT_DELAY(pt,5);
	   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x0,0x0));
	   PT_DELAY(pt,5);
	   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x1,(0<<4)+0));
	   PT_DELAY(pt,5);
	   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x2,(1<<4)+2));	

	PT_END(pt);
}


/*void ClockInit(unsigned char *clock_buf)//инициализация часов, установка
{

uint8_t reg;


/*reg=Read_I2C(TMR_ADDR,0x0);
_delay_us(1000);



if((reg&0x80)==0)
	return;



Write_I2C(TMR_ADDR,0x2,(1<<4)+2);// 
_delay_us(100);
//printf("hour ok\n");

Write_I2C(TMR_ADDR,0x1,(0<<4)+0);// 
_delay_us(100);
//printf("minute ok\n");

Write_I2C(TMR_ADDR,0x0,0x0);// 
_delay_us(100);*/
//printf("second ok\n");

//}
//---------------------------------------------------
/*void StoreTime(unsigned char *buf)
{


	Write_I2C(TMR_ADDR,0x2,(buf[4]<<4)|buf[3]);// 
	_delay_us(100);
//	printf("hour ok %d%d\n",buf[4],buf[3]);

	Write_I2C(TMR_ADDR,0x1,(buf[1]<<4)|buf[0]);// 
	_delay_us(100);
//	printf("minute ok %d%d\n",buf[1],buf[0]);

	Write_I2C(TMR_ADDR,0x0,0x0);// 
	_delay_us(100);
//	printf("second ok %d%d\n",1,1);


}
//--------------------------------------------------------
void StoreDate(unsigned char *buf)
{


	Write_I2C(TMR_ADDR,0x5,(buf[1]<<4)|buf[0]);// 
	_delay_us(100);
//	printf("month ok %d%d\n",buf[1],buf[0]);

	Write_I2C(TMR_ADDR,0x4,(buf[3]<<4)|(buf[2]&0x7F));// 
	_delay_us(100);
//	printf("day ok %d%d\n",buf[3],buf[2]&0x7F);

	
}
//--------------------------------------------------------
void StoreYear(unsigned char *buf)
{
	Write_I2C(TMR_ADDR,0x6,(buf[1]<<4)|buf[0]);// 
	_delay_us(100);
//	printf("year ok %d%d\n",buf[1],buf[0]);	
}*/
