/*template class*/
/*string hash map  <string, type T>*/
#include <string>
using namespace std;

#include <ext/hash_map>
using namespace __gnu_cxx;

struct StrHashFunc  //hash 函数
{
	size_t operator() (const string& str)const
	{
		__stl_hash_string(str.c_str());
	}
};

template <class T>
class YS_StrHashMap
{
public:
	YS_StrHashMap(){
		m_strHashMap.clear();
		m_bFlag = false;
	}
	~YS_StrHashMap(){
		m_strHashMap.clear();
	}
	bool AddKeyValue(const string& item, const T& value){
		return m_strHashMap.insert(T_StrHashMapPair(item, value)).second;
	}
	bool ReplaceKeyValue(const string& item, const T& value){
		T_StrHashMapIter iter = m_strHashMap.find(item);
		if(iter == m_strHashMap.end())
		{
			return false;
		}
		else
			iter->second = value;

		return true;
	}
	bool FindKeyValue(const string& item, T& value){
		T_StrHashMapIter iter = m_strHashMap.find(item);
		if(iter == m_strHashMap.end())
			return false;
		value = iter->second;
		return true;
	}
	int size(){ return m_strHashMap.size();}
	void Clear(){ m_strHashMap.clear();}
	//iterator
	void Begin(){ m_iterPointer = m_strHashMap.begin(); m_bFlag = true; }
	void Next(){ if(m_bFlag)++m_iterPointer; }
	bool IsEnd(){ 
		if(m_iterPointer == m_strHashMap.end()){ m_bFlag = false; return true;}
		else return false;
	}
	const string& IterFirst(){ 
		if(!m_bFlag)m_iterPointer = m_strHashMap.begin();
		return m_iterPointer->first;
	}
	T& IterSecond(){
		if(!m_bFlag)m_iterPointer = m_strHashMap.begin();
		return m_iterPointer->second;
	}
	/*YS_StrHashMap a; a.Begin(); while(!a.IsEnd()){ a.IterFirst();a.IterSceond();}*/
public:
	typedef hash_map<string, T, StrHashFunc> T_StrHashMap;
	typedef typename T_StrHashMap::iterator T_StrHashMapIter;
	typedef typename T_StrHashMap::const_iterator T_StrHashMapConstIter;
	typedef typename T_StrHashMap::value_type T_StrHashMapPair;	
	
private:
	T_StrHashMap m_strHashMap;
	T_StrHashMapIter m_iterPointer;
	bool m_bFlag;
};

