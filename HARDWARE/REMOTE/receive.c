#include "receive.h"
#include "config.h"
#define CUPTRE PAin(1)


#define CHANG_RISE TIM2->CCER&=0xffdf		//�л�Ϊ������
#define CHANG_FALL TIM2->CCER|=0x0020		//�л�Ϊ�½���


u16 cupture[CUPTURE_NUM];//
void 	(*Receive_IRQHandler) (void );

#define K_R  20  //����ֵ
#define K_L  (20)  //����ֵ

static u8 RECEIVE_ENABLE=0;//�������
//��ȡ���񵽵ĺ������

u16 RCV_NUM=0;//���񵽵ı��ظ�����ʵʱ����

u16 ALL_RCV=0;//���ս������ܽ��յı��ظ���

u8 Receive_GetState (void)//1,�пɶ���ֵ��0��û��
{
	if (RECEIVE_ENABLE==0)
	{
		if (ALL_RCV>5)
		{
			ALL_RCV=0;
			return 1;
		}
	}
	//Clear_Cupture();
	return 0;
}

void Receive_GetKey (u8 *buff,u16 num)
{
	u16 i=0;
	if (num>9) num=9;
	if (!RECEIVE_ENABLE)
	{
		for (i=0;i<9;i++)
		{
			buff[i]=0;
		}
		for (i=2;i<70;i++)
		{
				if ((cupture[i]>(0xe5-K_L))&&(cupture[i]<(0xe5+K_R)))
					buff[(i-2)/8]|=0x80>>((i-2)%8);
		}
			for (i=0;i<CUPTURE_NUM;i++)
				cupture[i]=0;
	RECEIVE_ENABLE=1;
	}
}





//�洢�յ�ң�ؼ�ֵ��1������2���أ�3�����£�4������
void Receive_SaveKey (u8 keytype)
{
	if (!RECEIVE_ENABLE)
	{
		if (keytype==1)
		{
			Save_Key_On();
			
		}
		else if (keytype==2)
		{
			Save_Key_Off();
		}
		else if (keytype==3)
		{
			Save_Key_Up();
		}
		else if (keytype==4)
		{
			Save_Key_Down();
		}
		else if (keytype==5)
		{
		}
		Clear_Cupture();
		RECEIVE_ENABLE=1;
	}
}

			//������񻺳�
void Clear_Cupture (void)
{
	u16 i=0;
		for (i=0;i<CUPTURE_NUM;i++)
			cupture[i]=0;
	ALL_RCV=0;
}







void Receive_GREE (void )
{
				//�л��������
	if (CUPTRE)
		CHANG_FALL;
	else
		CHANG_RISE;
//	if (RECEIVE_ENABLE) 
		cupture[RCV_NUM]= TIM_GetCapture2(TIM2);//��ȡ����ֵ
	TIM_SetCounter(TIM2,0);//����������Ϊ0
	RCV_NUM++;
	if (RCV_NUM>=CUPTURE_NUM)
	{
		RCV_NUM=0;
		RECEIVE_ENABLE=0;//�����������
	}
	else if (RCV_NUM==0)
	{
	}
	
}



void Receive_USER (void )
{
				//�л��������
	if (CUPTRE)
		CHANG_FALL;
	else
		CHANG_RISE;
	if (RECEIVE_ENABLE) 
	{
		cupture[RCV_NUM]= TIM_GetCapture2(TIM2);//��ȡ����ֵ
		TIM_SetCounter(TIM2,0);//����������Ϊ0
		RCV_NUM++;
	}
}





void Receive_init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM2_ICInitStructure;
	
	//����ָ���ʼ��
	Receive_IRQHandler=Receive_USER;
	
	//ʱ������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	

  //�������ų�ʼ��
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;  //PA0 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PA0 ����  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);						 //PA0 
	
	
/*******************************************************************************************


													��ʱ��2��ʼ��


*********************************************************************************************/
	//��ʱ��2������Ϣ
	TIM_TimeBaseStructure.TIM_Period = 4000; //����ʾ40ms
	TIM_TimeBaseStructure.TIM_Prescaler =720-1; //ÿ10us��������1 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

	//��ʼ��TIM5���벶�����
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC2ӳ�䵽TI2��
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//�����ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI2��
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);

	//��ʱ��2�ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//�򿪲���ʱ��
	Receive_Cmd(DISABLE);

}


void Receive_Cmd (FunctionalState t)
{
	TIM_Cmd(TIM2,t );
	if (t==ENABLE)
	{
		RECEIVE_ENABLE=1;
	}
	else
	{
		RECEIVE_ENABLE=0;
	}
}


//40ms��ʱ��
void ReceiveEnd_IRQHandler (void)
{
	if (RCV_NUM)
	{
		RECEIVE_ENABLE=0;//�����������
		ALL_RCV=RCV_NUM;
	}
	RCV_NUM=0;
}




void TIM2_IRQHandler (void )
{
	if (TIM_GetITStatus(TIM2, TIM_IT_CC2))
	{
		Receive_IRQHandler();
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
	}
	
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		ReceiveEnd_IRQHandler();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}



