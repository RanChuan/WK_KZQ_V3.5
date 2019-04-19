#include "system_init.h"
#include "config.h"
#include "wdg.h"



int main()
{	
	u8 buff[50]={0,0,0,0,0,0};

	u16 i=0;
	u8 GREE_DOW[10]={0x90,0x00,0x00,0x0a,0x40,0x00,0x40,0xf0,0xff,0x00};
	void DeviceLEDType (u16 i);
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2800);

	System_Init();//��ʼ��
	
	
	while(1)
	{
		
		
		//-----��������Լ�ִ��ģ��-------//
		if (AUX) 													//
			USART1_GetCmd (buff,50);				//
		else buff[0]=0;										//
		if (buff[0]!=0x00)								//
		{																	//
			Get_cmd (buff);									//
		}																	//
		//----------------------------------
		run_output();  
		delay_ms(50);
		
		
		
		
		//--------����״ָ̬ʾ��----------------//
		DeviceLEDType(i);												//
		i++;																		//
		if (i>100) i=0;													//
		//--------------------------------------//
		
		
		
		
		//--------���ư���ʵ�尴��---------------//
		Cfg_Model ( );				//����ģʽ��ѯ���� //
		//---------------------------------------//
		
		
		
		//----------------©ˮ����-----------//
		water_err();												 //
		//-----------------------------------//
		
//		delay_ms(200);
//		Remote_send_USER(2);


		//------------��װ�ؿ��Ź�------------//
		IWDG_Feed( );													//
		//------------------------------------//
	}
}


void DeviceLEDType (u16 i)
{
	u8 t=0;
	if (MY_STYLE==MYSTYLE_KT)
	{
		t=STYLE_KT;
	}
	else if (MY_STYLE==MYSTYLE_CS)
	{
		t=STYLE_CS;
	}
	else if (MY_STYLE==MYSTYLE_JH)
	{
		t=STYLE_JH;
	}
	else if (MY_STYLE==MYSTYLE_JS)
	{
		t=STYLE_CS;
	}
	else if (MY_STYLE==MYSTYLE_YT)
	{
		t=STYLE_CS;
	}
	else
	{
		t=0;
	}


	if (i==50) {
		//control_run (t,1);	//
		DATA_MAP->outputNew |=1<<(t-1);
	}
	if (i==100) 
	{
		//control_run (t,0);	//
		DATA_MAP->outputNew &=~(1<<(t-1));
	}

}


