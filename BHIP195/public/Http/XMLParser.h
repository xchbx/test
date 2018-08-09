#ifndef _XML_PARSER_H_
#define _XML_PARSER_H_

#include <string>
#include <map>
#include <time.h>

using namespace std;



extern int g_iDiffSysTime;	//校时服务器与本地时钟的偏差
#ifndef XML_HEAD_SUCCESS
#define XML_HEAD_SUCCESS(Type, MsgID)\
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"GB2312\" ?>\r\n");\
	xml.AddElem("Msg");\
	xml.SetAttrib("Version", 1);\
	xml.SetAttrib("MsgID", MsgID);\
	xml.SetAttrib("ReplayID", MsgID);\
	xml.IntoElem();\
	xml.AddElem("Type", Type);
#endif

#ifndef XML_END_SUCCESS
#define XML_END_SUCCESS()\
    xml.AddElem("Status", "Success");\
	xml.OutOfElem();
#endif


#ifndef XML_HEAD_FAILED
#define XML_HEAD_FAILED(Type, MsgID, ErrorCode, Comment)\
    xml.SetDoc("<?xml version=\"1.0\" encoding=\"GB2312\" ?>\r\n");\
    xml.AddElem("Msg");\
    xml.SetAttrib("Version", 1);\
    xml.SetAttrib("MsgID", MsgID);\
    xml.SetAttrib("ReplayID", MsgID);\
    xml.IntoElem();\
    xml.AddElem("Type", Type);\
    xml.AddElem("Status", "Failure");\
    xml.AddElem("ErrorCode", ErrorCode);\
    xml.AddElem("Comment", Comment);
#endif

#ifndef XML_END_FAILED
#define XML_END_FAILED()\
    xml.OutOfElem();
#endif


class CXMLParser
{
public:
	CXMLParser();
	virtual ~CXMLParser();

public:
	virtual void ParseXML(const string& strXML);
	virtual void RegParseInterface();
	virtual void SetCommStatus(bool bConn);
	void SetMsgFromAddr(const string& strFromIP, int nFromPort);

protected:
	string	m_strMsgFromIP;
	int m_nMsgFromPort;
};

//<Code, parser*>
typedef map<string, CXMLParser*>	XMLParserMap;

#endif

