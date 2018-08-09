#ifndef __XDRA_DECODER_H
#define __XDRA_DECODER_H
typedef struct tagxdra_frame_header
{
	int frame_header_type;
	int num_word;
	int num_block_per_frame;
	int sample_rate_index;//采样频率索引
	int num_normal_channel;//正常通道数
	int num_lfe_channel;//低频增强声道数
	int aux_data;
	int use_sum_diff;
	int use_jic;
	int jic_cb;
}xdra_frame_header;
//三个工具函数
//寻找帧头函数，返回是针对相对于传入缓冲区地址的偏移,
//没找到返回-1
int xdra_next_sync_word(unsigned char *buf, int len);
//解析帧头，输入必须以0x7FFF开始的缓冲区，size不能小于4
//解析失败返回-1
int xdra_parse_header(unsigned char *buf, int len,  xdra_frame_header *header);
//根据解析出来的帧头的索引得到采样率,index范围为0到12
int xdra_get_sample_rate(int index);
//////
//创建解码器,缓冲区地址可以为NULL,也可以以同步字开始的一帧数据
void *xdra_decoder_create(unsigned char *buf, int len);
//获得帧头信息，此函数要成功必须在put_data返回解码一帧成功以后。
int xdra_decoder_get_frame_header(void *decoder, xdra_frame_header *header);
//往解码器送缓冲区，可以是帧对齐，也可以不是帧对齐，内部自动会判断,
int xdra_decoder_put_data(void *decoder, unsigned char *data, int len);
//解码下一帧，在调用xdra_decoder_decode_begin后，调用xdra_decoder_decode_next_frame直到返回失败，失败为负数，成功为0
int xdra_decoder_decode_next_frame(void *decoder);

//解码一帧数据，输入必须是完整一帧，缓冲区以同步字开始即前两个字节为0x7F 0xFF
int xdra_decoder_decode_frame(void *decoder, unsigned char *frame, int len);
//如果xdra_decoder_decode_next_frame或xdra_decoder_decode_frame解码成功,则调用下面三个函数获得解码后的PCM数据，如果成功他们的返回值为样本数量，
//xdra_decoder_get_channel_f32和xdra_decoder_get_channel_s16固定为1024，xdra_decoder_get_stero_s16固定为2048如果失败为-1
//基于声道的获得浮点型解码数据，
int xdra_decoder_get_channel_f32(void *decoder, int channel, float *buf, int size);
//基于声道的有符号的短整型解码数据
int xdra_decoder_get_channel_s16(void *decoder, int channel, short *buf, int size);
//基于双通道的的有符号的短整型解码数据，如果码流本身只有一个声道数据，则复制该声道，形成双声道。
int xdra_decoder_get_stero_s16(void *decoder, short *buf, int size);
//解码器复位
void xdra_decoder_reset(void *decoder);
//解码器释放资源
void xdra_decoder_destroy(void *decoder);

#endif
