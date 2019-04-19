#include "cmd.h"
#include "config.h"
#include "drv_as62.h"
#include "uart_485.h"
#include "receive.h"


				//模式1用到的协议
void Cmd_return(u16 num);//返回数据
void Cmd_err (void);//出错
void Cmd_Icant (void); //不支持的命令




					//模式2的数据返回函数，cmd为0x03或0x06
void Cmd_return_MODE2 (u8 cmd,u16 datalength);
void Cmd_err_MODE2 (u8 cmd,u16 err);				//模式2的通信错误返回



							//控制命令超时自动关闭计时器，单位，秒
 u16 MY_OFFTIME=0;

										//除湿机启动停止不可操作标志
										//开启5分钟不可关闭，关闭5分钟不可打开
u16 CS_CANTCMD=0;
static u16 CS_BUSY_TIME=5*60*100;//5分钟CD，递减变量
static u8  CS_LAST_CMD=0;//除湿机在CD结束后执行的命令
#define  CS_OUTTIME 5*60*100


//除湿机，空调，升降温，空气净化器


		//格力空调电辅热升温至30度
const u8 GREE_UP[10]={0x30,0x70,0x00,0x0a,0x40,0x00,0x40,0x00,0x60};
		//格力空调制冷模式降温至16度
const u8 GREE_DOWN[10]={0x90,0x00,0x00,0x0a,0x40,0x00,0x40,0x00,0xa0};
		//格力空调自动模式开机
const u8 GREE_ON[10]={0x90,0x90,0x04,0x8a,0x40,0x00,0x40,0x00,0xf0};
		//格力空调自动模式关机
const u8 GREE_OFF[10]={0x00,0x90,0x00,0x0a,0x40,0x00,0x40,0x00,0xa0};



		//美的空调电辅热升温至30度
const u8 MIDEA_UP[10]={0xb2 ,0x4d ,0xbf ,0x40 ,0xbc ,0x43,0x00,0x00};
		//美的空调制冷模式降温至16度
const u8 MIDEA_DOWN[10]={0xb2 ,0x4d ,0xbf ,0x40 ,0x00 ,0xff,0x00,0x00};
		//美的空调自动模式开机
const u8 MIDEA_ON[10]={0xb2, 0x4d ,0x1f ,0xe0 ,0xd8 ,0x27,0x00,0x00};
		//美的空调自动模式关机
const u8 MIDEA_OFF[10]={0xb2 ,0x4d ,0x7b ,0x84 ,0xe0 ,0x1f,0x00,0x00};




/*************************************************************************************

读取命令格式：
0xf0,0xfe,addr=MY_ADDR,cmd=0x03,fun_h=0,fun_l=0x02,data0=0,data1=0x04,crc_h,crc_l
本机支持的读取命令固定为10个字节
有等号的变量的值只能为等号右边的固定值
fun_0,fun_1是功能标识，代表是控制器
如果检测到格式不对会发出错误信息或者不予应答

读取返回数据格式：
0xf0 ,0xfe ,addr ,cmd=0x03 ,msg0=0x00 ,mag1=0x02 ,long_h=0x00 ,long_l=0x04,
MY_STATE[0],MY_STATE[1],MY_STATE[2],MY_STATE[3],crc_h,crc_l
本机的返回数据固定为14字节，并且只会在读取命令期望读取的数据长度为4时才会返回
本机返回的有等号的变量固定为等号右边的值，不会因为运行条件的变化而变化
没有等号的变量可能是以下这些值：
MY_STATE[0]：0，除湿机关；		1，除湿机开									
MY_STATE[1]：0，空调,关；			1，空调开										
MY_STATE[2]：0，无状态；			1，空调升温；		2，空调降温	
MY_STATE[3]：0，空气净化器关（本机暂时不支持空气净化器的控制，固定为0）


控制命令格式：
0xf0,0xfe,addr=MY_ADDR,cmd=0x06,msg0,msg1,fun_h=0,fun_l=0x02,data0,data1,crc_h,crc_l
本机支持的控制命令固定为12个字节
fun_0,fun_1是功能标识，代表是控制器
有等号的变量是固定的，本机只能接受这种格式的命令
本机通过对msg0和msg1所附带值的判断来控制除湿机和空调的启停
			-------------------------
			|   msg0    |   msg1    |
			-------------------------
			|   0x10    |   0x11    |温度太高，通过此可开空调并开降温
			-------------------------
			|   0x10    |   0x12    |温度太低，通过此可开空调并开升温
			-------------------------
			|   0x20    |   0x21    |湿度太高，通过此可开除湿机
			-------------------------
			|   0x20    |   0x22    |湿度太低，通过此可关除湿机
			-------------------------
			程序中只通过msg0和msg1来控制除湿机
			
没有等号的变量可以有以下这些值：						
			-------------------------							
			|data0			|  data1		|							
			-------------------------							
			|		0				|			0			|空调关				
			-------------------------							
			|		0				|			1			|空调关				
			-------------------------							
			|		0				|			2			|空调关				
			-------------------------							
			|		1				|			0			|空调开				
			-------------------------							
			|		1				|			1			|空调开，升温	
			-------------------------							
			|		1				|			2			|空调开，降温	
			-------------------------							
如果检测到格式不对会发出错误信息或者不予应答

*****************************************************************************************/
#define __FR_DEBUG 0



						//命令解析函数指针
void (*Get_cmd) (u8 *buff1);







//以下是控制命令执行函数

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
	printf ("控制器：\r\n\t");
	printf ("除湿机开已执行\r\n");
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
	printf ("控制器：\r\n\t");
	printf ("除湿机关已执行\r\n");
#endif
	
}

void Cmd_err (void)
{
	
	
#if __FR_DEBUG==1
	printf ("控制器：\r\n\t");
	printf ("命令出错！\r\n");
#endif
	
	
}

void Cmd_kt_on (void)
{
	MY_OFFTIME=0;
	//这里插入空调的开机命令

	if (KT_STATE!=0x01)
	{
		control_run (POWER_1,1);
		control_run (POWER_2,1);

		
	while (Remote_GetState ( ));//保证红外发射时空闲的
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
	printf ("控制器：\r\n\t");
	printf ("空调开已执行\r\n");
#endif
}

void Cmd_kt_off(void)
{
	MY_OFFTIME=0;
	//这里插入空调的关机命令
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
	printf ("控制器：\r\n\t");
	printf ("空调关已执行\r\n");
#endif
}



void Cmd_kt_up (void)
{
	MY_OFFTIME=0;
	//这里插入空调的开机命令
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
	printf ("控制器：\r\n\t");
	printf ("空调升温已执行\r\n");
#endif
}

void Cmd_kt_down (void)
{
	MY_OFFTIME=0;
	//这里插入空调的开机命令
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
	printf ("控制器：\r\n\t");
	printf ("空调降温已执行\r\n");
#endif
}

void Cmd_Icant (void)
{
#if __FR_DEBUG==1
	printf ("控制器：\r\n\t");
	printf ("不支持的命令\r\n");
#endif
	
}

void Cmd_jh_on (void)//净化器开
{
	MY_OFFTIME=0;
	if (JH_STATE!=0x01)
	{
		control_run (POWER_1,1);
		control_run (POWER_2,1);


		while (Remote_GetState ( ));//保证红外发射时空闲的
		Remote_send_USER(1);


		
		control_run (DEVICE_ON,1);
		control_run (CMD_UP,0);
		control_run (CMD_DOWN,0);
		JH_STATE=0x01;
	}
#if __FR_DEBUG==1
	printf ("控制器：\r\n\t");
	printf ("净化器开已执行\r\n");
#endif

}
void Cmd_jh_off (void)//净化器关
{
	MY_OFFTIME=0;
	if (JH_STATE!=0x00)
	{
		control_run (POWER_1,0);
		control_run (POWER_2,0);

		while (Remote_GetState ( ));//保证红外发射时空闲的
		Remote_send_USER(2);

		
		control_run (DEVICE_ON,0);
		control_run (CMD_UP,0);
		control_run (CMD_DOWN,0);
		JH_STATE=0x00;
	}
#if __FR_DEBUG==1
	printf ("控制器：\r\n\t");
	printf ("净化器关已执行\r\n");
#endif

}



//处理按键消息，进入配置模式，测试控制功能
//配置模式在配置成功过后会自动退出，长按按键也会退出
//在测试功能时，会临时改变系统状态，退出配置模式时恢复
void Cfg_Model (void)
{
	u8 key=0;
	static u8 i=0;//静态循环变量
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
			MY_MODEL=0;								//退出配置模式
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
			//Config_Init( );						//重新初始化系统
			i=0;
	}
	if (MY_MODEL==1)
	{
		MY_OFFTIME=0;//重置自动关机计数器，防止自动关机
		if (KT_USER_CMD>=1&&KT_USER_CMD<=4)
		{
//			Receive_Cmd(ENABLE);
			if (Receive_GetState()) 
			{
				//如果接收了遥控键值，退出学习模式
				Receive_SaveKey(KT_USER_CMD);
				KT_USER_CMD=0;		//退出空调命令学习
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

															定时器4默认初始化

*****************************************************************************************************/

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 720-1; //，每10ms计数器加一
	TIM_TimeBaseStructure.TIM_Prescaler =1000; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

 
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //计数器更新中断

	//在发送红外的时候再打开定时器
	TIM_Cmd(TIM4,ENABLE ); 	//失能定时器1
	
	
	
}







//检测outputOld和outputNew是不是相同，如果不相同执行至相同

void run_output (void)
{
	u16 state_check=0;
	u16 state_old=0;
	u16 state_new=0;
	if (DATA_MAP->outputOld!=DATA_MAP->outputNew)
	{
		state_old=DATA_MAP->outputOld&0x01ff;
		state_new=DATA_MAP->outputNew&0x01ff;
		state_check=state_old^state_new;		//不同为1
		for (u8 i=0;i<9;i++)
		{
			if (state_check&(1<<i))		//如果不同
			{
				if (state_new&(1<<i))
					control_run(i+1,1);//执行为新的状态
				else
					control_run(i+1,0);//执行为新的状态
			}
		}
		
		state_old=(DATA_MAP->outputOld&(0x3<<9))>>9;
		state_new=(DATA_MAP->outputNew&(0x3<<9))>>9;
		if (state_old!=state_new)
		{
			switch (state_new)
			{
				case 0:		//关
					Remote_send_USER(2);
					break;
				case 1:		//开
					Remote_send_USER(1);
					break;
				case 2:		//升
					Remote_send_USER(3);
					break;
				case 3:		//降
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














//按键

#define KEY PBin(2)
static u8 KEYS;


//返回按键状态
u8 Get_Key(u8 keynum)
{
	u8 key_ret;
	key_ret=KEYS;
	KEYS=PRESS_NONE;
	return key_ret;
}



void KEY_IRQHandler(void)   //按键检测中断
{
	static u8 key_time;
	static u8 key_valid;
//KEY1																																				
	if (KEY==0)
	{
		if (key_valid==0)//按键有效
		{
			key_time++;
			if (key_time>=80)
			{
				KEYS=PRESS_LONG;
				key_valid=1;//触发长按，按键无效
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
		key_valid=0;//按键设为有效
	}
}


//除湿机不可操作计时器,每10ms调用一次
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
			CS_CANTCMD=0;//冷却已完毕
			CS_IN_CD=0;
			control_run (POWER_1,CS_LAST_CMD);//执行最后一次命令
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
			MY_OFFTIME++;//计数器每秒加一
			if (MY_OFFTIME>30)
			{
				
				Cmd_cs_off();
				Cmd_kt_off();
				Cmd_jh_off();
			}
		}
}





//每10ms进一次中断
void TIM4_IRQHandler ()
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)
	{
		Time_Out_IRQ();//超时自动关设备中断
		Run_Time_IRQ ();//系统运行计时中断
//		CS_BUSY_IRQ();	//除湿机CD计时中断
		KEY_IRQHandler();	//按键中断
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	}
}

