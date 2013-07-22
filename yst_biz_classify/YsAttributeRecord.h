/*
 * Ys_Attribute.h
 *
 * Created on: 2013-1-15
 *   Author : miffa chandler
 *
 */


#ifndef YS_ATTRIBUTE_RECORD_H_
#define YS_ATTRIBUTE_RECORD_H_

#include<string>
#include<vector>
#include<map>

using namespace std;

class Ys_Attribute{
private:
    //属性名
    string  m_strName;
    //该属性下的特征词(关键字)集合
    vector<string> m_WordList;
public:
    Ys_Attribute(const string &str);
     ~Ys_Attribute();
    string GetName();
    bool AddWord(const string &word);
    int GetWordNum();
    int ViewAttribute();
    bool GetWordList(vector<string> &vWordList);
	vector<string>& GetWordListReference();
	const vector<string> & GetWordListReference()const{ 
        return m_WordList ;
    };
    string GetAttributeName();
};


class Ys_Record{
private:
    map<string,Ys_Attribute*> m_mapAttributeList;
    int m_iID;
    string m_strClassName;
public:
    Ys_Record();
    Ys_Record(const int &ID);
    Ys_Record(const Ys_Record &tmp);
    Ys_Record(const int &ID,const string &strClass);
    Ys_Record(const string &strClass);
    
    ~Ys_Record();
    
    int AddAttribute(const string &str); 
    int AddWordToAttribute(const string &str,
                           const vector<string> &WordList);
	int AddWordToAttribute(const string &str,const string &Word);
    int ViewRecord();
    const Ys_Attribute* GetAttributeByName(const string &AttributeName);
    const map<string,Ys_Attribute*> &GetAllAttribute()const;    
    const string& GetClassName()const{return m_strClassName ;};
};

struct YsRetData
{
	string m_className; 
	double m_posProb;
	YsRetData():m_className(""),m_posProb(0.0){}
	YsRetData(const string& name, double data):m_className(name),m_posProb(data){}
	friend bool operator ==(const YsRetData& it1, const YsRetData& it2);
};

struct YsSortComparer
{
	bool operator() (const YsRetData&, const YsRetData&);
};

bool operator ==(const YsRetData& it1, const YsRetData& it2);

#endif /* YS_ATTRIBUTE_RECORD_H_ */




