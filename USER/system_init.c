#include "system_init.h"
#include "config.h"

//ϵͳ��ʼ������ʼ��˳��Ҫ�����޸�
void System_Init()
{

	NVIC_Configuration();	 
	delay_init();	    					//��ʱ������ʼ��
	control_init ( );						//���Ƴ�ʼ��
	Config_Init( );							//���ó�ʼ��Ҫ�ڿ��Ƴ�ʼ��֮��
	Remote_init (38000);				//�����ز�Ƶ����38000hz
	Receive_init ( );						//������ճ�ʼ��
	
	
	
	
	USART1_Init(115200);				//RF������Ϣ����
	AS32_Param_Init( );					//����ģ���ʼ��
	TIM4_TIMEout_init();				//��ʱ�Զ����豸��ʱ��
	USART3_Init(9600);					//����3��©ˮ����
	IWDG_Init(7,60000) ;				//�������Ź���ʼ��
}



