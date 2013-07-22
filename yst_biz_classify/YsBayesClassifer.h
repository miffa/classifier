/*
 *  YsBayesClassifer.h
 *   
 *   Created on 2013-1-15
 *     Author: miffa chandler
 *
 *
 * */




#ifndef __YSS_BAYES_CLASSIFER_H__
#define __YSS_BAYES_CLASSIFER_H__

#include <ext/hash_map>
using namespace __gnu_cxx;

#include <utility>
#include <functional>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <cmath>
#include "YsAttributeRecord.h"
#include "YsGetRecord.h"
#include "YsWordSegmentLR.h"
#include <cstdlib>

using namespace std;
 

#define LAPLACE_SMOOTH 1.0e-100
#define BUFFER_SIZE 15
union T_VALUE
{
	double m_dPropb;
	long m_lNumber;
};
typedef hash_map<string, T_VALUE, strHashFunc> StrNumberHashData;

typedef hash_map<string, double, strHashFunc> StrDoubleHashData;
typedef StrDoubleHashData::iterator StrDoubleHashDataIter;
typedef StrDoubleHashData::const_iterator StrDoubleHashDataCIter;
typedef StrDoubleHashData::value_type StrDoubleHashDataPair;


typedef hash_map<string, StrDoubleHashData, strHashFunc> AttributeHashData;
typedef AttributeHashData::iterator AttributeHashDataIter;
typedef AttributeHashData::value_type AttributeHashPair; 

typedef hash_map<string,AttributeHashData,strHashFunc> AttributeClassWordProbability;
typedef AttributeClassWordProbability::value_type  AttributeClassWordPair;
typedef AttributeClassWordProbability::iterator  AttributeClassWordIter;

typedef hash_map<string,int,strHashFunc> StrIntHashData;
typedef hash_map<string,StrIntHashData,strHashFunc> StrStrIntHashData;
	
typedef std::set<string> StrSet;
typedef StrSet::iterator StrSetIter;
typedef std::map<string, StrSet> StrMap;
typedef StrMap::value_type StrMapPair;
typedef StrMap::iterator StrMapIter;
typedef std::map<string,bool> StrPathMsp;

    	
template <class T>
bool addHashData(const string& key, T& value, StrDoubleHashData& dataSet);

class Ys_BayesClassifier{
public:
	Ys_BayesClassifier();
	~Ys_BayesClassifier();
    bool StartUp();
    //为Path等变量添加路径
    //改变AddDirToPathSet,参数直接为实例的m_v..set
    static bool AddDirToPathSet(const string &dir,std::map<string,bool> &SomePath);
    bool AddDirToSourcePath(const string &dir);
    bool AddDirToSecSourcePath(const string &dir);
    bool AddDirToPredictPath(const string &dir);
    //从Path变量中获得Record集合
    static bool GetRecordFromPath(std::map<string,bool> &mapPathSet,
                           std::vector<Ys_GetRecord*> &vRecordSet,
                           YS_WordSegmentLR &wordSegmentLR,
                           const bool bIfWordSeg);
    const StrDoubleHashData& GetAttributeWeight()const{ return m_mapAttributePrecision;};
    AttributeClassWordProbability& GetWordProbability(){return m_mapWordProbability;};      
    StrMap& GetAttributeWordList(){return m_mAttributeWordList;};
    StrDoubleHashData& GetClassData(){return m_mapClassDataMap;};

	bool TrainingData();
	bool TrainingServiceData();
	bool PredictingData();
    bool TrainingWeightData();
	
	//
	bool TrainingDataInfo();
	bool TrainingWeightDataInfo();
	bool PredictingByAttribute(const string&, const vector<string>& item , YsRetData& retItem);
	bool GetPostProbByAttribute(const string&, const vector<string>& item , YsRetData& retItem, vector<YsRetData> &vResultClass);

    //从xml文件上获取关键字概率
    bool GetWordProbabilityFromXml();
    //从xml文件上获取类别的先验概率
    bool GetClassProbabilityFromXml();
    //从xml文件上获取属性关键词列表
    static bool GetAttributeWordListFromXml(StrMap& m_mAttributeWordList,const string path);
    //从xml文件中获取属性的权重
    bool GetAttributePrecisionFromXml();
    //从xml文件中获取属性下面关键词的个数
    bool GetAttributeWordNumFromXml();

    //将关键字概率写入xml文件
    bool WriteWordProbabilityToXml();
    //将类别的先验概率写入xml文件
    bool WriteClassProbabilityToXml();
    //将属性下关键词列表写入xml文件
    static bool WriteAttributeWordListToXml(const StrMap m_mAttributeWordList,const string path);
    //将属性预测的正确率写入xml文件
    bool WriteAttributePrecisionToXml();
    //将属性下面关键词个数写入xml文件
    bool WriteAttributeWordNumToXml();

    bool ClassTest();
    bool printWordListInfo();
    bool printAveragePro();

	static bool addWord(const string& attribute, StrSet& strSet);
protected:
	bool AddWordToList(const string& word, const string& attribute);
	bool findWord(const string& word,const string& attribute);
	
	StrSet& findAttributeSet(const string& attribute) { return m_mAttributeWordList[attribute]; };
	bool findWord(const string& attribute, StrSet& strSet);
    //根据某一个属性进行判断 PostProb
    string PredictByAttribute(StrDoubleHashData &mapClassProbability,const Ys_Record &rRecordtmp,const string &strAttributeName);
    string PredictAll(StrDoubleHashData &vResultClass,const Ys_Record &rRecordtmp);
    string GetMostClass(const StrDoubleHashData &vResult);
private:
	
	StrSet m_setAttributeName;
    StrMap m_mAttributeWordList;
    
    StrPathMsp m_mapSourcePath;
    StrPathMsp m_mapSecSourcePath;
    StrPathMsp m_mapPredictPath;
    
    //对第一次训练的数据进行处理得到先验概率和条件概率
    std::vector<Ys_GetRecord*> m_vRecordSet; 

    //对第二次训练的数据进行处理得到每个属性预测的准确度
    std::vector<Ys_GetRecord*> m_vSecTrainRecordSet; 
     
    //对需要分类的数据进行预测分类
    std::vector<Ys_GetRecord*> m_vPredictRecordSet;

    
    //记录某个属性预测的准确度(权重)
    StrDoubleHashData m_mapAttributePrecision;
    //每一个类里面每一个属性中 特证词（关键词）的个数
    AttributeHashData  m_mapAttributeWordNum;
    //条件概率
	AttributeClassWordProbability m_mapWordProbability;  
    //每一个类的先验概率
	StrDoubleHashData m_mapClassDataMap; 
	int m_iAllKeyWordNum;//所有的类中的关键字总数
    int m_iAllRecordNum;
	
	//+20130606
	AttributeHashData m_averagePro;
};



#endif




