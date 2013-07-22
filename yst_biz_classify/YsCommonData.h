#ifndef YS_COMMON_DATA_H_
#define YS_COMMON_DATA_H_

#include <ext/hash_map>
using namespace __gnu_cxx; 

#include <string>
#include <list>
using namespace std;

struct stringHashFunc
{
    size_t operator()(const string& str) const
    {
        return __stl_hash_string(str.c_str());
    }
};

typedef list<string> TStrList;
typedef hash_map<string, TStrList, stringHashFunc> TMapList;
typedef TMapList::iterator TMapIter;

#define COLUMN_NUM 2
 
class Ys_WhiteList
{
	public:
		bool StartUp();
		bool StartUp(const string& filepath);
		bool IsInwhiteList(const string& name, TStrList& myList);
	private:
		TMapList m_whiteList;
};

class Ys_ConflictClass
{
	public:
		bool StartUp();
		bool IsConflict(const string& name1, const string& name2);
	private:
		TMapList m_conflictClassList;
};

#endif
