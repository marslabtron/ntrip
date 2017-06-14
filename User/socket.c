#include "socket.h"
#include "prefs.h"

#undef  PRINT
#define PRINT printf

#ifndef  TRUE
#define TRUE 1
#endif

#ifndef  FALSE
#define FALSE 0
#endif

typedef uint8_t BOOL;


static BOOL system_is_init = FALSE;

static int8_t qxwz_soc_init(void);
static int8_t init_GPRS(void);

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