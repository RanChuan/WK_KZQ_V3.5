#ifndef  __CONFIG_H
#define  __CONFIG_H
#include "stm32f10x.h"
#include "stmflash.h"
/**************************************************

系统配置文件
配置控制器类型、控制器地址



*************************************************/
#define MYSTYLE_KT 0x02					//空调
#define MYSTYLE_CS 0x03					//除湿机
#define MYSTYLE_JH 0x04					//空气净化机
#define MYSTYLE_JS 0x05					//加湿机
#define MYSTYLE_YT 0x06					//加湿除湿一体机



				//这个全局变量数组定义在stmflash.c文件里
				//由于这个变量数组保存了系统的配置信息，在这里引用
				//由于这个变量数组需要保存在flash里，在stmflash.c文件声明
//extern u16 MY_CONFIG[100];//配置信息
#define MY_CONFIG ((u32*)0x20000000)

#define DATA_MAP ((data_map*)0x20000000)



typedef struct {
	char devType[4];			//设备类型
	char softVersion[4];		//软件版本
	char extType[4];			//扩展类型
	u32 devAddr;
	u32 sysRunTime;
	u32 myStyle;			//本机类型的整形形式
	u32 extStyle;			//本机扩展类型的整形形式
	u32 outputNew;		//当这个值和旧的值不同时会动作
	u32 outputOld;		//在动作之后这个值会和新的值相同
	u32 hand;					//手动
	u32 warn;					//报警
	u32 inCfg;				//是否处于配置模式
} data_map;






#define CONFIG_OFFSET (sizeof(data_map)/4)


#define MY_ADDR DATA_MAP->devAddr

#define MY_STYLE DATA_MAP->myStyle

#define KT_STYLE DATA_MAP->extStyle

#define IF_HAND DATA_MAP->hand

#define WARN_WATER DATA_MAP->warn

#define MY_MODEL DATA_MAP->inCfg



#define CJ_ADDR   MY_CONFIG[9+CONFIG_OFFSET]		//所属采集器的地址，因为变量是u16，所以无需改动

#define WORK_MODE MY_CONFIG[8+CONFIG_OFFSET]		//工作模式单纯控制模式还是数据解析模式

#define KT_STATE MY_CONFIG[2+CONFIG_OFFSET]

#define KT_STATE1 MY_CONFIG[5+CONFIG_OFFSET]//空调升降温位

#define CS_STATE MY_CONFIG[3+CONFIG_OFFSET]  //加湿，除湿，恒湿机都使用这个标志位

#define JH_STATE MY_CONFIG[4+CONFIG_OFFSET]

#define IF_WORK   MY_CONFIG[10+CONFIG_OFFSET]  //是否在工作，1，在工作，0，没工作

#define WARN_WENDU MY_CONFIG[13+CONFIG_OFFSET]   //温度报警

#define WARN_SHIDU MY_CONFIG[14+CONFIG_OFFSET]   //湿度报警
#define WARN_TVOC 	MY_CONFIG[15+CONFIG_OFFSET]   //TVOC报警
#define WARN_PM2_5 MY_CONFIG[16+CONFIG_OFFSET]   //PM2.5报警

#define CS_IN_CD				MY_CONFIG[23+CONFIG_OFFSET]			//除湿机处于CD状态，不可开关

#define KT_USER_CMD    MY_CONFIG[24+CONFIG_OFFSET]	//配置时的空调自定义的红外命令，升温降温等



#if 0

				//为了使用方便，用宏改名
#define CONFIG_OFFSET 0

#define MY_ADDR MY_CONFIG[0+CONFIG_OFFSET]

#define MY_STYLE MY_CONFIG[1+CONFIG_OFFSET]

#define KT_STATE MY_CONFIG[2+CONFIG_OFFSET]

#define KT_STATE1 MY_CONFIG[5+CONFIG_OFFSET]//空调升降温位

#define CS_STATE MY_CONFIG[3+CONFIG_OFFSET]  //加湿，除湿，恒湿机都使用这个标志位

#define JH_STATE MY_CONFIG[4+CONFIG_OFFSET]



#define MY_MODEL MY_CONFIG[6+CONFIG_OFFSET]  //记录当前工作模式,配置模式还是正常工作模式

#define KT_STYLE MY_CONFIG[7+CONFIG_OFFSET]		//空调类型，格力还是美的

#define WORK_MODE MY_CONFIG[8+CONFIG_OFFSET]		//工作模式单纯控制模式还是数据解析模式

#define CJ_ADDR   MY_CONFIG[9+CONFIG_OFFSET]		//所属采集器的地址，因为变量是u16，所以无需改动

#define IF_WORK   MY_CONFIG[10+CONFIG_OFFSET]  //是否在工作，1，在工作，0，没工作

#define IF_HAND   MY_CONFIG[11+CONFIG_OFFSET]    //手动模式还是自动模式，1，手动，2，自动



#define WARN_WATER MY_CONFIG[12+CONFIG_OFFSET]   //漏水报警

#define WARN_WENDU MY_CONFIG[13+CONFIG_OFFSET]   //温度报警

#define WARN_SHIDU MY_CONFIG[14+CONFIG_OFFSET]   //湿度报警
#define WARN_TVOC 	MY_CONFIG[15+CONFIG_OFFSET]   //TVOC报警
#define WARN_PM2_5 MY_CONFIG[16+CONFIG_OFFSET]   //PM2.5报警



									

												//环境变量的上下限，环境变量上下限没有小数部分

#define LIMIT_WENDU_H		MY_CONFIG[17+CONFIG_OFFSET]			//温度上限，
#define LIMIT_WENDU_L		MY_CONFIG[18+CONFIG_OFFSET]			//温度下限
#define LIMIT_SHIDU_H		MY_CONFIG[19+CONFIG_OFFSET]			//湿度上限
#define LIMIT_SHIDU_L   MY_CONFIG[20+CONFIG_OFFSET]			//湿度下限
#define LIMIT_TVOC			MY_CONFIG[21+CONFIG_OFFSET]			//TVOC只有上限
#define LIMIT_PM2_5			MY_CONFIG[22+CONFIG_OFFSET]			//PM2_5只有上限



#define CS_IN_CD				MY_CONFIG[23+CONFIG_OFFSET]			//除湿机处于CD状态，不可开关


#define KT_USER_CMD    MY_CONFIG[24+CONFIG_OFFSET]	//配置时的空调自定义的红外命令，升温降温等


#endif



#define WORK_MODE1 0x01					//单纯控制模式
#define WORK_MODE2 0x02					//数据解析模式


void Config_Init(void);

//设置本机控制器类型，返回0xff失败,0成功
u8 Set_MyStyle (u8 style);

u8 Set_CJ_Addr (u16 addr);

void Set_Addr (u8 addr);


#endif

