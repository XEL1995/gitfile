/*
 * electric_measure.h
 *
 *  Created on: 2016��3��24��
 *      Author: Administrator
 */

#ifndef SAMPLE_APPS_HLW8012_HEADER_ELECTRIC_MEASURE_H_
#define SAMPLE_APPS_HLW8012_HEADER_ELECTRIC_MEASURE_H_

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//Time1��ʱ����ʱ,ʱ����� = 1ms
#define D_TIME1_1000US				1000
#define D_TIME1_20MS				20
#define D_TIME1_100MS				100
#define D_TIME1_150MS				150
#define D_TIME1_200MS				200
#define D_TIME1_400MS				400
#define D_TIME1_500MS				500
#define D_TIME1_1S				1000		//Time1��ʱ����ʱ1Sʱ�䳣��
#define D_TIME1_2S				2000
#define D_TIME1_3S				3000
#define D_TIME1_4S				4000
#define D_TIME1_6S				6000
#define D_TIME1_8S				8000
#define D_TIME1_9S				9000
#define D_TIME1_10S				10000
#define D_TIME1_20S				20000


#define D_TIME1_V_OVERFLOW                      500        //Time1��ʱ��,��ѹ��������趨Ϊ500mS,���˵���������ڴ���500mS
#define D_TIME1_I_OVERFLOW			8000	   //Time1��ʱ��,������������趨Ϊ10S,���˵���������ڴ���10S
#define D_TIME1_P_OVERFLOW			12000	   //Time1��ʱ��,������������趨Ϊ10S(Լ0.5W��Сֵ),���˵���������ڴ���10S
//#define D_TIME1_P_OVERFLOW			40000	   //Time1��ʱ��,������������趨Ϊ40S(Լ0.2W��Сֵ)
#define D_TIME1_E_TIME              3600
#define D_TIME1_CAL_TIME			36000	   //У��ʱ�䣬��¼�ڴ�ʱ���ڵ���������1000W�������õ�36Sʱ���ںķ�0.01�ȵ�
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


//HLW 8012 IO����
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//#define IO_HLW8012_CF1_S      PD_ODR_ODR3		//HLW8012 PIN8
//#define IO_HLW8012_CF1        PA_IDR_IDR2		//HLW8012 PIN7
//#define IO_HLW8012_CF         PC_IDR_IDR5		//HLW8012 PIN6
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


//����ģʽ
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define D_ERR_MODE                	0x00        //������ʾģʽ
#define D_NORMAL_MODE		      	0x10	    //��������ģʽ
#define D_CAL_START_MODE		0x21	    //У��ģʽ������
#define D_CAL_END_MODE		        0x23	    //У��ģʽ�����
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#define DATA_ADDR             0x104000
#define DATA_FLAG_ADDR        0x104000

#define P_SAVE_ADDR           0x104010
#define V_SAVE_ADDR           0x104020
#define I_SAVE_ADDR           0x104030
#define E_SAVE_ADDR           0x104040
#define E_001_Pluse_CNT_ADDR  0x104050

#define PAGESIZE              0x1000  //������С����


//ROM����
//--------------------------------------------------------------------------------------------
#define u1      _Bool
#define u8	unsigned char
#define u16	unsigned int
#define u32	unsigned long
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define FALSE 0
#define TRUE 1

union IntData
{
	u16  inte;
	u8 byte[2];
};
union LongData
{
    u32  word;
    u16  inte[2];
    u8   byte[4];
};

void Init_Rom(void);

void PC5_EXTI_Interrupt(void);

void PD2_EXTI_Interrupt(void);

void TIM1_UPD_Interrupt(void);

void HLW8012_Measure_P(void);

void HLW8012_Measure_V(void);

void HLW8012_Measure_I(void);

void HLW8012_Measure_COS(void);

void HLW8012_Measure_E(void);


#endif /* SAMPLE_APPS_HLW8012_HEADER_ELECTRIC_MEASURE_H_ */
