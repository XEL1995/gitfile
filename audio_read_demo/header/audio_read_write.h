/******************************************************************************   
* 文件名: audio_read_write.h 
* 功能描述: 存放各种语音播放的的函数和相关文件.
* 作者: sunyiwen  
* 日期: 2016年2月3日 上午11:42:03 
* 版本:   
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


//音频文件在flash中存放的地址等信息的结构体
typedef struct  audio_file_info{
uint32_t                 flash_init_flag; //flash结构体区域初始化标志
uint32_t                 start_addr;    //每次写文件的起始地址

uint32_t                 poweron_addr;//开机
uint16_t                 poweron_size;
uint32_t                 poweroff_addr; //关机
uint16_t                 poweroff_size;
uint32_t                 warm_addr; //制热
uint16_t                 warm_size;
uint32_t                 cold_addr; //制冷
uint16_t                 cold_size;
uint32_t                 dry_addr; //除湿
uint16_t                 dry_size;
uint32_t                 auto_addr; //全自动
uint16_t                 auto_size;
uint32_t                 wind_addr; //送风
uint16_t                 wind_size;
uint32_t                 light_wind_addr; //微风
uint16_t                 light_wind_size;
uint32_t                 low_wind_addr; //低风
uint16_t                 low_wind_size;
uint32_t                 mid_wind_addr; //中风
uint16_t                 mid_wind_size;
uint32_t                 high_wind_addr; //高风
uint16_t                 high_wind_size;
uint32_t                 strong_wind_addr; //强劲风
uint16_t                 strong_wind_size;
uint32_t                 auto_wind_addr; //自动风
uint16_t                 auto_wind_size;
uint32_t                 air_fresh_addr; //空清
uint16_t                 air_fresh_size;
uint32_t                 horizon_on_addr; //左右风向开
uint16_t                 horizon_on_size;
uint32_t                 horizon_off_addr; //左右风向关
uint16_t                 horizon_off_size;
uint32_t                 vertical_on_addr; //上下风向开
uint16_t                 vertical_on_size;
uint32_t                 vertical_off_addr; //上下风向关
uint16_t                 vertical_off_size;
uint32_t                 eleheat_on_addr; //电加热开
uint16_t                 eleheat_on_size;
uint32_t                 eleheat_off_addr; //电加热关
uint16_t                 eleheat_off_size;
uint32_t                 sleep_on_addr; //睡眠开
uint16_t                 sleep_on_size;
uint32_t                 sleep_off_addr; //睡眠关
uint16_t                 sleep_off_size;
uint32_t                 sixteen_addr; //16度
uint16_t                 sixteen_size;
uint32_t                 seventeen_addr; //17度
uint16_t                 seventeen_size;
uint32_t                 eighteen_addr; //18度
uint16_t                 eighteen_size;
uint32_t                 nineteen_addr; //19度
uint16_t                 nineteene_size;
uint32_t                 twenty_addr; //20度
uint16_t                 twenty_size;
uint32_t                 twenty_one_addr; //21度
uint16_t                 twenty_one_size;
uint32_t                 twenty_two_addr; //22度
uint16_t                 twenty_two_size;
uint32_t                 twenty_three_addr; //23度
uint16_t                 twenty_three_size;
uint32_t                 twenty_four_addr; //24度
uint16_t                 twenty_four_size;
uint32_t                 twenty_five_addr; //25度
uint16_t                 twenty_five_size;
uint32_t                 twenty_six_addr; //26度
uint16_t                 twenty_six_size;
uint32_t                 twenty_seven_addr; //27度
uint16_t                 twenty_seven_size;
uint32_t                 twenty_eight_addr; //28度
uint16_t                 twenty_eight_size;
uint32_t                 twenty_nine_addr; //29度
uint16_t                 twenty_nine_size;
uint32_t                 thirty_addr; //30度
uint16_t                 thirty_size;
uint32_t                 thirty_one_addr; //31度
uint16_t                 thirty_one_size;
uint32_t                 thirty_two_addr; //32度
uint16_t                 thirty_two_size;
}AudioFileInfo;

//语言词条编号
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

//写音频文件的数据到flash中.
int audio_write(const uint8_t *buf,uint32_t len, uint32_t address);
//从flash中固定地址读取音频文件数据.
int audio_read(uint8_t* buf,int len, uint32_t address);
//第一次往flash分区中存放结构体时调用这个初始化函数。
//int init_struct_flash(uint32_t address);
//检查给定地址开始的flash的状态标志.
int verify_flash_state(uint32_t address);
//打印结构体数据
int show_audio_file_info(AudioFileInfo *des);





#endif
