#ifndef __YSS_COMMON_H__
#define __YSS_COMMON_H__

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <math.h>
#include <set>
#include <sstream>
#include <ext/hash_set>
#include <ext/hash_map>
#include <netinet/in.h>
#include "YssStd.h"
#include "YssTypeDef.h"
#include "YssGbkUcsConv.h"

const static double PI = acos(-1.0);
struct ST{ 
    UINT32 m_uiVal1;
    UINT32 m_uiVal2;
};

union STT{
    UINT64 m_ullValue;
    struct ST st; 
};

#define chr2bcd(c) (c>'9' ? c-'A'+10 : c-'0')

#ifndef __isleap
/* Nonzero if YEAR is a leap year (every 4 years,
except every 100th isn't, and every 400th is).  */
# define __isleap(year)	\
	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#endif

class CCommon {
public:
	static time_t GetTime(const string &strTimeStamp);
	static time_t my_mktime(struct tm *tmbuf);
	static bool IsExistProccess(char const* appName);
	static char* GetAppName(char* appPathName);
	static bool GetDateTime(string& datetime);
	static bool GetDateTime(time_t tt, string& datetime);
	static bool GetY4MD2(time_t tt, string& datetime);
	static bool GetY4MDHM2(time_t tt, string& datetime);
	static void GetY2MDHM2(time_t tt, string& datetime);
	static bool GetY4MDHMS2(time_t tt, string& datetime);
	static void GetTimeDate(UINT32& uiDate);
	static bool GetTimeDate(time_t tt, UINT32& uiDate);
	static bool GetDateTime(const string& old, int second, string& strNew);
	static bool GetDateTimeNoFormat(time_t tt, string& datetime);
	static time_t GetTimeByY4MDHMS2(const string& date);
	static int GetTimeByY4MDHMS2(const string& date, time_t &tTime);
	static time_t GetTimeByPublishDate(const string& date);
	static time_t GetTimeByPublishDate(const char* szDate);
	static void TrimDatetimeToHour(time_t & ttDatetime);
	static void TrimDatetimeToDay(time_t & ttDatetime);
	static int GetYear(time_t tt);
	static int GetMonth(time_t tt);
	static int GetDay(time_t tt);
	static int GetHour(time_t tt);
	static int GetMinute(time_t tt);
	static int GetSecond(time_t tt);
	static void GetYMonDay(time_t tt, UINT32& uiYmd);	
	static bool CreateNoExistDirectory(const string& path);
	static bool IsExistFile(const string& strFile);
	static bool IsDirectory(const string& strFile);
	static INT64 GetFileSize(const string& strFile);
	static off_t GetFileSize(const string& strFile, bool& bSuccess);
	static time_t GetFileModifyDatetime(const string& strFile);
	static bool CreateDirectory(const string& strFile);
	static bool MoveFile(const string& src, const string& dest, bool bzip);
	static bool DelFile(const string& strFile);
	static bool DelDirectory(const string& strPath);
	static bool ChangeDirectory(const string& strPath);
	static bool GetDirFile(const string& strPath, list<string>& files);
	static bool GetDirFilePath(const string& strPath, list<string>& files);
	static bool GetDirDir(const string& strPath, list<string>& dirs);
	static int GetMemUsage(int &nMByte);
	static void GetLastDirectoryName(const string& dir, string& last);
    static void GetDirectoryName(const string& path, string& dirName);
	static void CopyString(char* dest, const char* src, size_t len);
	static void CopyString(char* desc, const char* src, size_t desc_len, size_t src_len);
	//加密手机号码
	static void EncryptMsisdn(char* szMsisdn);
	//解密手机号码
	static void DecryptMsisdn(char* szMsisdn);
	static void split(const string& src, list<string>& value);
	static void split(const string& src, list<string>& value, char ch);
	static void split(const string& src, list<string>& value, char ch, UINT32 uiNumber);
	static void splitIgnoreSpace(const string& src, list<string>& value, char ch);
	static void splitIgnoreSpace(const string& src, list<string>& value, string strToken);
	static void splitIgnoreSpace(const string& src, vector<string>& value, char ch);
	static void splitIgnoreSpace(const string& src, vector<string>& value, string strToken);
	static void split(const string& src, list<pair<string, string> >& value, char ch);
	static void split(const string& src, vector<string>& value, char ch = '|');
	static void split(const string& src, vector<string>& value, char ch, UINT32 uiNumber);
	// src字符串不包含GBK汉字,且其中无空格,\t,\r,\n
	static void splitWithoutGBK(const string& src, vector<string>& value, char ch);
	static void split(const string& src, vector<string>& value, string strToken);
	static bool Pair(const string& src, pair<string, string>& value);
	static bool GetPair(const list<pair<string, string> >& values, const string& name, pair<string, string>& item);
	static bool ReadLine(FILE* fd, string& line);
	static bool ReadLine(FILE* fd, char* szBuf, UINT32 uiBufLen);
	static bool ReadData(FILE* fd, string& data, INT64 size);
	static bool IsInteger(const string& value);
	static bool IsDouble(const string& value);
	static void trimIncludeTab(string& value);
	static void trim(string& value);
	static void trim(char* value);
	static void rtrim(char* value);
	static void rtrimIncludeComma(char* value);
	static void rtrimIncludeComma(string& value);
	static bool ToWebDomain(char* szUrl);
	static int CountChar(const char* szSrc, char ch);
	static int CountChar(const string& strSrc, char ch);
	static void ReplaceAll(string& strSrc, const string& str1, const string& str2);
	static void ReplaceAll(char const* src, char* dst, size_t& dst_len, char const* from, char const* to);
	static bool LoadFile(string const& strFileName, string& strData);

	static size_t length(LONGLONG value);
	static size_t length(int value);
	static size_t length(unsigned int value);
	static size_t length(short value);
	static size_t length(unsigned short value);
	static size_t length(double value);
	static size_t length(char value);
	static size_t length(unsigned char value);
	static void toString(LONGLONG data, string& value);
	static void toString(UINT64 data, string& value);
	static void toString(int data, string& value);
	static void toString(unsigned int data, string& value);
	static void toString(short data, string& value);
	static void toString(unsigned short data, string& value);
	static void toString(double data, string& value);
	static void toString(char data, string& value);
	static void toString(unsigned char data, string& value);
	static void Utf8ToAscii(char* src, size_t src_len, char* desc, size_t& dest_len, bool bEscapeUri);
	static void BinnaryToHex(char const* src, size_t src_len, char* desc, size_t& dest_len);
	static bool IsValidGbkStr(char const* src, size_t src_len);
	static void RemoveSpace(char * str, bool bIncludeEnglish = false);
	static void RemoveDigestSpace(char * str);
	static void MarkHtmlRed(char const* src, size_t src_len, char const* words, char* dest, size_t& dest_len);
	static void SameMemCopy(char* dest, char const* src, size_t len);
	static void ToLower(char* src);
	static void ToUpper(char* src);
	static bool IsGroup(UINT32 id, UINT16 unGroup, UINT16 unGroupIndex);

	static int strgbkcasecmp(char const* src, char const* dst);
	static int strngbkcasecmp(char const* src, char const* dst, size_t n);
	static void strgbktolower(char* src);
	static void GBKToLower(char* src);
	static bool HasGbk(char const* szKeyword);

	static void RemoveTailBracket(string& str);
	static void RemoveTailBracket(char* szStr);
	static bool RemoveTailBracket(char* szStr, string& strTail);
	static bool EndWith(const string& src, const string& suffix);
	static bool EndWith(const char* szSrc, const char* szSuffix);
	static bool EndWithCase(const char* szSrc, const char* szSuffix);

	static size_t hashgbkcase(char const* s1);
	static size_t hashstrcase(char const* s1);
	static inline size_t MemoryChangeCheck(char const* pszSrc, char const* pszDst, size_t& uiLen) {
		size_t begin;
		size_t end;
		for (begin = 0; begin < uiLen; begin++) {
			if (pszSrc[begin] ^ pszDst[begin])
				break;
		}
		for (end = uiLen; end > begin; end--) {
			if (pszSrc[end - 1] ^ pszDst[end - 1])
				break;
		}
		uiLen = end - begin;
		return begin;
	}

	static bool LoadBinaryFile(string const& strFileName, char* szFileBuf, INT64& iBufLen);
	static bool IsSingleChinese(const char* szSrc);
	//change 全角到半角，替换大写到小写
	static void SBCToDBC(char* src1);
	//remove punctuation,remain only 0-9,a-z,A-Z,gbk,remove multi space to one
	static void RemovePunctuation(char* src1);
	//remove name punctuation
	static void RemoveNamePunctuation(char* src1);
	//remove punctuation and Space,remain only 0-9,a-z,A-Z,gbk
	static void RemovePunctuationAndSpace(char* src1);
	//remove ascii punctuation, include @,[,\,],^_`,{,|,},~
	static void RemoveAsciiPunctuation(char* src1, set<UINT32>& setPos);
	//remove |,\r,\n
	static void RemoveSplitAndEnter(char* src1);
	static void RemoveSplitAndEnter(string& value);
	//remain number
	static void RemainNumber(char* src1);
	//reverse a str
	static void ReverseStr(char* src1);
    
    static int DecodeMapBar(const char* poi, double& lfLon, double& lfLat) {
		if (NULL == poi)
			return -1;
		char* szSep = strchr(poi, ',');
		if (NULL == szSep)
			return -1;
        double x = strtod(poi, NULL);
		double y = strtod(szSep + 1, NULL);
		return DecodeMapBar(x, y, lfLon, lfLat);
    }
    
    static int DecodeMapBar(double lfMapBarLon, double lfMapBarLat, double& lfLon, double& lfLat) {
        #define MAPBAR_A7(x, y) (x) - cos((y) / 100000) * ((x) / 18000) + sin((x) / 100000) * ((y) / 9000)
        #define MAPBAR_WQDMW(x, y) (y) - sin((y) / 100000) * ((x) / 18000) + cos((x) / 100000) * ((y) / 9000)
		double x = ((long)(lfMapBarLon * 100000)) % 36000000;
		double y = ((long)(lfMapBarLat * 100000)) % 36000000;
		double o51_4 = x - MAPBAR_A7(x, y);
		double nmg5 = y - MAPBAR_WQDMW(x, y);
        o51_4 = x - MAPBAR_A7(o51_4, nmg5) + ((x <= 0.0) ? -1 : 1);
        nmg5 = y - MAPBAR_WQDMW(o51_4, nmg5) + ((y <= 0.0) ? -1 : 1);
        lfLon = o51_4 / 100000;
        lfLat = nmg5 / 100000;
		return 0;
    }
    
    static int EncodeMapBar(double lfLon, double lfLat, double& lfMapBarLon, double& lfMapBarLat) {
        DecodeMapBar(lfLon, lfLat, lfMapBarLon, lfMapBarLat);
        lfMapBarLon += 2 * (lfLon - lfMapBarLon);
        lfMapBarLat += 2 * (lfLat - lfMapBarLat);
        return 0;
    }
    
	static int DecodeDianPing(const char* poi, double& lfLon, double& lfLat) {
		const static int digi = 16;
		const static int add = 10;
		const static int plus = 7;
		const static int cha = 36;
		char buf[20];
		int index = -1;
		int count = 0;
		string code = "";
		int len = strlen(poi);
		int apiKey = poi[len - 1];
		len--;
		for (int i = 0; i < len; i++) {
			snprintf(buf, 20, "%c", poi[i]);
			int hash = (int) strtol(buf, NULL, cha) - add;
			if (hash >= add)
				hash = hash - plus;
			itoa(hash, buf, cha);
			code += buf;
			if (hash > count) {
				index = i;
				count = hash;
			}
		}
		if (-1 == index)
			return -1;
		int subLL = (int) strtol(code.substr(0, index).c_str(), NULL, digi);
		int addLL = (int) strtol(code.substr(index + 1, code.length() - index - 1).c_str(), NULL, digi);
		lfLon = (subLL + addLL - apiKey) / 2.0;
		lfLat = (addLL - lfLon) / 100000.0;
		lfLon /= 100000.0;
		return 0;
	}
	static int DecodeLocoso(const char* poi, double& lfLon, double& lfLat) {
		if (NULL == poi)
			return -1;
		char* szSep = strchr(poi, ',');
		if (NULL == szSep)
			return -1;
		lfLon = strtod(szSep + 1, NULL);
		lfLat = strtod(poi, NULL);
		return 0;
	}
	static void UrlToDomain(const string& strUrl, string& strDomain) {
		size_t stPos = strUrl.find("//", 0);
		if (string::npos == stPos)
			stPos = 0;
		else
			stPos += 2;

		size_t stPos1 = strUrl.find("/", stPos);
		if (string::npos == stPos1)
			stPos1 = strUrl.length();
		strDomain = strUrl.substr(stPos, stPos1 - stPos);
		if (0 == strncmp(strDomain.c_str(), "www.", strlen("www.")))
			strDomain = strDomain.substr(4, strDomain.length() - 4);
	}
	static char* itoa(int value, char* str, int radix) {
		int i, j, sign;
		char ps[256];
		memset(ps, 0, 256);
		if ((radix > 36) || (radix < 2))
			return 0;

		sign = 0;
		if (value < 0) {
			sign = -1;
			value = -value;
		}

		i = 0;
		do {
			if ((value % radix) > 9)
				ps[i] = value % radix + '0' + 39;
			else
				ps[i] = value % radix + '0';
			i++;
		} while ((value /= radix) > 0);

		if (sign < 0)
			ps[i] = '-';
		else
			i--;
		for (j = i; j >= 0; j--)
			str[i - j] = ps[j];
		return str;
	}

	static bool IsNumber(const char* szBuf);
	static bool IsAlpha(const char* szBuf);
	static bool IsSingleWord(const char* szWord);
	static double CalcDis(double lng1, double lat1, double lng2, double lat2) {
		double radLat1 = lat1 * PI / 180;
		double radLat2 = lat2 * PI / 180;
		double a = radLat1 - radLat2;
		double b = lng1 * PI / 180 - lng2 * PI / 180;
		double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
		s = s * 6378137.0;
		s = ((long long) (s * 10000 + 0.5)) * 1.0 / 10000;
		return s;
	}
	//format a string to the standard string of time.
	static bool FormatTime(const char* szDate, string &strDatetime);
	static string::size_type find(const string& str, char ch);
	static string::size_type find(const string& str, char ch, string::size_type begin);
	static string::size_type find(const string& str, const string& ch);
	static string::size_type find(const string& str, const string& ch, string::size_type begin);
	static char* StrStr(const char* psrc, const char* ptoken);
	static char* StrStr(const char* psrc, const char* ptoken, UINT32 uiTokenLen);
	static UINT32 Log8(UINT32 uiNum) {
		UINT32 uiValue = 0;
		while (uiNum) {
			uiValue++;
			uiNum = uiNum >> 3;
		}
		return uiValue;
	}
	inline static bool IsAlpha(char c){
		if('a' <= c && 'z' >= c)
			return true;
		if('A' <= c && 'Z' >= c)
			return true;
		return false;
	}

    static int LCS(const char* szA, const char* szB){
		int iA = strlen(szA);
		int iB = strlen(szB);
		if(0 == iA || 0 == iB)
			return 0;
		char* szArray = new char[iA * iB];
        int iReturn = LCS(szA, szB, iA, iB, szArray, iA * iB);
		delete[] szArray;
        return iReturn;
    }

	static int LCS(const char* szA, const char* szB, int iA, int iB, char* szArray, int iArrayLen){
		if(0 == iA || 0 == iB)
			return 0;
        if(NULL == szArray || iArrayLen < iA * iB)
            return 0;
		int iLeft, iUpper, iUpperLeft, iReturn;
		int i = 0, j = 0;
		
		for(i=0; i < iA; ){
			// A当前为汉字
			if (i+1 < iA && YS_GbkUcsConv::IsGbkCode(szA+i)) {
				for(j=0; j < iB; ){
					if (i-1 < 0 || j-1 < 0)
						iUpperLeft = 0;
					else
						iUpperLeft = szArray[(i-1)*iB+j-1];
		
					iLeft = iUpper = 0;
					if(0 <= i-1)
						iLeft = szArray[(i-1)*iB+j];
					else
						iLeft = 0;
					if(0 <= j-1)
						iUpper = szArray[i*iB+j-1];
					else
						iUpper = 0;
		
					// B当前为汉字
					if (j+1 < iB && YS_GbkUcsConv::IsGbkCode(szB+j)) {
						// 两个汉字相等
						if (szA[i] == szB[j] && szA[i+1] == szB[j+1]) {
							szArray[i*iB+j] = iUpperLeft + 2;
							szArray[(i+1)*iB+j+1] = szArray[i*iB+j];
							szArray[(i+1)*iB+j] = szArray[i*iB+j];
							szArray[i*iB+j+1] = szArray[i*iB+j];
						}
						// 两个汉字不相等
						else {
							szArray[i*iB+j] = (iLeft > iUpper)?iLeft:iUpper;
							szArray[(i+1)*iB+j+1] = szArray[i*iB+j];
							szArray[(i+1)*iB+j] = szArray[i*iB+j];
							szArray[i*iB+j+1] = szArray[i*iB+j];
						}
		
						j += 2;
					}
					// B当前不是汉字,二者肯定不等
					else {
						szArray[i*iB+j] = (iLeft > iUpper)?iLeft:iUpper;
						szArray[(i+1)*iB+j] = (iLeft > iUpper)?iLeft:iUpper;
		
						j++;
					}
				}
		
				i += 2;
			}
			// A当前不是汉字
			else {
				for(j=0; j < iB; ){
					iLeft = iUpper = 0;
					if(0 <= i-1)
						iLeft = szArray[(i-1)*iB+j];
					else
						iLeft = 0;
					if(0 <= j-1)
						iUpper = szArray[i*iB+j-1];
					else
						iUpper = 0;
		
					// B当前为汉字,二者肯定不等
					if (j+1 < iB && YS_GbkUcsConv::IsGbkCode(szB+j)) {
						szArray[i*iB+j] = (iLeft > iUpper)?iLeft:iUpper;
						szArray[i*iB+j+1] = szArray[i*iB+j];
		
						j += 2;
					}
					// B当前不是汉字
					else {
						// 不区分大小写
						if (szA[i] == szB[j]
							|| CCommon::IsAlpha(szA[i]) && CCommon::IsAlpha(szB[j]) && (szA[i]+32 == szB[j] || szA[i] == szB[j]+32)){
							if(0 <= i-1 && 0 <= j-1)
								szArray[i*iB+j] = szArray[(i-1)*iB+j-1] + 1;
							else
								szArray[i*iB+j] = 1;
						}
						else {
							szArray[i*iB+j] = (iLeft > iUpper)?iLeft:iUpper;
						}
						j++;
					}
				} // for B
		
				i++;
			} // else A当前不是汉字
		}
		
		iReturn = szArray[(iA-1)*iB + iB-1];
		return iReturn;
	}

	static int LCSWithoutGBK(const char* szA, const char* szB){
        int iA = strlen(szA);
        int iB = strlen(szB);
        if(0 == iA || 0 == iB)
                return 0;
        int iLeft, iUpper, iReturn;
        char* szArray = new char[iA * iB];
        for(int i=0; i < iA; i++){
                for(int j=0; j < iB; j++){
                        if(szA[i] == szB[j]){
							if(0 <= i-1 && 0 <= j-1)
								szArray[i*iB+j] = szArray[(i-1)*iB+j-1] + 1;
							else
								szArray[i*iB+j] = 1;
						}else if(IsAlpha(szA[i]) && IsAlpha(szB[j]) && (szA[i]+32 == szB[j] || szA[i] == szB[j]+32)){
							if(0 <= i-1 && 0 <= j-1)
								szArray[i*iB+j] = szArray[(i-1)*iB+j-1] + 1;
							else
								szArray[i*iB+j] = 1;
						}else{
							iLeft = iUpper = 0;
							if(0 <= i-1)
								iLeft = szArray[(i-1)*iB+j];
							else
								iLeft = 0;
							if(0 <= j-1)
								iUpper = szArray[i*iB+j-1];
							else
								iUpper = 0;
							szArray[i*iB+j] = (iLeft > iUpper)?iLeft:iUpper;
                        }
                }
        }
        iReturn = szArray[(iA-1)*iB + iB-1];
        delete[] szArray;
        return iReturn;
	}
	static void ProcessSqlData(char* szBuf){
		if(NULL == szBuf || '\0' == szBuf[0])
			return;
		int i=0;
		while(szBuf[i]){
			if('\'' == szBuf[i])
				szBuf[i] = '\"';
			i++;
		}
	}

	static void ReplaceSpaceToComma(char* szBuf){
		if(NULL == szBuf || '\0' == szBuf[0])
			return;
		int i = 0;
		while(szBuf[i]){
			if(' ' == szBuf[i])
				szBuf[i] = ',';
			i++;
		}
	}

	static UINT64 _ConvertDecimalToBCD(UINT64 ullValue){
		UINT64 ullResult = 0, ullResidue;
		UINT64 ullFactor = 1;
		while(0 != ullValue){
			ullResidue = ullValue % 10; 
			ullResult += ullFactor * ullResidue;
			ullFactor *= 16; 
			ullValue /= 10; 
		}   
		return ullResult;
	}

	static UINT64 _ConvertBCDToDecimal(UINT64 ullValue){
		UINT64 ullResult = 0, ullResidue;
		UINT64 ullFactor = 1;
		while(0 != ullValue){
			ullResidue = ullValue & (0xF);
			if(0xF == ullResidue)
				break;
			ullResult += ullFactor * ullResidue;
			ullFactor *= 10; 
			ullValue >>= 4;
		}   
		return ullResult;
	}

	static UINT64 ConvertDecimalToBCD(UINT64 ullValue){
		char szMsisdn[18];
		snprintf(szMsisdn, 18, "%llu", ullValue);
		return ConvertDecimalToBCD(szMsisdn);
	}	

	static UINT64 ConvertDecimalToBCD(const char* szMsisdn){
		UINT64 ullResult = 0xFFFFFFFFFFFFFFFF;
        char* p = (char*)(&ullResult);
        const char* nbr = szMsisdn;

        for(int len=0; len<8; len++,p++)
        {
            if (*nbr)
            {
                *p=chr2bcd(*nbr)|0xf0;
                nbr++;
            }else
                break;
            if (*nbr)
            {
                *p = (*p&0x0f)|(chr2bcd(*nbr)<<4);
                nbr++;
            }
            else
                break;
        }
                        
		return ullResult;
	}

	static UINT64 _ConvertDecimalToBCD(const char* szMsisdn){
		UINT64 ullResult = 0xFFFFFFFFFFFFFFFF;
		int i = 0;
		while('\0' != *szMsisdn && i < 16){
			ullResult = (ullResult << 4) + ((*szMsisdn) - '0');
			szMsisdn++;
			i++;
		}
		return ullResult;
	}

	static void ConvertBCDToDecimal(UINT64 ullValue, string& strMsisdn){
		char szMsisdn[17];
        char* p = (char*)(&ullValue);
        int i;
        char *buf = szMsisdn;
        for(i=8; i>0; i--){
            *(buf)=(*p & 0xf) + '0';
            if( *(buf)== 0x3f )
                break;
            buf++;
            *(buf)=((*(p++)>>4) & 0xf) + '0';
            if( *(buf)==0x3f )
                break;
            buf++;
        };
        *buf='\0';
        strMsisdn = szMsisdn;
	}

	static void _ConvertBCDToDecimal(UINT64 ullValue, string& strMsisdn){
		char szMsisdn[17];
		int i = 0;
		UINT64 ullResidue;
		while(0 != ullValue && i < 16){
			ullResidue = (ullValue & (0xF));
			if(0xF == ullResidue)
				break;
			szMsisdn[i] = '0' + ullResidue;
			i++;
			ullValue >>= 4;
		}
		szMsisdn[i] = '\0';
		ReverseStr(szMsisdn);
		strMsisdn = szMsisdn;
	}

	static UINT64 htonll(UINT64 ullValue){
        union {
            UINT16 m_unValue;
            char m_sValue[sizeof(UINT16)];
        } check;
        check.m_unValue = 0x0102;
        if(1 == check.m_sValue[0] && 2 == check.m_sValue[1]) { //本机序即网络序
            return ullValue;
        } else {
            union STT stt1, stt2;
            stt1.m_ullValue = ullValue;
            stt2.st.m_uiVal1 = htonl(stt1.st.m_uiVal2);
            stt2.st.m_uiVal2 = htonl(stt1.st.m_uiVal1);
            return stt2.m_ullValue;
        }
	}

	static UINT64 ntohll(UINT64 ullValue){
        union {
            UINT16 m_unValue;
            char m_sValue[sizeof(UINT16)];
        } check;
        check.m_unValue = 0x0102;
        if(1 == check.m_sValue[0] && 2 == check.m_sValue[1]) { //本机序即网络序
            return ullValue;
        } else {
            union STT stt1, stt2;
            stt1.m_ullValue = ullValue;
            stt2.st.m_uiVal1 = ntohl(stt1.st.m_uiVal2);
            stt2.st.m_uiVal2 = ntohl(stt1.st.m_uiVal1);
            return stt2.m_ullValue;
        }
	}
    //黑龙江信令适配
    //网络数据需要转小端字节序(LE)，而不是网络字节序
    static UINT16 htonsLE(UINT16 unValue){
        //由于网络字节序是大端字节序，所以先本机到网络字节序，再颠倒顺序
        union {
            UINT16 m_unValue;
            char m_sValue[sizeof(UINT16)];
        } check;
        check.m_unValue = htons(unValue);
        ChangeOrder(check.m_sValue, sizeof(UINT16));
        return check.m_unValue;
    }
    static UINT16 ntohsLE(UINT16 unValue){
        //由于网络字节序是大端字节序，所以先颠倒顺序，然后再网络到本机字节序
        union {
            UINT16 m_unValue;
            char m_sValue[sizeof(UINT16)];
        } check;
        check.m_unValue = unValue;
        ChangeOrder(check.m_sValue, sizeof(UINT16));
        return ntohs(check.m_unValue);
	}
    static UINT32 htonlLE(UINT32 uiValue){
        //由于网络字节序是大端字节序，所以先本机到网络字节序，再颠倒顺序
        union {
            UINT32 m_uiValue;
            char m_sValue[sizeof(UINT32)];
        } check;
        check.m_uiValue = htonl(uiValue);
        ChangeOrder(check.m_sValue, sizeof(UINT32));
        return check.m_uiValue;
    }
	static UINT32 ntohlLE(UINT32 uiValue){
        //由于网络字节序是大端字节序，所以先颠倒顺序，然后再网络到本机字节序
        union {
            UINT32 m_uiValue;
            char m_sValue[sizeof(UINT32)];
        } check;
        check.m_uiValue = uiValue;
        ChangeOrder(check.m_sValue, sizeof(UINT32));
        return ntohl(check.m_uiValue);
	}
    //输入一个字符串buf，将他们的首尾全部颠倒，例如1234 => 4321
    static void ChangeOrder(char* szBuf, UINT32 uiLen) {
        char t = '\0';
        for(UINT32 i = 0; i < (uiLen / 2); i++) {
            t = szBuf[i];
            szBuf[i] = szBuf[uiLen - i - 1];
            szBuf[uiLen - i - 1] = t;
        }
    }
    //小端字节序结束
};
struct eqgbk {
	//大小写敏感
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) == 0;
	}
};
struct eqstr {
	//忽略大小写
	size_t operator()(const char* s1) const {
		return CCommon::hashstrcase(s1);
	}
	bool operator()(const char* s1, const char* s2) const {
		return strcasecmp(s1, s2) == 0;
	}
};

struct cmpgbkstr {
	//忽略大小写
	bool operator()(const char* s1, const char* s2) const {
		return (0 > CCommon::strgbkcasecmp(s1, s2) );
	}
};

struct eqgbkstr {
	//忽略大小写及GBK
	size_t operator()(const char* s1) const {
		return CCommon::hashgbkcase(s1);
	}
	bool operator()(const char* s1, const char* s2) const {
		return CCommon::strgbkcasecmp(s1, s2) == 0;
	}
};

struct findgbkstr {
	findgbkstr(const char* szValue){
		m_szValue = szValue;
	}
	//忽略大小写及GBK
	bool operator()(const char* s1) const {
		return CCommon::strgbkcasecmp(s1, m_szValue) == 0;
	}
	const char* m_szValue;
};

struct eqstring {
	//忽略大小写及GBK
	size_t operator()(const string & s1) const {
		return CCommon::hashgbkcase(s1.c_str());
	}
	bool operator()(const string & s1, const string & s2) const {
		return CCommon::strgbkcasecmp(s1.c_str(), s2.c_str()) == 0;
	}
};

struct equll {
    size_t operator()(const UINT64 ullKey) const {
        return ullKey;
    }
    bool operator()(const UINT64 ullKey1, const UINT64 ullKey2) const {
        return ullKey1 == ullKey2;
    }
};

typedef hash_set<string, eqstring, eqstring> HASH_SET_STRING;
typedef hash_set<const char*, eqgbkstr, eqgbkstr> HASH_SET_GEKSTR;
typedef hash_map<string, UINT32, eqstring, eqstring> HASH_MAP_STRING;
typedef hash_map<const char*, UINT32, eqgbkstr, eqgbkstr> HASH_MAP_GEKSTR;

struct cmpstr {
	//忽略大小写
	bool operator()(const char* s1, const char* s2) const {
		return ( 0 > strcasecmp(s1, s2) );
	}
};

struct cmpStr {
	//忽略大小写
	bool operator()(const string &str1, const string &str2) const {
		return (0 > strcasecmp(str1.c_str(), str2.c_str()));
	}
};
typedef map<const char*, UINT32, cmpstr> MAP_STR;

struct eqsimplestr {
	//大小写敏感
	size_t operator()(const char* s1) const {
		return __stl_hash_string(s1);
	}
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) == 0;
	}
};

struct eqSimpleString {
	//大小写敏感
	size_t operator()(const string& str) const {
		return __stl_hash_string(str.c_str());
	}
	bool operator()(const string& str1, const string& str2) const {
		return strcmp(str1.c_str(), str2.c_str()) == 0;
	}
};

typedef hash_set<const char*, eqsimplestr, eqsimplestr> HASH_SET_SIMPLESTR;

#endif

