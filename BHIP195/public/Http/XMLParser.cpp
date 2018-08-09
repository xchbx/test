#include "XMLParser.h"


CXMLParser::CXMLParser()
{

}

CXMLParser::~CXMLParser()
{

}

void CXMLParser::ParseXML(const string& strXML)
{
	return;
}

void CXMLParser::RegParseInterface()
{
	return;
}

void CXMLParser::SetCommStatus(bool bConn)
{
	return;
}

void CXMLParser::SetMsgFromAddr(const string& strFromIP, int nFromPort)
{
	m_strMsgFromIP = strFromIP;
	m_nMsgFromPort = nFromPort;
}

