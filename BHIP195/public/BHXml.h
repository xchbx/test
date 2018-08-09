#ifndef _H_BH_XML
#define _H_BH_XML

#include <string>
using namespace std;

#define MAX_LAYER		16
#define MAX_ELEM_SIZE	64
#define MAX_CONTENT		64

enum StatusType
{
	STATUS_INIT			= 0x0,
	STATUS_ELEM_ADD		= 0x1,
	STATUS_ELEM_FINISH	= 0x2
};

class CBHXml
{
public:
	CBHXml(void);
public:
	~CBHXml(void);

public:

    bool Reserve(int nBufSize);

	bool SetBuffer(int nBufSize);

	bool SetDoc(const char *pDoc);

	char *GetDoc();

	bool Save(const char *strPath);

	bool AddElem(const char *pStr, const char *pContent=NULL);

	bool IntoElem();

	bool OutOfElem();

	bool AddAttrib(const char *pKey, const char *pValue);

	bool SetAttrib(const char *pKey, const char *pValue);

	bool AddAttrib(const char *pKey, const int nValue);

	bool SetAttrib(const char *pKey, const int nValue);

    bool AddAttrib(const char *pKey, const unsigned int nValue);

    bool SetAttrib(const char *pKey, const unsigned int nValue);

    bool AddUint64Attrib(const char *pKey, const unsigned long long nValue);

    bool AddSubDoc( const char* pData );

private:
	bool AddStatusCompletion();
	int ConvertSpecialChar(const char* pSrc, int nSrcLen, char* pDes, int nDesLen);

private:

	char*   m_pBuf;
	int     m_nBufSize;

	int     m_nLayer;
	char*   m_pElem[MAX_LAYER];

	char*	m_pContent;
	int		m_nContentLen;
	int     m_nStatus;

	int     m_nPos;

};


#endif
