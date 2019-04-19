#include "uart_485.h"


#include "cmd.h"
#include "config.h"
#include "drv_as62.h"
#include "stmflash.h"
#include "receive.h"
/*
			����ļ�����֧���µĶ���Э��

*/

u32 RUN_TIME_S=0;//���ϵ�����ϴ����������ڵĹ���ʱ�䣬��λ��

extern u16 CS_CANTCMD;//��cmd.c�ж����
							//�������ʱ�Զ��رռ�ʱ������λ����
extern u16 MY_OFFTIME;




/*****************************************************************

							�������ã����������ݽ�����Ч����֤
												����������ݽ���crcУ�飬�ȶԵ�ַ��
												������������Ͳ�ͬ�ַ�����ִ�к���

									�������������������



****************************************************************/
void Get_cmd_NEW_MODE (u8 *buff1)
{
	u16 datalength=0;//���������ݵĳ��ȣ������ܳ��ȣ�
	u16 temp=0;
	u8 crc[2];
	
								//����ͷУ���Լ����Ȼ�ȡ
	if (buff1[0]==0xff&&buff1[1]==0xff)
	{
		datalength=(buff1[5]<<8)|buff1[6];
	}
	else
		return;
	
								//����CRCУ��
	Get_Crc16(buff1,datalength+7,crc);
	if ((crc[0]==buff1[datalength+7])&&(crc[1]==buff1[datalength+8]))//У��ɹ���������Ч
	{
	}
	else 
		return ;
	
	
								//���е�ַУ��
	temp=(buff1[2]<<8)|buff1[3];
	if (temp==MY_ADDR||((buff1[4]>=0x06)&&buff1[4]<=0x08))
	{
		MY_OFFTIME=0;
		switch (buff1[4])
		{
			case 0x01://��ȡϵͳ����
				cmd_0x01 (buff1);
				break;
			case 0x02://��ѯ�豸����
				cmd_0x02 (buff1);
				break;
			case 0x03://�����豸
				cmd_0x03 (buff1);
				break;
			case 0x04://��ȡ�ڴ����
				cmd_0x04 (buff1);
				break;
			case 0x05://��ȡ�豸flash
				cmd_0x05 (buff1);
				break;
			case 0x06://����ģʽ
				cmd_0x06 (buff1);
				break;
			case 0x07:
				cmd_0x07 (buff1);
				break;
			case 0x08:
				cmd_0x08 (buff1);
				break;
			case 0x09:
				cmd_0x09 (buff1);
				break;
			default:
				Return_NEW(buff1[4],ERR_NULLCMD);//��֧�ֵ�����
				break;
		}
	}
	else 
		return;
}



			//����ϵͳ����
void cmd_0x01 (u8 *buff)
{
	u8 data[30];
	u8 crc[2];
	
	data[0]=0xff;
	data[1]=0xff;
	data[2]=MY_ADDR>>8;
	data[3]=MY_ADDR;
	data[4]=buff[4]|0x80;
	data[5]=0x00;
	data[6]=14;
	data[7]=0;		//��������
	data[8]=0;
				
					//����λ
	data[9+0]=MY_STYLE;//��������
	data[9+1]=IF_WORK;//�Ƿ��ڹ���
	data[9+2]=RUN_TIME_S>>24;
	data[9+3]=RUN_TIME_S>>16;
	data[9+4]=RUN_TIME_S>>8;
	data[9+5]=RUN_TIME_S;
	data[9+6]=IF_HAND;
	
				//������������������2λ
	if (MY_STYLE==MYSTYLE_KT)
	{
		data[9+7]=KT_STATE;
		data[9+8]=KT_STATE1;
		data[9+10]=KT_STYLE;
		data[9+11]=WARN_WENDU;
	}
	else if (MY_STYLE==MYSTYLE_CS)
	{
		data[9+7]=CS_STATE;
		data[9+8]=0;
		data[9+9]=CS_CANTCMD;
		data[9+11]=WARN_SHIDU;
		data[9+11]=WARN_SHIDU;
	}
	else if (MY_STYLE==MYSTYLE_JS)//��ʪ�ͳ�ʪʹ��ͬһ���ֽ�
	{
		data[9+7]=CS_STATE;
		data[9+8]=0;
		data[9+9]=CS_CANTCMD;
		data[9+11]=WARN_SHIDU;
	}
	else if (MY_STYLE==MYSTYLE_YT)//��ʪ����8λ�ǿ���״̬����8λ���趨��ʪ��ֵ
	{
		data[9+7]=CS_STATE>>8;
		data[9+8]=CS_STATE;
		data[9+11]=WARN_WATER;
	}
	else if (MY_STYLE==MYSTYLE_JH)
	{
		data[9+7]=JH_STATE;
		data[9+8]=0;
		data[9+11]=WARN_TVOC;
	}
	
	Get_Crc16(data,9+12,crc);
	data[9+12]=crc[0];
	data[9+13]=crc[1];
	AS32_TX_none(data,9+14);//���ͷ�������
	
}


						//��ѯ�豸����
void cmd_0x02 (u8 *buff)
{
	u8 data[30];
	u8 crc[2];
	
	data[0]=0xff;
	data[1]=0xff;
	data[2]=MY_ADDR>>8;
	data[3]=MY_ADDR;
	data[4]=buff[4]|0x80;
	data[5]=0x00;
	data[6]=12;
	data[7]=0;		//��������
	data[8]=0;
	
							//��û�й��ϣ�1����
	if (MY_STYLE==MYSTYLE_KT)
	{
		if (WARN_WENDU) data[9+0]=1;
	}
	else if (MY_STYLE==MYSTYLE_CS)
	{
		if (WARN_SHIDU) data[9+0]=1;
	}
	else if (MY_STYLE==MYSTYLE_JH)
	{
		if (WARN_TVOC) data[9+0]=1;
		if (WARN_PM2_5) data[9+0]=1;
	}
	
	data[9+1]=WARN_WATER;
	Get_Crc16(data,9+2,crc);
	data[9+2]=crc[0];
	data[9+3]=crc[1];
	AS32_TX_none(data,9+4);//���ͷ�������
}

								//��������ִ��
void cmd_0x03 (u8 *buff)
{
	u8 data[30];
	u8 crc[2];
	
	data[0]=0xff;
	data[1]=0xff;
	data[2]=MY_ADDR>>8;
	data[3]=MY_ADDR;
	data[4]=buff[4]|0x80;
	data[5]=0x00;
	data[6]=2;
	data[7]=0;		//��������
	data[8]=0;

	
	if (buff[7]!=MY_STYLE)
	{	
		Return_NEW (buff[4],ERR_NOTMYTYPE);
		return ;
	}
	else
	{
		MyDevice_On(buff[8],buff[9]);
	}
	Get_Crc16(data,9,crc);
	data[9]=crc[0];
	data[9+1]=crc[1];
	AS32_TX_none(data,9+2);//���ͷ�������

}


//��ȡָ���ڴ��ַ��ֵ0,�ɹ���1��ʧ��
u32 getMemValue (u8 *buff,u32 addr,u16 len)
{
	#if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
		OS_CPU_SR  cpu_sr;
	#endif
	u8 *base_addr=(u8 *)addr;
	if (base_addr==0) return 1;
	//OS_ENTER_CRITICAL();
	while(len--)
	{
		*buff++=*base_addr++;
	}
	//OS_EXIT_CRITICAL();
	return 0;
}



u32 setMemValue (u8 *buff,u32 addr,u16 len)
{
	#if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
		OS_CPU_SR  cpu_sr;
	#endif
	u8 *base_addr=(u8 *)addr;
	if (base_addr==0) return 1;
	//OS_ENTER_CRITICAL();
	while(len--)
	{
		*base_addr++=*buff++;
	}
	//OS_EXIT_CRITICAL();
	return 0;
}





#include "string.h"


void cmd_0x04 (u8 *recv)
{
	u32 addr;
	u16 datalen=(recv[12]<<8)|recv[13];
	u8 data[58]={0};
	u8 crc[2]={0};
	data[0]=0xff;
	data[1]=0xff;
	data[2]=MY_ADDR>>8;
	data[3]=MY_ADDR;
	data[4]=recv[4]|0x80;
	data[5]=(datalen+2)>>8;
	data[6]=datalen+2;
	data[7]=0;	//Ĭ���ǳɹ�
	data[8]=0;
	if (datalen>58-12) //���µ������ݳ���
	{
		datalen=58-12;
	}
	addr=(recv[8]<<24)|(recv[9]<<16)|(recv[10]<<8)|recv[11];
	
	if (recv[7]==0)//��
	{
		memcpy (&data[9],(u8 *)addr,datalen);
		
		Get_Crc16(data,7+(datalen+2),crc);
		data[7+(datalen+2)]=crc[0];
		data[7+(datalen+2)+1]=crc[1];
		AS32_TX_none(data,7+(datalen+2)+2);//���ͷ�������
	}
	else if (recv[7]==1)//д
	{
		u16 wdatalen=(recv[5]<<8)|recv[6];
		wdatalen-=5;
		memcpy ((u8 *)addr,&recv[12],wdatalen);
		Return_NEW(recv[4],ERR_SUCCESS);
	}
}



						//��ȡflash
void cmd_0x05 (u8 *buff)
{
	u8 data[300];
	u8 crc[2];
	u32 flash_addr=0;
	u16 want=0;
	u16 want_i=0;
	u16 i=0;
	u16 num_now=0;
	u16 num_all=0;
	u16 buf[100]={0};
	data[0]=0xff;
	data[1]=0xff;
	data[2]=MY_ADDR>>8;
	data[3]=MY_ADDR;
	data[4]=buff[4]|0x80;
	data[5]=0x00;
	data[6]=2;
	data[7]=0;		//��������
	data[8]=0;

	if (buff[7]==1)//д��
	{
		
	}
	else if (buff[7]==0)//��ȡ
	{
		flash_addr=(buff[10]<<24)|(buff[11]<<16)|(buff[12]<<8)|buff[13];
		want=(buff[8]<<8)|buff[9];
		
		num_all=want/200;
		if (want%200) num_all++;
		while(want)
		{
			for (i=0;i<100;i++) buf[i]=0;
			if (want>200)
			{
				want_i=200;
				want-=200;
			}
			else
			{
				want_i=want;
				want=0;
			}
			STMFLASH_Read(flash_addr,buf,want_i/2);
			flash_addr+=want_i/2;
			
			data[6]=want_i+2;
			num_now++;
			data[9]=num_now;
			data[10]=num_all;
			for (i=0;i<want_i;i++)
			{
				data[11+i]=buf[i]>>8;
				data[12+i]=buf[i];
			}
			
			Get_Crc16(data,want_i+2+7,crc);
			data[want_i+2+7]=crc[0];
			data[want_i+2+8]=crc[1];
			AS32_TX_none(data,want_i+2+9);//���ͷ�������
			
		}
	}
}

						
void cmd_0x06 (u8 *buff)
{
	u8 data[30]={0};
	u8 crc[2]={0};
	data[0]=0xff;
	data[1]=0xff;
	data[2]=MY_ADDR>>8;
	data[3]=MY_ADDR;
	data[4]=buff[4]|0x80;
	data[5]=0x00;
	data[6]=3;
	data[7]=0;		//��������
	data[8]=0;
	MY_MODEL=buff[7];
	data[9]=MY_MODEL;
	//control_run(MODEL,MY_MODEL);
	if (MY_MODEL==0) 
	{
		DATA_MAP->outputNew &=~(1<<(MODEL-1));
		//Config_Init( );	
	}
	else
	{
		DATA_MAP->outputNew |=1<<(MODEL-1);
	}
	Get_Crc16(data,data[6]+7,crc);
	data[data[6]+7]=crc[0];
	data[data[6]+8]=crc[1];
	AS32_TX_none(data,data[6]+9);//���ͷ�������
	
}
void cmd_0x07 (u8 *buff)
{
	if (MY_MODEL)
	{
		MY_ADDR=(buff[7]<<8)|buff[8];
		MY_STYLE=buff[9];
		if (MY_STYLE==MYSTYLE_KT)
		{
			KT_STYLE=buff[10];
		}
		Save_Config();
		Return_NEW(buff[4],ERR_SUCCESS);
	}
	else
	{
		Return_NEW(buff[4],ERR_CANNOTCFG);
	}
}
void cmd_0x08 (u8 *buff)
{
	if (MY_MODEL==0)
	{
		Return_NEW(buff[4],ERR_CANNOTCFG);
		return;
	}
	if (buff[7]!=0x02)
	{
		Return_NEW(buff[4],ERR_DATAVALE);
		return;
	}
	if (1)
	{
		if (buff[8]==0x01)
		{
			control_run(CMD_UP,0);
			control_run(DEVICE_ON,1);
			control_run(CMD_DOWN,0);
		}
		else if (buff[8]==0x02)
		{
			control_run(CMD_UP,1);
			control_run(DEVICE_ON,0);
			control_run(CMD_DOWN,1);
		}
		else if (buff[8]==0x03)
		{
			control_run(CMD_UP,1);
			control_run(DEVICE_ON,0);
			control_run(CMD_DOWN,0);
		}
		else if (buff[8]==0x04)
		{
			control_run(CMD_UP,0);
			control_run(DEVICE_ON,0);
			control_run(CMD_DOWN,1);
		}
		KT_USER_CMD=buff[8];
		Clear_Cupture();//����Ѵ��ڵĲ��񻺳�������
		Receive_Cmd(ENABLE);
		Return_NEW(buff[4],ERR_SUCCESS);
	}
	
}
void cmd_0x09 (u8 *buff)
{
	if (buff[7]==2)
	{
		Return_NEW(buff[4],ERR_SUCCESS);
		delay_ms(100);
		NVIC_SystemReset();
	}
	else
	{
		Return_NEW(buff[4],ERR_DATAVALE);
	}
}







void Return_NEW (u8 cmd,u16 err)
{
	u8 data[20];
	u8 crc[2];
	
	data[0]=0xff;
	data[1]=0xff;
	data[2]=MY_ADDR>>8;
	data[3]=MY_ADDR;
	data[4]=cmd|0x80;
	data[5]=0x00;
	data[6]=0x02;
	data[7]=err>>8;		//��������
	data[8]=err;
	
	Get_Crc16(data,9,crc);
	data[9]=crc[0];
	data[10]=crc[1];
	AS32_TX_none(data,11);//���ͷ�������
	
}






void MyDevice_On (u8 state,u8 updown)
{
	if (MY_STYLE==MYSTYLE_CS)
	{
		if (state==1)
		{
			if (updown==1)//��ʪ
			{
				Cmd_cs_off();
			}
			else if (updown==2)//��ʪ
			{
				Cmd_cs_on();
			}
		}
		else
		{
			Cmd_cs_off();
		}
	}
	else if (MY_STYLE==MYSTYLE_KT)
	{
		if (state==1)
		{
			if (updown==1)//����
			{
				Cmd_kt_up();
			}
			else if (updown==2)//����
			{
				Cmd_kt_down();
			}
			else
			{
				Cmd_kt_on();
			}
		}
		else
		{
			Cmd_kt_off();
		}
	}
	else if (MY_STYLE==MYSTYLE_JH)
	{
		if (state==1)
		{
			if (updown==1)//�ӿ���΢������������������
			{
				Cmd_jh_off();
			}
			else if (updown==2)//������΢��
			{
				Cmd_jh_on();
			}
			else
			{
				Cmd_jh_on();
			}
		}
		else
		{
			Cmd_jh_off();
		}
	}
	else if (MY_STYLE==MYSTYLE_JS)
	{
		if (state==1)
		{
			if (updown==1)//��ʪ��
			{
				Cmd_cs_on();		//���ںͳ�ʪ��ʹ�ò�ͬ�İ��ӣ�����ʹ�ó�ʪ���Ŀ��ƺ���
			}
			else if (updown==2)//��
			{
				Cmd_cs_off();
			}
			else
			{
				Cmd_cs_on();
			}
		}
		else
		{
			Cmd_cs_off();
		}
	}
	else if (MY_STYLE==MYSTYLE_YT)
	{
		yiti_power(state);
		delay_ms(100);
		yiti_set(updown);
	}
	DATA_MAP->outputNew=DATA_MAP->outputOld;//ִ����֮��ͬ��״̬��Ϣ
}



















			//ϵͳ����ʱ������жϣ�ÿ10msһ���ж�
void Run_Time_IRQ (void)
{
	static u8 i=0;
	i++;
	if (i>=100)
	{
		RUN_TIME_S++;
		DATA_MAP->sysRunTime=RUN_TIME_S;
		i=0;//�޸��豸����ʱ���ʱ��׼ȷ��2018.8.17
	}
}















