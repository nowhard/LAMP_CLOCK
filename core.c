#include "core.h"

// структура записи из списка обработчиков
typedef struct{
  msg_num msg; // обрабатываемое событие
  handler hnd; // собственно сам обработчик
} iHandler;

// структура события из буфера событий
typedef struct{
  msg_num msg; // номер события
  msg_par par; // параметр
} iMessage;

// структура таймера
typedef struct{
  msg_num msg; // номер генерируемого сообщения
  msg_par par; // его параметр
  unsigned int time; // таймер в условных тиках (сейчас 1 мсек)
} iTimer;

static volatile iTimer    lTimer[maxTimers]={0}; // список таймеров
static volatile iHandler   lHandler[maxHandlers]={0}; // список обработчиков

static volatile iMessage  lMessage[maxMessages]={0}; // буфер сообщений
static volatile unsigned int  lMesPointer=0,hMesPointer=0; // указатели на начало и конец буфера

//------------------------------------------------------------------------------
// установка обработчика события
// вызывается: setHandler(MSG_KEY_PRESS, &checkKey);
void setHandler(msg_num msg, handler hnd) {
  unsigned char i,j;
  i=0; j=0;
  while (i<maxHandlers) {
    if (lHandler[i].msg==0) { // ищем свободное место
      lHandler[i].hnd = hnd; // и регистрирем обработчик
      lHandler[i].msg = msg;
      break;
    }
    i++;
  }
}
//------------------------------------------------------------------------------
// снятие обработчика события
// вызывается: killHandler(MSG_KEY_PRESS, &checkKey);
/*void killHandler(msg_num msg, handler hnd) {
  unsigned char i,j;
  i=0; j=0;
  while (i<maxHandlers) {
    
    if ((lHandler[i].msg==msg) && (lHandler[i].hnd==hnd)) {
      lHandler[i].msg = 0; // если нашли нужный, очищаем
    }
    
    if (lHandler[i].msg != 0) {
      if (i != j) { // сдвигаем все записи к началу списка, чтобы дырок не было
        lHandler[j].msg = lHandler[i].msg;
        lHandler[j].hnd = lHandler[i].hnd;
        lHandler[i].msg = 0;
      }
      j++;
    }
    i++;
  }
}*/
//------------------------------------------------------------------------------
// занести событие в очередь
// пример вызова: sendMessage(MSG_KEY_PRESS, KEY_MENU)
void sendMessage(msg_num msg, msg_par par) 
{
  hMesPointer = (hMesPointer+1) & (maxMessages-1); // сдвигаем указатель головы
  
  lMessage[hMesPointer].msg = msg; // заносим событие и параметр
  lMessage[hMesPointer].par = par;
  if (hMesPointer == lMesPointer) 
  { // догнали начало очереди, убиваем необработанное сообытие
    lMesPointer = (lMesPointer+1) & (maxMessages-1);
  }
}
//------------------------------------------------------------------------------
// обработка событий
void dispatchMessage(void) 
 {	//возможно тормозит, оптимизировать
  char volatile i;
  unsigned char volatile res; //результат
  msg_num volatile msg;
  msg_par volatile par;
  
  if (hMesPointer == lMesPointer) 
  { // если пустая очередь - возврат
    return;
	
  }
  
  lMesPointer = (lMesPointer+1) & (maxMessages-1); // сдвинем указатель
  
  msg = lMessage[lMesPointer].msg;
  par = lMessage[lMesPointer].par;
  
  if (msg==0)
    return;

  
  for(i=maxHandlers-1; i>=0; i--) 
  { 
  // просматриваем обработчики с конца
    if (lHandler[i].msg==msg) 
	{ 
	// последний занесенный имеет приоритет
      res = lHandler[i].hnd(par); // вызываем обработчик
		 
      if (res)
	   { // если обработчик вернул 1, перываем обработку события
        break;
       }
    }
  }
  
}
//------------------------------------------------------------------------------
// установить таймер
// пример вызова: setTimer(MSG_LCD_REFRESH, 0, 50);


void setTimer(msg_num msg, msg_par par, unsigned int time) {
  unsigned char i,firstFree;
  firstFree = maxTimers+1;
  if (time == 0) 
  {
    sendMessage(msg, par);
  } 
  else 
  {
  
    for (i=0; i<maxTimers; i++) 
	{ 
	   // ищем установленный таймер
      if (lTimer[i].msg == 0) 
	  {
        firstFree = i;
      } 
	  else 
	  { 
	  // если нашли - обновляем время
        if ((lTimer[i].msg == msg) && (lTimer[i].par == par))
	    {
          lTimer[i].time = time;
          return;
        }  
      }
    }
    if (firstFree <= maxTimers)
	{ // иначе - просто добавляем новый
      lTimer[firstFree].msg = msg;
      lTimer[firstFree].par = par;
      lTimer[firstFree].time = time;
    }
  }
}
//------------------------------------------------------------------------------
// убить таймер
// особенность - убивает все установленные таймеры на данное событие,
// не зависимо от параметра события
/*void killTimer(msg_num msg) {
  unsigned char i;
  for (i=0; i<maxTimers; i++) {
    if (lTimer[i].msg == msg) {
      lTimer[i].msg = 0;
    }
  }
}*/
//------------------------------------------------------------------------------
// диспетчер таймеров
void dispatchTimer() {	 //похоже тормозит
  unsigned char i;
  msg_num msg;
  msg_par par;
  
  for (i=0; i<maxTimers; i++) 
  {    
		if (lTimer[i].msg == 0)
		{
	      continue;
		}
	    
	    if (lTimer[i].time == 0) 
		{ 
		// если пришло время
	      msg = lTimer[i].msg;
	      par =lTimer[i].par;
	      lTimer[i].msg = 0;
		  	 
	      sendMessage(msg, par); // создаем событие
		  //LED1=~LED1; 
	    } 
	else 
	{
      lTimer[i].time--; // иначе просто уменьшаем время
    }
  }
}
//#pragma ot(0)
//------------------------------------------------------------------------------
//#pragma AREGS
ISR(TIMER0_OVF_vect) //обработчик прерывания таймера0 
{
	dispatchTimer(); 
	TCNT0=130;
	TIFR&=!(1<<TOV0);
	
	return;
}
//#pragma NOAREGS
