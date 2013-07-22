#include "YsCommonData.h"
#include "YsTools.h"
#include "YssCommon.h"
bool Ys_WhiteList::StartUp()
{
	string strFile = YS_ConfigData::GetInstance()->m_strWhiteList;
	INF_LOG("whist listpath is  %s", strFile.c_str());
	string strData;
	//CCommon common;
	if ( !CCommon::LoadFile(strFile, strData))
	{
		ERR_LOG("open whitelist strFile failed ");
		return false;
	}
	
	list<string> listLine;
	CCommon::splitIgnoreSpace(strData, listLine, '\n');
	INF_LOG("whist list size is  %ld", listLine.size());
	for (list<string>::iterator it=listLine.begin(); it!=listLine.end();it++)
	{
		list<string> templist;
		CCommon::splitIgnoreSpace(*it,templist,'|');
		if (COLUMN_NUM != templist.size())
			continue;
		list<string>::iterator iter = templist.begin();
		string name = *iter;
		string value = *(++iter);
		list<string> value_list;
		CCommon::splitIgnoreSpace(value,value_list, ',');
		pair<TMapIter,bool> remark = m_whiteList.insert(TMapList::value_type(name,value_list));
		if (!remark.second)
		{
			ERR_LOG("one insert failed ");
		}
		else
		{
			INF_LOG("[%s] insert into whitelist " , name.c_str());
		}
	}
	INF_LOG("white list 	init ok ==========================" );
	return true;
}

bool Ys_WhiteList::StartUp(const string& filepath)
{
	string strFile = filepath;
	INF_LOG("white tag listpath is  %s", strFile.c_str());
	string strData;
	//CCommon common;
	if ( !CCommon::LoadFile(strFile, strData))
	{
		ERR_LOG("open whitelist strFile failed ");
		return false;
	}
	
	list<string> listLine;
	CCommon::splitIgnoreSpace(strData, listLine, '\n');
	INF_LOG("whist list size is  %ld", listLine.size());
	for (list<string>::iterator it=listLine.begin(); it!=listLine.end();it++)
	{
		list<string> templist;
		CCommon::splitIgnoreSpace(*it,templist,'|');
		if (COLUMN_NUM != templist.size())
			continue;
		list<string>::iterator iter = templist.begin();
		string name = *iter;
		string value = *(++iter);
		list<string> value_list;
		CCommon::splitIgnoreSpace(value,value_list, ',');
		pair<TMapIter,bool> remark = m_whiteList.insert(TMapList::value_type(name,value_list));
		if (!remark.second)
		{
			ERR_LOG("one insert failed ");
		}
		else
		{
			INF_LOG("[%s] insert into whitelist " , name.c_str());
		}
	}
	INF_LOG("white list 	init ok ==========================" );
	return true;
}
bool Ys_WhiteList::IsInwhiteList(const string& name, TStrList& myList)
{
	myList.clear();
	TMapIter iter;
	if ((iter=m_whiteList.find(name))!=m_whiteList.end())
	{
		myList = iter->second;
		INF_LOG("name:%s in whitelist ",  name.c_str());
		return true;;
	}
	INF_LOG("name:%s is not in whitelist  ",  name.c_str());
	return false;
}


bool Ys_ConflictClass::StartUp()
{
	string fFilePath = YS_ConfigData::GetInstance()->m_strCollfictList;
	string strData;
	if ( !CCommon::LoadFile(fFilePath, strData))
	{
		ERR_LOG("open CollfictList strFile failed ");
		return false;
	}
	list<string> listLine;
	CCommon::splitIgnoreSpace(strData, listLine, ' ');
	for (list<string>::iterator iter=listLine.begin(); iter!=listLine.end(); iter++)
	{
		list<string> templist;
		CCommon::splitIgnoreSpace(*iter,templist,'|');
		if (COLUMN_NUM!=templist.size())
			continue;
		list<string>::iterator it = templist.begin();
		string name=*it++;
		string value=*it;
		list<string> value_list;
		CCommon::splitIgnoreSpace(value,value_list, ',');
		pair<TMapIter,bool> remark = m_conflictClassList.insert(TMapList::value_type(name,value_list));
		if (!remark.second)
		{
			ERR_LOG("one insert failed");
		}
	}
	return true;
}

bool Ys_ConflictClass::IsConflict(const string& name1, const string& name2)
{
	TMapIter iter;
	if ((iter=m_conflictClassList.find(name1))!=m_conflictClassList.end())
	{
		for (list<string>::iterator it=iter->second.begin();it!=iter->second.end();it++)
		{
			if (*it==name2)
				return true;
		}		
	}
	if ((iter=m_conflictClassList.find(name2))!=m_conflictClassList.end())
	{
		for (list<string>::iterator it=iter->second.begin();it!=iter->second.end();++it)
		{
			if (*it==name1)
				return true;
		}		
	}
	return false;
}

	
	
