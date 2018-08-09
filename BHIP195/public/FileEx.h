/*******************************************************************************
* 版权所有 (C) 2008
* 
* 文件名称： FileEx.h
* 文件标识： 
* 内容摘要： 文件操作辅助类。
* 其它说明： 本类提供的操作在linux下依然有效。
* 当前版本： V1.0
* 作    者： 周锋
* 完成日期： 2008-01-04
*******************************************************************************/
#ifndef _FILE_EX_75894328849318493216789054320573409
#define _FILE_EX_75894328849318493216789054320573409

#include <vector>
#include <string>

#if (!defined _WINDOWS_) && (!defined _WIN32)
#include <unistd.h>
#include <ftw.h>
#include <sys/vfs.h>

typedef unsigned int UINT;
#endif

class CFileEx
{
public:
	//获取文件分隔符（对于windows返回'\\'，linux返回'/'）
	static char Separator();

	//获取当前目录(返回值不包括末尾的“\\”或“/”)
	static std::string GetCurDirectory();

	//获取可执行程序所在目录(返回值不包括末尾的“\\”或“/”)
	static std::string GetExeDirectory();

	//设置当前工作目录
	static bool SetCurDirectory(const char * lpszFolder);

	//创建指定的多级文件目录
	static bool CreateFolder(const char * lpszFolder);
	
	//不考虑当前路径创建多级目录
	static bool CreateFolderEx(const char * lpszFolder);
	//为创建指定的文件创建必要的文件目录
	static bool CreateFolderForFile(const char * lpszFile);

	//根据全路径获取文件名
	static std::string Path2FileName(const char *lpszPath);

	//获取磁盘剩余空间
	static unsigned int GetFreeDiskSpace(const char* lpszPath);

	//删除指定文件夹（包括文件夹里面的所有内容）
	static bool DelFolder(const char* lpszFolder);

private:
	CFileEx(void){}
	~CFileEx(void){}

#if (!defined _WINDOWS_) && (!defined _WIN32)
	static int m_snDepth;	//目录层次
	static bool m_sbFile;	//TRUE for file and FALSE for folder
	static std::vector<std::string> m_svecFile;
	static int FileFunc(const char *file, const struct stat *sb, int flag);
#endif

};

#endif
