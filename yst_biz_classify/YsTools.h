#ifndef _YS_TOOLS_H_
#define _YS_TOOLS_H_

#include<ext/hash_map>
using namespace __gnu_cxx;



#include "logglobal.h"
#include "YssCommon.h"
#include "YssCommonStructDef.h"
#include <signal.h>
#include <cstdlib>
#include <string>
#include <set>
#include <list>
using namespace std;

#define SPLIT_CHAR '$'
#define MAX_BUFFER_LEN 2048
#define BUFFER_LEN 128
#define MIN_CHAR_LEN 2

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
typedef hash_map<string,double,strHashFunc>::value_type  ProportionPair;
#define TMP_BUFFER_LEN 128

struct YS_ConfigData
{
	string m_strSourcePath;   // data source
	string m_strTrainPath;    // training data source
	string m_strTestPath;    //  test data source
	string m_strWhiteList;   //  white list
	string m_strCollfictList;  //collfict
	//string m_strWeightPath;
	string m_strDictPath;//
	string m_strGoodDataPath;//
	string m_strBadDataPath;//
	string m_strBakPath;//
    hash_map<string,double,strHashFunc> m_mapAttrWordProportion;
    string m_strWordSelectedFilePath;
    bool m_bIfTrain;
    string m_strClassProbabilityPath;
    string m_strWordProbabilityPath;
    string m_strAttributeWordListPath;
    string m_strAttributePrecisionPath;
    string m_strAttributeWordNumPath;
	string m_strFeatureScore;
	int m_logLevel;
	bool m_bIfWordSeg;
	string m_attributeKey;
	list<string> m_attributeTitle;
	static YS_ConfigData* m_pInstance;
	
	YS_ConfigData();
	~YS_ConfigData(){};
	bool StartUp();
	void printInfo();
	static YS_ConfigData* GetInstance();
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
	static YS_WordSegmentAgent* GetInstance();
    void ExtractWordSegment(const char* szKeyword, list<YSS_Word>& lstWord);
    void DestroyWordSegment(list<YSS_Word>& lstWord);	
private:
	YS_WordSegmentAgent();
	YS_WordSegmentAgent(const YS_WordSegmentAgent&);
	YS_WordSegmentAgent& operator=(const YS_WordSegmentAgent&);
private:
	YS_WordSegmentLR* m_wordSegmentLR;
	static YS_WordSegmentAgent* m_instance;
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

    class TOOLS
    {
        public:
            static bool reverseWord(const string& src, string& des);
            static bool partialWord(const string& src, list<string>& des);
            static int IsTextUTF8(unsigned char chr);
            static bool IsGbkCode(char const* src);
        private:
            static const int BUF_LEN = 128;
    };
	
typedef void SigFunc(int);
SigFunc* mysignal(int signo, SigFunc* func);

#endif

