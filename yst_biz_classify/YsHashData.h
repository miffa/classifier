
#ifndef _YS_HASHDATA_H_
#define _YS_HASHDATA_H_

#include <ext/hash_map>
#include <ext/hash_set>
using namespace __gnu_cxx;

#include <string>
#include <list>
using namespace std;

typedef list<string> T_StrList;
typedef T_StrList::const_iterator T_StrConstListIter;
typedef T_StrList::iterator T_StrListIter;

struct StringHashFunc  //hash 函数
{
	size_t operator() (const string& str)const;
};

struct StrCompare   //判断是否相等， 两个值hash统一位置，需要调用此函数判断主键是否存在
{
    bool operator()(const string& str, const string& str1) const
    {
        return strcmp(str.c_str(), str1.c_str())==0;
    }
};
typedef hash_set<string, StringHashFunc, StrCompare> T_StrEqueHashSet;

struct YS_DataNode
{
	string m_strValue;
	YS_DataNode* m_next;
	YS_DataNode(const string& va, YS_DataNode* ne):m_strValue(va),m_next(ne){}
};

class YS_KeyHashSet
{
public:
	YS_KeyHashSet(){ m_labelList.clear();}
	~YS_KeyHashSet(){ m_labelList.clear();}
	bool AddLable(const string& item);
	bool FindLabel(const string& item)const;
	int size(){ return m_labelList.size();};
	void Clear(){ m_labelList.clear();};
	void ChangeToArray(list<string>& value);
	YS_DataNode* ChangeToArray(YS_DataNode*& head);
	void PrintInfo();
private:
	typedef hash_set<string, StringHashFunc> T_StrHashSet;
	typedef T_StrHashSet::iterator T_StrHashSetIter;
	typedef T_StrHashSet::const_iterator T_StrHashSetConstIter;
	T_StrHashSet m_labelList;
};


class YS_KeyHashMap
{
public:
	YS_KeyHashMap();
	~YS_KeyHashMap();
	bool AddKeyValue(const string& item, const string& value);
	bool FindKeyValue(const string& item, const string& value)const;
	const YS_KeyHashSet* getHashSet(const string& item)const;
	int size(){ return m_keyHashMap.size();};
	void Clear();
	void PrintInfo();
	
public:
	typedef hash_map<string, YS_KeyHashSet*, StringHashFunc> T_KeyHashMap;
	typedef T_KeyHashMap::iterator T_KeyHashMapIter;
	typedef T_KeyHashMap::const_iterator T_KeyHashMapConstIter;
	typedef T_KeyHashMap::value_type T_KeyHashMapPair;
private:
	T_KeyHashMap m_keyHashMap;
};

#endif
