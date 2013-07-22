#ifndef __YSS_ADAPTER_H__
#define __YSS_ADAPTER_H__

#include "YsCommonData.h"
#include "YsBayesClassifer.h"
#include "YsDecisionTree.h"
#include "YsTrieTree.h"

#include <string>
using namespace std;


enum YS_CLASSIFIER_TYPE_T { T_BEIYES, T_JUECESHU, T_WHITE,T_TAG_WHITE, T_TIRETREE, TUNKNOWN };
class Ys_ClassifierAdapter
{
public:
	
	Ys_ClassifierAdapter(YS_CLASSIFIER_TYPE_T type):m_adapterType(type){};
	//第一个参数为属性名称，第二个参数为一条记录，第三个为结果集合
	virtual bool ProcessClassifier(const string& name, const Ys_Record& buseniessData, list<string>& clasName);
	virtual ~Ys_ClassifierAdapter(){};
	bool StartUp();
	//判断是否冲突
	bool IsBreak(list<string>& clasName);

protected:
	//分类器类型
	YS_CLASSIFIER_TYPE_T m_adapterType;	
	Ys_ConflictClass m_conflictManager;
};

typedef hash_map<string, double, strHashFunc> TKeyDataMap;
typedef TKeyDataMap::iterator   TKeyDataIter;

class Ys_BayesAdapter:public Ys_ClassifierAdapter
{
public:
	Ys_BayesAdapter():Ys_ClassifierAdapter(T_BEIYES){}; 
	virtual ~Ys_BayesAdapter(){};
	
	bool StartUp();

	virtual bool ProcessClassifier(const string& name, const Ys_Record& buseniessData, list<string>& clasName);
protected:
	//get class whoes Posterior probability is max
	bool ProcessClassifierMaxData(const string& name, const Ys_Record& buseniessData, list<string>& clasName);
	//get Posterior probability for every class
	bool ProcessClassifierListData(const string& name, const Ys_Record& buseniessData, list<string>& clasName);
private:
	Ys_BayesClassifier m_bayesClassManager;
	static const double POPRO_BODONG_XISHU = 0.30;//后验概率波动系数
};

class Ys_WhiteAdapter:public Ys_ClassifierAdapter
{
public:
	Ys_WhiteAdapter():Ys_ClassifierAdapter(T_WHITE){};
	virtual ~Ys_WhiteAdapter(){};
	
	bool StartUp();
	
	virtual bool ProcessClassifier(const string& name, const Ys_Record& buseniessData, list<string>& clasName);
private:
	Ys_WhiteList m_whiteListManager;
};

class Ys_TagWhiteAdapter:public Ys_ClassifierAdapter
{
public:
	Ys_TagWhiteAdapter():Ys_ClassifierAdapter(T_TAG_WHITE){};
	virtual ~Ys_TagWhiteAdapter(){};
	
	bool StartUp();
	
	virtual bool ProcessClassifier(const string& name, const Ys_Record& buseniessData, list<string>& clasName);
private:
	Ys_WhiteList m_whiteListManager;
};

class Ys_DecsionTreeAdapter:public Ys_ClassifierAdapter
{
public:
	Ys_DecsionTreeAdapter():Ys_ClassifierAdapter(T_JUECESHU){};
	virtual ~Ys_DecsionTreeAdapter(){};
	
	bool StartUp();
	
	bool ProcessClassifier(const string& name,const Ys_Record& businessData, list<string>& classNames);
private:
	YS_DecisionForest m_decisionForest;
};

class Ys_TireTreeAdapter:public Ys_ClassifierAdapter
{
public:
	Ys_TireTreeAdapter():Ys_ClassifierAdapter(T_TIRETREE){};
	virtual ~Ys_TireTreeAdapter(){};
	
	bool StartUp();
	
	virtual bool ProcessClassifier(const string& name, const Ys_Record& buseniessData, list<string>& clasName);
private:
	YsTireTree m_TiredTree;
	static const int __BUFFER_LEN = 1024;
};

#endif
