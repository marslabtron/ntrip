#include "string.h"
#include "stdio.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_rcc.h"
#include "prefs.h"
#include "qxwz_rtcm.h"
#include "socket.h"

#undef  PRINT
#define PRINT printf

#ifndef  TRUE
#define TRUE 1
#endif

#ifndef  FALSE
#define FALSE 0
#endif

char uart1_gp=0,uart2_gp=0;
char MODE=0;//0=DEBUG 1=TRANS


int gprs_step=0;

char gprs_rx[2048];
int gprs_rxi=0;

char gprs_tx[2048];
int gprs_txi=0;

int send_step=0;

char strbuf[1024];
int strbufi;


typedef uint8_t BOOL;


static BOOL system_is_init = FALSE;

static int8_t qxwz_soc_init(void);
static int8_t init_GPRS(void);


int buff_find(char *src,int src_len,char *dest,int dest_len)
{
	int i,j,c;
	if(src_len<dest_len)
	{
		return -1;
	}
	for(i=0;i<src_len;i++)
	{
		if(src[i]==dest[0])
		{
			c=0;
			for(j=0;j<dest_len;j++)
			{
				if(src[i+j]==dest[j])
				{
					c++;
					if(c==dest_len)
					{
						return i;
					}					
				}
				else
				{
					break;
				}
			}
		}
	}
 	return -1;
}

void delay_ms(int time)
{
//	dly_tmr.Set(sys_tmr);
//	while(sys_tmr.GetTotalMS()-dly_tmr.GetTotalMS()<time);
   int i=0;
   while(time--) 
   { 
      i=12000;
      while(i--);
   }
}

void GPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	// UART1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
	
	// UART2
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	// BSY | SYS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// MODE (INPUT)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// SWDIO (OUTPUT)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// SWCLK (INPUT)
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void NVIC_init()
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* NVIC configuration */
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 11;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void UART1_init()
{
  USART_InitTypeDef USART_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_Parity = USART_Parity_No;  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
  USART_Cmd(USART1, ENABLE);
}

void UART1_send_byte(char data)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, data);
}
void UART1_send(char* data,int len)
{
	int i=0;
	for(i=0;i<len;i++)
	{
		UART1_send_byte(data[i]);
	}
}
void UART1_send_string(char* data)
{
	while(*data)
	{
		UART1_send_byte(*data++);
	}
}

void UART2_init()
{
  USART_InitTypeDef USART_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_Parity = USART_Parity_No;  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
  USART_Cmd(USART2, ENABLE);
}

void UART2_send_byte(char data)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, data);
}
void UART2_send(char* data,int len)
{
	int i=0;
	for(i=0;i<len;i++)
	{
		UART2_send_byte(data[i]);
	}
}
void UART2_send_string(char* data)
{
	while(*data)
	{
		UART2_send_byte(*data++);
	}
}
void gprs_setup_connection()
{
	gprs_step=1;
	while(gprs_step<7)
	{
		if(gprs_step==1)
		{
			if(buff_find(gprs_rx,gprs_rxi,"AT\r\r\nOK\r\n",9)<0)
			{
				gprs_rxi=0;
				UART2_send_string("AT\r\n");
			}
			else
			{
				gprs_step++;
			}
		}
		else if(gprs_step==2)
		{		
			if(buff_find(gprs_rx,gprs_rxi,"AT+QPOWD=1\r\r\nNORMAL POWER DOWN \r\n",31)<0)
			{
				gprs_rxi=0;
				UART2_send_string("AT+QPOWD=1\r\n");
			}
			else
			{
				gprs_step++;
			}
		}
		else if(gprs_step==3)
		{
			if(buff_find(gprs_rx,gprs_rxi,"AT\r\r\nOK\r\n",9)<0)
			{
				gprs_rxi=0;
				UART2_send_string("AT\r\n");
			}
			else
			{
				gprs_step++;
			}
		}
		else if(gprs_step==4)
		{
			if(buff_find(gprs_rx,gprs_rxi,"AT+CGREG?\r\r\n+CGREG: 0,1\r\n\r\nOK\r\n",31)>=0)
			{
				gprs_step++;
			}
			else if(buff_find(gprs_rx,gprs_rxi,"AT+CGREG?\r\r\n+CGREG: 0,5\r\n\r\nOK\r\n",31)>=0)
			{				
				gprs_step++;
			}
			else
			{				
				gprs_rxi=0;
				UART2_send_string("AT+CGREG?\r\n");
			}
		}
		else if(gprs_step==5)
		{
			if(buff_find(gprs_rx,gprs_rxi,"\r\r\nOK\r\n",7)<0)
			{
				gprs_rxi=0;
				UART2_send_string("AT+QIOPEN=\"TCP\",\"123.123.123.123\",6006\r\n");
			}
			else
			{
				gprs_step++;
			}
		}
		else if(gprs_step==6)
		{
			if(buff_find(gprs_rx,gprs_rxi,"CONNECT OK\r\n",12)<0)
			{
			}
			else
			{
				gprs_rxi=0;
				gprs_step++;
			}
		}
		delay_ms(200);
	}
}
void send_steps(char d[],int len)
{
	int i=0;
	send_step=1;
	while(send_step<4)
	{
		if(send_step==1)
		{
			gprs_rxi=0;
			sprintf(strbuf,"AT+QISEND=%d\r\n",len);
			UART2_send_string(strbuf);
			send_step++;
		}
		else if(send_step==2)
		{
			if(buff_find(gprs_rx,gprs_rxi,"> ",2)>-1)
			{
				gprs_rxi=0;
				for(i=0;i<len;i++)
				{
					UART2_send_byte(d[i]);
				}
				send_step++;
			}
		}
		else if(send_step==3)
		{
			if(buff_find(gprs_rx,gprs_rxi,"SEND OK",7)>-1)
			{
				gprs_rxi=0;
				send_step++;				
			}
		}
	}
}



qxwz_soc qxwz_soc_create(void)
{
//  qxwz_soc socket_fd = -1;
//  socket_fd = qxwz_soc_init();
//  if(socket_fd >= 0)
//  {
//    PRINT(" socket create successful, socket_fd = %d \r\n", socket_fd);
//    return socket_fd;
//  }
//  else
//  {
//    PRINT(" socket create failed!");
//    return -1;
//  }
    //qxwz_soc_init();
  gprs_setup_connection();
}

int8_t qxwz_soc_connect(qxwz_soc soc,qxwz_soc_address address)
{
  
}

qxwz_ssize_t qxwz_soc_send(qxwz_soc soc,char *send_buffer,size_t length)
{
  
}

//void qxwz_on_soc_data_received(qxwz_soc soc, char * buffer, size_t length)
//{
//  
//}

void qxwz_soc_close(qxwz_soc soc)
{
  
}

//void qxwz_soc_error(qxwz_soc soc)
//{
//  
//}

//void qxwz_soc_connect_complete(qxwz_soc soc,int8_t connect_status)
//{
//  
//}

//void qxwz_soc_send_complete(qxwz_soc soc,qxwz_ssize_t send_length)
//{
//}

//void qxwz_soc_close_complete(qxwz_soc soc,int8_t close_status)
//{
//}

int8_t qxwz_soc_init(void)
{
//  int8_t temp = 0;
//  if(system_is_init == FALSE)
//  {
//    //temp = init_GPRS();
//    gprs_setup_connection();
//    if(temp == 0)
//    {
//      system_is_init = TRUE;
//    }
//    else
//    {
//      PRINT("init GPRS Module failed£¬return code = %d\r\n", temp);
//      return -1;
//    }
//  }
//  else 
//    return 1;
    gprs_setup_connection();
    return 0;
}
       
static int8_t init_GPRS(void)
{
  
}

int main()
{
        qxwz_soc sockfd;
	qxwz_soc_address address;
	 
	config.appkey = "";
	config.appsecret = "";
	config.device_ID = "";
	config.device_Type = "";
	SysTick_Config(48000);
	
	GPIO_init();
	NVIC_init();
	
	UART1_init();
	UART2_init();
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//BSY
	GPIO_SetBits(GPIOB,GPIO_Pin_13);//SYS
	
	//gprs_setup_connection();
        qxwz_soc_create();
	
	while(1)
	{
		send_steps("hello\r\n",7);
		delay_ms(10000);
	}
}


void USART1_IRQHandler()
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//SYS
	if(USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		char rd=USART_ReceiveData(USART1);
		gprs_tx[gprs_txi++]=rd;
		//uart1_rb[uart1_rbi++]=rd;
	}
	if(USART_GetITStatus(USART1, USART_IT_IDLE))
	{
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
		uart1_gp=1;
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);//SYS
}

void USART2_IRQHandler()
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//SYS
	if(USART_GetITStatus(USART2, USART_IT_RXNE))
	{
		char rd=USART_ReceiveData(USART2);
		gprs_rx[gprs_rxi++]=rd;
                UART1_send_byte(rd);
	}
	if(USART_GetITStatus(USART2, USART_IT_IDLE))
	{
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
		uart2_gp=1;
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);//SYS
}
