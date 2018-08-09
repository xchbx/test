#ifndef _TS_TABLE_H_
#define _TS_TABLE_H_

#include <string.h>
class CCRC
{
public:
	CCRC();
	~CCRC();
	void init();
	unsigned int calcCRC(unsigned char *pHead, unsigned char* pEnd);
protected:
	unsigned int m_CRCTable[256];
};

class CPAT_INFO
{
public:
	CPAT_INFO();
	~CPAT_INFO();
	void init();
public:
	int 	m_pmt_pid;
	unsigned char	m_program_num;
};

class CPMT_INFO
{
public:
	CPMT_INFO();
	~CPMT_INFO();
	void init();
public:
	int    m_program_num;
	int	m_pmt_pid;
	int	m_pcr_pid;
	int	m_video_pid;
	int	m_audio_pid;
	int	m_video_enc_type;//0x1b is h264 ...
	int	m_audio_enc_type;//0x0f is aac,0x03 is mp2, 0x6 ac3
};

class CPAT
{
public:
	CPAT();
	~CPAT();
	void init();
	void setPatInfo(CPAT_INFO pat_info);
	int getPatData(unsigned long long pcr, unsigned char *pData);

protected:
	unsigned long long m_pcr;
	unsigned char	m_continuity_counter;
	unsigned char m_packet[188];
	CCRC m_crc;
};

class CPMT
{
public:
	CPMT();
	~CPMT();
	void init();
	void setPmtInfo(CPMT_INFO pmt_info);
	int getPmtData(unsigned long long pcr, unsigned char *pData);

protected:
	unsigned long long m_pcr;
	unsigned char	m_continuity_counter;
	unsigned char m_packet[188];
	CCRC m_crc;
};

#endif
