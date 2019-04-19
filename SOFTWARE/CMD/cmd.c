#include "cmd.h"
#include "config.h"
#include "drv_as62.h"
#include "uart_485.h"
#include "receive.h"


				//ģʽ1�õ���Э��
void Cmd_return(u16 num);//��������
void Cmd_err (void);//����
void Cmd_Icant (void); //��֧�ֵ�����




					//ģʽ2�����ݷ��غ�����cmdΪ0x03��0x06
void Cmd_return_MODE2 (u8 cmd,u16 datalength);
void Cmd_err_MODE2 (u8 cmd,u16 err);				//ģʽ2��ͨ�Ŵ��󷵻�



							//�������ʱ�Զ��رռ�ʱ������λ����
 u16 MY_OFFTIME=0;

										//��ʪ������ֹͣ���ɲ�����־
										//����5���Ӳ��ɹرգ��ر�5���Ӳ��ɴ�
u16 CS_CANTCMD=0;
static u16 CS_BUSY_TIME=5*60*100;//5����CD���ݼ�����
static u8  CS_LAST_CMD=0;//��ʪ����CD������ִ�е�����
#define  CS_OUTTIME 5*60*100


//��ʪ�����յ��������£�����������


		//�����յ��縨��������30��
const u8 GREE_UP[10]={0x30,0x70,0x00,0x0a,0x40,0x00,0x40,0x00,0x60};
		//�����յ�����ģʽ������16��
const u8 GREE_DOWN[10]={0x90,0x00,0x00,0x0a,0x40,0x00,0x40,0x00,0xa0};
		//�����յ��Զ�ģʽ����
const u8 GREE_ON[10]={0x90,0x90,0x04,0x8a,0x40,0x00,0x40,0x00,0xf0};
		//�����յ��Զ�ģʽ�ػ�
const u8 GREE_OFF[10]={0x00,0x90,0x00,0x0a,0x40,0x00,0x40,0x00,0xa0};



		//���Ŀյ��縨��������30��
const u8 MIDEA_UP[10]={0xb2 ,0x4d ,0xbf ,0x40 ,0xbc ,0x43,0x00,0x00};
		//���Ŀյ�����ģʽ������16��
const u8 MIDEA_DOWN[10]={0xb2 ,0x4d ,0xbf ,0x40 ,0x00 ,0xff,0x00,0x00};
		//���Ŀյ��Զ�ģʽ����
const u8 MIDEA_ON[10]={0xb2, 0x4d ,0x1f ,0xe0 ,0xd8 ,0x27,0x00,0x00};
		//���Ŀյ��Զ�ģʽ�ػ�
const u8 MIDEA_OFF[10]={0xb2 ,0x4d ,0x7b ,0x84 ,0xe0 ,0x1f,0x00,0x00};




/*************************************************************************************

��ȡ�����ʽ��
0xf0,0xfe,addr=MY_ADDR,cmd=0x03,fun_h=0,fun_l=0x02,data0=0,data1=0x04,crc_h,crc_l
����֧�ֵĶ�ȡ����̶�Ϊ10���ֽ�
�еȺŵı�����ֵֻ��Ϊ�Ⱥ��ұߵĹ̶�ֵ
fun_0,fun_1�ǹ��ܱ�ʶ�������ǿ�����
�����⵽��ʽ���Իᷢ��������Ϣ���߲���Ӧ��

��ȡ�������ݸ�ʽ��
0xf0 ,0xfe ,addr ,cmd=0x03 ,msg0=0x00 ,mag1=0x02 ,long_h=0x00 ,long_l=0x04,
MY_STATE[0],MY_STATE[1],MY_STATE[2],MY_STATE[3],crc_h,crc_l
�����ķ������ݹ̶�Ϊ14�ֽڣ�����ֻ���ڶ�ȡ����������ȡ�����ݳ���Ϊ4ʱ�Ż᷵��
�������ص��еȺŵı����̶�Ϊ�Ⱥ��ұߵ�ֵ��������Ϊ���������ı仯���仯
û�еȺŵı���������������Щֵ��
MY_STATE[0]��0����ʪ���أ�		1����ʪ����									
MY_STATE[1]��0���յ�,�أ�			1���յ���										
MY_STATE[2]��0����״̬��			1���յ����£�		2���յ�����	
MY_STATE[3]��0�������������أ�������ʱ��֧�ֿ����������Ŀ��ƣ��̶�Ϊ0��


���������ʽ��
0xf0,0xfe,addr=MY_ADDR,cmd=0x06,msg0,msg1,fun_h=0,fun_l=0x02,data0,data1,crc_h,crc_l
����֧�ֵĿ�������̶�Ϊ12���ֽ�
fun_0,fun_1�ǹ��ܱ�ʶ�������ǿ�����
�еȺŵı����ǹ̶��ģ�����ֻ�ܽ������ָ�ʽ������
����ͨ����msg0��msg1������ֵ���ж������Ƴ�ʪ���Ϳյ�����ͣ
			-------------------------
			|   msg0    |   msg1    |
			-------------------------
			|   0x10    |   0x11    |�¶�̫�ߣ�ͨ���˿ɿ��յ���������
			-------------------------
			|   0x10    |   0x12    |�¶�̫�ͣ�ͨ���˿ɿ��յ���������
			-------------------------
			|   0x20    |   0x21    |ʪ��̫�ߣ�ͨ���˿ɿ���ʪ��
			-------------------------
			|   0x20    |   0x22    |ʪ��̫�ͣ�ͨ���˿ɹس�ʪ��
			-------------------------
			������ֻͨ��msg0��msg1�����Ƴ�ʪ��
			
û�еȺŵı���������������Щֵ��						
			-------------------------							
			|data0			|  data1		|							
			-------------------------							
			|		0				|			0			|�յ���				
			-------------------------							
			|		0				|			1			|�յ���				
			-------------------------							
			|		0				|			2			|�յ���				
			-------------------------							
			|		1				|			0			|�յ���				
			-------------------------							
			|		1				|			1			|�յ���������	
			-------------------------							
			|		1				|			2			|�յ���������	
			-------------------------							
�����⵽��ʽ���Իᷢ��������Ϣ���߲���Ӧ��

*****************************************************************************************/
#define __FR_DEBUG 0



						//�����������ָ��
void (*Get_cmd) (u8 *buff1);







//�����ǿ�������ִ�к���

void Cmd_cs_on (void)
{
	MY_OFFTIME=0;
	if (CS_STATE!=0x01)
	{
			control_run (DEVICE_ON,1);
			control_run (CMD_UP,0);
			control_run (CMD_DOWN,0);
			CS_STATE=0x01;
			control_run (POWER_1,1);
			control_run (POWER_2,1);
			
	}

#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("��ʪ������ִ��\r\n");
#endif
}

void Cmd_cs_off (void)
{
	MY_OFFTIME=0;
	if (CS_STATE!=0x00)
	{
			control_run (DEVICE_ON,0);
			control_run (CMD_UP,0);
			control_run (CMD_DOWN,0);
			CS_STATE=0x00;
			control_run (POWER_1,0);
			control_run (POWER_2,0);
	}
	
	
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("��ʪ������ִ��\r\n");
#endif
	
}

void Cmd_err (void)
{
	
	
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("�������\r\n");
#endif
	
	
}

void Cmd_kt_on (void)
{
	MY_OFFTIME=0;
	//�������յ��Ŀ�������

	if (KT_STATE!=0x01)
	{
		control_run (POWER_1,1);
		control_run (POWER_2,1);

		
	while (Remote_GetState ( ));//��֤���ⷢ��ʱ���е�
	if (KT_STYLE==GREE)
		Remote_send ((u8*)GREE_ON,GREE);
	else if (KT_STYLE==MIDEA)
		Remote_send ((u8*)MIDEA_ON,MIDEA);
	else if (KT_STYLE==USER)
		Remote_send_USER(1);
		
		
		control_run (DEVICE_ON,1);
		control_run (CMD_UP,0);
		control_run (CMD_DOWN,0);
		KT_STATE=0x01;
	}
	
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("�յ�����ִ��\r\n");
#endif
}

void Cmd_kt_off(void)
{
	MY_OFFTIME=0;
	//�������յ��Ĺػ�����
	if (KT_STATE!=0x00)
	{
		control_run (POWER_1,0);
		control_run (POWER_2,0);

	while (Remote_GetState ( ));
	if (KT_STYLE==GREE)
		Remote_send ((u8*)GREE_OFF,GREE);
	else if (KT_STYLE==MIDEA)
		Remote_send ((u8*)MIDEA_OFF,MIDEA);
	else if (KT_STYLE==USER)
		Remote_send_USER(2);


		
		control_run (DEVICE_ON,0);
		control_run (CMD_UP,0);
		control_run (CMD_DOWN,0);
		KT_STATE=0x00;
	}
	KT_STATE1=0x00;
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("�յ�����ִ��\r\n");
#endif
}



void Cmd_kt_up (void)
{
	MY_OFFTIME=0;
	//�������յ��Ŀ�������
	if (KT_STATE1!=0x01)
	{
		control_run (POWER_1,1);
		control_run (POWER_2,0);

		while (Remote_GetState ( ));
	if (KT_STYLE==GREE)
		Remote_send ((u8*)GREE_UP,GREE);
	else if (KT_STYLE==MIDEA)
		Remote_send ((u8*)MIDEA_UP,MIDEA);
	else if (KT_STYLE==USER)
		Remote_send_USER(3);


		control_run (DEVICE_ON,1);
		control_run (CMD_UP,1);
		control_run (CMD_DOWN,0);
		KT_STATE1=0x01;
	}
	KT_STATE=0x01;
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("�յ�������ִ��\r\n");
#endif
}

void Cmd_kt_down (void)
{
	MY_OFFTIME=0;
	//�������յ��Ŀ�������
	if (KT_STATE1!=0x02)
	{
		control_run (POWER_1,0);
		control_run (POWER_2,1);

		while (Remote_GetState ( ));
	if (KT_STYLE==GREE)
		Remote_send ((u8*)GREE_DOWN,GREE);
	else if (KT_STYLE==MIDEA)
		Remote_send ((u8*)MIDEA_DOWN,MIDEA);
	else if (KT_STYLE==USER)
		Remote_send_USER(4);
		
		
		
		control_run (DEVICE_ON,1);
		control_run (CMD_UP,0);
		control_run (CMD_DOWN,1);
		KT_STATE1=0x02;
	}
		KT_STATE=0x01;
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("�յ�������ִ��\r\n");
#endif
}

void Cmd_Icant (void)
{
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("��֧�ֵ�����\r\n");
#endif
	
}

void Cmd_jh_on (void)//��������
{
	MY_OFFTIME=0;
	if (JH_STATE!=0x01)
	{
		control_run (POWER_1,1);
		control_run (POWER_2,1);


		while (Remote_GetState ( ));//��֤���ⷢ��ʱ���е�
		Remote_send_USER(1);


		
		control_run (DEVICE_ON,1);
		control_run (CMD_UP,0);
		control_run (CMD_DOWN,0);
		JH_STATE=0x01;
	}
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("����������ִ��\r\n");
#endif

}
void Cmd_jh_off (void)//��������
{
	MY_OFFTIME=0;
	if (JH_STATE!=0x00)
	{
		control_run (POWER_1,0);
		control_run (POWER_2,0);

		while (Remote_GetState ( ));//��֤���ⷢ��ʱ���е�
		Remote_send_USER(2);

		
		control_run (DEVICE_ON,0);
		control_run (CMD_UP,0);
		control_run (CMD_DOWN,0);
		JH_STATE=0x00;
	}
#if __FR_DEBUG==1
	printf ("��������\r\n\t");
	printf ("����������ִ��\r\n");
#endif

}



//��������Ϣ����������ģʽ�����Կ��ƹ���
//����ģʽ�����óɹ�������Զ��˳�����������Ҳ���˳�
//�ڲ��Թ���ʱ������ʱ�ı�ϵͳ״̬���˳�����ģʽʱ�ָ�
void Cfg_Model (void)
{
	u8 key=0;
	static u8 i=0;//��̬ѭ������
	key=Get_Key(0);
	if (key==PRESS_SHORT)
	{
		if (MY_MODEL==0)
		{
			MY_MODEL=1;
			//control_run(MODEL,1);
			DATA_MAP->outputNew |=1<<(MODEL-1);

		}
		else
		{
			i++;
			switch (i)
			{
													
				case 1:
					MyDevice_On(1,0);
					break;
				case 2:
					MyDevice_On(1,1);
					break;
				case 3:
					MyDevice_On(1,2);
					break;
				case 4:
					MyDevice_On(0,0);
					break;
				default:
					i=0;
					break;
					
			}
		}
	}
	else if (key==PRESS_LONG)
	{
			MY_MODEL=0;								//�˳�����ģʽ
			//control_run(MODEL,0);
			DATA_MAP->outputNew &=~(1<<(MODEL-1));
			//control_run(STYLE_KT,0);
			DATA_MAP->outputNew &=~(1<<(STYLE_KT-1));
			//control_run(STYLE_CS,0);
			DATA_MAP->outputNew &=~(1<<(STYLE_CS-1));
			//control_run(STYLE_JH,0);
			DATA_MAP->outputNew &=~(1<<(STYLE_JH-1));
			//control_run(DEVICE_ON,0);
			DATA_MAP->outputNew &=~(1<<(DEVICE_ON-1));
			//control_run(CMD_UP,0);
			DATA_MAP->outputNew &=~(1<<(CMD_UP-1));
			//control_run(CMD_DOWN,0);
			DATA_MAP->outputNew &=~(1<<(CMD_DOWN-1));
			//MyDevice_On(0,0);
			//Config_Init( );						//���³�ʼ��ϵͳ
			i=0;
	}
	if (MY_MODEL==1)
	{
		MY_OFFTIME=0;//�����Զ��ػ�����������ֹ�Զ��ػ�
		if (KT_USER_CMD>=1&&KT_USER_CMD<=4)
		{
//			Receive_Cmd(ENABLE);
			if (Receive_GetState()) 
			{
				//���������ң�ؼ�ֵ���˳�ѧϰģʽ
				Receive_SaveKey(KT_USER_CMD);
				KT_USER_CMD=0;		//�˳��յ�����ѧϰ
				//control_run(DEVICE_ON,0);
				DATA_MAP->outputNew &=~(1<<(DEVICE_ON-1));
				//control_run(CMD_UP,0);
				DATA_MAP->outputNew &=~(1<<(CMD_UP-1));
				//control_run(CMD_DOWN,0);
				DATA_MAP->outputNew &=~(1<<(CMD_DOWN-1));
				Receive_Cmd(DISABLE);
			}
		}
	}
}








void TIM4_TIMEout_init (void )
{
/***********************************************************************************************

															��ʱ��4Ĭ�ϳ�ʼ��

*****************************************************************************************************/

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 720-1; //��ÿ10ms��������һ
	TIM_TimeBaseStructure.TIM_Prescaler =1000; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

 
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //�����������ж�

	//�ڷ��ͺ����ʱ���ٴ򿪶�ʱ��
	TIM_Cmd(TIM4,ENABLE ); 	//ʧ�ܶ�ʱ��1
	
	
	
}







//���outputOld��outputNew�ǲ�����ͬ���������ִͬ������ͬ

void run_output (void)
{
	u16 state_check=0;
	u16 state_old=0;
	u16 state_new=0;
	if (DATA_MAP->outputOld!=DATA_MAP->outputNew)
	{
		state_old=DATA_MAP->outputOld&0x01ff;
		state_new=DATA_MAP->outputNew&0x01ff;
		state_check=state_old^state_new;		//��ͬΪ1
		for (u8 i=0;i<9;i++)
		{
			if (state_check&(1<<i))		//�����ͬ
			{
				if (state_new&(1<<i))
					control_run(i+1,1);//ִ��Ϊ�µ�״̬
				else
					control_run(i+1,0);//ִ��Ϊ�µ�״̬
			}
		}
		
		state_old=(DATA_MAP->outputOld&(0x3<<9))>>9;
		state_new=(DATA_MAP->outputNew&(0x3<<9))>>9;
		if (state_old!=state_new)
		{
			switch (state_new)
			{
				case 0:		//��
					Remote_send_USER(2);
					break;
				case 1:		//��
					Remote_send_USER(1);
					break;
				case 2:		//��
					Remote_send_USER(3);
					break;
				case 3:		//��
					Remote_send_USER(4);
					break;
				default:
					break;
			}
		}

		state_old=(DATA_MAP->outputOld&(0x1<<11))>>11;
		state_new=(DATA_MAP->outputNew&(0x1<<11))>>11;
		if (state_old!=state_new)
		{
			yiti_power(state_new);
		}
	}
}














//����

#define KEY PBin(2)
static u8 KEYS;


//���ذ���״̬
u8 Get_Key(u8 keynum)
{
	u8 key_ret;
	key_ret=KEYS;
	KEYS=PRESS_NONE;
	return key_ret;
}



void KEY_IRQHandler(void)   //��������ж�
{
	static u8 key_time;
	static u8 key_valid;
//KEY1																																				
	if (KEY==0)
	{
		if (key_valid==0)//������Ч
		{
			key_time++;
			if (key_time>=80)
			{
				KEYS=PRESS_LONG;
				key_valid=1;//����������������Ч
			}
		}
	}
	else
	{
	if (key_time>1&&key_time<80)
		{
			KEYS=PRESS_SHORT;
		}
		key_time=0;
		key_valid=0;//������Ϊ��Ч
	}
}


//��ʪ�����ɲ�����ʱ��,ÿ10ms����һ��
void CS_BUSY_IRQ (void)
{
	if (CS_CANTCMD)//
	{
		if (CS_BUSY_TIME) 
		{
			CS_BUSY_TIME--;
			CS_IN_CD=1;
		}
		else
		{
			CS_CANTCMD=0;//��ȴ�����
			CS_IN_CD=0;
			control_run (POWER_1,CS_LAST_CMD);//ִ�����һ������
			control_run (POWER_2,CS_LAST_CMD);

		}
	}
}



void Time_Out_IRQ (void)
{
	static u16 ms10=0;
		ms10++;
		if (ms10>=100)
		{
			ms10=0;
			MY_OFFTIME++;//������ÿ���һ
			if (MY_OFFTIME>30)
			{
				
				Cmd_cs_off();
				Cmd_kt_off();
				Cmd_jh_off();
			}
		}
}





//ÿ10ms��һ���ж�
void TIM4_IRQHandler ()
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)
	{
		Time_Out_IRQ();//��ʱ�Զ����豸�ж�
		Run_Time_IRQ ();//ϵͳ���м�ʱ�ж�
//		CS_BUSY_IRQ();	//��ʪ��CD��ʱ�ж�
		KEY_IRQHandler();	//�����ж�
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	}
}

