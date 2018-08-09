
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BHXml.h"
#include <assert.h>

#pragma warning(disable:4996)
CBHXml::CBHXml(void)
{
	m_nBufSize = 256*16;
	m_nPos = 0;
	m_nStatus = 0;
	m_nLayer = 0;
	m_pBuf = (char *)malloc(m_nBufSize);
	memset(m_pBuf, 0, m_nBufSize);

	m_pContent = (char *)malloc(MAX_CONTENT);
	memset(m_pContent, 0, MAX_CONTENT);
	m_nContentLen = 0;
	for(int i=0; i<MAX_LAYER; i++)
	{
		m_pElem[i] = (char *)malloc(MAX_ELEM_SIZE);
		memset(m_pElem[i], 0, MAX_ELEM_SIZE);
	}
}

CBHXml::~CBHXml(void)
{
	if(NULL != m_pBuf)
	{
		free(m_pBuf);
	}

	if(NULL != m_pContent)
	{
		free(m_pContent);
	}

	for(int i=0; i<MAX_LAYER; i++)
	{
		if(NULL != m_pElem[i])
		{
			free(m_pElem[i]);
		}
	}
}

bool CBHXml::Reserve(int nBufSize)
{
    return SetBuffer(nBufSize);
}

bool CBHXml::SetBuffer(int nBufSize)
{
	if(nBufSize < m_nBufSize)
	{
		return false;
	}

	char *pBuf = (char *)malloc(nBufSize);

	if(NULL != m_pBuf)
	{
		memcpy(pBuf, m_pBuf, m_nPos);
		free(m_pBuf);
	}

	m_pBuf = pBuf;
	m_nBufSize = nBufSize;
	return true;
}


bool CBHXml::SetDoc(const char *pDoc)
{
	int nLen = ( int )strlen(pDoc);
	int nBufSize = m_nBufSize;
	while(nLen > nBufSize)
	{
		nBufSize <<= 1;
	}

	if(SetBuffer(nBufSize) == false)
	{
		return false;
	}

	assert(m_nBufSize >= nLen);
	memcpy(m_pBuf, pDoc, nLen);
	m_nPos = nLen;

	return true;
}


char *CBHXml::GetDoc()
{
	AddStatusCompletion();

	while(m_nLayer)
	{
		m_nLayer--;
		
		// Edit by Gengzl
		while((m_nPos + 4 + MAX_ELEM_SIZE) > m_nBufSize)
		{
			SetBuffer(2*m_nBufSize);
		}

		m_pBuf[m_nPos] = '<';
		m_pBuf[m_nPos + 1] = '/';
		m_nPos += 2;
		int nLen = (int)strlen(m_pElem[m_nLayer]);
		memcpy(m_pBuf + m_nPos, m_pElem[m_nLayer], nLen);
		m_nPos += nLen;
		m_pBuf[m_nPos] = '>';
		m_pBuf[m_nPos + 1] = '\n';
		m_nPos += 2;
	}

	// Edit by Gengzl
	while (m_nPos+1 > m_nBufSize)
	{
		SetBuffer(2*m_nBufSize);
	}

	assert(m_nBufSize >= m_nPos+1);
	m_pBuf[m_nPos] = '\0';
	m_nPos++;

	return m_pBuf;
}

bool CBHXml::Save(const char *strPath)
{
	std::string strContext = GetDoc();

	FILE *pFile;
	pFile = fopen(strPath, "w+b");
	if (NULL == pFile)
	{
		printf("fopen failed %s\n", strPath);
		return false;
	}

	size_t nWrite = fwrite(strContext.c_str(), 1, strContext.size(), pFile);
	if (!nWrite)
	{
		printf("fwrite failed %s\n", strPath);
		fclose(pFile);
		return false;
	}

	fclose(pFile);
	return true;
}

bool CBHXml::AddElem(const char *pStr, const char *pContent)
{
	int nLen = (int)strlen(pStr);
	int nContentLen = pContent ? strlen(pContent):0;

	//Elem的长度不得大于MAX_ELEM_SIZE-1
	if (nLen > MAX_ELEM_SIZE - 1 || nContentLen > MAX_CONTENT - 1)
	{
		return false;
	}

	//记录当前的Elem
	memcpy(m_pElem[m_nLayer], pStr, nLen);
	m_pElem[m_nLayer][nLen] = '\0';

	//上一次也为AddElem，则需要将上一次的结束
	AddStatusCompletion();
	
	//如果Elem有内容的话,需要记录一下
	m_nContentLen = nContentLen;
	if(0 != m_nContentLen)
	{
		memcpy(m_pContent, pContent, m_nContentLen);
	}

	// Edit by Gengzl
	while (nLen + m_nPos + 1 > m_nBufSize)
	{
		SetBuffer(m_nBufSize * 2);
	}

	//添加新的Elem
	m_pBuf[m_nPos] = '<';
	m_nPos++;
	memcpy(m_pBuf + m_nPos, pStr, nLen);
	m_nPos += nLen;

	m_nStatus = STATUS_ELEM_ADD;		//记录当前状态

	return true;
}

bool CBHXml::IntoElem()
{
	int nLen = 4;

	while (nLen + m_nPos + m_nContentLen > m_nBufSize)
	{
		SetBuffer(m_nBufSize * 2);
	}

	//上一次也为AddElem，则需要将上一次的结束
	if(STATUS_ELEM_ADD == m_nStatus)
	{
		assert(m_nBufSize >= m_nPos+2);
		m_pBuf[m_nPos] = '>';
		m_pBuf[m_nPos + 1] = '\n';
		m_nPos += 2;

		if(0 != m_nContentLen)
		{
			assert(m_nBufSize >= m_nPos+1+m_nContentLen);
			memcpy(m_pBuf + m_nPos, m_pContent, m_nContentLen);
			m_nPos += m_nContentLen;
			m_pBuf[m_nPos] = '\n';
			m_nPos += 1;
		}
	}

	//层数++
	m_nLayer++;
	if(m_nLayer >= 16)
	{
		return false;
	}
	m_pElem[m_nLayer][0] = '\0';

	m_nStatus = STATUS_ELEM_FINISH;		//记录当前状态
	return true;
}

bool CBHXml::OutOfElem()
{
	if(m_nLayer == 0)
	{
		return false;
	}

	int nLen = (int)strlen(m_pElem[m_nLayer]);

	if (nLen)
	{
		AddStatusCompletion();
	}

	m_nLayer--;

	nLen = (int)strlen(m_pElem[m_nLayer]);
	while(nLen + m_nPos + 4> m_nBufSize)
	{
		SetBuffer(m_nBufSize * 2);
	}

	m_pBuf[m_nPos] = '<';
	m_pBuf[m_nPos + 1] = '/';
	m_nPos += 2;
	memcpy(m_pBuf + m_nPos, m_pElem[m_nLayer], nLen);
	m_nPos += nLen;
	m_pBuf[m_nPos] = '>';
	m_pBuf[m_nPos + 1] = '\n';
	m_nPos += 2;

	return true;
}

// Edit by Gengzl
int CBHXml::ConvertSpecialChar(const char* pSrc, int nSrcLen, char* pDes, int nDesLen)
{
	int nPos = 0;
	for ( int n = 0; n < nSrcLen; n++ )
	{
		if(nPos > nDesLen - 5)	
		{
			break;
		}
		if ( '&' == pSrc[n] )
		{
			pDes[nPos++] = '&';
			pDes[nPos++] = 'a';
			pDes[nPos++] = 'm';
			pDes[nPos++] = 'p';
			pDes[nPos++] = ';';
		}
		else if('\"' == pSrc[n])
		{
			pDes[nPos++] = '&';
			pDes[nPos++] = 'q';
			pDes[nPos++] = 'u';
			pDes[nPos++] = 'o';
			pDes[nPos++] = 't';
			pDes[nPos++] = ';';
		}else if ('>' == pSrc[n])
		{
			pDes[nPos++] = '&';
			pDes[nPos++] = 'g';
			pDes[nPos++] = 't';
			pDes[nPos++] = ';';
		}else if ('<' == pSrc[n])
		{
			pDes[nPos++] = '&';
			pDes[nPos++] = 'l';
			pDes[nPos++] = 't';
			pDes[nPos++] = ';';
		}else if ('\'' == pSrc[n])
		{
			pDes[nPos++] = '&';
			pDes[nPos++] = 'a';
			pDes[nPos++] = 'p';
			pDes[nPos++] = 'o';
			pDes[nPos++] = 's';
			pDes[nPos++] = ';';
		}
		else
		{
			pDes[nPos++] = pSrc[n];
		}
	}
	pDes[nPos] = '\0';

	return nPos;
}

bool CBHXml::AddAttrib(const char *pKey, const char *pValue)
{
	if(STATUS_ELEM_FINISH == m_nStatus)
	{
		return false;
	}

	int nKeyLen = (int)strlen(pKey);
	int nValueLen = (int)strlen(pValue);

	//替换特殊字符 &= &amp;and so on
	char* new_val = (char*)malloc( nValueLen + 512 );
	int nPos = ConvertSpecialChar(pValue, nValueLen, new_val, nValueLen+512);

	while(nKeyLen + nPos + 4 + m_nPos > m_nBufSize)
	{
		SetBuffer(m_nBufSize * 2);
	}

	m_pBuf[m_nPos] = ' ';
	m_nPos++;
	memcpy(m_pBuf + m_nPos, pKey, nKeyLen);
	m_nPos += nKeyLen;
	m_pBuf[m_nPos] = '=';
	m_nPos++;
	m_pBuf[m_nPos] = '\"';
	m_nPos++;
	memcpy(m_pBuf + m_nPos, new_val, nPos);
	m_nPos += nPos;
	m_pBuf[m_nPos] = '\"';
	m_nPos++;

	free( new_val );
	return true;
}

bool CBHXml::SetAttrib(const char *pKey, const char *pValue)
{
	return AddAttrib(pKey, pValue);
}

bool CBHXml::AddAttrib(const char *pKey, const int nValue)
{
	char pValue[32];

	sprintf(pValue, "%d", nValue);

	return AddAttrib(pKey, pValue);
}

bool CBHXml::SetAttrib(const char *pKey, const int nValue)
{
	return AddAttrib(pKey, nValue);
}

bool CBHXml::AddAttrib(const char *pKey, const unsigned int nValue)
{
    char pValue[32];

    sprintf(pValue, "%u", nValue);

    return AddAttrib(pKey, pValue);
}

bool CBHXml::SetAttrib(const char *pKey, const unsigned int nValue)
{
    return AddAttrib(pKey, nValue);
}

bool CBHXml::AddUint64Attrib(const char *pKey, const unsigned long long nValue)
{
    char pValue[64];

    sprintf(pValue, "%lu", nValue);

    return AddAttrib(pKey, pValue);
}

/* added by ysq at 2014-07-01 for adding block data */
// Edit by Gengzl
bool  CBHXml::AddSubDoc( const char* pData )
{
    if ( NULL == pData )
    {
        return false;
    }
    
    int nLen = (int)strlen(pData);

	AddStatusCompletion();

	while ( nLen + m_nPos > m_nBufSize )
	{
		SetBuffer( m_nBufSize * 2 );
	}

    memcpy( m_pBuf + m_nPos, pData, nLen );
    m_nPos += nLen;
    return true;
}

bool CBHXml::AddStatusCompletion()
{
	if(STATUS_ELEM_ADD != m_nStatus)
	{
		return false;
	}

	// Edit by Gengzl
	while((m_nPos + 5 + MAX_ELEM_SIZE + m_nContentLen) > m_nBufSize)
	{
		SetBuffer(2*m_nBufSize);
	}

	if(0 == m_nContentLen)
	{
		m_pBuf[m_nPos] = ' ';
		m_pBuf[m_nPos + 1] = '/';
		m_pBuf[m_nPos + 2] = '>';
		m_pBuf[m_nPos + 3] = '\n';
		m_nPos += 4;
	}
	else
	{
		m_pBuf[m_nPos] = '>';
		m_nPos += 1;

		memcpy(m_pBuf + m_nPos, m_pContent, m_nContentLen);
		m_nPos += m_nContentLen;

		m_pBuf[m_nPos] = '<';
		m_pBuf[m_nPos + 1] = '/';
		m_nPos += 2;
		int nElemLen = (int)strlen(m_pElem[m_nLayer]);
		memcpy(m_pBuf + m_nPos, m_pElem[m_nLayer], nElemLen);
		m_nPos += nElemLen;

		m_pBuf[m_nPos] = '>';
		m_pBuf[m_nPos + 1] = '\n';
		m_nPos += 2;
	}

	m_nStatus = STATUS_ELEM_FINISH;

	return true;
}

