/*
 * audio_read_write.c
 *
 *  Created on: 2016年1月28日
 *      Author: Administrator
 */
#include <string.h>
#include <stdint.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <wmerrno.h>
#include <wm_utils.h>
#include <ctype.h>
#include <wmlist.h>
#include <psm-v2.h>
#include "../header/audio_read_write.h"


//写音频文件的数据到flash中.
int audio_write(const uint8_t *buf,uint32_t len, uint32_t address)
{
	void * dev;  //flash设备
	wmprintf("In audio_write the address is : %08x the len is: %d\r\n", address, len);

	flash_drv_init();

	dev = flash_drv_open(0);
	if (dev == NULL)
	{
		wmprintf("Flash driver init is required before open");
		return -WM_FAIL;
	}

	if (flash_drv_erase(dev, address, len) < 0) {
		flash_drv_close(dev);
		wmprintf("Failed to erase partition\r\n");
		return -WM_FAIL;
	}

	if (flash_drv_write(dev, buf, len, address) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}
	flash_drv_close(dev);
	return WM_SUCCESS;

}

//从flash中固定地址读取音频文件数据.
int audio_read(uint8_t* buf,int len, uint32_t address)
{
	void* dev;

	flash_drv_init();

	dev = flash_drv_open(0);
	if (dev == NULL) {
		wmprintf("Flash driver init is required before open");
		return -WM_FAIL;
	}

	int rv = flash_drv_read(dev, buf, len, address);
	if (rv != WM_SUCCESS) {
		wmprintf("Unable to read from flash: %x: %d", address,len);
		return rv;
	}
	wmprintf("read from flash: %x\r\n", address);
	flash_drv_close(dev);
	return  WM_SUCCESS;
}
#if 0
//第一次往flash分区中存放结构体时调用这个初始化函数。
int init_struct_flash(uint32_t address)
{	
	wmprintf("In function init_struct_flash\r\n");
	uint32_t ret;
	AudioFileInfo audio_file_test;
	/*AudioFileInfo audio_file = {0x1,0x1cc400,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
								0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
								0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
								0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};*/

	AudioFileInfo audio_file = {0x1, 0x1cc400, 0x46, 0x46, 0xAA, 0x2F, 0x00, 0x00, 0x1cc400, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 
								0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 
								0x01, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x86, 0x2F, 0x00, 0x00, 0x7F, 0x80, 0x80, 0x81, 
								0x82, 0x82, 0x81, 0x80, 0x80, 0x7E, 0x7E, 0x7D, 0x7D, 0x7D, 0x7E, 0x7F, 0x7F, 0x80, 0x82, 0x82, 
								0x82, 0x83, 0x83, 0x83, 0x83, 0x83, 0x81, 0x80, 0x7F, 0x7E, 0x7D, 0x7C, 0x7C, 0x7D, 0x7E, 0x7F};
	show_audio_file_info(&audio_file);
	audio_read(&audio_file_test, sizeof(AudioFileInfo), INFO_STRUCT_ADDR);
	os_thread_sleep(os_msec_to_ticks(2000));
	show_audio_file_info(&audio_file_test);
	ret = audio_write(&audio_file,sizeof(AudioFileInfo), INFO_STRUCT_ADDR);
	      //audio_write(&audio_file,sizeof(AudioFileInfo), INFO_STRUCT_ADDR);
	
	
	if(WM_SUCCESS != ret)
	{
		wmprintf("Init struct flash fail!\r\n");
		return WM_FAIL;
	}

	return WM_SUCCESS;
}
#endif

//检查给定地址开始的flash的状态标志.
/*
int verify_flash_state(uint32_t address)
{
	uint32_t flash_state;
	uint32_t ret;

	ret = audio_read(&flash_state,sizeof(uint32_t), address);
	if(WM_SUCCESS != ret)
	{	
		wmprintf("read fail\r\n");
		return WM_FAIL;
	}
	//检查结构体flash区是否初始化
	wmprintf("The flash_state is %d\r\n", flash_state);
	//if(flash_state != EXIST_ALREADY)
	//{
		//初始化结构体，并写入flash分区。
		init_struct_flash(address);
	//}

	return WM_SUCCESS;
}*/



//打印结构体数据
int show_audio_file_info(AudioFileInfo *des)
{
	//打印结构体数据.
	wmprintf("des->flash_init_flag is : %02x \r\n", des->flash_init_flag);
	wmprintf("des->start_addr is : %02x \r\n", des->start_addr);
	wmprintf("des->poweron_addr is : %02x \r\n", des->poweron_addr);
	wmprintf("des->poweron_size is : %02x \r\n", des->poweron_size);
	wmprintf("des->poweroff_addr is : %02x \r\n", des->poweroff_addr);
	wmprintf("des->poweroff_size is : %02x \r\n", des->poweroff_size);
	wmprintf("des->warm_addr is : %02x \r\n", des->warm_addr);
	wmprintf("des->warm_size is : %02x \r\n", des->warm_size);
	wmprintf("des->cold_addr is : %02x \r\n", des->cold_addr);
	wmprintf("des->cold_size is : %02x \r\n", des->cold_size);
	wmprintf("des->dry_addr is : %02x \r\n", des->dry_addr);
	wmprintf("des->dry_size is : %02x \r\n", des->dry_size);
	wmprintf("des->auto_addr is : %02x \r\n", des->auto_addr);
	wmprintf("des->auto_size is : %02x \r\n", des->auto_size);
	wmprintf("des->wind_addr is : %02x \r\n", des->wind_addr);
	wmprintf("des->wind_size is : %02x \r\n", des->wind_size);
	wmprintf("des->light_wind_addr is : %02x \r\n", des->light_wind_addr);
	wmprintf("des->light_wind_size is : %02x \r\n", des->light_wind_size);
	wmprintf("des->low_wind_addr is : %02x \r\n", des->low_wind_addr);
	wmprintf("des->low_wind_size is : %02x \r\n", des->low_wind_size);
	wmprintf("des->mid_wind_addr is : %02x \r\n", des->mid_wind_addr);
	wmprintf("des->mid_wind_size is : %02x \r\n", des->mid_wind_size);
	wmprintf("des->high_wind_addr is : %02x \r\n", des->high_wind_addr);
	wmprintf("des->high_wind_size is : %02x \r\n", des->high_wind_size);
	wmprintf("des->strong_wind_addr is : %02x \r\n", des->strong_wind_addr);
	wmprintf("des->strong_wind_size is : %02x \r\n", des->strong_wind_size);
	wmprintf("des->auto_wind_addr is : %02x \r\n", des->auto_wind_addr);
	wmprintf("des->auto_wind_size is : %02x \r\n", des->auto_wind_size);
	wmprintf("des->air_fresh_addr is : %02x \r\n", des->air_fresh_addr);
	wmprintf("des->air_fresh_size is : %02x \r\n", des->air_fresh_size);
	wmprintf("des->horizon_on_addr is : %02x \r\n", des->horizon_on_addr);
	wmprintf("des->horizon_on_size is : %02x \r\n", des->horizon_on_size);
	wmprintf("des->horizon_off_addr is : %02x \r\n", des->horizon_off_addr);
	wmprintf("des->horizon_off_size is : %02x \r\n", des->horizon_off_size);
	wmprintf("des->vertical_on_addr is : %02x \r\n", des->vertical_on_addr);
	wmprintf("des->vertical_on_size is : %02x \r\n", des->vertical_on_size);
	wmprintf("des->vertical_off_addr is : %02x \r\n", des->vertical_off_addr);
	wmprintf("des->vertical_off_size is : %02x \r\n", des->vertical_off_size);
	wmprintf("des->eleheat_on_addr is : %02x \r\n", des->eleheat_on_addr);
	wmprintf("des->eleheat_on_size is : %02x \r\n", des->eleheat_on_size);
	wmprintf("des->eleheat_off_addr is : %02x \r\n", des->eleheat_off_addr);
	wmprintf("des->eleheat_off_size is : %02x \r\n", des->eleheat_off_size);
	wmprintf("des->sleep_on_addr is : %02x \r\n", des->sleep_on_addr);
	wmprintf("des->sleep_on_size is : %02x \r\n", des->sleep_on_size);
	wmprintf("des->sleep_off_addr is : %02x \r\n", des->sleep_off_addr);
	wmprintf("des->sleep_off_size is : %02x \r\n", des->sleep_off_size);
	wmprintf("des->sixteen_addr is : %02x \r\n", des->sixteen_addr);
	wmprintf("des->sixteen_size is : %02x \r\n", des->sixteen_size);
	wmprintf("des->seventeen_addr is : %02x \r\n", des->seventeen_addr);
	wmprintf("des->seventeen_size is : %02x \r\n", des->seventeen_size);
	wmprintf("des->eighteen_addr is : %02x \r\n", des->eighteen_addr);
	wmprintf("des->eighteen_size is : %02x \r\n", des->eighteen_size);
	wmprintf("des->nineteen_addr is : %02x \r\n", des->nineteen_addr);
	wmprintf("des->nineteene_size is : %02x \r\n", des->nineteene_size);
	wmprintf("des->twenty_addr is : %02x \r\n", des->twenty_addr);
	wmprintf("des->twenty_size is : %02x \r\n", des->twenty_size);
	wmprintf("des->twenty_one_addr is : %02x \r\n", des->twenty_one_addr);
	wmprintf("des->twenty_one_size is : %02x \r\n", des->twenty_one_size);
	wmprintf("des->twenty_two_addr is : %02x \r\n", des->twenty_two_addr);
	wmprintf("des->twenty_two_size is : %02x \r\n", des->twenty_two_size);
	wmprintf("des->twenty_three_addr is : %02x \r\n", des->twenty_three_addr);
	wmprintf("des->twenty_three_size is : %02x \r\n", des->twenty_three_size);
	wmprintf("des->twenty_four_addr is : %02x \r\n", des->twenty_four_addr);
	wmprintf("des->twenty_four_size is : %02x \r\n", des->twenty_four_size);
	wmprintf("des->twenty_five_addr is : %02x \r\n", des->twenty_five_addr);
	wmprintf("des->twenty_five_size is : %02x \r\n", des->twenty_five_size);
	wmprintf("des->twenty_six_addr is : %02x \r\n", des->twenty_six_addr);
	wmprintf("des->twenty_six_size is : %02x \r\n", des->twenty_six_size);
	wmprintf("des->twenty_seven_addr is : %02x \r\n", des->twenty_seven_addr);
	wmprintf("des->twenty_seven_size is : %02x \r\n", des->twenty_seven_size);
	wmprintf("des->twenty_eight_addr is : %02x \r\n", des->twenty_eight_addr);
	wmprintf("des->twenty_eight_size is : %02x \r\n", des->twenty_eight_size);
	wmprintf("des->twenty_nine_addr is : %02x \r\n", des->twenty_nine_addr);
	wmprintf("des->twenty_nine_size is : %02x \r\n", des->twenty_nine_size);
	wmprintf("des->thirty_addr is : %02x \r\n", des->thirty_addr);
	wmprintf("des->thirty_size is : %02x \r\n", des->thirty_size);
	wmprintf("des->thirty_one_addr is : %02x \r\n", des->thirty_one_addr);
	wmprintf("des->thirty_one_size is : %02x \r\n", des->thirty_one_size);
	wmprintf("des->thirty_two_addr is : %02x \r\n", des->thirty_two_addr);
	wmprintf("des->thirty_two_size is : %02x \r\n", des->thirty_two_size);

	//wmprintf("the sizeof (uint64_t) is : %d\r\n ", sizeof(uint64_t));

	return 0;
}

#if 0
int main(int argc,char * argv[])
{
	int i = 0;
	uint32_t ret;
	uint8_t buf_read[5];

	wmstdio_init(UART0_ID, 0);
	os_thread_sleep(os_msec_to_ticks(5000));

	/*ret = verify_flash_state(INFO_STRUCT_ADDR); 
	if(WM_SUCCESS != ret)
	{
		wmprintf("verify_flash fial!\r\n");
		return WM_FAIL;
	}*/
	
	wmprintf("buffer:");
	/*for(i = 0;i < 5;i++)
	{
		wmprintf("%02x ",buffer[i]);
	}*/
	wmprintf("\r\n");
	AudioFileInfo wav_src = {0x1,0x1cc400,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
								0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
								0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
								0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

	
	AudioFileInfo wav_src_two = {0x01, 0x1cc400, 0x52, 0x49, 0x46, 0x46, 0xAA, 0x28, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 
	0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 
	0x01, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x86, 0x28, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
	0x80, 0x80, 0x80, 0x81, 0x81, 0x80, 0x81, 0x81, 0x81, 0x80, 0x80, 0x7F, 0x7F, 0x7F};
	AudioFileInfo wav_des;
	//audio_write(&wav_src,sizeof(AudioFileInfo), INFO_STRUCT_ADDR);
	wmprintf("start read...\r\n");
	
	audio_read(&wav_des,sizeof(AudioFileInfo), INFO_STRUCT_ADDR);
	show_audio_file_info(&wav_des);
	audio_write(&wav_src,sizeof(AudioFileInfo), INFO_STRUCT_ADDR);
	os_thread_sleep(os_msec_to_ticks(5000));
	while(i++ < 200)
	{
		audio_read(&wav_des,sizeof(AudioFileInfo), INFO_STRUCT_ADDR);
		show_audio_file_info(&wav_des);
		os_thread_sleep(os_msec_to_ticks(500));
		wmprintf("the count of i is >>>>>>>>>> %d\r\n", i);
	}
	wmprintf("\r\n");
	return 0;
}
#endif
