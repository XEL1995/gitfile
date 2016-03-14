/*
 * save_file_addr.h
 *
 *  Created on: 2016年2月4日
 *      Author: Administrator
 */

#ifndef SAMPLE_APPS_IO_DEMO_FILE_SAVE_SRC_SAVE_FILE_ADDR_H_
#define SAMPLE_APPS_IO_DEMO_FILE_SAVE_SRC_SAVE_FILE_ADDR_H_
#include "audio_read_write.h"
//初始化结构体
//int init_struct_flash(uint32_t address);
int init_struct_flash(AudioFileInfo *audio_file);

int file_write(char* file_buf,/*存储文件内容*/
		        unsigned char num,/*文件编号*/
				int length/*文件长度*/);

void file_read(int num);

void read_struct(AudioFileInfo *audio_file_info);

void write_struct(AudioFileInfo *audio_file_info);
//写音频文件的数据到flash中.
//int audio_write(const uint8_t *buf,uint32_t len, uint32_t address);
//从flash中固定地址读取音频文件数据.
//int audio_read(uint8_t* buf,int len, uint32_t address);
#endif /* SAMPLE_APPS_IO_DEMO_FILE_SAVE_SRC_SAVE_FILE_ADDR_H_ */
