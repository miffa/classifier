#ifndef _YS_TOOLS_H_
#define _YS_TOOLS_H_

#include<ext/hash_map>
using namespace __gnu_cxx;


#include "YssCommon.h"
#include "YssCommonStructDef.h"
#include <signal.h>
#include<cstdlib>
#include <string>
#include <set>
#include <list>
#include <iconv.h>
#include <iostream>
using namespace std;

#define SPLIT_CHAR '$'
#define MAX_BUFFER_LEN 2048
#define BUFFER_LEN 128
#define MIN_CHAR_LEN 2
#define OUTLEN 1024*128

struct strHashFunc
{
    size_t operator()(const string& str) const
    {
        return __stl_hash_string(str.c_str());
    }
};

class YS_FileProcessor
{
public:
	YS_FileProcessor(const char* filename);
	~YS_FileProcessor();
	
	bool OpenForWrite();
	bool OpenForAppend();
	bool OpenForRead();
	bool CloseFile();
	
	bool SeekByIndex(long index);
	int ReadInt();
	long ReadLong();
	bool ReadByteArray(void* dest, long size);
	bool ReadData(void* dest, long size);
	bool ReadCharArray(char* dest, long size);
	bool ReadLine(char* dest, long size);
	bool WriteLine(const char* buff);
	bool WriteBin(const void* buff, long size);
	
	//将java long 转换为C long   只适合X86_64
	long GetLongFromJavaToC(long data);
	//将java int 转换为C int
	int GetIntFromJavaToC(int data);
	
	long SetFileLen();
	long GetFileLen(){ return m_lFileLen; };
	bool IsEnd();
	int GetFileFd();
	
private:
	string m_strFileName;
	FILE* m_filePtr;
	long m_lFileLen;
	static const unsigned int ONE_TIME = 1;
	static const unsigned int JAVA_INT_BIT = 4;
	static const unsigned int JAVA_LONG_BIT = 8;
	
};

class Ys_BadCharacter
{
private:
	static string m_strBadChar;
	static string m_strBadChineseChar;
	static Ys_BadCharacter* m_instance;
	HASH_SET_STRING m_hashBadCharSet;
public:
	static Ys_BadCharacter* GetInstance();
	bool StartUp();
	bool findBadChar(const char*);
	bool findBadChar(const string&);
	bool earseInvalidChar(string& str);
};

class YS_WordSegmentLR;
class YS_WordSegmentAgent
{
public:
	~YS_WordSegmentAgent();
	bool StartUp();
	void SetDictPath(const string& path){ m_strDicPath=path;};
	static YS_WordSegmentAgent* GetInstance();
    void ExtractWordSegment(const char* szKeyword, list<YSS_Word>& lstWord);
    void DestroyWordSegment(list<YSS_Word>& lstWord);	
private:
	YS_WordSegmentAgent();
	YS_WordSegmentAgent(const YS_WordSegmentAgent&);
	YS_WordSegmentAgent& operator=(const YS_WordSegmentAgent&);
private:
	YS_WordSegmentLR* m_wordSegmentLR;
	string m_strDicPath;
	static YS_WordSegmentAgent* m_instance;
};

class YS_CodeConverter
{
public:
	//
	YS_CodeConverter(const char *from_charset,const char *to_charset);
	//
	~YS_CodeConverter();
	//
	int Convert(char *inbuf,int inlen,char *outbuf,int& outlen);
private:
	iconv_t m_codeConvertItem;
};
//#define ERR_LOG(fmt, arg...) do{ fprintf(stderr,"ERROR: %s:%d "fmt, __FILE__, __LINE__, ##arg); fflush(stderr); }while(0)
/*
#ifdef _DEBUG
	#define INF_LOG(fmt, arg...) do{ fprintf(stdout,"INFO: %s:%d "fmt, __FILE__, __LINE__, ##arg); fflush(stdout); }while(0)
#else
	#define INF_LOG(fmt, arg...)
#endif
*/

#define ERR_LOG(fmt, arg...) logError("%s:%d "fmt, __FILE__, __LINE__, ##arg)
#define INF_LOG(fmt, arg...) logInfo3("%s:%d "fmt, __FILE__, __LINE__, ##arg)
#define INF_LOG2(fmt, arg...) logInfo2("%s:%d "fmt, __FILE__, __LINE__, ##arg)
#define INF_LOG3(fmt, arg...) logInfo("%s:%d "fmt, __FILE__, __LINE__, ##arg) 

#ifdef _DEBUG
	#define TAP_ENTER //getchar()
	#define DEBUG_LOG(fmt, arg...) logInfo3("%s:%d "fmt, __FILE__, __LINE__, ##arg)
	#define PRINT(fmt, arg...)  printf(fmt, ##arg)
#else
	#define TAP_ENTER
	#define DEBUG_LOG
	#define PRINT
#endif

typedef void SigFunc(int);
SigFunc* mysignal(int signo, SigFunc* func);
bool InitSignal();
void onSigUsr2(int);
void onSigChld(int);
void onSigPipe(int);
#endif

