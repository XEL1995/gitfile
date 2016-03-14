/******************************************************************************   
* �ļ���: audio_read_write.h 
* ��������: ��Ÿ����������ŵĵĺ���������ļ�.
* ����: sunyiwen  
* ����: 2016��2��3�� ����11:42:03 
* �汾:   
******************************************************************************/
#ifndef AUDIO_READ_WRITE_H
#define AUDIO_READ_WRITE_H
#include <stdint.h>

#define FC_AUDIO_FILE 7
#define AUDIO_INFO_LEN 12
#define INFO_STRUCT_ADDR 0x1cc000
#define AUDIO_ADDR 0x1cd000
#define BUF_SIZE 10

#define EXIST_ALREADY  0x1
#define EXIST_NONE     0X0


//��Ƶ�ļ���flash�д�ŵĵ�ַ����Ϣ�Ľṹ��
typedef struct  audio_file_info{
uint32_t                 flash_init_flag; //flash�ṹ�������ʼ����־
uint32_t                 start_addr;    //ÿ��д�ļ�����ʼ��ַ

uint32_t                 poweron_addr;//����
uint16_t                 poweron_size;
uint32_t                 poweroff_addr; //�ػ�
uint16_t                 poweroff_size;
uint32_t                 warm_addr; //����
uint16_t                 warm_size;
uint32_t                 cold_addr; //����
uint16_t                 cold_size;
uint32_t                 dry_addr; //��ʪ
uint16_t                 dry_size;
uint32_t                 auto_addr; //ȫ�Զ�
uint16_t                 auto_size;
uint32_t                 wind_addr; //�ͷ�
uint16_t                 wind_size;
uint32_t                 light_wind_addr; //΢��
uint16_t                 light_wind_size;
uint32_t                 low_wind_addr; //�ͷ�
uint16_t                 low_wind_size;
uint32_t                 mid_wind_addr; //�з�
uint16_t                 mid_wind_size;
uint32_t                 high_wind_addr; //�߷�
uint16_t                 high_wind_size;
uint32_t                 strong_wind_addr; //ǿ����
uint16_t                 strong_wind_size;
uint32_t                 auto_wind_addr; //�Զ���
uint16_t                 auto_wind_size;
uint32_t                 air_fresh_addr; //����
uint16_t                 air_fresh_size;
uint32_t                 horizon_on_addr; //���ҷ���
uint16_t                 horizon_on_size;
uint32_t                 horizon_off_addr; //���ҷ����
uint16_t                 horizon_off_size;
uint32_t                 vertical_on_addr; //���·���
uint16_t                 vertical_on_size;
uint32_t                 vertical_off_addr; //���·����
uint16_t                 vertical_off_size;
uint32_t                 eleheat_on_addr; //����ȿ�
uint16_t                 eleheat_on_size;
uint32_t                 eleheat_off_addr; //����ȹ�
uint16_t                 eleheat_off_size;
uint32_t                 sleep_on_addr; //˯�߿�
uint16_t                 sleep_on_size;
uint32_t                 sleep_off_addr; //˯�߹�
uint16_t                 sleep_off_size;
uint32_t                 sixteen_addr; //16��
uint16_t                 sixteen_size;
uint32_t                 seventeen_addr; //17��
uint16_t                 seventeen_size;
uint32_t                 eighteen_addr; //18��
uint16_t                 eighteen_size;
uint32_t                 nineteen_addr; //19��
uint16_t                 nineteene_size;
uint32_t                 twenty_addr; //20��
uint16_t                 twenty_size;
uint32_t                 twenty_one_addr; //21��
uint16_t                 twenty_one_size;
uint32_t                 twenty_two_addr; //22��
uint16_t                 twenty_two_size;
uint32_t                 twenty_three_addr; //23��
uint16_t                 twenty_three_size;
uint32_t                 twenty_four_addr; //24��
uint16_t                 twenty_four_size;
uint32_t                 twenty_five_addr; //25��
uint16_t                 twenty_five_size;
uint32_t                 twenty_six_addr; //26��
uint16_t                 twenty_six_size;
uint32_t                 twenty_seven_addr; //27��
uint16_t                 twenty_seven_size;
uint32_t                 twenty_eight_addr; //28��
uint16_t                 twenty_eight_size;
uint32_t                 twenty_nine_addr; //29��
uint16_t                 twenty_nine_size;
uint32_t                 thirty_addr; //30��
uint16_t                 thirty_size;
uint32_t                 thirty_one_addr; //31��
uint16_t                 thirty_one_size;
uint32_t                 thirty_two_addr; //32��
uint16_t                 thirty_two_size;
}AudioFileInfo;

//���Դ������
typedef enum{
  VOICE_INIT = 0,
  POWER_ON_FILE,
  POWER_OFF_FILE,
  WARM_MODE_FILE,
  COLD_MODE_FILE,
  DRY_MODE_FILE,
  AUTO_MODE_FILE,
  WIND_MODE_FILE,
  LIGHT_WIND_FILE,
  LOW_WIND_FILE,
  MID_WIND_FILE,
  HIGH_WIND_FILE,
  STRONG_WIND_FILE,
  AUTO_WIND_FILE,
  AIR_FRESH_FILE,
  HORIZON_ON_FILE,
  HORIZON_OFF_FILE,
  VERTICAL_ON_FILE,
  VERTICAL_OFF_FILE,
  ELEHEAT_ON_FILE,
  ELEHEAT_OFF_FILE,
  SLEEP_ON_FILE,
  SLEEP_OFF_FILE,
  TEM_SIXTEEN_FILE,
  TEM_SEVENTEEN_FILE,
  TEM_EIGHTEEN_FILE,
  TEM_NINETEEN_FILE,
  TEM_TWENTY_FILE,
  TEM_TWENTY_ONE_FILE,
  TEM_TWENTY_TWO_FILE,
  TEM_TWENTY_THREE_FILE,
  TEM_TWENTY_FOUR_FILE,
  TEM_TWENTY_FIVE_FILE,
  TEM_TWENTY_SIX_FILE,
  TEM_TWENTY_SEVEN_FILE,
  TEM_TWENTY_EIGHT_FILE,
  TEM_TWENTY_NINE_FILE,
  TEM_THIRTY_FILE,
  TEM_THIRTY_ONE_FILE,
  TEM_THIRTY_TWO_FILE,

}VOICE_Type;

//д��Ƶ�ļ������ݵ�flash��.
int audio_write(const uint8_t *buf,uint32_t len, uint32_t address);
//��flash�й̶���ַ��ȡ��Ƶ�ļ�����.
int audio_read(uint8_t* buf,int len, uint32_t address);
//��һ����flash�����д�Žṹ��ʱ���������ʼ��������
//int init_struct_flash(uint32_t address);
//��������ַ��ʼ��flash��״̬��־.
int verify_flash_state(uint32_t address);
//��ӡ�ṹ������
int show_audio_file_info(AudioFileInfo *des);





#endif
