#include "YssCommon.h"
#include "YssGbkUcsConv.h"

static time_t tt2000YearTimeArray[] = 
{
    946656000,
    978278400,
    1009814400,
    1041350400,
    1072886400,
    1104508800,
    1136044800,
    1167580800,
    1199116800,
    1230739200,
    1262275200,
    1293811200,
    1325347200,
    1356969600,
    1388505600,
    1420041600,
    1451577600,
    1483200000,
    1514736000,
    1546272000,
    1577808000,
    1609430400,
    1640966400,
    1672502400,
    1704038400,
    1735660800,
    1767196800,
    1798732800,
    1830268800,
    1861891200,
    1893427200,
    1924963200,
    1956499200,
    1988121600,
    2019657600,
    2051193600,
    2082729600,
    2114352000,
    2145888000,
    2177424000,
    2208960000,
    2240582400,
    2272118400,
    2303654400,
    2335190400,
    2366812800,
    2398348800,
    2429884800,
    2461420800,
    2493043200,
    2524579200,
    2556115200,
    2587651200,
    2619273600,
    2650809600,
    2682345600,
    2713881600,
    2745504000,
    2777040000,
    2808576000,
    2840112000,
    2871734400,
    2903270400,
    2934806400,
    2966342400,
    2997964800,
    3029500800,
    3061036800,
    3092572800,
    3124195200,
    3155731200,
    3187267200,
    3218803200,
    3250425600,
    3281961600,
    3313497600,
    3345033600,
    3376656000,
    3408192000,
    3439728000,
    3471264000,
    3502886400,
    3534422400,
    3565958400,
    3597494400,
    3629116800,
    3660652800,
    3692188800,
    3723724800,
    3755347200,
    3786883200,
    3818419200,
    3849955200,
    3881577600,
    3913113600,
    3944649600,
    3976185600,
    4007808000,
    4039344000,
    4070880000
};

/* How many days come before each month (0-12).  */
const UINT16 unMonDaysArray[2][13] =
{
	/* Normal years.  */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* Leap years.  */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

static UINT8 ucEncryptMsisdnArray[10][10] = 
{
	{4, 3, 1, 5, 9, 8, 0, 2, 6, 7},
	{6, 2, 0, 7, 5, 4, 8, 9, 1, 3},
	{3, 1, 4, 7, 8, 9, 6, 5, 2, 0},
	{3, 0, 2, 6, 5, 8, 4, 1, 9, 7},
	{9, 4, 5, 2, 7, 0, 3, 1, 8, 6},
	{8, 4, 0, 2, 6, 9, 1, 7, 3, 5},
	{2, 1, 4, 9, 3, 5, 6, 8, 0, 7},
	{3, 0, 2, 6, 4, 7, 1, 8, 9, 5},
	{5, 0, 4, 2, 8, 9, 7, 1, 6, 3},
	{2, 7, 9, 6, 5, 0, 8, 4, 1, 3}
};

static UINT8 ucDecryptMsisdnArray[10][10] = 
{
	{6, 2, 7, 1, 0, 3, 8, 9, 5, 4},
	{2, 8, 1, 9, 5, 4, 0, 3, 6, 7},
	{9, 1, 8, 0, 2, 7, 6, 3, 4, 5},
	{1, 7, 2, 0, 6, 4, 3, 9, 5, 8},
	{5, 7, 3, 6, 1, 2, 9, 4, 8, 0},
	{2, 6, 3, 8, 1, 9, 4, 7, 0, 5},
	{8, 1, 0, 4, 2, 5, 6, 9, 7, 3},
	{1, 6, 2, 0, 4, 9, 3, 5, 7, 8},
	{1, 7, 3, 9, 2, 0, 8, 6, 4, 5},
	{5, 8, 0, 9, 7, 4, 3, 1, 6, 2}
};

bool CCommon::IsExistProccess(char const* appName) {
	char szCmd[512];
	bool bExist;
	sprintf(szCmd, "ps -e|grep \"[ /]%s\"|grep -v %d |grep -v \\<defunct\\> >/tmp/%s.pid", appName, getpid(), appName);
	system(szCmd);
	sprintf(szCmd, "/tmp/%s.pid", appName);
	if (CCommon::GetFileSize(szCmd) > 0) {
		bExist = true;
	} else {
		bExist = false;
	}
	CCommon::DelFile(szCmd);
	return bExist;
}
char* CCommon::GetAppName(char* appPathName) {
	if (0 != appPathName) {
		int pos;
		for (pos = strlen(appPathName); pos > 0; pos--) {
			if (appPathName[pos - 1] == '/')
				break;
		}
		return appPathName + pos;
	}
	return NULL;
}

bool CCommon::GetDateTime(string& datetime) {
	char buf[20];
	struct tm tmNow;
	time_t tt = time(NULL);
	datetime.clear();
	localtime_r(&tt, &tmNow);

	sprintf(buf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
	datetime = buf;
	return true;
}

bool CCommon::GetDateTime(time_t tt, string& datetime) {
	char buf[20];
	struct tm tmNow;
	datetime.clear();
	localtime_r(&tt, &tmNow);

	sprintf(buf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
	datetime = buf;
	return true;
}

// 根据time_t获取YYYYMMDD
bool CCommon::GetY4MD2(time_t tt, string& datetime) {
	char buf[20];
	struct tm tmNow;
	datetime.clear();
	localtime_r(&tt, &tmNow);

	sprintf(buf, "%.4d%.2d%.2d", tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday);
	datetime = buf;
	return true;
}

// 根据time_t获取YYYYMMDDHHMM
bool CCommon::GetY4MDHM2(time_t tt, string& datetime) {
	char buf[20];
	struct tm tmNow;
	datetime.clear();
	localtime_r(&tt, &tmNow);

	sprintf(buf, "%.4d%.2d%.2d%.2d%.2d", tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min);
	datetime = buf;
	return true;
}

// 根据time_t获取YYMMDDHHMM
void CCommon::GetY2MDHM2(time_t tt, string& datetime) {
	char buf[20];
	struct tm tmNow;
	datetime.clear();
	localtime_r(&tt, &tmNow);

	sprintf(buf, "%.4d%.2d%.2d%.2d%.2d", tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min);
	datetime = buf + 2;
}

// 根据time_t获取YYYYMMDDHHMMSS
bool CCommon::GetY4MDHMS2(time_t tt, string& datetime) {
	char buf[20];
	struct tm tmNow;
	datetime.clear();
	localtime_r(&tt, &tmNow);

	sprintf(buf, "%.4d%.2d%.2d%.2d%.2d%.2d", tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
	datetime = buf;
	return true;
}

void CCommon::GetTimeDate(UINT32& uiDate) {
	time_t tt = time(NULL);
	GetTimeDate(tt, uiDate);
}

bool CCommon::GetTimeDate(time_t tt, UINT32& uiDate) {
	struct tm tmNow;
	localtime_r(&tt, &tmNow);
	uiDate = (tmNow.tm_year + 1900) * 10000 + (tmNow.tm_mon + 1) * 100 + tmNow.tm_mday;
	return true;
}

bool CCommon::GetDateTime(const string& old, int second, string& strNew) {
	return GetDateTimeNoFormat(GetTimeByY4MDHMS2(old) + second, strNew);
}

time_t CCommon::GetTimeByY4MDHMS2(const string& date) {
	if (14 != date.length())
		return 0;
	struct tm tmDate;
	memset(&tmDate, 0x00, sizeof(tmDate));
	tmDate.tm_year = atoi(date.substr(0, 4).c_str()) - 1900;
	tmDate.tm_mon = atoi(date.substr(4, 2).c_str()) - 1;
	tmDate.tm_mday = atoi(date.substr(6, 2).c_str());
	tmDate.tm_hour = atoi(date.substr(8, 2).c_str());
	tmDate.tm_min = atoi(date.substr(10, 2).c_str());
	tmDate.tm_sec = atoi(date.substr(12, 2).c_str());
	tmDate.tm_isdst = 0;
	return my_mktime(&tmDate);
}

int CCommon::GetTimeByY4MDHMS2(const string& date, time_t &tTime) {
	if (14 != date.length())
		return -1;
	struct tm tmDate;
	tmDate.tm_year = atoi(date.substr(0, 4).c_str()) - 1900;
	tmDate.tm_mon = atoi(date.substr(4, 2).c_str()) - 1;
	tmDate.tm_mday = atoi(date.substr(6, 2).c_str());
	tmDate.tm_hour = atoi(date.substr(8, 2).c_str());
	tmDate.tm_min = atoi(date.substr(10, 2).c_str());
	tmDate.tm_sec = atoi(date.substr(12, 2).c_str());
	tmDate.tm_isdst = 0;
	tTime = my_mktime(&tmDate);
	return 0;
}


time_t CCommon::GetTimeByPublishDate(const string& date) {
	if (19 != date.length())
		return 0;
	struct tm tmDate;
	memset(&tmDate, 0x00, sizeof(tmDate));
	tmDate.tm_year = atoi(date.substr(0, 4).c_str()) - 1900;
	tmDate.tm_mon = atoi(date.substr(5, 2).c_str()) - 1;
	tmDate.tm_mday = atoi(date.substr(8, 2).c_str());
	tmDate.tm_hour = atoi(date.substr(11, 2).c_str());
	tmDate.tm_min = atoi(date.substr(14, 2).c_str());
	tmDate.tm_sec = atoi(date.substr(17, 2).c_str());
	tmDate.tm_isdst = 0;
	return my_mktime(&tmDate);
}

time_t CCommon::GetTimeByPublishDate(const char* szDate) {
	if (19 != strlen(szDate))
		return 0;
	struct tm tmDate;
	memset(&tmDate, 0x00, sizeof(tmDate));
	int a, b, c, d;

	c = szDate[17] - 0x30;
	d = szDate[18] - 0x30;
	tmDate.tm_sec = (c << 3) + (c << 1) + d;

	c = szDate[14] - 0x30;
	d = szDate[15] - 0x30;
	tmDate.tm_min = (c << 3) + (c << 1) + d;

	c = szDate[11] - 0x30;
	d = szDate[12] - 0x30;
	tmDate.tm_hour = (c << 3) + (c << 1) + d;

	c = szDate[8] - 0x30;
	d = szDate[9] - 0x30;
	tmDate.tm_mday = (c << 3) + (c << 1) + d;

	c = szDate[5] - 0x30;
	d = szDate[6] - 0x30;
	tmDate.tm_mon= (c << 3) + (c << 1) + d - 1;

	a = szDate[0] - 0x30;
	b = szDate[1] - 0x30;
	c = szDate[2] - 0x30;
	d = szDate[3] - 0x30;
	tmDate.tm_year = (a << 10) - (a << 4) - (a << 3) + (b << 6) + (b << 5) + (b << 2) + (c << 3) + (c << 1) + d - 1900;
	tmDate.tm_isdst = 0;
	return my_mktime(&tmDate);
}

bool CCommon::GetDateTimeNoFormat(time_t tt, string& datetime) {
	char buf[20];
	struct tm tmNow;
	datetime.clear();
	localtime_r(&tt, &tmNow);

	snprintf(buf, 20, "%.4d%.2d%.2d%.2d%.2d%.2d", tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
	datetime = buf;
	return true;
}

void CCommon::TrimDatetimeToHour(time_t & ttDatetime) {
	UINT32 uiMode = ttDatetime % 3600;
	ttDatetime -= uiMode;
}

void CCommon::TrimDatetimeToDay(time_t & ttDatetime) {
	struct tm tmNow;
	time_t ttDate = ttDatetime;
	localtime_r(&ttDate, &tmNow);
	ttDatetime -= (tmNow.tm_hour * 3600 + tmNow.tm_min * 60 + tmNow.tm_sec);
}

void CCommon::GetYMonDay(time_t tt, UINT32& uiYmd) {
	struct tm tmNow;
	localtime_r(&tt, &tmNow);
	uiYmd = (tmNow.tm_year + 1900) * 10000 + tmNow.tm_mon * 100 + tmNow.tm_mday;
}
int CCommon::GetYear(time_t tt) {
	struct tm tm_time;
	localtime_r(&tt, &tm_time);
	return tm_time.tm_year + 1900;
}
int CCommon::GetMonth(time_t tt) {
	struct tm tm_time;
	localtime_r(&tt, &tm_time);
	return tm_time.tm_mon + 1;
}
int CCommon::GetDay(time_t tt) {
	struct tm tm_time;
	localtime_r(&tt, &tm_time);
	return tm_time.tm_mday;
}
int CCommon::GetHour(time_t tt) {
	struct tm tm_time;
	localtime_r(&tt, &tm_time);
	return tm_time.tm_hour;
}
int CCommon::GetMinute(time_t tt) {
	struct tm tm_time;
	localtime_r(&tt, &tm_time);
	return tm_time.tm_min;
}
int CCommon::GetSecond(time_t tt) {
	struct tm tm_time;
	localtime_r(&tt, &tm_time);
	return tm_time.tm_sec;
}

bool CCommon::CreateNoExistDirectory(const string& path) {
	if (false == IsDirectory(path)) {
		if (false == CreateDirectory(path))
			return false;
		else
			return true;
	}
	return true;
}

bool CCommon::IsExistFile(const string& strFile) {
	struct stat info;
	if (0 == stat(strFile.c_str(), &info)) {
		return S_ISREG(info.st_mode);
	}
	return false;
}

bool CCommon::IsDirectory(const string& strFile) {
	struct stat info;
	if (0 == stat(strFile.c_str(), &info)) {
		return S_ISDIR(info.st_mode);
	}
	return false;
}

INT64 CCommon::GetFileSize(const string& strFile) {
	struct stat info;
	if (0 == stat(strFile.c_str(), &info)) {
		return info.st_size;
	}
	return -1;
}

off_t CCommon::GetFileSize(const string& strFile, bool& bSuccess) {
	struct stat info;
	bSuccess = true;
	if (0 == stat(strFile.c_str(), &info)) {
		return info.st_size;
	}
	bSuccess = false;
	return 0;
}

time_t CCommon::GetFileModifyDatetime(const string& strFile) {
	struct stat info;
	if (0 == stat(strFile.c_str(), &info)) {
		return info.st_mtime;
	}
	return 0;
}

bool CCommon::CreateDirectory(const string& strFile) {
	if (0 == mkdir(strFile.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP))
		return true;
	return false;
}

bool CCommon::MoveFile(const string& src, const string& dest, bool bzip) {
	if (CCommon::IsExistFile(dest))
		DelFile(dest);
	if (0 == rename(src.c_str(), dest.c_str())) {
		if (bzip) {
			char cmd[1024];
			sprintf(cmd, "gzip %s", dest.c_str());
			if (0 != system(cmd))
				return false;
		}
		return true;
	}
	return false;
}

bool CCommon::DelFile(const string& strFile) {
	if (0 == remove(strFile.c_str()))
		return true;
	return false;

}

bool CCommon::DelDirectory(const string& strPath) {
	if (0 == rmdir(strPath.c_str()))
		return true;
	return false;
}

bool CCommon::ChangeDirectory(const string& strPath) {
	if (0 == chdir(strPath.c_str()))
		return true;
	return false;
}

bool CCommon::GetDirFile(const string& strPath, list<string>& files) {
	files.clear();
	DIR* dp;
	struct dirent* dir;

	if (NULL == (dp = opendir(strPath.c_str())))
		return false;
	string file;
	while (NULL != (dir = readdir(dp))) {
		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
			continue;
		if(dir->d_name[0]=='.')//隐藏文件
			continue;
		if (IsDirectory(strPath + dir->d_name))
			continue;
		file = dir->d_name;
		files.push_back(file);
	}
	closedir(dp);
	return true;
}

bool CCommon::GetDirFilePath(const string& strPath, list<string>& files) {
	files.clear();
	DIR* dp;
	struct dirent* dir;

	if (NULL == (dp = opendir(strPath.c_str())))
		return false;
	string file;
	while (NULL != (dir = readdir(dp))) {
		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
			continue;
		if(dir->d_name[0]=='.')//隐藏文件
			continue;
		if (IsDirectory(strPath + dir->d_name))
			continue;
		file = dir->d_name;
		files.push_back(strPath+"/"+file);
	}
	closedir(dp);
	return true;
}

bool CCommon::GetDirDir(const string& strPath, list<string>& dirs) {
	dirs.clear();
	DIR* dp;
	struct dirent* dir;

	if (NULL == (dp = opendir(strPath.c_str())))
		return false;
	string dirname;
	while (NULL != (dir = readdir(dp))) {
		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
			continue;
		if (!IsDirectory(strPath +"/"+ dir->d_name))
			continue;
		dirname = dir->d_name;
		dirs.push_back(dirname);
	}
	closedir(dp);
	return true;
}

/* Get the name beyond the last occurence of directory separator '/', 
 * if there is no '/', return the whole string.
 */
void CCommon::GetLastDirectoryName(const string& dir, string& last) {
	string::size_type pos = dir.find_last_of('/');
	if (string::npos == pos) {
		last = dir;
	} else {
		last = dir.substr(++pos);
	}
}

/* Get the dir name of the full path, if there is no '/' in the path,
 * return the './' as the current directory.
 */
void CCommon::GetDirectoryName(const string& path, string& dirName){
    string::size_type pos = path.find_last_of('/');
    if(string::npos == pos)
        dirName = "./";
    else
        dirName = path.substr(0, pos);
}

// 得到占用的内存数
// 详情参看 man proc
int CCommon::GetMemUsage(int &nMByte)
{
	pid_t pid = getpid();
	const int BUFSZ = 100;
	char szBuf[BUFSZ];
	if (snprintf(szBuf, BUFSZ, "/proc/%d/status", pid) >= BUFSZ) {
		fprintf(stderr, "pid:%d too big\n", pid);
		return -1;
	}
	ifstream ifsStatus(szBuf, ios::in);
	if (!ifsStatus.good()) {
		fprintf(stderr, "open file:%s failed\n", szBuf);
		return -1;
	}

	string strLine;
	const string strRSS = "VmRSS:\t";
	while (getline(ifsStatus, strLine)) {
		size_t nPos = string::npos;
		if (string::npos != (nPos = strLine.find(strRSS))) {
			nPos += strRSS.size();
			string strValue = strLine.substr(nPos);
			CCommon::trim(strValue);
			// KB => MB
			nMByte = atoi(strValue.c_str()) / 1024;
			ifsStatus.close();
			return 0;
		}
	}

	ifsStatus.close();
	return -1;
}

//return value: true:success, false:error
//读正确返回true,读取文件错误返回false,是否到文件末尾需要自己程序判断。
bool CCommon::ReadLine(FILE* fd, string& line) {
	char szBuf[8192];
	line.erase();
	memset(szBuf, 0x00, 8192);
	if (fgets(szBuf, 8192, fd) == NULL) {
		if (!feof(fd))
			return false;
		return true;
	}
	if (szBuf[strlen(szBuf) - 1] == '\n')
		szBuf[strlen(szBuf) - 1] = 0x00;
	line = szBuf;
	return true;
}

//return value: true:success, false:error
//读正确返回true,读取文件错误返回false,是否到文件末尾需要自己程序判断。
bool CCommon::ReadLine(FILE* fd, char* szBuf, UINT32 uiBufLen) {
    szBuf[0] = '\0';
	if (fgets(szBuf, uiBufLen, fd) == NULL) {
		if (!feof(fd))
			return false;
		return true;
	}
	if (szBuf[strlen(szBuf) - 1] == '\n')
		szBuf[strlen(szBuf) - 1] = 0x00;
	return true;
}

bool CCommon::ReadData(FILE* fd, string& data, INT64 size) {
	INT64 iLeft = size;
	INT64 iReadNum;
	INT64 iRet;
	char szBuf[2049];
	data.erase();

	while (iLeft > 0) {
		memset(szBuf, 0x00, 2049);
		iReadNum = iLeft > 2048 ? 2048 : iLeft;
		iRet = fread(szBuf, 1, iReadNum, fd);
		data += szBuf;
		if (iRet != iReadNum) {
			if (!feof(fd))
				return false;
			return true;
		}
		iLeft -= iRet;
	}
	return true;
}

void CCommon::CopyString(char* dest, const char* src, size_t len) {
	if (NULL == dest)
		return;
	if (NULL == src) {
		dest[0] = 0x00;
		return;
	}
	if (strlen(src) < len)
		len = strlen(src);
	memcpy(dest, src, len);
	dest[len] = 0x00;
}
void CCommon::CopyString(char* dest, const char* src, size_t desc_len, size_t src_len) {
	if (NULL == dest)
		return;
	if (NULL == src) {
		dest[0] = 0x00;
		return;
	}
	if (src_len < desc_len)
		desc_len = src_len;
	memcpy(dest, src, desc_len);
	dest[desc_len] = 0x00;
}

void CCommon::split(const string& src, list<string>& value) {
	split(src, value, '|');
}

void CCommon::split(const string& src, list<string>& value, char ch) {
	string::size_type begin = 0;
	string::size_type end = find(src, ch);
	value.clear();

	while (string::npos != end) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		value.push_back(str);
		begin = end + 1;
		end = find(src, ch, begin);
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
	value.push_back(str);
}

void CCommon::split(const string& src, list<string>& value, char ch, UINT32 uiNumber) {
	UINT32 uiTimes = 0;
	string::size_type begin = 0;
	string::size_type end = find(src, ch);
	value.clear();

	while (string::npos != end && uiTimes < uiNumber) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		value.push_back(str);
		begin = end + 1;
		end = find(src, ch, begin);
		uiTimes++;
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
	value.push_back(str);
}

void CCommon::split(const string& src, vector<string>& value, char ch) {
	string::size_type begin = 0;
	string::size_type end = find(src, ch);
	value.clear();
	value.reserve(src.size()/2);

	while (string::npos != end) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		value.push_back(str);
		begin = end + 1;
		end = find(src, ch, begin);
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
	value.push_back(str);
}

void CCommon::split(const string& src, vector<string>& value, char ch, UINT32 uiNumber) {
	UINT32 uiTimes = 0;
	string::size_type begin = 0;
	string::size_type end = find(src, ch);
	value.clear();

	while (string::npos != end && uiTimes < uiNumber) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		value.push_back(str);
		begin = end + 1;
		end = find(src, ch, begin);
		uiTimes++;
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
	value.push_back(str);
}

void CCommon::splitWithoutGBK(const string& src, vector<string>& value, char ch) {
	string::size_type begin = 0;
	string::size_type end = src.find(ch);
	value.clear();
	if (string::npos == end)
		return;
	value.reserve(src.size()/2);

	while (string::npos != end) {
		string str = src.substr(begin, end - begin);
		value.push_back(str);
		begin = end + 1;
		end = src.find(ch, begin);
	}
	string str = src.substr(begin);
	value.push_back(str);
}

void CCommon::splitIgnoreSpace(const string& src, list<string>& value, char ch) {
	string::size_type begin = 0;
	string::size_type end = find(src, ch);
	value.clear();

	while (string::npos != end) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		if (!str.empty())
			value.push_back(str);
		begin = end + 1;
		end = find(src, ch, begin);
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
	if (!str.empty())
		value.push_back(str);
}

void CCommon::splitIgnoreSpace(const string& src, list<string>& value, string strToken) {
	string::size_type begin = 0;
	string::size_type end = src.find(strToken);
	value.clear();

	while (string::npos != end) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		if (!str.empty())
			value.push_back(str);
		begin = end + strToken.length();
		end = src.find(strToken, begin);
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
	if (!str.empty())
		value.push_back(str);
}

void CCommon::splitIgnoreSpace(const string& src, vector<string>& value, char ch) {
	string::size_type begin = 0;
	string::size_type end = find(src, ch);
	value.clear();
	value.reserve(src.size()/2);

	while (string::npos != end) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		if (!str.empty())
			value.push_back(str);
		begin = end + 1;
		end = find(src, ch, begin);
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
	if (!str.empty())
		value.push_back(str);
}

void CCommon::splitIgnoreSpace(const string& src, vector<string>& value, string strToken) {
	string::size_type begin = 0;
	string::size_type end = src.find(strToken);
	value.clear();
	value.reserve(src.size()/2);

	while (string::npos != end) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		if (!str.empty())
			value.push_back(str);
		begin = end + strToken.length();
		end = src.find(strToken, begin);
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
	if (!str.empty())
		value.push_back(str);
}

void CCommon::split(const string& src, list<pair<string, string> >& value, char ch) {
	string::size_type begin = 0;
	string::size_type end = find(src, ch);
	pair<string, string> item;
	value.clear();

	while (string::npos != end) {
		if (CCommon::Pair(src.substr(begin, end - begin), item)) {
			value.push_back(item);
		}
		begin = end + 1;
		end = find(src, ch, begin);
	}
	if (CCommon::Pair(src.substr(begin, end - begin), item)) {
		value.push_back(item);
	}
}
void CCommon::split(const string& src, vector<string>& value, string strToken) {
	string::size_type begin = 0;
	string::size_type end = src.find(strToken);
	value.clear();
	value.reserve(src.size()/2);

	while (string::npos != end) {
		string str = src.substr(begin, end - begin);
		trimIncludeTab(str);
		value.push_back(str);
		begin = end + strToken.length();
		end = src.find(strToken, begin);
	}
	string str = src.substr(begin);
	trimIncludeTab(str);
    value.push_back(str);
}

bool CCommon::GetPair(const list<pair<string, string> >& values, const string& name, pair<string, string>& item) {
	list<pair<string, string> >::const_iterator iter = values.begin();
	while (iter != values.end()) {
		item = (*iter);
		if (strcasecmp(item.first.c_str(), name.c_str()) == 0)
			return true;
		iter++;
	}
	return false;

}

bool CCommon::Pair(const string& src, pair<string, string>& value) {
	string::size_type end = src.find('=');
	string name;
	string data;
	if (string::npos != end) {
		name = src.substr(0, end);
		data = src.substr(end + 1);
		CCommon::trim(name);
		CCommon::trim(data);
		value.first = name;
		value.second = data;
		return true;
	}
	return false;
}

bool CCommon::LoadFile(string const& strFileName, string& strData) {
	INT64 iFileSize = CCommon::GetFileSize(strFileName);
	if (-1 == iFileSize)
		return false;
	FILE* fd = fopen(strFileName.c_str(), "rb");
	bool bRet = CCommon::ReadData(fd, strData, iFileSize);
	fclose(fd);
	return bRet;
}

bool CCommon::IsInteger(const string& value) {
	if (value.length() == 0)
		return false;
	for (size_t i = 0; i < value.length(); i++) {
		switch (value[i]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			case '+':
				continue;
			default:
				return false;
		}
	}
	return true;
}

bool CCommon::IsDouble(const string& value) {
	if (value.length() == 0)
		return false;
	for (size_t i = 0; i < value.length(); i++) {
		switch (value[i]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			case 'e':
			case 'E':
			case '+':
			case '.':
				continue;
			default:
				return false;
		}
	}
	return true;
}

void CCommon::trim(string& value) {
	string::size_type begin = 0;
	string::size_type end = 0;
	string::size_type len = value.length();
	string::size_type i;
	for (i = 0; i < len; i++) {
		if (value[i] != ' ')
			break;
	}
	begin = i;
	for (i = len; i > begin; i--) {
		if (value[i - 1] != ' ')
			break;
	}
	end = i;
	if (end - begin != len)
		value = value.substr(begin, end - begin);
}
void CCommon::trimIncludeTab(string& value) {
	string::size_type begin = 0;
	string::size_type end = 0;
	string::size_type len = value.length();
	string::size_type i;
	for (i = 0; i < len; i++) {
		if (value[i] != ' ' && value[i] != '\t' && value[i] != '\r')
			break;
	}
	begin = i;
	for (i = len; i > begin; i--) {
		if (value[i - 1] != ' ' && value[i - 1] != '\t' && value[i-1] != '\r')
			break;
	}
	end = i;
	if (end - begin != len)
		value = value.substr(begin, end - begin);
}
void CCommon::trim(char* value) {
	size_t len = strlen(value);
	size_t i;
	for (i = len; i > 0; i--) {//trim right
		if (value[i - 1] != ' ')
			break;
		value[i - 1] = 0x00;
	}
	len = strlen(value);
	for (i = 0; i < len; i++) {
		if (value[i] != ' ')
			break;
	}
	if (i != 0) {
		SameMemCopy(value, value + i, len - i);
	}
}

void CCommon::rtrim(char* value) {
	if (!value)
		return;
	size_t i;
	for (i = strlen(value); i > 0; i--) {
		if (value[i - 1] == ' ')
			value[i - 1] = 0x00;
		else
			break;
	}
}

void CCommon::rtrimIncludeComma(char* value) {
	if (!value)
		return;
	size_t i;
	for (i = strlen(value); i > 0; i--) {
		if (value[i - 1] == ' ' || value[i - 1] == ',')
			value[i - 1] = 0x00;
		else
			break;
	}
}

void CCommon::rtrimIncludeComma(string& value) {
	string::size_type len = value.length();
	string::size_type i;

	for (i = len; i > 0; i--) {
		if (value[i - 1] != ' ' && value[i - 1] != ',')
			break;
	}
	if (i != len)
		value = value.substr(0, i);
}

bool CCommon::ToWebDomain(char* szUrl) {
	//format http://xxx.xxx.xx
	if (strncasecmp(szUrl, "http://", strlen("http://")) != 0) {
		return false;
	}

	for (int i = strlen("http://") + 3; i < (int) strlen(szUrl); i++) {
		if (szUrl[i] == '/') {
			szUrl[i] = 0x00;
			break;
		}
		if (szUrl[i] == ':') {
			szUrl[i] = 0x00;
			break;
		}
	}
	return true;
}

int CCommon::CountChar(const char* szSrc, char ch){
	int iCount = 0;
	int i = 0;
	while(szSrc[i]){
		if(ch == szSrc[i])
			iCount++;
		i++;
	}
	return iCount;
}

int CCommon::CountChar(const string& strSrc, char ch){
	int iCount = 0;
	for(size_t i = 0; i < strSrc.length(); i++){
		if(ch == strSrc[i])
			iCount++;
	}
	return iCount;
}

void CCommon::ReplaceAll(string& strSrc, const string& str1, const string& str2) {
	string::size_type pos = 0;
	while (true) {
		pos = find(strSrc, str1, pos);
		if (pos == string::npos)
			return;

		strSrc.replace(pos, str1.length(), str2);
		pos += str2.length();
	}
}

void CCommon::ReplaceAll(char const* src, char *dst, size_t& dst_len, char const *from, char const*to) {
	int from_len = strlen(from);
	int to_len = strlen(to);
	int left_len = dst_len;
	int src_len = 0;
	int copy_len = 0;

	char const* psrc = src;
	char * pnext = NULL;

	do {
		pnext = StrStr(psrc, from);
		if (NULL == pnext) {
			src_len = strlen(psrc);
			copy_len = left_len > src_len ? src_len : left_len;
			memcpy(dst + dst_len - left_len, psrc, copy_len);
			left_len -= copy_len;
			break;
		}
		if (pnext - psrc >= left_len) {
			copy_len = left_len;
			memcpy(dst + dst_len - left_len, psrc, copy_len);
			left_len -= copy_len;
			break;
		} else {
			copy_len = pnext - psrc;
			memcpy(dst + dst_len - left_len, psrc, copy_len);
			left_len -= copy_len;
		}
		//replace
		if (to_len >= left_len) {
			copy_len = left_len;
			memcpy(dst + dst_len - left_len, to, copy_len);
			left_len -= copy_len;
			break;
		} else {
			copy_len = to_len;
			memcpy(dst + dst_len - left_len, to, copy_len);
			left_len -= copy_len;
		}
		psrc = pnext + from_len;

	} while (1);
	if (left_len > 0)
		dst[dst_len - left_len] = '\0';
	dst_len -= left_len;
}

size_t CCommon::length(LONGLONG value) {
	string str;
	toString(value, str);
	return str.length();
}
size_t CCommon::length(int value) {
	string str;
	toString(value, str);
	return str.length();
}
size_t CCommon::length(unsigned int value) {
	string str;
	toString(value, str);
	return str.length();
}
size_t CCommon::length(short value) {
	string str;
	toString(value, str);
	return str.length();
}
size_t CCommon::length(unsigned short value) {
	string str;
	toString(value, str);
	return str.length();
}
size_t CCommon::length(double value) {
	string str;
	toString(value, str);
	return str.length();
}

size_t CCommon::length(char value) {
	string str;
	toString(value, str);
	return str.length();
}
size_t CCommon::length(unsigned char value) {
	string str;
	toString(value, str);
	return str.length();
}

void CCommon::toString(LONGLONG data, string& value) {
	char buf[64];
	sprintf(buf, "%lld", data);
	value = buf;
}
void CCommon::toString(UINT64 data, string& value) {
	char buf[64];
	sprintf(buf, "%llu", data);
	value = buf;
}
void CCommon::toString(int data, string& value) {
	char buf[64];
	sprintf(buf, "%d", data);
	value = buf;
}
void CCommon::toString(unsigned int data, string& value) {
	char buf[64];
	sprintf(buf, "%u", data);
	value = buf;
}
void CCommon::toString(short data, string& value) {
	char buf[64];
	sprintf(buf, "%d", data);
	value = buf;
}
void CCommon::toString(unsigned short data, string& value) {
	char buf[64];
	sprintf(buf, "%u", data);
	value = buf;
}
void CCommon::toString(double data, string& value) {
	char buf[64];
	sprintf(buf, "%.6f", data);
	value = buf;
}

void CCommon::toString(char data, string& value) {
	char buf[64];
	sprintf(buf, "%d", data);
	value = buf;
}
void CCommon::toString(unsigned char data, string& value) {
	char buf[64];
	sprintf(buf, "%d", data);
	value = buf;
}

void CCommon::Utf8ToAscii(char* src, size_t src_len, char* desc, size_t& dest_len, bool bEscapeUrl) {
	char tmp1[16];
	size_t len = 0;
	size_t pos = 0;
	size_t utf8_len;
	size_t uit8_pos = 0;

	while (pos < src_len) {
		utf8_len = YS_GbkUcsConv::GetUtf8Len(src + pos);
		if (1 == utf8_len) {
			tmp1[0] = src[pos];
			tmp1[1] = 0x00;
			len = 1;
			if (bEscapeUrl) {
				len = 3;
				switch (src[pos]) {
					case '+':
						strcpy(tmp1, "%2B");
						break;
					case ' ':
						strcpy(tmp1, "%20");
						break;
					case '/':
						strcpy(tmp1, "%2F");
						break;
					case '?':
						strcpy(tmp1, "%3F");
						break;
					case '%':
						strcpy(tmp1, "%25");
						break;
					case '#':
						strcpy(tmp1, "%23");
						break;
					case '&':
						strcpy(tmp1, "%26");
						break;
					case '=':
						strcpy(tmp1, "%3D");
						break;
					default:
						len = 1;
						break;
				}
			}
			if (uit8_pos + len > dest_len)
				break;
			memcpy(desc + uit8_pos, tmp1, len);
			uit8_pos += len;
			pos++;
		} else if (2 == utf8_len) {
			if (uit8_pos + 6 >= dest_len)
				break;

			sprintf(tmp1, "%X", src[pos]);
			sprintf(tmp1, "%%%s", tmp1 + 6);
			memcpy(desc + uit8_pos, tmp1, strlen(tmp1));
			uit8_pos += strlen(tmp1);

			sprintf(tmp1, "%X", src[pos + 1]);
			sprintf(tmp1, "%%%s", tmp1 + 6);
			memcpy(desc + uit8_pos, tmp1, strlen(tmp1));
			uit8_pos += strlen(tmp1);
			pos += 2;
		} else {
			if (uit8_pos + 9 >= dest_len)
				break;
			sprintf(tmp1, "%X", src[pos]);
			sprintf(tmp1, "%%%s", tmp1 + 6);
			memcpy(desc + uit8_pos, tmp1, strlen(tmp1));
			uit8_pos += strlen(tmp1);

			sprintf(tmp1, "%X", src[pos + 1]);
			sprintf(tmp1, "%%%s", tmp1 + 6);
			memcpy(desc + uit8_pos, tmp1, strlen(tmp1));
			uit8_pos += strlen(tmp1);

			sprintf(tmp1, "%X", src[pos + 2]);
			sprintf(tmp1, "%%%s", tmp1 + 6);
			memcpy(desc + uit8_pos, tmp1, strlen(tmp1));
			uit8_pos += strlen(tmp1);
			pos += 3;
		}
	}
	desc[uit8_pos] = 0x00;
	dest_len = uit8_pos;
}

void CCommon::BinnaryToHex(char const* src, size_t src_len, char* desc, size_t& dest_len) {
	unsigned char c;
	size_t i;
	for (i = 0; i < src_len; i++) {
		if (dest_len <= 2 * i + 1)
			break;
		c = (unsigned char) src[i];
		desc[i * 2] = (c >> 4) & 0x0F;
		if (desc[i * 2] > 9)
			desc[i * 2] += 'A' - 10;
		else
			desc[i * 2] += '0';
		desc[i * 2 + 1] = c & 0x0F;
		if (desc[i * 2 + 1] > 9)
			desc[i * 2 + 1] += 'A' - 10;
		else
			desc[i * 2 + 1] += '0';
	}
	dest_len = i * 2;
	desc[dest_len] = 0x00;
}

bool CCommon::IsValidGbkStr(char const* src, size_t src_len) {
	size_t pos = 0;
	while (pos < src_len) {
		if (src[pos] > 0) {
			pos++;
			continue;
		}
		if (false == YS_GbkUcsConv::IsGbkCode(src + pos))
			return false;
		pos += 2;
	}
	return true;
}
void CCommon::MarkHtmlRed(char const* src, size_t src_len, char const* words, char* dest, size_t& dest_len) {
	char* pstr;

	size_t pos = 0;
	size_t dest_pos = 0;
	size_t dest_append_len = 0;
	size_t lask_key_len = 0;
	char const* const pMarkStart = "<font color=red>";
	size_t const iMarkStartLen = strlen("<font color=red>");
	char const* const pMakrEnd = "</font>";
	size_t const iMarkEndLen = strlen("</font>");

	while (pos < src_len) {
		if (YS_GbkUcsConv::IsGbkCode(src + pos)) {
			lask_key_len = 2;
			/*        }else if (' ' == src[pos]){
			 pos++;
			 continue;*/
		} else {
			for (lask_key_len = 0; lask_key_len < src_len - pos; lask_key_len++) {
				if ((src[pos + lask_key_len] >= 'a') && (src[pos + lask_key_len] <= 'z') || (src[pos + lask_key_len] >= 'A') && (src[pos + lask_key_len] <= 'Z'))
					continue;
				break;
			}
			if (0 == lask_key_len)
				lask_key_len = 1;
		}
		//format is <mark-begin>+appended+last+<mark-end>
		if ((dest_len < dest_pos + iMarkStartLen + dest_append_len + lask_key_len + iMarkEndLen) || //no space
				(false == IsValidGbkStr(src + pos, lask_key_len))) {//invalid gbk
			if (dest_append_len > 0) {//has marked char
				if ((1 == dest_append_len) || ((2 == dest_append_len) && YS_GbkUcsConv::IsGbkCode(dest + dest_pos + iMarkStartLen))) {//single char, not mark.
					//restore the memory
					memcpy(dest + dest_pos, dest + dest_pos + iMarkStartLen, dest_append_len);
					dest_pos += dest_append_len;
				} else {//mark it
					memcpy(dest + dest_pos + iMarkStartLen + dest_append_len, pMakrEnd, iMarkEndLen);
					dest_pos += iMarkStartLen + dest_append_len + iMarkEndLen;
				}
			}
			break;
		}

		if (dest_append_len > 0) {
			memcpy(dest + dest_pos + iMarkStartLen + dest_append_len, src + pos, lask_key_len);
			dest[dest_pos + iMarkStartLen + dest_append_len + lask_key_len] = 0x00;
		} else {
			memcpy(dest + dest_pos, src + pos, lask_key_len);
			dest[dest_pos + lask_key_len] = 0x00;
		}

		pstr = strstr(words, dest + dest_pos + (dest_append_len > 0 ? iMarkStartLen : 0));
		if (NULL == pstr) {//not find
			//deal with the data appended before.
			if (dest_append_len > 0) {//has marked char
				if ((1 == dest_append_len) || ((2 == dest_append_len) && YS_GbkUcsConv::IsGbkCode(dest + dest_pos + iMarkStartLen))) {//single char, not mark.
					//restore the memory
					memcpy(dest + dest_pos, dest + dest_pos + iMarkStartLen, dest_append_len);
					dest_pos += dest_append_len;
				} else {//mark it
					memcpy(dest + dest_pos + iMarkStartLen + dest_append_len, pMakrEnd, iMarkEndLen);
					dest_pos += iMarkStartLen + dest_append_len + iMarkEndLen;
				}
				//reset dest_append_len
				dest_append_len = 0;

				memcpy(dest + dest_pos, src + pos, lask_key_len);
				dest[dest_pos + lask_key_len] = 0x00;
				//re-check the key
				if (strstr(words, dest + dest_pos)) {//find
					memcpy(dest + dest_pos, pMarkStart, iMarkStartLen);
					memcpy(dest + dest_pos + iMarkStartLen, src + pos, lask_key_len);
					dest_append_len = lask_key_len;
				} else {//not find
					dest_pos += lask_key_len;
				}
			} else {
				dest_pos += lask_key_len;
			}
		} else {
			if (0 == dest_append_len) {
				memcpy(dest + dest_pos, pMarkStart, iMarkStartLen);
				memcpy(dest + dest_pos + iMarkStartLen, src + pos, lask_key_len);
			}
			dest_append_len += lask_key_len;
		}
		pos += lask_key_len;//skip space

	}//end while
	if (dest_append_len > 0) {
		if ((1 == dest_append_len) || ((2 == dest_append_len) && YS_GbkUcsConv::IsGbkCode(dest + dest_pos + iMarkStartLen))) {//single char, not mark.
			//restore the memory
			memcpy(dest + dest_pos, dest + dest_pos + iMarkStartLen, dest_append_len);
			dest_pos += dest_append_len;
		} else {//mark it
			memcpy(dest + dest_pos + iMarkStartLen + dest_append_len, pMakrEnd, iMarkEndLen);
			dest_pos += iMarkStartLen + dest_append_len + iMarkEndLen;
		}
	}
	dest[dest_pos] = 0x00;
	dest_len = dest_pos;
}

void CCommon::SameMemCopy(char* dest, char const* src, size_t len) {
	for (size_t i = 0; i < len; i++) {
		dest[i] = src[i];
	}
}

void CCommon::RemoveSpace(char * str, bool bIncludeEnglish) {
	size_t len = strlen(str);
	size_t pos = 0;
	for (size_t i = 0; i < len; i++) {
		if (' ' == str[i]) {
			if (bIncludeEnglish) {
				continue; //remove it
			} else {
				if (2 <= pos) {
					if (YS_GbkUcsConv::IsGbkCode(str + pos - 2))
						continue; //remove it
					if (i + 3 < len) {
						if (YS_GbkUcsConv::IsGbkCode(str + i + 1))
							continue; //remove it
					}
				} else {
					continue; //remove it
				}
			}
		}
		str[pos++] = str[i];
	}
	str[pos] = 0x00;
}

void CCommon::RemoveDigestSpace(char * str) {
	size_t len = strlen(str);
	size_t pos = 0;
	bool bPrevSpace = false;
	for (size_t i = 0; i < len; i++) {
		if (' ' == str[i]) {
			if (!bPrevSpace) {//remove it
				bPrevSpace = true;
				continue; //remove it
			}
		} else {
			bPrevSpace = false;
		}
		str[pos++] = str[i];
	}
	str[pos] = 0x00;
}

void CCommon::ToLower(char* src) {
	for (size_t i = 0; i < strlen(src); i++) {
		if ((src[i] >= 'A') && (src[i] <= 'Z')) {
			src[i] += 'a' - 'A';
		}
	}
}
void CCommon::ToUpper(char* src) {
	for (size_t i = 0; i < strlen(src); i++) {
		if ((src[i] >= 'a') && (src[i] <= 'z')) {
			src[i] += 'A' - 'a';
		}
	}
}

bool CCommon::IsGroup(UINT32 id, UINT16 unGroup, UINT16 unGroupIndex) {
	if (unGroup < 2)
		return true;
	if ((id % unGroup) == unGroupIndex)
		return true;
	return false;
}

void CCommon::GBKToLower(char* src) {
    size_t stLen = strlen(src);
	for (size_t i = 0; i < stLen;) {
		if (YS_GbkUcsConv::IsGbkCode(src + i))
			i += 2;
		else {
			if ((src[i] >= 'A') && (src[i] <= 'Z')) {
				src[i] += 'a' - 'A';
			}
			i++;
		}
	}
}

int CCommon::strgbkcasecmp(char const* src, char const* dst) {
	int i;
	int ret = 0;
	char temp1, temp2;
	do {
		if (YS_GbkUcsConv::IsGbkCode(dst) || YS_GbkUcsConv::IsGbkCode(src)) {
			i = 0;
			while (i < 2 && !(ret = *(unsigned char *) src - *(unsigned char *) dst)){
				++src, ++dst, i++;
                if(!(*dst))
                    break;
            }
			if (ret != 0)
				break;
		} else {
			temp1 = *(unsigned char *) src;
			if (temp1 >= 'A' && temp1 <= 'Z')
				temp1 += 'a' - 'A';
			temp2 = *(unsigned char *) dst;
			if (temp2 >= 'A' && temp2 <= 'Z')
				temp2 += 'a' - 'A';
			ret = temp1 - temp2;
			if (ret != 0)
				break;
			src++;
			dst++;
		}
	} while (*dst);
	if (!(*dst) && *src)
		ret = 1;
	if (ret < 0)
		return -1;
	else if (ret > 0)
		return 1;
	return ret;
}

int CCommon::strngbkcasecmp(char const* src, char const* dst, size_t n) {
	if(0 == n)
		return 0;
	int i;
	int ret = 0;
	char temp1, temp2;
	char const* end = dst + n;
	do {
		if (YS_GbkUcsConv::IsGbkCode(dst) || YS_GbkUcsConv::IsGbkCode(src)) {
			i = 0;
			while (i < 2 && !(ret = *(unsigned char *) src - *(unsigned char *) dst))
				++src, ++dst, i++;
			if (ret != 0)
				break;
		} else {
			temp1 = *(unsigned char *) src;
			if (temp1 >= 'A' && temp1 <= 'Z')
				temp1 += 'a' - 'A';
			temp2 = *(unsigned char *) dst;
			if (temp2 >= 'A' && temp2 <= 'Z')
				temp2 += 'a' - 'A';
			ret = temp1 - temp2;
			if (ret != 0)
				break;
			src++;
			dst++;
		}
	} while (*dst && dst < end);
	if (!(*dst) && *src && dst != end)
		ret = 1;
	if (ret < 0)
		return -1;
	else if (ret > 0)
		return 1;
	return ret;
}

void CCommon::strgbktolower(char* src) {
	int i;
	char temp1;
	while (*src) {
		if (YS_GbkUcsConv::IsGbkCode(src)) {
			i = 0;
			while (i < 2 && *(unsigned char *) src)
				++src, i++;
		} else {
			temp1 = *(unsigned char *) src;
			if (temp1 >= 'A' && temp1 <= 'Z')
				temp1 += 'a' - 'A';
			*(unsigned char *) src = temp1;
			src++;
		}
	}
}

bool CCommon::HasGbk(char const* szKeyword) {
	while (*szKeyword) {
		if (YS_GbkUcsConv::IsGbkCode(szKeyword))
			return true;
		szKeyword++;
	}
	return false;
}

void CCommon::RemoveTailBracket(string& str) {
	string::reverse_iterator riter = str.rbegin();
	if (')' == *riter || EndWith(str.c_str(), "）")) {
		size_t pos = str.rfind('(');
		if (string::npos == pos)
			pos = str.rfind("（");
		if (string::npos != pos)
			str = str.substr(0, pos);
	}else if(']' == *riter || EndWith(str.c_str(), "】")){
		size_t pos = str.rfind('[');
		if (string::npos == pos)
			pos = str.rfind("【");
		if (string::npos != pos)
			str = str.substr(0, pos);
	}
}

void CCommon::RemoveTailBracket(char* szStr) {
	int iPos = strlen(szStr) - 1;
	if (')' == szStr[iPos] || EndWith(szStr, "）")) {
		for (; iPos >= 0; iPos--) {
			if ('(' == szStr[iPos])
				break;
			else if (0 == strncmp(szStr + iPos, "（", strlen("（")))
				break;
		}
		if (-1 != iPos)
			szStr[iPos] = '\0';
	}
}

bool CCommon::RemoveTailBracket(char* szStr, string& strTail) {
	strTail.clear();
	if('\0' == szStr[0])
		return false;
	int iPos = strlen(szStr) - 1;
	if (')' == szStr[iPos]) {
		szStr[iPos] = '\0';
		for (; iPos >= 0; iPos--)
			if ('(' == szStr[iPos])
				break;
		if (-1 != iPos) {
			strTail = szStr + iPos + 1;
			szStr[iPos] = '\0';
			while (iPos > 0) {
				if (' ' == szStr[iPos - 1]) {
					szStr[iPos - 1] = '\0';
					iPos--;
				} else
					break;
			}
			return true;
		}
	}else if(']' == szStr[iPos]){
		szStr[iPos] = '\0';
		for (; iPos >= 0; iPos--)
			if ('[' == szStr[iPos])
				break;
		if (-1 != iPos) {
			strTail = szStr + iPos + 1;
			szStr[iPos] = '\0';
			while (iPos > 0) {
				if (' ' == szStr[iPos - 1]) {
					szStr[iPos - 1] = '\0';
					iPos--;
				} else
					break;
			}
			return true;
		}
	}else if(iPos >= 1 && 0 == strcmp(szStr + iPos - 1, "】")){
		iPos--;
		szStr[iPos] = '\0';		
		for (; iPos >= 0; iPos--)
			if (0 == strncmp(szStr + iPos, "【", strlen("【")))
				break;
		if (-1 != iPos) {
			strTail = szStr + iPos + 2;
			szStr[iPos] = '\0';
			while (iPos > 0) {
				if (' ' == szStr[iPos - 1]) {
					szStr[iPos - 1] = '\0';
					iPos--;
				} else
					break;
			}
			return true;
		}
	}
	return false;
}

bool CCommon::EndWith(const string& src, const string& suffix) {
	size_t stPos;
	stPos = src.rfind(suffix);
	if (string::npos != stPos && stPos == src.length() - suffix.length())
		return true;
	return false;
}

bool CCommon::EndWith(const char* szSrc, const char* szSuffix) {
	size_t lenSrc = strlen(szSrc);
	size_t lenSuffix = strlen(szSuffix);
	if (lenSrc >= lenSuffix && 0 == strcmp(szSrc + lenSrc - lenSuffix, szSuffix))
		return true;
	return false;
}

bool CCommon::EndWithCase(const char* szSrc, const char* szSuffix) {
	size_t lenSrc = strlen(szSrc);
	size_t lenSuffix = strlen(szSuffix);
	if (lenSrc >= lenSuffix && 0 == strcasecmp(szSrc + lenSrc - lenSuffix, szSuffix))
		return true;
	return false;
}

size_t CCommon::hashgbkcase(char const* s1) {
	int i;
	char temp1;
	size_t hashval = 0;
	const unsigned char *p = (const unsigned char *) s1;
	while (*p) {
		if (YS_GbkUcsConv::IsGbkCode((char const*)p)) {
			i = 0;
			while (i < 2 && *(unsigned char *) p) {
				hashval = hashval * 5 + (*p);
				++p, i++;
			}
		} else {
			temp1 = *(unsigned char *) p;
			if (temp1 >= 'A' && temp1 <= 'Z')
				temp1 += 'a' - 'A';
			hashval = hashval * 5 + temp1;
			p++;
		}
	}
	return hashval;

}

size_t CCommon::hashstrcase(char const* s1) {
	const unsigned char *p = (const unsigned char *) s1;
	size_t hashval = 0;
	for (; *p; p++) {
		if ('A' <= *p && 'Z' >= *p)
			hashval = hashval * 5 + (*p + 'a' - 'A'); //   or   whatever
		else
			hashval = hashval * 5 + (*p);
	}
	return (hashval);
}

bool CCommon::LoadBinaryFile(string const& strFileName, char* szFileBuf, INT64& iBufLen) {
	INT64 iFileSize = CCommon::GetFileSize(strFileName);
	if (-1 == iFileSize || iFileSize > iBufLen)
		return false;
    if (0 == iFileSize){
        iBufLen = 0;
        return true;
    }
	FILE* fd = fopen(strFileName.c_str(), "rb");
	if (NULL == fd)
		return false;
	iBufLen = iFileSize;
	INT64 iLeft = 0;
	INT64 iReadNum;
	INT64 iRet;
	while (iLeft < iFileSize) {
		iReadNum = (iFileSize - iLeft > 4096) ? 4096 : (iFileSize - iLeft);
		iRet = fread(szFileBuf + iLeft, 1, iReadNum, fd);
		if (iRet != iReadNum) {
			if (!feof(fd)){
                fclose(fd);
				return false;
            }
			fclose(fd);
			return true;
		}
		iLeft += iRet;
	}

	fclose(fd);
	return true;
}

bool CCommon::IsSingleChinese(const char* szSrc) {
	if (szSrc && 2 == strlen(szSrc) && YS_GbkUcsConv::IsGbkCode(szSrc))
		return true;
	return false;
}

void CCommon::SBCToDBC(char* src1) {
	if (NULL == src1)
		return;
	unsigned char* src = (unsigned char*) src1;
	int iScanPos = 0;
	int iConvertPos = 0;
	while (src[iScanPos]) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + iScanPos)) {
			if (163 == src[iScanPos] && 164 != src[iScanPos + 1]) { //￥ case
				src[iConvertPos] = src[iScanPos + 1] - 128;
				if (src[iConvertPos] >= 'A' && src[iConvertPos] <= 'Z')
					src[iConvertPos] = src[iConvertPos] - 'A' + 'a';
				iConvertPos++;
				iScanPos += 2;
			} else if (161 == src[iScanPos] && 161 == src[iScanPos + 1]) {
				src[iConvertPos++] = ' ';
				iScanPos += 2;
			} else {
				src[iConvertPos++] = src[iScanPos++];
				src[iConvertPos++] = src[iScanPos++];
			}
		} else {
			if (src[iScanPos] >= 'A' && src[iScanPos] <= 'Z')
				src[iConvertPos++] = src[iScanPos++] - 'A' + 'a';
			else
				src[iConvertPos++] = src[iScanPos++];
		}
	}
	src[iConvertPos] = '\0';
	while (iConvertPos > 0) {
		if (' ' == src[iConvertPos - 1]) {
			src[iConvertPos - 1] = '\0';
			iConvertPos--;
		} else
			break;
	}
}

void CCommon::RemovePunctuation(char* src1) {
	if (NULL == src1)
		return;
	unsigned char* src = (unsigned char*) src1;
	int iScanPos = 0;
	int iConvertPos = 0;
	bool bSpace = true;
	while (src[iScanPos]) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + iScanPos)) {
			if (163 == src[iScanPos]) {
                src[iConvertPos] = src[iScanPos + 1] - 128;
				if (src[iConvertPos] >= '0' && src[iConvertPos] <= '9'){
					bSpace = false;
                    iConvertPos++;
				} else if (src[iConvertPos] >= 'a' && src[iConvertPos] <= 'z') {
					bSpace = false;
                    iConvertPos++;
				} else if (src[iConvertPos] >= 'A' && src[iConvertPos] <= 'Z') {
					bSpace = false;
                    iConvertPos++;
				} else {
					if (!bSpace) {
						src[iConvertPos] = ' ';
                        iConvertPos++;
						bSpace = true;
					}
				}
                iScanPos += 2;
            }else if (161 == src[iScanPos]) {
				if (164 == src[iScanPos + 1]) {
					src[iConvertPos++] = src[iScanPos++];
					src[iConvertPos++] = src[iScanPos++];
					bSpace = false;
				} else if (161 <= src[iScanPos + 1] && 254 >= src[iScanPos + 1]) {
					if (!bSpace) {
						src[iConvertPos++] = ' ';
						iScanPos += 2;
						bSpace = true;
					} else
						iScanPos += 2;
				} else {
					src[iConvertPos++] = src[iScanPos++];
					src[iConvertPos++] = src[iScanPos++];
					bSpace = false;
				}
			} else {
				src[iConvertPos++] = src[iScanPos++];
				src[iConvertPos++] = src[iScanPos++];
				bSpace = false;
			}
		} else {
			if (src[iScanPos] >= '0' && src[iScanPos] <= '9') {
				src[iConvertPos++] = src[iScanPos++];
				bSpace = false;
			} else if (src[iScanPos] >= 'a' && src[iScanPos] <= 'z') {
				src[iConvertPos++] = src[iScanPos++];
				bSpace = false;
			} else if (src[iScanPos] >= 'A' && src[iScanPos] <= 'Z') {
				src[iConvertPos++] = src[iScanPos++];
				bSpace = false;
			} else {
				if (!bSpace) {
					src[iConvertPos++] = ' ';
					iScanPos++;
					bSpace = true;
				} else
					iScanPos++;
			}
		}
	}
	if (iConvertPos > 0 && ' ' == src[iConvertPos - 1])
		iConvertPos--;
	src[iConvertPos] = '\0';
}

void CCommon::RemoveNamePunctuation(char* src1) {
        if (NULL == src1)
                return;
        unsigned char* src = (unsigned char*) src1;
        int iScanPos = 0;
        int iConvertPos = 0;
        bool bSpace = true;
        while (src[iScanPos]) {
                if (YS_GbkUcsConv::IsGbkCode((char*) src + iScanPos)) {
                        if (0 == strncmp((char*) src + iScanPos, "，", 2)) {
                                if (!bSpace) {
					src[iConvertPos++] = ' ';
					iScanPos += 2;	
					bSpace = true;
				} else
					iScanPos += 2;
                        } else if(0 == strncmp((char*) src + iScanPos, "？", 2)){
				if (!bSpace) {
                                        src[iConvertPos++] = ' ';
                                        iScanPos += 2;  
                                        bSpace = true;
                                } else
                                        iScanPos += 2;
			} else{
                                src[iConvertPos++] = src[iScanPos++];
                                src[iConvertPos++] = src[iScanPos++];
                                bSpace = false;
                        }
                } else {
			if(',' == src[iScanPos]){
                                if (!bSpace) {
                                        src[iConvertPos++] = ' ';
                                        iScanPos++;
                                        bSpace = true;
                                } else
                                        iScanPos++;
                        }else if('?' == src[iScanPos]){
                                if (!bSpace) {
                                        src[iConvertPos++] = ' ';
                                        iScanPos++;
                                        bSpace = true;
                                } else
                                        iScanPos++;
                        }else{
				if(' ' == src[iScanPos])
					bSpace = true;
				else
					bSpace = false;
				src[iConvertPos++] = src[iScanPos++];
			}
                }
        }
        if (iConvertPos > 0 && ' ' == src[iConvertPos - 1])
                iConvertPos--;
        src[iConvertPos] = '\0';
}

void CCommon::RemovePunctuationAndSpace(char* src1) {
        if (NULL == src1)
                return;
        unsigned char* src = (unsigned char*) src1;
        int iScanPos = 0;
        int iConvertPos = 0;
        while (src[iScanPos]) {
                if (YS_GbkUcsConv::IsGbkCode((char*) src + iScanPos)) {
                        if (161 == src[iScanPos]) {
                                if (164 == src[iScanPos + 1]) {
                                        src[iConvertPos++] = src[iScanPos++];
                                        src[iConvertPos++] = src[iScanPos++];
                                } else if (162 <= src[iScanPos + 1] && 254 >= src[iScanPos + 1]) {
					iScanPos += 2;
                                } else {
                                        src[iConvertPos++] = src[iScanPos++];
                                        src[iConvertPos++] = src[iScanPos++];
                                }
                        } else {
                                src[iConvertPos++] = src[iScanPos++];
                                src[iConvertPos++] = src[iScanPos++];
                        }
                } else {
                        if (src[iScanPos] >= '0' && src[iScanPos] <= '9') {
                                src[iConvertPos++] = src[iScanPos++];
                        } else if (src[iScanPos] >= 'a' && src[iScanPos] <= 'z') {
                                src[iConvertPos++] = src[iScanPos++];
                        } else if (src[iScanPos] >= 'A' && src[iScanPos] <= 'Z') {
                                src[iConvertPos++] = src[iScanPos++];
                        } else {
				iScanPos++;
                        }
                }
        }
        if (iConvertPos > 0 && ' ' == src[iConvertPos - 1])
                iConvertPos--;
        src[iConvertPos] = '\0';
}

void CCommon::RemoveAsciiPunctuation(char* src1, set<UINT32>& setPos) {
	setPos.clear();
	if (NULL == src1)
		return;
	unsigned char* src = (unsigned char*) src1;
	int iScanPos = 0;
	while (src[iScanPos]) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + iScanPos)) {
			iScanPos += 2;
			setPos.insert((UINT32) iScanPos);
		} else {
			if ('@' == src[iScanPos] || (src[iScanPos] >= '[' && src[iScanPos] <= '`') || (src[iScanPos] >= '{' && src[iScanPos] <= 127)) {
				src[iScanPos] = ' ';
			}
			++iScanPos;
		}
	}
}

void CCommon::RemoveSplitAndEnter(char* src1) {
	if (NULL == src1)
		return;
	unsigned char* src = (unsigned char*) src1;
	int iScanPos = 0;
	while (src[iScanPos]) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + iScanPos)) {
			iScanPos += 2;
		} else {
			if ('|' == src[iScanPos] || '\r' == src[iScanPos] || '\n' == src[iScanPos]) {
				src[iScanPos] = ' ';
			}
			++iScanPos;
		}
	}
}
void CCommon::RemoveSplitAndEnter(string& value) {
	if (0 == value.length())
		return;
	string::size_type len = value.length();
	string::size_type i;
	for (i = 0; i < len; i++) {
		if (YS_GbkUcsConv::IsGbkCode((char*) value.c_str()+ i)) {
			i++;
		}else{
			if ('|' == value[i] || '\r' == value[i] || '\n' == value[i]) 
				value[i] = ' ';
		}
	}
}


void CCommon::RemainNumber(char* src1) {
	if (NULL == src1)
		return;
	unsigned char* src = (unsigned char*) src1;
	int iScanPos = 0;
	int iConvertPos = 0;
	bool bSpace = true;
	while (src[iScanPos]) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + iScanPos)) {
			if (!bSpace) {
				src[iConvertPos++] = ' ';
				iScanPos += 2;
				bSpace = true;
			} else
				iScanPos += 2;
		} else {
			if (src[iScanPos] >= '0' && src[iScanPos] <= '9') {
				src[iConvertPos++] = src[iScanPos++];
				bSpace = false;
			} else {
				if (!bSpace) {
					src[iConvertPos++] = ' ';
					iScanPos++;
					bSpace = true;
				} else
					iScanPos++;
			}
		}
	}
	if (iConvertPos > 0 && ' ' == src[iConvertPos - 1])
		iConvertPos--;
	src[iConvertPos] = '\0';
}

void CCommon::ReverseStr(char* szMsg) {
	if (NULL == szMsg)
		return;
	int i = 0, j = strlen(szMsg) - 1;
	char c;
	while (i < j) {
		c = szMsg[i];
		szMsg[i] = szMsg[j];
		szMsg[j] = c;
		i++;
		j--;
	}
}

bool CCommon::IsNumber(const char* szBuf) {
	if('\0' == szBuf[0])
		return false;
	int i = 0;
	while (szBuf[i]) {
		if ('0' > szBuf[i] || '9' < szBuf[i])
			return false;
		i++;
	}
	return true;
}

bool CCommon::IsAlpha(const char* szBuf) {
    if('\0' == szBuf[0])
        return false;
	int i = 0;
	while (szBuf[i]) {
		if (false == IsAlpha(szBuf[i]))
			return false;
		i++;
	}
	return true;
}

bool CCommon::IsSingleWord(const char* szWord) {
	UINT32 uiLen = strlen(szWord);
	if (1 == uiLen || (2 == uiLen && YS_GbkUcsConv::IsGbkCode(szWord)))
		return true;
	return false;
}
/**
 * name:FormatTime
 * input:string &
 * output:bool
 * description:
 format a string to the standard string of time.
 1.get a string
 2.split the string to an array
 3.format the string to a number
 4.use struct tm and get the value
 5.sprintf tm to a string.
 * create : zhouxuhui 2008-10-20
 * modify list:
 */
bool CCommon::FormatTime(const char* szDate, string &strDatetime) {
	strDatetime.clear();
	if (NULL == szDate)
		return false;
	char szDateTimeBuf[100];
	snprintf(szDateTimeBuf, 100, "%s", szDate);
	char* szToken = strchr(szDateTimeBuf, '[');
	if (NULL != szToken)
		*szToken = '\0';
	char buf[20];
	int iyear, imon, iday, ihour, imin, isec, icurrentyear;
	//default value
	time_t tt = time(NULL);
	iyear = icurrentyear = GetYear(tt);
	imon = iday = ihour = imin = isec = 0;

	//replace
	CCommon::RemainNumber(szDateTimeBuf);
	strDatetime = szDateTimeBuf;
	list<string> lstTime;
	CCommon::splitIgnoreSpace(strDatetime, lstTime, ' ');
	list<string>::iterator iterTime = lstTime.begin();
	UINT32 uiSize = lstTime.size();
	if (2 == uiSize) {
		//month day
		imon = atoi((*iterTime).c_str());
		iday = atoi((*(++iterTime)).c_str());
	} else if (3 == uiSize) {
		//year month day
		iyear = atoi((*iterTime).c_str());
		imon = atoi((*(++iterTime)).c_str());
		iday = atoi((*(++iterTime)).c_str());
	} else if (4 == uiSize) {
		imon = atoi((*iterTime).c_str());
		iday = atoi((*(++iterTime)).c_str());
		ihour = atoi((*(++iterTime)).c_str());
		imin = atoi((*(++iterTime)).c_str());
	} else if (5 == uiSize) {
		//year month day hour min sec
		iyear = atoi((*iterTime).c_str());
		imon = atoi((*(++iterTime)).c_str());
		iday = atoi((*(++iterTime)).c_str());
		ihour = atoi((*(++iterTime)).c_str());
		imin = atoi((*(++iterTime)).c_str());
	} else if (uiSize >= 6) {
		//year month day hour min sec
		iyear = atoi((*iterTime).c_str());
		imon = atoi((*(++iterTime)).c_str());
		iday = atoi((*(++iterTime)).c_str());
		ihour = atoi((*(++iterTime)).c_str());
		imin = atoi((*(++iterTime)).c_str());
		isec = atoi((*(++iterTime)).c_str());
	} else {
		//unknow case
		return false;
	}

	//check the scope of value
	if (iyear < icurrentyear - 1 || iyear > icurrentyear + 1)
		return false;
	else if (imon < 1 || imon > 12)
		return false;
	else if (iday < 1 || iday > 31)
		return false;
	else if (ihour < 0 || ihour > 59)
		return false;
	else if (imin < 0 || imin > 59)
		return false;
	else if (isec < 0 || isec > 59)
		return false;

	sprintf(buf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", iyear, imon, iday, ihour, imin, isec);
	strDatetime = buf;
	return true;
}

string::size_type CCommon::find(const string& str, char ch) {
	unsigned char* src = (unsigned char*) str.c_str();
	for (string::size_type i = 0; i < str.length();) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + i)) {
			i += 2;
		} else {
			if (ch == str[i])
				return i;
			i++;
		}
	}
	return string::npos;
}

string::size_type CCommon::find(const string& str, char ch, string::size_type begin) {
	unsigned char* src = (unsigned char*) str.c_str();
	for (string::size_type i = begin; i < str.length();) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + i)) {
			i += 2;
		} else {
			if (ch == str[i])
				return i;
			i++;
		}
	}
	return string::npos;
}

string::size_type CCommon::find(const string& str, const string& ch) {
	unsigned char* src = (unsigned char*) str.c_str();
	UINT32 uiLen = ch.length();
	for (string::size_type i = 0; i < str.length();) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + i)) {
			if(uiLen > 1 && 0 == strcmp((char*) src + i, ch.c_str()))
				return i;
			i += 2;
		} else {
			if (0 == strncmp((char*) src + i, ch.c_str(), uiLen))
				return i;
			i++;
		}
	}
	return string::npos;
}

string::size_type CCommon::find(const string& str, const string& ch, string::size_type begin) {
	unsigned char* src = (unsigned char*) str.c_str();
	UINT32 uiLen = ch.length();
	for (string::size_type i = begin; i < str.length();) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + i)) {
			if(uiLen > 1 && 0 == strcmp((char*) src + i, ch.c_str()))
				return i;
			i += 2;
		} else {
			if (0 == strncmp((char*) src + i, ch.c_str(), uiLen))
				return i;
			i++;
		}
	}
	return string::npos;
}

char* CCommon::StrStr(const char* psrc, const char* ptoken) {
	unsigned char* src = (unsigned char*) psrc;
	UINT32 uiLen = strlen(ptoken);
	string::size_type psrc_len = strlen(psrc);
	for (string::size_type i = 0; i < psrc_len;) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + i)) {
			if(uiLen > 1 && 0 == strncmp((char*) src + i, ptoken, uiLen))
				return ((char*) src + i);
			i += 2;
		} else {
			if (0 == strncmp((char*) src + i, ptoken, uiLen))
				return ((char*) src + i);
			i++;
		}
	}
	return NULL;
}

char* CCommon::StrStr(const char* psrc, const char* ptoken, UINT32 uiTokenLen) {
	unsigned char* src = (unsigned char*) psrc;
	UINT32 uiLen = uiTokenLen;
	string::size_type psrc_len = strlen(psrc);
	for (string::size_type i = 0; i < psrc_len;) {
		if (YS_GbkUcsConv::IsGbkCode((char*) src + i)) {
			if(uiLen > 1 && 0 == strncmp((char*) src + i, ptoken, uiLen))
				return ((char*) src + i);
			i += 2;
		} else {
			if (0 == strncmp((char*) src + i, ptoken, uiLen))
				return ((char*) src + i);
			i++;
		}
	}
	return NULL;
}

time_t CCommon::GetTime(const string &strTimeStamp) {
	// 20090423152911
	if (strTimeStamp.size() < 14)
		return -1;

	string strYear = strTimeStamp.substr(0, 4);
	string strMon = strTimeStamp.substr(4, 2);
	string strDay = strTimeStamp.substr(6, 2);
	string strHour = strTimeStamp.substr(8, 2);
	string strMin = strTimeStamp.substr(10, 2);
	string strSec = strTimeStamp.substr(12);

	struct tm tmTime;
	tmTime.tm_year = atoi(strYear.c_str()) - 1900;
	tmTime.tm_mon = atoi(strMon.c_str()) - 1;
	tmTime.tm_mday = atoi(strDay.c_str());
	tmTime.tm_hour = atoi(strHour.c_str());
	tmTime.tm_min = atoi(strMin.c_str());
	tmTime.tm_sec = atoi(strSec.c_str());
    tmTime.tm_isdst = 0;

	return my_mktime(&tmTime);
}

//快速转换时间的函数
time_t CCommon::my_mktime(struct tm *ptmTime)
{
	if(ptmTime->tm_year >= 100 && ptmTime->tm_year < 200){
		time_t ttTime = tt2000YearTimeArray[ptmTime->tm_year - 100];
		if(ptmTime->tm_mon < 0 || ptmTime->tm_mon > 11)
			return 0;
		if(__isleap(ptmTime->tm_year + 1900))
			ttTime += unMonDaysArray[1][ptmTime->tm_mon] * 86400;
		else
			ttTime += unMonDaysArray[0][ptmTime->tm_mon] * 86400;
		// 再加天的秒数
		ttTime += (ptmTime->tm_mday - 1) * 86400;			
		// 加小时
		ttTime += ptmTime->tm_hour * 3600;
		// 加分钟
		ttTime += ptmTime->tm_min * 60;
		// 加秒数
		ttTime += ptmTime->tm_sec;
		return ttTime;
	}else if(ptmTime->tm_year < 0 || ptmTime->tm_mon < 0 || ptmTime->tm_mon > 11)
		return 0;
	return mktime(ptmTime);
}

//加密手机号码
void CCommon::EncryptMsisdn(char* szMsisdn){
	if( IsNumber(szMsisdn) ){
		//加密手机号码
		UINT32 uiLen = strlen(szMsisdn);
		UINT16 unShift = (szMsisdn[uiLen-8] - '0') + (szMsisdn[uiLen-7] - '0') + (szMsisdn[uiLen-6] - '0') + (szMsisdn[uiLen-5] - '0');
		unShift = unShift % 10;
		UINT8 uc = szMsisdn[uiLen-4] - '0';
		szMsisdn[uiLen-4] = ucEncryptMsisdnArray[unShift][uc] + '0';
		uc = szMsisdn[uiLen-3] - '0';
		szMsisdn[uiLen-3] = ucEncryptMsisdnArray[unShift][uc] + '0';
		uc = szMsisdn[uiLen-2] - '0';
		szMsisdn[uiLen-2] = ucEncryptMsisdnArray[unShift][uc] + '0';
		uc = szMsisdn[uiLen-1] - '0';
		szMsisdn[uiLen-1] = ucEncryptMsisdnArray[unShift][uc] + '0';
	}
}

//解密手机号码
void CCommon::DecryptMsisdn(char* szMsisdn){
	if( IsNumber(szMsisdn) ){
		//解密手机号码
		UINT32 uiLen = strlen(szMsisdn);
		UINT16 unShift = (szMsisdn[uiLen-8] - '0') + (szMsisdn[uiLen-7] - '0') + (szMsisdn[uiLen-6] - '0') + (szMsisdn[uiLen-5] - '0');
		unShift = unShift % 10;
		UINT8 uc = szMsisdn[uiLen-4] - '0';
		szMsisdn[uiLen-4] = ucDecryptMsisdnArray[unShift][uc] + '0';
		uc = szMsisdn[uiLen-3] - '0';
		szMsisdn[uiLen-3] = ucDecryptMsisdnArray[unShift][uc] + '0';
		uc = szMsisdn[uiLen-2] - '0';
		szMsisdn[uiLen-2] = ucDecryptMsisdnArray[unShift][uc] + '0';
		uc = szMsisdn[uiLen-1] - '0';
		szMsisdn[uiLen-1] = ucDecryptMsisdnArray[unShift][uc] + '0';
	}
}
