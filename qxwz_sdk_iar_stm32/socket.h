#ifndef __QXWZ_SOC_H
#define __QXWZ_SOC_H
#include <stdlib.h>
#include <stdint.h>
#define QXWZ_SOCKET_DATA_CACHE_LEN 1024//socket���ݰ������С
typedef int qxwz_ssize_t;
typedef int8_t qxwz_soc;


typedef struct {
    char *hostName;
    short port;
}qxwz_soc_address;//���ӵ�ַ

/**
 *  ����һ��TCP(***����ʵ��***)
 *  @return tcpͨ����
 */
qxwz_soc qxwz_soc_create(void);


/**
 *  ���ӵ�ָ���ķ�����(***����ʵ��***)
 *
 *  @param soc         tcpͨ����(�ɵ��÷���qxwz_soc_create���)
 *  @param address ��������ַ
 *
 *  @return ����״̬(0:���ӳɹ�,1:address��Ч,2:����ʧ��)
 */
int8_t qxwz_soc_connect(qxwz_soc soc,qxwz_soc_address address);



/**
 *  ��������(***����ʵ��***)
 *
 *  @param soc         tcpͨ����
 *  @param send_buffer ���͵����ݻ���
 *  @param length      �������ݵĳ���
 *
 *  @return ʵ�ʷ������ݵĳ���(�������ʧ�ܷ���-1)
 */

qxwz_ssize_t qxwz_soc_send(qxwz_soc soc,char *send_buffer,size_t length); 


/**
 *  ���յ����ݻص������������յ����ݵ���ʱ�����������ñ�������
 *	Ŀǰsdk��ȡ�ķ����ǣ�ÿ�����ݰ���С���������С����ôֱ�Ӷ�����������������д治����һ�����ݰ��ˣ���ô������ݰ�Ҳ�ᱻ����
 *  @param soc         tcpͨ����
 *  @param send_buffer �������ݻ���
 *  @param length     	�������ݵĳ���
 *
 */
void qxwz_on_soc_data_received(qxwz_soc soc, char * buffer, size_t length);

/**
 *  �ر�ĳ��TCP����(***����ʵ��***)
 *
 *  @param soc TCPͨ����
 */
void qxwz_soc_close(qxwz_soc soc);

/**
 *  ��socket���ӷ��������ʱ������������֪ͨ��SDK
 *
 *  @param soc TCPͨ����
 */
void qxwz_soc_error(qxwz_soc soc);
 




/** -------------------------------------------- ʹ���첽SOCKETʱ���� -------------------------------------------------*/


/**
 *  ������ɻص�,ʹ���첽socketʱʹ��
 *
 *  @param soc         tcpͨ����(�ɵ��÷���qxwz_soc_create���)
 *  @param connect_status �Ƿ����ӳɹ�(0:���ӳɹ�,1:address��Ч,2:����ʧ��)
 */
void qxwz_soc_connect_complete(qxwz_soc soc,int8_t connect_status);



/**
 * 	���ݷ�����ɻص�
 *
 *  @param soc         tcpͨ����(�ɵ��÷���qxwz_soc_create���)
 *  @param sendLength ʵ�ʷ������ݵĳ���(�������ʧ�ܷ���-1)
 */
void qxwz_soc_send_complete(qxwz_soc soc,qxwz_ssize_t send_length);




/**
 *  �ر�ĳ��TCP���ӣ��ص�
 *
 *  @param soc TCPͨ����
 *  @param close_status �Ƿ�رճɹ�(0:�رճɹ�,1:address��Ч,2:�ر�ʧ��)
 */
void qxwz_soc_close_complete(qxwz_soc soc,int8_t close_status);

#endif
