#include "YsAdapter.h"
#include "YsClassifierManager.h"
#include "YsCommonData.h"
#include "YsAttributeRecord.h"
#include <algorithm>
#include <math.h>
#include "YsTools.h"
#include "YsTemplateHashData.h"

#include <set>
#include <list>
using namespace std;

bool Ys_ClassifierAdapter::ProcessClassifier(const string& name, const Ys_Record& buseniessData, list<string>& clasName)
{
	INF_LOG("base class processClassifier ");
	return true;
}

bool Ys_ClassifierAdapter::StartUp()
{
	if(!m_conflictManager.StartUp())
	{
		ERR_LOG("conflict list init error ");
		return false;
	}
}
bool Ys_ClassifierAdapter::IsBreak(list<string>& className)
{
	if(className.size()==1)
	{
		return false;
	}
	
	list<string>::iterator pos1=className.begin();
	list<string>::iterator beiter = pos1;
	for(; pos1!=className.end(); ++pos1)
	{
		for(list<string>::iterator pos2=++beiter; pos2!=className.end(); ++pos2)
		{
			if( m_conflictManager.IsConflict(*pos1, *pos2))
			{
				return true;
			}
		}
	}
	return false;
}
	
bool Ys_BayesAdapter::StartUp()
{
	Ys_ClassifierAdapter::StartUp();
	if(!m_bayesClassManager.StartUp())
	{
		ERR_LOG("bayes classifier init error ");
		return false;
	}
	
	return Ys_ClassifierManager::GetInstance()->AddAdapter( T_BEIYES,this);
}

bool Ys_BayesAdapter::ProcessClassifier(const string& name,const Ys_Record& serData, list<string>& className)
{
#ifdef _SINGLE_ATTR
	return 	ProcessClassifierMaxData(name, serData, className);
#else
	return 	ProcessClassifierListData(name, serData, className);
#endif
}

bool Ys_BayesAdapter::ProcessClassifierListData(const string& name,const Ys_Record& serData, list<string>& className)
{
	INF_LOG("Ys_BayesAdapter::ProcessClassifier called  ");
	
	typedef  map<string,Ys_Attribute*> TAttrMap;
	typedef  map<string,Ys_Attribute*>::iterator TAttrMapIter;
	typedef  map<string,Ys_Attribute*>::const_iterator TAttrMapCIter;

	vector<YsRetData> retDataSet;
	retDataSet.clear();
	
	StrDoubleHashData resultProbData;
	
	const TAttrMap& attrList = serData.GetAllAttribute();
	const StrDoubleHashData&  mWeightMap = m_bayesClassManager.GetAttributeWeight();
	for(TAttrMapCIter it=attrList.begin(); it!=attrList.end(); ++it )
	{
		INF_LOG("Ys_BayesAdapter::ProcessClassifier  attribute %s has keywords(%u)   ", it->second->GetName().c_str(), attrList.size());
		if(it->second->GetWordListReference().size()==0)
		{
			ERR_LOG("[%s] no value to predicting  ", it->second->GetName().c_str());
			continue;
		}
		
		double dWeight;
		StrDoubleHashDataCIter iter = mWeightMap.find(it->second->GetName());
		if(iter== mWeightMap.end())
			dWeight = log(LAPLACE_SMOOTH);
		else
			dWeight = log(iter->second);
		
		YsRetData retItem;
		vector<YsRetData> returnProbData;
		if(!m_bayesClassManager.GetPostProbByAttribute(it->second->GetName(), it->second->GetWordListReference(), retItem, returnProbData)) 
		{
			ERR_LOG("[%s] Predicted false", it->second->GetName().c_str());
		}
		else
		{
			//合并Posterior probability
			for(vector<YsRetData>::iterator sIter=returnProbData.begin(); sIter!=returnProbData.end(); ++sIter)
			{
				StrDoubleHashDataIter hmIter = resultProbData.find(sIter->m_className);
				if(hmIter==resultProbData.end())
				{
					resultProbData.insert(StrDoubleHashDataPair(sIter->m_className, (sIter->m_posProb+dWeight)));
				}
				else
				{
					hmIter->second = log( exp(hmIter->second) + exp(sIter->m_posProb+dWeight) );
				}
			}
			
			INF_LOG2("attribute[%s:%s] Predicted true [%s:%e]over  ", it->second->GetName().c_str(), it->second->GetName().c_str(), retItem.m_className.c_str(), retItem.m_posProb);
			
			//合并相同分类的retDataSet item概率
			vector<YsRetData>::iterator retIter = find(retDataSet.begin(), retDataSet.end(), retItem);
			if(retIter == retDataSet.end())
				retDataSet.push_back(retItem); 
			for(int iPos=0; iPos<retDataSet.size(); ++iPos)
			{
				StrDoubleHashDataIter mIter = resultProbData.find(retDataSet[iPos].m_className);
				if(mIter==resultProbData.end())
				{
					ERR_LOG("%s is not in resultProbData  ", retItem.m_className.c_str());
					continue;
				}
				retDataSet[iPos].m_posProb = mIter->second;
			}
		}
	}
	
	if(retDataSet.size()==0) //分类失败
	{
		ERR_LOG("%s retDataSet size ==0, error ", name.c_str());
		return false;
	}
	INF_LOG("retDataSet.size()=%u ", retDataSet.size());
	
	//将结果返回集合降序排序
	sort(retDataSet.begin(), retDataSet.end(), YsSortComparer());

#ifdef _DEBUG
	for(int pos=0; pos<retDataSet.size(); ++pos)
		INF_LOG("%d  [%s:%e]over  ",   pos, retDataSet[pos].m_className.c_str(), retDataSet[pos].m_posProb);
#endif
	
	///1.获取最大后验概率波动5%之间的数据作为备选分类, 2.冲突检测, 3.分类失败检测
 	//1.获取最大后验概率波动5%之间的数据作为备选分类
	vector<YsRetData>::iterator maxIt=retDataSet.begin();
	//className.push_back(maxIt->m_className);
	for(vector<YsRetData>::iterator it=retDataSet.begin(); it!=retDataSet.end(); ++it)
	{
		if( (abs(maxIt->m_posProb - it->m_posProb)) < abs(maxIt->m_posProb*POPRO_BODONG_XISHU))
		{
			className.push_back(it->m_className);
			INF_LOG2("%s is belong to %s  ", name.c_str(), it->m_className.c_str());
		}
		else
		{
			break;
		}
	}
	
	//2.冲突检测
	if(IsBreak(className)) //冲突
	{
		ERR_LOG("result is break  ");
		return false;
	}
	//3.分类失败检测
	//...
	INF_LOG("result is ok  ");
	return true;
}

bool Ys_BayesAdapter::ProcessClassifierMaxData(const string& name,const Ys_Record& serData, list<string>& className)
{
	INF_LOG("Ys_BayesAdapter::ProcessClassifier called  ");
	
	typedef  map<string,Ys_Attribute*> TAttrMap;
	typedef  map<string,Ys_Attribute*>::iterator TAttrMapIter;
	typedef  map<string,Ys_Attribute*>::const_iterator TAttrMapCIter;

	vector<YsRetData> retDataSet;
	set<string> badDataSet;  //记录分类失败类别
	retDataSet.clear();
	badDataSet.clear();
	
	const TAttrMap& attrList = serData.GetAllAttribute();
	const StrDoubleHashData&  mWeightMap = m_bayesClassManager.GetAttributeWeight();
	for(TAttrMapCIter it=attrList.begin(); it!=attrList.end(); ++it )
	{
		INF_LOG("Ys_BayesAdapter::ProcessClassifier  attribute %s has keywords(%u)   ", it->second->GetName().c_str(), attrList.size());
		if(it->second->GetWordListReference().size()==0)
		{
			ERR_LOG("[%s] no value to predicting  ", it->second->GetName().c_str());
			continue;
		}
		
		YsRetData retItem;
		if(!m_bayesClassManager.PredictingByAttribute(it->second->GetName(), it->second->GetWordListReference(), retItem)) 
		{
			ERR_LOG("[%s] Predicted false, nowords in training data  ", it->second->GetName().c_str());
		}
		else
		{
			//后验概率几个过log计算后都为负值, 所以权重计算时采用除法 -5/5  > -5/4
			StrDoubleHashDataCIter iter = mWeightMap.find(it->second->GetName());
			if(iter== mWeightMap.end())
				retItem.m_posProb = retItem.m_posProb+log(LAPLACE_SMOOTH);
			else
				retItem.m_posProb = retItem.m_posProb+log(iter->second);
			INF_LOG("Ys_BayesAdapter::ProcessClassifier  attribute[%s:%s]  [%s:%e]over  ",  it->second->GetName().c_str(), it->second->GetName().c_str(), retItem.m_className.c_str(), retItem.m_posProb);
			
			//合并相同分类的概率
			vector<YsRetData>::iterator retIter = find(retDataSet.begin(), retDataSet.end(), retItem);
			if(retIter != retDataSet.end())
				retIter->m_posProb = log( exp(retIter->m_posProb) + exp(retItem.m_posProb) );
			else
				retDataSet.push_back(retItem); 
		}
	}
	
	if(retDataSet.size()==0) //分类失败
	{
		ERR_LOG("retDataSet size ==0, error ");
		return false;
	}
	INF_LOG("retDataSet.size()=%u ", retDataSet.size());
	
	//将结果返回集合降序排序
	sort(retDataSet.begin(), retDataSet.end(), YsSortComparer());

	
#ifdef _DEBUG
	for(int pos=0; pos<retDataSet.size(); ++pos)
		printf("%d  [%s:%e]over  ",   pos, retDataSet[pos].m_className.c_str(), retDataSet[pos].m_posProb);
#endif
	
	///1.获取最大后验概率波动5%之间的数据作为备选分类, 2.冲突检测, 3.分类失败检测
 	//1.获取最大后验概率波动5%之间的数据作为备选分类
	vector<YsRetData>::iterator maxIt=retDataSet.begin();
	for(vector<YsRetData>::iterator it=retDataSet.begin(); it!=retDataSet.end(); ++it)
	{
		if( (abs(maxIt->m_posProb - it->m_posProb)) < abs(maxIt->m_posProb*POPRO_BODONG_XISHU))
		{
			className.push_back(it->m_className);
			INF_LOG("%s is belong to %s  ", name.c_str(), it->m_className.c_str());
		}
		else
		{
			break;
		}
	}
	
	//2.冲突检测
	if(IsBreak(className)) //冲突
	{
		INF_LOG("result is break  ");
		return false;
	}
	//3.分类失败检测

	
	INF_LOG("result is ok  ");
	return true;
}

bool Ys_WhiteAdapter::StartUp()
{
	if(!m_whiteListManager.StartUp())
	{
		ERR_LOG("Ys_WhiteAdapter init error ");
		return false;
	}
	//return true;
	return Ys_ClassifierManager::GetInstance()->AddAdapter( T_WHITE,this);
}
	
bool Ys_WhiteAdapter::ProcessClassifier(const string& name,const Ys_Record& buseniessData, list<string>& clasName)
{
	//todo:
	
	return m_whiteListManager.IsInwhiteList(name, clasName);
}

/**************************/
bool Ys_TagWhiteAdapter::StartUp()
{
	string sufixFile = string(getenv("HOME")) + "/tools/trunk/data/whitelist_data/tag_whitelist.txt";
	if(!m_whiteListManager.StartUp(sufixFile))
	{
		ERR_LOG("Ys_WhiteAdapter init error ");
		return false;
	}
	//return true;
	return Ys_ClassifierManager::GetInstance()->AddAdapter( T_TAG_WHITE, this);
}
	
bool Ys_TagWhiteAdapter::ProcessClassifier(const string& name,const Ys_Record& buseniessData, list<string>& clasName)
{
	const Ys_Attribute* attriNode = const_cast<Ys_Record&> (buseniessData).GetAttributeByName("tag");
	if( attriNode == NULL )
	{
		ERR_LOG("not tag attribute in name:[%s]", name.c_str());
		return false;
	}
	const vector<string>& wordlist = attriNode->GetWordListReference();
	if(wordlist.size() == 0)
	{
		ERR_LOG("not tag attribute words in  name:[%s]", name.c_str());
		return false;
	}
	return m_whiteListManager.IsInwhiteList(wordlist[0], clasName);
}

/***********************/

bool Ys_DecsionTreeAdapter::StartUp()
{
	Ys_ClassifierAdapter::StartUp();
	if(!m_decisionForest.StartUp())
	{
		ERR_LOG("Ys_DecsionTreeAdapter init error ");
		return false;
	}
	//return true;
	return Ys_ClassifierManager::GetInstance()->AddAdapter( T_JUECESHU, this);
}

bool Ys_DecsionTreeAdapter::ProcessClassifier(const string& name,const Ys_Record& businessData, list<string>& classNames)
{
	typedef  map<string,Ys_Attribute*> TAttrMap;
	typedef  map<string,Ys_Attribute*>::iterator TAttrMapIter;
	typedef  map<string,Ys_Attribute*>::const_iterator TAttrMapCIter;
	
	YS_StrHashMap<int> classFlag;
	int iMaxCounter = 0;
	string strMaxClass("");
	const TAttrMap& attrList = businessData.GetAllAttribute();
	for(TAttrMapCIter it=attrList.begin(); it!=attrList.end(); ++it )
	{
		
		INF_LOG("Ys_DecsionTreeAdapter::ProcessClassifier  attribute %s has keywords(%u)   ", it->second->GetName().c_str(), attrList.size());
		if(it->second->GetWordListReference().size()==0)
		{
			ERR_LOG("[%s] no value to predicting  ", it->second->GetName().c_str());
			continue;
		}
		
		vector<string>& items = it->second->GetWordListReference();
		list<string> lstItems;
		for(int iPos=0; iPos<items.size(); ++iPos)
			lstItems.push_back(items[iPos]);
		string className("");
		if(!m_decisionForest.PredictingByAttribute(it->second->GetName(), lstItems, className))
		{
			ERR_LOG("[%s] Predicted false", it->second->GetName().c_str());
		}
		else
		{
			///*
			int counter = 1;
			if(!classFlag.FindKeyValue(className, counter))// not exist
				classFlag.AddKeyValue(className, counter);
			else
			{
				counter += 1;
				classFlag.ReplaceKeyValue(className, counter); //exist
			}
			
			if( counter > iMaxCounter )
			{
				strMaxClass = className;
				iMaxCounter = counter;
			}
			///*/
			classNames.push_back(className);
		}
	}
	if(iMaxCounter > 0)
	{
		classNames.clear();
		classNames.push_back(strMaxClass);
	}
	else
	{
		return false;
	}
	//2.冲突检测
	if(IsBreak(classNames)) //冲突
	{
		ERR_LOG("result is break  ");
		return false;
	}
	//3.分类失败检测
	//...
	INF_LOG("result is ok  ");
	return true;
}

/**************************/
bool Ys_TireTreeAdapter::StartUp()
{
   
	if(!m_TiredTree.InitTrie())
	{
		ERR_LOG("TiredTreeAdapter init error ");
		return false;
	}

	string sufixFile = string(getenv("HOME")) + "/tools/trunk/data/whitelist_data/sufixTree.txt";
	YS_FileProcessor sufixTreeFile(sufixFile.c_str());
	if(!sufixTreeFile.OpenForRead())
	{
		ERR_LOG("sufixTreeFile open err :[%s]", sufixFile.c_str());
		return false;
	}
	//INF_LOG("open file  %s  ",strFile.c_str());
	list<string> wordlist;
	list<string> itemlist;
	char cLineBuf[__BUFFER_LEN];
	while(!sufixTreeFile.IsEnd())
	{
		sufixTreeFile.ReadLine( cLineBuf, __BUFFER_LEN);
		wordlist.clear();
		itemlist.clear();
		CCommon::splitIgnoreSpace(cLineBuf, itemlist, '|');
		if(itemlist.size()!=2)continue;
		list<string>::iterator niter = itemlist.begin();
		list<string>::iterator viter = niter;
		viter++;
		INF_LOG("%s:%s insert into tired tree", niter->c_str(), viter->c_str());
		TOOLS::partialWord(*(niter), wordlist);
		m_TiredTree.Insert(wordlist, *(viter));		
	}
	sufixTreeFile.CloseFile();	
	INF_LOG("sufixtree init ok");
	//return true;
	return Ys_ClassifierManager::GetInstance()->AddAdapter( T_TIRETREE, this);
}
	
bool Ys_TireTreeAdapter::ProcessClassifier(const string& name,const Ys_Record& buseniessData, list<string>& clasName)
{
	//const Ys_Attribute* attriNode = const_cast<Ys_Record&> (buseniessData).GetAttributeByName("tag");
	/*if( attriNode == NULL )
	{
		ERR_LOG("not tag attribute in name:[%s]", name.c_str());
		return false;
	}*/
	
	string value;
	list<string> namelist;
	TOOLS::partialWord(name, namelist);
	bool bFlag = m_TiredTree.Find (namelist, value);
	if(bFlag)
		clasName.push_back(value);
	return bFlag;
}