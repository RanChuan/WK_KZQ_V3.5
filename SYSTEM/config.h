#ifndef  __CONFIG_H
#define  __CONFIG_H
#include "stm32f10x.h"
#include "stmflash.h"
/**************************************************

ϵͳ�����ļ�
���ÿ��������͡���������ַ



*************************************************/
#define MYSTYLE_KT 0x02					//�յ�
#define MYSTYLE_CS 0x03					//��ʪ��
#define MYSTYLE_JH 0x04					//����������
#define MYSTYLE_JS 0x05					//��ʪ��
#define MYSTYLE_YT 0x06					//��ʪ��ʪһ���



				//���ȫ�ֱ������鶨����stmflash.c�ļ���
				//��������������鱣����ϵͳ��������Ϣ������������
				//�����������������Ҫ������flash���stmflash.c�ļ�����
//extern u16 MY_CONFIG[100];//������Ϣ
#define MY_CONFIG ((u32*)0x20000000)

#define DATA_MAP ((data_map*)0x20000000)



typedef struct {
	char devType[4];			//�豸����
	char softVersion[4];		//����汾
	char extType[4];			//��չ����
	u32 devAddr;
	u32 sysRunTime;
	u32 myStyle;			//�������͵�������ʽ
	u32 extStyle;			//������չ���͵�������ʽ
	u32 outputNew;		//�����ֵ�;ɵ�ֵ��ͬʱ�ᶯ��
	u32 outputOld;		//�ڶ���֮�����ֵ����µ�ֵ��ͬ
	u32 hand;					//�ֶ�
	u32 warn;					//����
	u32 inCfg;				//�Ƿ�������ģʽ
} data_map;






#define CONFIG_OFFSET (sizeof(data_map)/4)


#define MY_ADDR DATA_MAP->devAddr

#define MY_STYLE DATA_MAP->myStyle

#define KT_STYLE DATA_MAP->extStyle

#define IF_HAND DATA_MAP->hand

#define WARN_WATER DATA_MAP->warn

#define MY_MODEL DATA_MAP->inCfg



#define CJ_ADDR   MY_CONFIG[9+CONFIG_OFFSET]		//�����ɼ����ĵ�ַ����Ϊ������u16����������Ķ�

#define WORK_MODE MY_CONFIG[8+CONFIG_OFFSET]		//����ģʽ��������ģʽ�������ݽ���ģʽ

#define KT_STATE MY_CONFIG[2+CONFIG_OFFSET]

#define KT_STATE1 MY_CONFIG[5+CONFIG_OFFSET]//�յ�������λ

#define CS_STATE MY_CONFIG[3+CONFIG_OFFSET]  //��ʪ����ʪ����ʪ����ʹ�������־λ

#define JH_STATE MY_CONFIG[4+CONFIG_OFFSET]

#define IF_WORK   MY_CONFIG[10+CONFIG_OFFSET]  //�Ƿ��ڹ�����1���ڹ�����0��û����

#define WARN_WENDU MY_CONFIG[13+CONFIG_OFFSET]   //�¶ȱ���

#define WARN_SHIDU MY_CONFIG[14+CONFIG_OFFSET]   //ʪ�ȱ���
#define WARN_TVOC 	MY_CONFIG[15+CONFIG_OFFSET]   //TVOC����
#define WARN_PM2_5 MY_CONFIG[16+CONFIG_OFFSET]   //PM2.5����

#define CS_IN_CD				MY_CONFIG[23+CONFIG_OFFSET]			//��ʪ������CD״̬�����ɿ���

#define KT_USER_CMD    MY_CONFIG[24+CONFIG_OFFSET]	//����ʱ�Ŀյ��Զ���ĺ���������½��µ�



#if 0

				//Ϊ��ʹ�÷��㣬�ú����
#define CONFIG_OFFSET 0

#define MY_ADDR MY_CONFIG[0+CONFIG_OFFSET]

#define MY_STYLE MY_CONFIG[1+CONFIG_OFFSET]

#define KT_STATE MY_CONFIG[2+CONFIG_OFFSET]

#define KT_STATE1 MY_CONFIG[5+CONFIG_OFFSET]//�յ�������λ

#define CS_STATE MY_CONFIG[3+CONFIG_OFFSET]  //��ʪ����ʪ����ʪ����ʹ�������־λ

#define JH_STATE MY_CONFIG[4+CONFIG_OFFSET]



#define MY_MODEL MY_CONFIG[6+CONFIG_OFFSET]  //��¼��ǰ����ģʽ,����ģʽ������������ģʽ

#define KT_STYLE MY_CONFIG[7+CONFIG_OFFSET]		//�յ����ͣ�������������

#define WORK_MODE MY_CONFIG[8+CONFIG_OFFSET]		//����ģʽ��������ģʽ�������ݽ���ģʽ

#define CJ_ADDR   MY_CONFIG[9+CONFIG_OFFSET]		//�����ɼ����ĵ�ַ����Ϊ������u16����������Ķ�

#define IF_WORK   MY_CONFIG[10+CONFIG_OFFSET]  //�Ƿ��ڹ�����1���ڹ�����0��û����

#define IF_HAND   MY_CONFIG[11+CONFIG_OFFSET]    //�ֶ�ģʽ�����Զ�ģʽ��1���ֶ���2���Զ�



#define WARN_WATER MY_CONFIG[12+CONFIG_OFFSET]   //©ˮ����

#define WARN_WENDU MY_CONFIG[13+CONFIG_OFFSET]   //�¶ȱ���

#define WARN_SHIDU MY_CONFIG[14+CONFIG_OFFSET]   //ʪ�ȱ���
#define WARN_TVOC 	MY_CONFIG[15+CONFIG_OFFSET]   //TVOC����
#define WARN_PM2_5 MY_CONFIG[16+CONFIG_OFFSET]   //PM2.5����



									

												//���������������ޣ���������������û��С������

#define LIMIT_WENDU_H		MY_CONFIG[17+CONFIG_OFFSET]			//�¶����ޣ�
#define LIMIT_WENDU_L		MY_CONFIG[18+CONFIG_OFFSET]			//�¶�����
#define LIMIT_SHIDU_H		MY_CONFIG[19+CONFIG_OFFSET]			//ʪ������
#define LIMIT_SHIDU_L   MY_CONFIG[20+CONFIG_OFFSET]			//ʪ������
#define LIMIT_TVOC			MY_CONFIG[21+CONFIG_OFFSET]			//TVOCֻ������
#define LIMIT_PM2_5			MY_CONFIG[22+CONFIG_OFFSET]			//PM2_5ֻ������



#define CS_IN_CD				MY_CONFIG[23+CONFIG_OFFSET]			//��ʪ������CD״̬�����ɿ���


#define KT_USER_CMD    MY_CONFIG[24+CONFIG_OFFSET]	//����ʱ�Ŀյ��Զ���ĺ���������½��µ�


#endif



#define WORK_MODE1 0x01					//��������ģʽ
#define WORK_MODE2 0x02					//���ݽ���ģʽ


void Config_Init(void);

//���ñ������������ͣ�����0xffʧ��,0�ɹ�
u8 Set_MyStyle (u8 style);

u8 Set_CJ_Addr (u16 addr);

void Set_Addr (u8 addr);


#endif

