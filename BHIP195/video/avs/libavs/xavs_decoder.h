#ifndef _XAVS_DECODER_H
#define _XAVS_DECODER_H

typedef struct tagxavs_param
{
	
	unsigned  int i_color_space;
	unsigned char *p_in;//压缩帧，数据输入,或者以序列头开始的数据
	unsigned  int  i_in_length;//压缩帧长度
	unsigned char *p_out;//解码后图象缓冲区
	unsigned  int i_out_pitch;//解码后的图象缓冲区pitch值

	long           l_result;
}xavs_param;
typedef struct tagxavs_seq_info
{
	int iWidth;
	int iHeight;
	int iAspectRatioX;
	int iAspectRatioY;
	int iFrameRateX;
	int iFrameRateY;
	int iProgressiveSequence;
	  
}xavs_seq_info;

#define XAVS_SEQ_HEADER  1
#define XAVS_FRAME_OUT  2


typedef struct AVSFrame 
{
	int nWidth;      //图像宽高
		
	int nHeight;
		
	unsigned char *data[4];   //0、1、2分别放Y、U、V
	
	int linesize[4];     //0、1、2分别放Y、U、V的宽度
	
	int nFrameType;   //帧类型 0I 1P 2B
	
	int nBitrate;     //码率

	int nFrameCoded;
	int nTopFieldFirst;
	int nFrameIndex;
	void *param0;
	void *param1;
} AVSFrame;

//创建decoder
int xavs_decoder_create(void **pp_decoder);

//输入压缩数据
int xavs_decoder_put_data(void *p_decoder,unsigned char *data,int len);
//获取解码数据，如果got_frame获得解码帧，则继续取得直到got_frame返回为0
int xavs_decoder_get_decode_video(void *p_decoder,int *got_frame,AVSFrame *data);
//复位解码器
int xavs_decoder_reset(void *p_decoder);
//释放解码器
void xavs_decoder_destroy(void *p_decoder);
//复位搜索帧buffer
int xavs_decoder_reset_buffer(void *p_decoder);
//
//must a complete frame
int xavs_decoder_decode_frame(void *p_decoder,unsigned char *p_buf,int i_len,int *have_frame,AVSFrame *p_param);
////这两个函数内部使用，外部基本不使用
int xavs_decoder_get_seq(void *p_decoder,xavs_seq_info *p_si);

//新增几口，用于设置丢帧模式，0不丢帧，为默认值； 1丢B帧等非参考帧；2只保留I帧，设置该参数会可能会复位解码器
int xavs_decoder_set_frame_skip_mode(void *p_decoder,int skip_mode);

//根据缓冲区获得帧类型
int xavs_decoder_get_frame_type(void *p_decoder, unsigned char *data,int len);
int xavs_get_frame_type(unsigned char *data,int len, int profile_id);



void *xavs_malloc(int);
void *xavs_realloc(void *p, int i_size);
void  xavs_free(void *);

typedef int  (* xavs_decoder_create_t)(void **pp_decoder);
typedef int  (* xavs_decoder_put_data_t)(void *p_decoder,unsigned char *data,int len);
typedef int  (* xavs_decoder_get_decode_video_t)(void *p_decoder,int *got_frame,AVSFrame *data);
typedef int  (* xavs_decoder_reset_t)(void *p_decoder);
typedef void (* xavs_decoder_destroy_t)(void *p_decoder);
typedef int  (* xavs_decoder_reset_buffer_t)(void *p_decoder);
typedef int  (* xavs_decoder_decode_frame_t)(void *p_decoder,unsigned char *p_buf,int i_len,int *have_frame,AVSFrame *p_param);
typedef int  (* xavs_decoder_get_seq_t)(void *p_decoder,xavs_seq_info *p_si);
typedef int  (* xavs_get_frame_tye_t)(unsigned char *data,int len, int profile_id);
typedef int  (* xavs_decoder_set_frame_skip_mode_t)(void *p_decoder,int skip_mode);
typedef int  (* xavs_decoder_get_frame_type_t)(void *p_decoder, unsigned char *data,int len);

typedef int  (* xavs_get_frame_type_t)(unsigned char *data,int len, int profile_id);
typedef struct tag_xavs_decoder_funs
{
	xavs_decoder_create_t                   create;
	xavs_decoder_set_frame_skip_mode_t      set_frame_skip_mode;
	xavs_decoder_put_data_t                 put_data;
	xavs_decoder_get_decode_video_t         get_decode_video;
	xavs_decoder_reset_t                    reset;
	xavs_decoder_destroy_t                  destroy;
	xavs_decoder_reset_buffer_t             reset_buffer;
	xavs_decoder_decode_frame_t             decode_frame;
	xavs_decoder_get_seq_t                  get_seq;
	xavs_decoder_get_frame_type_t           get_frame_type;
	//xavs_get_frame_tye_t                    xavs_get_frame_type;
}xavs_decoder_funs;
typedef int  (* xavs_decoder_load_funs_t)(xavs_decoder_funs *funs, int mode);
int xavs_decoder_load_funs(xavs_decoder_funs *funs, int mode);

#ifdef WIN32
#define __LoadLibrary LoadLibrary
#define __GetProcAddress GetProcAddress

#else
typedef void * HMODULE;
#define __LoadLibrary(filename) dlopen(filename,RTLD_LAZY)
#define __GetProcAddress dlsym
#endif

#define XAVS_LOAD_LIB(filename, funs, mode) \
	HMODULE handle = LoadLibrary(filename); \
	xavs_decoder_load_funs_t load_fun =  (xavs_decoder_load_funs_t)GetProcAddress(handle,"xavs_decoder_load_funs"); \
    if(load_fun != NULL) \
	{\
	     load_fun(&funs, mode);\
	}	

#endif