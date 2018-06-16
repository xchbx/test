#include <windows.h>
#include "Markup.h"
#include <stdio.h>
#include <string>
using namespace std;

void CreateXML()
{
	CMarkup xml1;
	CMarkup xmlTestResults;
	xmlTestResults.AddElem( ("Test"));

	xml1.AddElem(( "ORDER"));
	xml1.IntoElem();
	xml1.AddElem( "ITEM");
	xml1.IntoElem();
	xml1.AddElem( "NAME",  "crank casing");
	xml1.AddElem( "SN",  "132487A-J");
	xml1.AddElem( "QTY",  "1");
	xml1.Save( "./test.xml");
	string strXML = xml1.GetDoc();
}

void readXML()
{
	CMarkup xml2;
	xml2.Load( "./test.xml");   
	xml2.FindElem("ORDER");  
	xml2.IntoElem(); 
	while (xml2.FindElem( "ITEM"))
	{
		xml2.IntoElem();

		xml2.FindElem( "SN");
		string strSN = xml2.GetData();
		printf("%s\n", strSN.c_str());
		xml2.ResetMainPos();  

		xml2.FindElem( "QTY");
		printf("QTY = %d\n", atoi((const char *)MCD_2PCSZ(xml2.GetData())));

		xml2.OutOfElem();
	}
}

bool Change()
{
	CMarkup xml2;
	xml2.Load("./test.xml");
	xml2.ResetMainPos();//把xml对象指向的位置初始化，使其指向文件开始
	if (!xml2.FindElem("ORDER"))
		return false;

	xml2.IntoElem();

	if (!xml2.FindElem("ITEM"))
		return false;

	xml2.IntoElem();
	
	while (xml2.FindElem("SN"))
	{
		string test111 = xml2.GetData();

		if (test111 == "132487A-J")
		{
			xml2.SetData("1");
			printf("run\n");
			xml2.Save("./test.xml");
			break;
		}
	}
	return true;
}

int main()
{
	CreateXML();
	readXML();
	Change();
	readXML();
	return 0;
}
