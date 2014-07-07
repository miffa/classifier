/**/
#include "YsHashData.h"
//#include "YsTools.h"
/***********************/
/*common hash function */
/***********************/
size_t StringHashFunc::operator() (const string& str)const
{
	return __stl_hash_string(str.c_str());
}

/***********************/
/*  YS_KeyHashSet      */
/***********************/
bool YS_KeyHashSet::AddLable(const string& item)
{
	T_StrHashSetIter iter = m_labelList.find(item);
	if( m_labelList.end() == iter)
	{
		return m_labelList.insert(item).second;
	}
	return true;
}

bool YS_KeyHashSet::FindLabel(const string& item)const
{
	return m_labelList.find(item) != m_labelList.end();
}

void YS_KeyHashSet::ChangeToArray(list<string>& value)
{
	
	for(T_StrHashSetIter iter = m_labelList.begin();m_labelList.end()!=iter; ++iter)
	{
		value.push_back(*iter);
	}
}

YS_DataNode* YS_KeyHashSet::ChangeToArray(YS_DataNode*& head)
{
	for(T_StrHashSetIter iter = m_labelList.begin();m_labelList.end()!=iter; ++iter)
	{
		if(head==NULL)
		{
			head = new YS_DataNode(*iter, NULL);
		}
		else
		{
			YS_DataNode* node = new YS_DataNode(*iter, head);
			head = node;
		}
	}
	return head;
}

void YS_KeyHashSet::PrintInfo()
{
	for(T_StrHashSetIter iter = m_labelList.begin();m_labelList.end()!=iter; ++iter)
	{
		printf("%s|", iter->c_str());
	}	
}
/***********************/
/*  YS_KeyHashMap      */
/***********************/
YS_KeyHashMap::YS_KeyHashMap()
{
	m_keyHashMap.clear();
}

YS_KeyHashMap::~YS_KeyHashMap()
{
	Clear();
}

void YS_KeyHashMap::PrintInfo()
{
	for(T_KeyHashMapIter it=m_keyHashMap.begin(); it!=m_keyHashMap.end(); ++it)
	{
		printf("####  key :%s\n",it->first.c_str());
		it->second->PrintInfo();
		printf("###\n");
	}
}

void YS_KeyHashMap::Clear()
{
	for(T_KeyHashMapIter it=m_keyHashMap.begin(); it!=m_keyHashMap.end(); ++it)
		delete it->second;
	m_keyHashMap.clear();
}

bool YS_KeyHashMap::AddKeyValue(const string& item, const string& value)
{
	YS_KeyHashSet* finder = NULL;
	T_KeyHashMapIter iter = m_keyHashMap.find(item);
	if(iter == m_keyHashMap.end())
	{
		finder = new(nothrow) YS_KeyHashSet();
		if(finder == NULL)
		{
			//ERR_LOG("new operator error");
			return false;
		}
		
		if(!m_keyHashMap.insert(T_KeyHashMapPair(item, finder)).second)
		{
			//ERR_LOG(" insert YS_KeyHashSet to m_keyHashMap error");
			delete finder;
		}
	}
	else
		finder = iter->second;

	return finder->AddLable(value);
}

bool YS_KeyHashMap::FindKeyValue(const string& item, const string& value)const
{
	const YS_KeyHashSet* finder = NULL;
	if( (finder=getHashSet(item)) == NULL)
		return false;

	return finder->FindLabel(value);
}

const YS_KeyHashSet* YS_KeyHashMap::getHashSet(const string& item)const
{
	T_KeyHashMapConstIter iter = m_keyHashMap.find(item);
	if(iter == m_keyHashMap.end())
		return NULL;
	else
		return iter->second;
}


