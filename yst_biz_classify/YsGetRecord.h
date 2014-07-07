/*
 *YsGetRecord.h
 *  Created on 2013-1-16
 *      Author:miffa chandler
 *
 */

#ifndef YS_GETRECORD_H_
#define YS_GETRECORD_H_


#define MAX_LENGTH 1000

#include "YsAttributeRecord.h"
#include "YsTools.h"
#include "YsWordSegmentLR.h"

#include <string>
using namespace std;

class Ys_FileRecordList
{
private:
    string m_strClassName;
    string m_strFileNumber;
    vector <Ys_Record*> m_vRecordList;
public:
    Ys_FileRecordList(const string strClassName,const string strFileNumber);
    ~Ys_FileRecordList();
    const string& GetClassName()const
    {
        return m_strClassName;
    }
    const string& GetFileNumber()const
    {
        return m_strFileNumber;
    }
    const vector<Ys_Record*>& GetRecordList();
    bool AddRecord(Ys_Record* Recordtmp);
};

//this a bad class design
class Ys_GetRecord{
private:
    string m_strPath;
    bool m_boolHaveFile;
    vector <Ys_FileRecordList*> m_vRecordFileList;
public:
    const string& GetPath()const{ return m_strPath; };
    Ys_GetRecord(const string &dir);
    ~Ys_GetRecord();
    bool StartGet(YS_WordSegmentLR &wordSegment,const bool bIfWordSegment);
    const vector<Ys_FileRecordList*>& GetFileRecordList(); 
	void GetWordList(const string &strWhole,vector<string> &strWordList, char delimiter);
	string trim(const string &s);
};


//
#endif /*YS_GETRECORD_H_*/

