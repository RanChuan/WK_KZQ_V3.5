/*****************************************************************************


			�������Լ���������������Ľ����߼�
			���ߣ�Ƚ��
			���ʱ�䣺2018.3.15


*******************************************************************************/

#ifndef  __CMD_H
#define  __CMD_H
#include "usart.h"
#include "delay.h"
#include "control.h"
#include "remote.h"
#include "crc8_16.h"

//���������̰�����
#define PRESS_SHORT 1
#define PRESS_LONG 2
#define PRESS_NONE 0



//ģʽ2�еĴ������Ͷ���

#define ERR_SUCCESS         0x0000			//�ɹ�
#define ERR_FAIL						0x0001			//δ֪����
#define ERR_DATAVALE				0x0002			//���ݴ���
#define ERR_DATALENGTH			0x0003			//���ݳ��ȴ���
#define ERR_WANTLENGTH			0x0004			//��Ҫ��ȡ�����ݳ��ȴ���
#define ERR_NULLCMD					0x0005			//��֧�ֵ��������ͣ�
#define ERR_NOCONNECT				0x0006			//û�����ӣ�һ�в�����Ҫ��ͨ�ſ�ʼ֮��
#define ERR_NOCJADDR				0x0007			//�ɼ����ĵ�ַ���Ǳ����󶨵Ĳɼ�����ַ
#define ERR_CANNOTCFG				0x0008			//�ڲ��ǿ�����״̬���յ�����������
#define ERR_NOTMYTYPE				0x0009			//��Ҫ���Ƶ��豸�뱾������
#define ERR_TIMEOUT					0x000a   //��ʱ














/*��cmd.c�ﶨ��ĺ���*/

extern void (*Get_cmd) (u8 *buff1);
u8 Get_Key(u8 keynum);

							//ģʽ1���������
void Get_cmd_MODE1 (u8 *buff1);

							//ģʽ2���������
void Get_cmd_MODE2 (u8 *buff1);

					//���»�����������ֵ
u8 Cmd_Limit_Updata(u8 *);

void CS_BUSY_IRQ (void);

void TIM4_TIMEout_init (void );
void Cfg_Model (void);

//���outputOld��outputNew�ǲ�����ͬ���������ִͬ������ͬ

void run_output (void);


void Cmd_kt_on(void);//�յ���
void Cmd_kt_off(void);//�յ���
void Cmd_cs_on(void);//��ʪ����
void Cmd_cs_off(void);//��ʪ����
void Cmd_kt_up (void);//�յ�����
void Cmd_kt_down (void);//�յ�����
void Cmd_jh_on (void);//��������
void Cmd_jh_off (void);//��������






/*��cmd1.c�ﶨ��ĺ���*/
				//֧���µĶ���Э��

void Get_cmd_NEW_MODE (u8 *buff1);
void Run_Time_IRQ (void);

void Return_NEW (u8,u16 );
					//�����ҵ��豸
void MyDevice_On (u8 state,u8 updown);



void cmd_0x01 (u8 *buff);
void cmd_0x02 (u8 *buff);
void cmd_0x03 (u8 *buff);
void cmd_0x04 (u8 *buff);
void cmd_0x05 (u8 *buff);
void cmd_0x06 (u8 *buff);
void cmd_0x07 (u8 *buff);
void cmd_0x08 (u8 *buff);
void cmd_0x09 (u8 *buff);





#endif

