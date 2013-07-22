/*
 *YsBayesClassifer.cpp 
 *  Created on 2013-1-15
 *   Author:miffa chandler
 *
 * */

#include "YsBayesClassifer.h"
#include "YsClassifierManager.h"
#include "YsTools.h"
#include "YsWordSegmentLR.h"
#include <iostream>
#include "tinyxml.h"
#include "tinystr.h"
#include <cstdio>

Ys_BayesClassifier::Ys_BayesClassifier(){
    m_mapSourcePath.clear(); 
    m_mapPredictPath.clear();
    m_mAttributeWordList.clear();

    m_vRecordSet.clear();
    m_iAllKeyWordNum = 0 ;
    m_iAllRecordNum = 0;
}

Ys_BayesClassifier::~Ys_BayesClassifier(){
     
}
bool Ys_BayesClassifier::AddDirToPathSet(const string &dir,std::map<string,bool> &SomePath){
    if(SomePath.find(dir)!= SomePath.end()){
        ERR_LOG("%s has been in the source path set",dir.c_str());
        return false;
    }
    SomePath.insert(std::pair<string,bool>(dir,false));
    return true; 
}

//获取已分类的训练数据
bool Ys_BayesClassifier::GetRecordFromPath(std::map<string,bool> &mapPathSet,
                                           std::vector<Ys_GetRecord *> &vRecordSet,
                                           YS_WordSegmentLR &wordSegment,
                                           const bool bIfWordSegment){        
    std::map<string,bool>::iterator it;
    for(it = mapPathSet.begin();it != mapPathSet.end();++it){
        if(it->second == true)
            continue;
        Ys_GetRecord* tmp = new Ys_GetRecord(it->first);
        if(tmp->StartGet(wordSegment,bIfWordSegment)){
            vRecordSet.push_back(tmp);
            it->second = true;
        }
    }
    return true;
}

//获取待分类的数据
bool Ys_BayesClassifier::TrainingData()
{
    if(m_vRecordSet.size()==0){
        ERR_LOG("There is no data to calculate\n");
        return false;
    }
    for(unsigned int i=0;i<m_vRecordSet.size();i++)
    {
        const vector<Ys_FileRecordList*>& vFileRecordList = m_vRecordSet[i]->GetFileRecordList(); 
        //INF_LOG("train record num   %d \n", vRecordList.size());
		//遍历所有记录
        for(unsigned int j=0;j<vFileRecordList.size();j++)
        {
            //读取一条记录
            const vector<Ys_Record*>& Recordtmp = vFileRecordList[j]->GetRecordList();
            for(unsigned int k =0 ;k<Recordtmp.size();++k)
            {
                Ys_Record* tmp = Recordtmp[k];
            
                if(tmp->GetClassName()=="")
                    continue;                           //如果该条记录不存在类别信息则跳过
		        AttributeClassWordIter itAttClassWord; 
                AttributeHashDataIter itAttrHash;     
                //判断该记录所属的类是否存在，不存在将该类的记录数记为1，存在则加1；
                //count the number of records belong to every class
                StrDoubleHashDataIter dataIter;
			    if((dataIter=m_mapClassDataMap.find(tmp->GetClassName()))==m_mapClassDataMap.end())
                { 
                    m_mapClassDataMap.insert(StrDoubleHashDataPair(tmp->GetClassName(),1));
                }
                else
                {
                    dataIter->second += 1.0;
                }
				
                const map<string,Ys_Attribute*>& mapAllAttribute = tmp->GetAllAttribute();
                map<string,Ys_Attribute*>::const_iterator constItAttribute;
                //traverse every attribute item
                for(constItAttribute = mapAllAttribute.begin();constItAttribute != mapAllAttribute.end();++constItAttribute)
                {
                    //not found the attribute name
                    //then add all the word under the attribute to the item

                    StrSet& tmpStrSet = findAttributeSet(constItAttribute->first);
                
                    if((itAttClassWord=m_mapWordProbability.find(constItAttribute->first)) == m_mapWordProbability.end())
                    {
                        StrDoubleHashData mapAttributeWordNum;
                        mapAttributeWordNum.clear();
                        AttributeHashData  ClassHashDatatmp;
                        ClassHashDatatmp.clear();
                        StrDoubleHashData mapWordtmp;
                        mapWordtmp.clear();
                        vector<string> tmpWordList = constItAttribute->second->GetWordListReference();
                     
                        m_setAttributeName.insert(constItAttribute->first);
                        for(int i=0;i<tmpWordList.size();i++)
                        {   
                            mapWordtmp.insert(StrDoubleHashDataPair(tmpWordList[i],1.0)); 
                            addWord(tmpWordList[i],tmpStrSet);           
                        }
                        //m_mapAttributeWordNum didn't have the attribute item;
                        mapAttributeWordNum.insert(StrDoubleHashDataPair(tmp->GetClassName(),tmpWordList.size()));
                        m_mapAttributeWordNum.insert(AttributeHashPair(constItAttribute->first,mapAttributeWordNum));

                        ClassHashDatatmp.insert(AttributeHashPair(tmp->GetClassName(),mapWordtmp));
                        m_mapWordProbability.insert(AttributeClassWordPair(constItAttribute->first,ClassHashDatatmp)); 
                    
                        continue;
                     
                    }    
                    //found the attribute name
                    //then begin find the class name
                    //not found then

                    if((itAttrHash=itAttClassWord->second.find(tmp->GetClassName()))==itAttClassWord->second.end())
                    {
                        StrDoubleHashData mapWordtmp;
                        mapWordtmp.clear();
                        vector<string> tmpWordList = constItAttribute->second->GetWordListReference();
                        for(int i = 0;i<tmpWordList.size();i++)
                        {
                            mapWordtmp.insert(StrDoubleHashDataPair(tmpWordList[i],1.0));
                            addWord(tmpWordList[i],tmpStrSet);           
                        }
                        m_mapAttributeWordNum[itAttClassWord->first].insert(StrDoubleHashDataPair(tmp->GetClassName(),tmpWordList.size()));
                        itAttClassWord->second.insert(AttributeHashPair(tmp->GetClassName(),mapWordtmp));
                        continue;
                    }
                //found the class name
                    vector<string> tmpWordList = constItAttribute->second->GetWordListReference();
                    for(int i = 0 ; i<tmpWordList.size();i++)
                    {
                        StrDoubleHashDataIter itWord;
                        if((itWord = itAttrHash->second.find(tmpWordList[i])) == itAttrHash->second.end())
                        {
                            itAttrHash->second.insert(StrDoubleHashDataPair(tmpWordList[i],1.0));
                            continue;
                        }
                        itWord->second += 1;
                        addWord(tmpWordList[i],tmpStrSet);           
                    }
                    m_mapAttributeWordNum[itAttClassWord->first][tmp->GetClassName()] += tmpWordList.size();
                
                }
                m_iAllRecordNum++;
            }//delete tmp;
			//tmp = NULL;
        }//end for vRecordList
		INF_LOG("train file number   %u   over \n", vFileRecordList.size());
    }
    StrDoubleHashData::iterator it; //记录每一个类的概率的map的迭代器
    for(it=m_mapClassDataMap.begin();it!=m_mapClassDataMap.end();++it){
        it->second /=m_iAllRecordNum;
    }
    //compute every word probability
    hash_map<string,AttributeHashData,strHashFunc>::iterator itAttr;
    for(itAttr = m_mapWordProbability.begin();itAttr!=m_mapWordProbability.end();++itAttr){
        AttributeHashData::iterator itClass;
		StrDoubleHashData averageProItem; //+20130606
        for(itClass = itAttr->second.begin();itClass!=itAttr->second.end();++itClass)
        {
            averageProItem.insert(StrDoubleHashDataPair(itClass->first, static_cast<double>(itClass->second.size())));//+20130606
			StrDoubleHashData::iterator itWord;
            m_mapAttributeWordNum[itAttr->first][itClass->first] += m_mapWordProbability[itAttr->first][itClass->first].size()*LAPLACE_SMOOTH;
            for(itWord = itClass->second.begin();itWord!=itClass->second.end();++itWord)
            {
                itWord->second = (itWord->second+LAPLACE_SMOOTH) / m_mapAttributeWordNum[itAttr->first][itClass->first]; 
            }
        }
		m_averagePro.insert(AttributeHashPair(itAttr->first,averageProItem));//+20130606
    }
     
	INF_LOG("train all  data over ");
	INF_LOG("train data as follows:");
#ifdef _DEBUG
    //TrainingDataInfo();
	//printWordListInfo();
	printAveragePro();
#endif
    
    return  true;
}

//获取待分类的数据

bool Ys_BayesClassifier::TrainingWeightData(){
    if(m_vSecTrainRecordSet.size()== 0){
        ERR_LOG("There is no data to train!\n");
        return false;
    } 
    //all the predicting right record;
    double doubleAllRecordNum = 0.0;
    //the record number * the attribute number
    double doubleTotalRecordNum = 0.0;
    if(m_setAttributeName.size() == 0){
        ERR_LOG("There is no Attribute data\n");
		return false;
    }
    m_mapAttributePrecision.clear();
    for(std::set<string>::iterator itAttribute = m_setAttributeName.begin();itAttribute != m_setAttributeName.end(); ++itAttribute){
            INF_LOG("attribute:  m_mapAttributePrecision[%s] will set data \n", itAttribute->c_str());
			m_mapAttributePrecision[*itAttribute] = 1.0 ;
    }
#ifdef _DEBUG
	TrainingWeightDataInfo();
#endif
	
    for(unsigned int i = 0;i < m_vSecTrainRecordSet.size();++i )
    {
        const vector<Ys_FileRecordList*> &vFileRecordtmp = m_vSecTrainRecordSet[i]->GetFileRecordList();
        for(unsigned int k = 0; k < vFileRecordtmp.size(); ++k)
        {
            const vector<Ys_Record*> &vRecordtmp = vFileRecordtmp[k]->GetRecordList();
            for(unsigned int j = 0; j< vRecordtmp.size();++j)
            {
                for(std::set<string>::iterator iter = m_setAttributeName.begin();iter != m_setAttributeName.end(); ++iter)
                {
				//INF_LOG("  train weight (%s)\n", iter->c_str());
				    const Ys_Attribute* attrSet = vRecordtmp[j]->GetAttributeByName(*iter);
				    if(attrSet == NULL) 
                        continue;
				    YsRetData retItem;
				    if( !PredictingByAttribute(*iter, vRecordtmp[j]->GetAttributeByName(*iter)->GetWordListReference() , retItem) )
                    {
                        continue;
                        INF_LOG("TrainingWeightData: attribute:%s has no word feature",iter->c_str());
                    }               
				
                    if (retItem.m_className == vRecordtmp[j]->GetClassName())
                    {
                        m_mapAttributePrecision[*iter] += 1.0;
					    doubleAllRecordNum += 1.0;
                    }
                    else 
                    {
                        vRecordtmp[j]->ViewRecord();
						INF_LOG("Trainingweightdata: attribute:%s predict class :%s %e real class:%s\n", iter->c_str(), retItem.m_className.c_str(), retItem.m_posProb ,vRecordtmp[j]->GetClassName().c_str());
                    }
					
			        doubleTotalRecordNum += 1.0;
                }
            }
        }
    }
	if( doubleAllRecordNum == 0.0 )
	{
		ERR_LOG("TrainingWeightData total record is zero\n");
		return false;
	}
    for(std::set<string>::iterator itAttri = m_setAttributeName.begin(); itAttri != m_setAttributeName.end(); ++itAttri){
        INF_LOG3("before Attri:%s:%e   total %lf", itAttri->c_str(), m_mapAttributePrecision[*itAttri], doubleAllRecordNum);
		m_mapAttributePrecision[*itAttri] /= doubleAllRecordNum;
		INF_LOG3("after Attri:%s:%e   accuraty %lf total %lf", itAttri->c_str(), m_mapAttributePrecision[*itAttri], doubleAllRecordNum, doubleTotalRecordNum);
    }
	
#ifdef _DEBUG
	TrainingWeightDataInfo();
#endif
    return true;
}

bool Ys_BayesClassifier::StartUp()
{
   bool bIfDo = YS_ConfigData::GetInstance()->m_bIfTrain;
//If getting data from xml file failed , start training bayes
    ERR_LOG("Getting file from xml file failed,start training bayes model...\n");
    bool bIfSegment = YS_ConfigData::GetInstance()->m_bIfWordSeg;
    YS_WordSegmentLR& wordSegment = Ys_ClassifierManager::GetInstance()->GetWordSegment(); 
//////////////////////////////////insert dir to path set////////////////////////////////////////////////////////
    if(AddDirToPathSet(YS_ConfigData::GetInstance()->m_strTrainPath,m_mapSourcePath) == false)
	{
        ERR_LOG("Ys_BayesClassifier::Adding training data path failed\n");
        return false;
    }
    INF_LOG("Ys_BayesClassifier::Adding training data path succeeded...\n");
    if(AddDirToPathSet(YS_ConfigData::GetInstance()->m_strTestPath,m_mapSecSourcePath) == false)
	{
		ERR_LOG("Ys_BayesClassifier::Adding Test data path  failed\n");
        return false;
    }
    INF_LOG("Ys_BayesClassifier::Adding Test data path succeeded...\n");
    if(AddDirToPathSet(YS_ConfigData::GetInstance()->m_strSourcePath,m_mapPredictPath) == false)
	{
        ERR_LOG("Ys_BayesClassifier::Adding source path  failed\n");
		return false;
    }
    INF_LOG("Ys_BayesClassifier::Adding source path succeeded...\n");
//////////////////////////////get record from path set//////////////////////////////////////////////////////////
    if(GetRecordFromPath(m_mapSourcePath,m_vRecordSet,wordSegment,bIfSegment) == false)
	{
        ERR_LOG("Ys_BayesClassifier::GetRecord from source path failed\n");
        return false;
    }
    INF_LOG("Ys_BayesClassifier::GetRecord from source path succeeded...\n");
	
    if(GetRecordFromPath(m_mapSecSourcePath,m_vSecTrainRecordSet,wordSegment,bIfSegment) == false)
	{
        ERR_LOG("Ys_BayesClassifier::GetRecord from test training path failed\n");
        return false;
    }
    INF_LOG("Ys_BayesClassifier::GetRecord from test path succeeded...\n");
//////读取待分类数据///////////////////////////////////////////////////////////////////////////////////    
//    if(GetRecordFromPath(m_mapPredictPath,m_vPredictRecordSet,wordSegment,bIfSegment) == false){
//        cout<<"GetRecord from predict data path  failed"<<endl;
//        return false;
//    }
///////////////////////////////////////////////////////////////////////////////////////////////////////
    //if(TrainingServiceData() == false)
    if(bIfDo == false)
    {
        //If getting data from xml file succeed,just return
        string strWordListPath = YS_ConfigData::GetInstance()->m_strAttributeWordListPath;
        if(GetAttributeWordNumFromXml()&&GetClassProbabilityFromXml()&&GetWordProbabilityFromXml()&&GetAttributeWordListFromXml(m_mAttributeWordList,strWordListPath)&&GetAttributePrecisionFromXml())
        {
            INF_LOG("Getting data from xml file succeeded!\n");
            return true;
        }
    }
    if(TrainingData() == false)
	{
       ERR_LOG("Training bayes data failed \n");
        return false;
    }
    INF_LOG("Ys_BayesClassifier::Training bayes data succeeded...\n");

    if(!WriteWordProbabilityToXml())
    {
        ERR_LOG("Ys_BayesClassifier::writing word probability into xml file failed!\n");
        return false;
    }
    if(!WriteClassProbabilityToXml())
    {
        ERR_LOG("Ys_BayesClassifier::writing class probability into xml file failed!\n");
        return false;
    }
    string strWordListPath = YS_ConfigData::GetInstance()->m_strAttributeWordListPath;
    strWordListPath = strWordListPath + "/attribute_word_list.xml";
    if(!WriteAttributeWordListToXml(m_mAttributeWordList,strWordListPath))
    {
        ERR_LOG("Ys_BayesClassifier::writing word list into xml file failed!\n");
        return false;
    }
    if(!WriteAttributeWordNumToXml())
    {
        ERR_LOG("Ys_BayesClassifier::writing attribute word number into xml file failed!");
        return false;
    }

    if(TrainingWeightData() == false)
	{
        ERR_LOG("Ys_BayesClassifier::Training weight data failed\n");
        return false;
    }
    if(!WriteAttributePrecisionToXml())
    {
        ERR_LOG("Ys_BayesClassifier::writing attribute precision failed\n");
        return false;
    }
    INF_LOG("Ys_BayesClassifier::whole thing has been done!\n");
    return true;
}

bool Ys_BayesClassifier::PredictingByAttribute(const string& attrName, const vector<string>& arriList , YsRetData& retItem)
{
    if(m_mapWordProbability.size() == 0)
	{
		ERR_LOG("Word Probability is empty, fatal error\n");
		return false;
	}
	if( arriList.size() == 0)
	{
        return false;
	}

	StrSet& strSet = findAttributeSet(attrName);
	if(strSet.size() == 0)
	{
		ERR_LOG("%s attribute word set is empty");
		return false;	
	}
	
	string strName = "";
    bool ifAbandon = false;
	bool ifGetValue = false;
	double dMaxProbability;
	double uiBadNumFlag = arriList.size();
	bool bFlag = false;
	for(AttributeHashDataIter itClass = m_mapWordProbability[attrName].begin();itClass != m_mapWordProbability[attrName].end();++itClass)
    { 
        double doubleProbability = 0.0;
		unsigned int uiBadNum = 0;
		
		StrDoubleHashDataIter sdIter = m_mapClassDataMap.find(itClass->first);
		if( sdIter == m_mapClassDataMap.end()){ 
        	ERR_LOG("there is no %s  prior probability in m_mapClassDataMap\n");
        	continue;
        }
        doubleProbability = log(sdIter->second);
		//count every word's probability	
        for(unsigned int i= 0;i<arriList.size();i++)
        {
        	if(!findWord(arriList[i], strSet))
        	{
        		//INF_LOG("%s  is not in %s's propoSet \n", arriList[i].c_str(), attrName.c_str());
        		uiBadNum++;
        		continue;
        	}
        	//INF_LOG("%s  is  in %s's propoSet \n", arriList[i].c_str(), attrName.c_str());
            //如果没有特证词没有在m_mapWordProbability中找到，则使用laplace平滑
            StrDoubleHashData mapWordProbability = m_mapWordProbability[attrName][itClass->first];
            StrDoubleHashDataIter sdIter;
            if( (sdIter=mapWordProbability.find(arriList[i])) == mapWordProbability.end())
            {
                if(m_mapAttributeWordNum[attrName][itClass->first] == 0)
                {
                    ifAbandon = true;
                    break;
                }
                doubleProbability += (log(LAPLACE_SMOOTH)-log(m_mapAttributeWordNum[attrName][itClass->first])) ;
                //cout<<"not find word"<<doubleProbability<<endl;
				++uiBadNum; //使用拉普拉斯平滑的关键字个数(关键字未在训练集合的个数)
				//INF_LOG(">>badnum laplace pro %e  total pro %e\n", log(LAPLACE_SMOOTH/(m_mapAttributeWordNum[itClass->first][attrName]*(1+LAPLACE_SMOOTH))), doubleProbability);
            }
            else
            {//找到该词，则直接计算概率
				doubleProbability += log(sdIter->second);
                //cout<<"find word"<<doubleProbability<<endl;
            	//INF_LOG(">>goodnum   word pro %e  total pro %e\n", log(sdIter->second), doubleProbability);
			}
        }
		//INF_LOG(">>>badnum %u, goodnum %u, size:%u   pro %e (%s)\n", uiBadNum, arriList.size()-uiBadNum, arriList.size(), doubleProbability, itClass->first.c_str());
        if(ifAbandon == true)
        {
            ifAbandon = false;
            continue;
        }
		if(!ifGetValue){
			if(uiBadNumFlag < uiBadNum) continue;
			uiBadNumFlag = uiBadNum;
			dMaxProbability = doubleProbability;
			strName = itClass->first;
			bFlag = (uiBadNum!=arriList.size()&& arriList.size()!=0 )? true:false;
			ifGetValue = true;
			continue;
		}
		if( doubleProbability > dMaxProbability )
		{
			//INF_LOG(">>>>>>>PredictingByAttribute choice  class:%s->%s[%e>%e]   %s\n", strName.c_str(), itClass->first.c_str(), dMaxProbability, doubleProbability, bFlag? "true":"false");
			if(uiBadNumFlag < uiBadNum) continue;
			uiBadNumFlag = uiBadNum;
			dMaxProbability = doubleProbability;
			strName = itClass->first;
			bFlag = (uiBadNum!=arriList.size()&& arriList.size()!=0 )? true:false;
		}
    }
	retItem.m_posProb = dMaxProbability;
	retItem.m_className = strName;
	//INF_LOG(">>>>>>>>>>>>PredictingByAttribute FINAL class:%s[%e]  %s\n", retItem.m_className.c_str(), retItem.m_posProb, bFlag? "true":"false");
    return bFlag;
}

bool Ys_BayesClassifier::GetPostProbByAttribute(const string& attrName, const vector<string>& arriList , YsRetData& retItem, vector<YsRetData>  &vResultClass)
{
   if(m_mapWordProbability.size() == 0)
	{
		ERR_LOG("Word Probability is empty, fatal error\n");
		return false;
	}
	if(  arriList.size() == 0)
	{
		return false;
	}
    
	for(unsigned int pp= 0; pp<arriList.size();pp++)
	{
		INF_LOG("           %s", arriList[pp].c_str());
	}
	
	StrSet& strSet = findAttributeSet(attrName);
	if(strSet.size() == 0)
	{
		ERR_LOG("%s attribute word set is empty");
		return false;	
	}
    bool ifAbandon = false;
	unsigned int uiBadNumFlag = strSet.size();
	string strName = "";
	bool ifGetValue = false;
	double dMaxProbability;
	bool bFlag = false;
	
	for(AttributeHashDataIter itClass = m_mapWordProbability[attrName].begin();itClass != m_mapWordProbability[attrName].end();++itClass){ 
        double doubleProbability = 0.0;
		unsigned int uiBadNum = 0;
		
		StrDoubleHashDataIter sdIter = m_mapClassDataMap.find(itClass->first);
		if( sdIter ==m_mapClassDataMap.end()){ 
        	ERR_LOG("there is no %s  prior probability in m_mapClassDataMap\n");
        	continue;
        }
        doubleProbability = log(sdIter->second);
		
		//对于每一个类别分别进行计算其概率	
        for(unsigned int i= 0;i<arriList.size();i++)
		{
        	if(!findWord(arriList[i], strSet))
        	{
        		//INF_LOG("%s  is not in %s's propoSet \n", arriList[i].c_str(), attrName.c_str());
        		uiBadNum++;
        		continue;
        	}
            //如果没有特证词没有在m_mapWordProbability中找到，则使用laplace平滑
            StrDoubleHashData mapWordProbability = m_mapWordProbability[attrName][itClass->first];
            StrDoubleHashDataIter sdIter;
            if( (sdIter=mapWordProbability.find(arriList[i])) == mapWordProbability.end()){
                //
                if(m_mapAttributeWordNum[attrName][itClass->first] == 0)
                {
                    ifAbandon = true;
                    break;
                }
                doubleProbability += (log(LAPLACE_SMOOTH)-log(m_mapAttributeWordNum[attrName][itClass->first])) ;
				++uiBadNum; //使用拉普拉斯平滑的关键字个数(关键字未在训练集合的个数)
				//INF_LOG(">>badnum laplace pro %e  total pro %e\n", log(LAPLACE_SMOOTH/(m_mapAttributeWordNum[itClass->first][attrName]*(1+LAPLACE_SMOOTH))), doubleProbability);
            }else{//找到该词，则直接计算概率
				doubleProbability += log(sdIter->second);
            	INF_LOG(">>[%s]goodnum   word %s pro %e  total pro %e  ", itClass->first.c_str(), arriList[i].c_str(),log(sdIter->second), doubleProbability);
			}
        }
		//INF_LOG(">>>badnum %u, goodnum %u, size:%u   pro %e (%s)\n", uiBadNum, arriList.size()-uiBadNum, arriList.size(), doubleProbability, itClass->first.c_str());
        if(ifAbandon)
        {
            ifAbandon = false;
            break;
        }
		if(uiBadNum!=arriList.size())
			vResultClass.push_back(YsRetData(itClass->first, doubleProbability));
		
		if(!ifGetValue){
		    if(uiBadNumFlag < uiBadNum) continue;
			uiBadNumFlag = uiBadNum;
			dMaxProbability = doubleProbability;
			strName = itClass->first;
			bFlag = (uiBadNum!=arriList.size()&& arriList.size()!=0 )? true:false;
			ifGetValue = true;
			continue;
		}
		if( doubleProbability > dMaxProbability )
		{
			INF_LOG(">>>>>>>PredictingByAttribute choice  class:%s->%s[%e<%e]   %s\n", strName.c_str(), itClass->first.c_str(), dMaxProbability, doubleProbability, bFlag? "true":"false");
			if(uiBadNumFlag < uiBadNum) continue;
			uiBadNumFlag = uiBadNum;
			dMaxProbability = doubleProbability;
			strName = itClass->first;
			bFlag = (uiBadNum!=arriList.size())? true:false;
		}
    }
	retItem.m_posProb = dMaxProbability;
	retItem.m_className = strName;
	INF_LOG("total word[%d] not match[%d] ", arriList.size(), uiBadNumFlag);
	//+20130606
#ifdef _AVERAGE_PRO
	AttributeHashDataIter attrAvergeProMap = m_averagePro.find(attrName);
	if(attrAvergeProMap == m_averagePro.end())
	{
		ERR_LOG("m_averagePro has no attribute[%s] info \n", attrName.c_str());
	}
	else
	{
		StrDoubleHashDataIter averageProItem = attrAvergeProMap->second.find(strName);
		if(averageProItem == attrAvergeProMap->second.end())
		{
			ERR_LOG("m_averagePro[%s] has no %s \n", attrName.c_str(), strName.c_str());
		}
		else
		{
			double averagePorbo = log(1/averageProItem->second)*arriList.size();
			//(log(LAPLACE_SMOOTH)-log(m_mapAttributeWordNum[attrName][strName]))
			if( dMaxProbability <  averagePorbo)
			{
				bFlag = false;
				INF_LOG("[%s:%s] propo [%e] is less than average propo[%e]\n",attrName.c_str(), strName.c_str(), dMaxProbability, averagePorbo);
			}
		}
	}
#endif
	INF_LOG(">>>>>>>>>>>>PredictingByAttribute FINAL class:%s[%e]  %s", retItem.m_className.c_str(), retItem.m_posProb, bFlag? "true":"false");
    return bFlag;
}

bool Ys_BayesClassifier::AddDirToSourcePath(const string &dir){
    return AddDirToPathSet(dir,m_mapSourcePath);
} 

bool Ys_BayesClassifier::AddDirToSecSourcePath(const string &dir){
    return AddDirToPathSet(dir,m_mapSecSourcePath);
}

bool Ys_BayesClassifier::AddDirToPredictPath(const string &dir){
    return AddDirToPathSet(dir,m_mapPredictPath);
}
//读取所有类所有属性所有关键词的概率
bool Ys_BayesClassifier::GetWordProbabilityFromXml()
{
    string fullPath = YS_ConfigData::GetInstance()->m_strWordProbabilityPath;
    fullPath = fullPath + "/word_probability.xml";
    TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
    if(!(myDocument->LoadFile()))
    {
        ERR_LOG("Load word probability from xml file failed");
        return false;
    }
    TiXmlElement *pRootElement = myDocument->RootElement();
    TiXmlElement *pAttributeElement = NULL;
    //遍历所有类节点
    if(pRootElement->FirstChild() == NULL)
    {
        ERR_LOG("there is no record in word probability in xml file");
        return false;
    }
    for(pAttributeElement = pRootElement->FirstChild()->ToElement();pAttributeElement != 0 ; pAttributeElement = pAttributeElement->NextSiblingElement())
    {
        string strAttributeName = pAttributeElement->FirstAttribute()->Value();
        if(m_mapWordProbability.find(strAttributeName) != m_mapWordProbability.end())
        {
            ERR_LOG("can't find the attribute name while get word probability\n");
            return false;
        }
        AttributeHashData  ClassHashDatatmp;
        TiXmlElement *pClassElement = NULL;
        //遍历类下面所有属性节点
        if(pAttributeElement->FirstChild() == NULL)
        {
            ERR_LOG("There is no class record in %s attribute word probability xml file",strAttributeName.c_str());
            continue;
        }
        for(pClassElement = pAttributeElement->FirstChild()->ToElement();pClassElement != 0 ; pClassElement = pClassElement->NextSiblingElement())
        {
            string strClassName = pClassElement->FirstAttribute()->Value();
            if(ClassHashDatatmp.find(strClassName) != ClassHashDatatmp.end())
            {
                ERR_LOG("can't find the attribute name while get word probability\n");
                return false;
            }
            StrDoubleHashData WordProbabilitytmp;
            TiXmlElement *pWordElement = NULL;
            //遍历属性下面所有关键词节点
            if(pClassElement->FirstChild() == NULL)
            {
                ERR_LOG("can't find word record in %s class %s attribtue word probability xml file",strClassName.c_str(),strAttributeName.c_str());
                continue;
            }
            for(pWordElement = pClassElement->FirstChild()->ToElement();pWordElement != 0; pWordElement = pWordElement->NextSiblingElement())
            {
                string strWordName = pWordElement->FirstAttribute()->Value();
                if(WordProbabilitytmp.find(strWordName) != WordProbabilitytmp.end())
                {
                    ERR_LOG("can't find the word name while get word probability \n");
                    return false;
                }
                WordProbabilitytmp.insert(StrDoubleHashDataPair(strWordName,strtod(pWordElement->GetText(),NULL)));
            } 
            ClassHashDatatmp.insert(AttributeHashPair(strClassName,WordProbabilitytmp));
        }     
        m_mapWordProbability.insert(AttributeClassWordPair(strAttributeName,ClassHashDatatmp));
    } 
    return true;   
}
//读取所有类的先验概率
bool Ys_BayesClassifier::GetClassProbabilityFromXml()
{
    string fullPath = YS_ConfigData::GetInstance()->m_strClassProbabilityPath;
    fullPath = fullPath + "/class_probability.xml";
    TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
    if(!(myDocument->LoadFile()))
    {
        ERR_LOG("load class probability from xml file failed");
        return false;
    }
    TiXmlElement *pRootElement = myDocument->RootElement();
    TiXmlElement *pClassElement = NULL;
    //遍历读取每个类的先验概率
    if(pRootElement->FirstChild() == NULL)
    {
        INF_LOG("there is no class record in ClassProbability xml file");
        return false;
    }
    for(pClassElement = pRootElement->FirstChild()->ToElement();pClassElement != 0 ;pClassElement = pClassElement->NextSiblingElement())
    {
        string strClassName = pClassElement->FirstAttribute()->Value();
        if(m_mapClassDataMap.find(strClassName) != m_mapClassDataMap.end())
        {
            ERR_LOG("can't find class name while get class probability \n");
            return false;
        }
        m_mapClassDataMap.insert(StrDoubleHashDataPair(strClassName,strtod(pClassElement->GetText(),NULL)));
    }
    return true;
}
//读取属性关键字词典的函数
bool Ys_BayesClassifier::GetAttributeWordListFromXml(StrMap& m_mAttributeWordList,const string fullPath)
{
    //string fullPath = YS_ConfigData::GetInstance()->m_strAttributeWordListPath;
    string tfullPath = fullPath + "/attribute_word_list.xml";
    TiXmlDocument *myDocument = new TiXmlDocument(tfullPath.c_str());
    if(!(myDocument->LoadFile()))
    {
        ERR_LOG("there is no AttributeWordList xml file ");
        return false; 
    }
    TiXmlElement *pRootElement = myDocument->RootElement();
    TiXmlElement *pAttributeElement = NULL;
    //遍历所有属性
    if(pRootElement->FirstChild() == NULL)
    {
        INF_LOG("there is no attribute record  in attribute word list xml file");
        return false;
    }
    for(pAttributeElement = pRootElement->FirstChild()->ToElement();pAttributeElement != 0 ;pAttributeElement = pAttributeElement->NextSiblingElement())
    {
        string strAttributeName = pAttributeElement->FirstAttribute()->Value();
        if(m_mAttributeWordList.find(strAttributeName) != m_mAttributeWordList.end())
        {
            ERR_LOG("can't find attribute name while get attribute word list \n");
            return false;
        }
        StrSet strSettmp;
        TiXmlElement *pWordElement = NULL;
        //遍历该属性下的关键词
        
        if(pAttributeElement->FirstChild() == NULL)
        {   
            INF_LOG("there is no word record  in attribute %s attribute word list xml file",strAttributeName.c_str());
            continue;;
        }
        for(pWordElement = pAttributeElement->FirstChild()->ToElement() ; pWordElement != 0;pWordElement = pWordElement->NextSiblingElement())
        {
            strSettmp.insert(pWordElement->GetText());
        }
        
        m_mAttributeWordList.insert(StrMapPair(strAttributeName,strSettmp));
    }

    return true;
}
bool Ys_BayesClassifier::GetAttributePrecisionFromXml()
{
    string fullPath = YS_ConfigData::GetInstance()->m_strAttributePrecisionPath;
    fullPath = fullPath + "/attribute_precision.xml";
    TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
    if(!(myDocument->LoadFile()))
    {
        ERR_LOG("Load attribute precision from xml file failed");
        return false;
    }
    TiXmlElement *pRootElement = myDocument->RootElement();
    TiXmlElement *pAttributeElement = NULL;

    if(pRootElement->FirstChild() == NULL)
    {
        INF_LOG("there is no attribute record  in attribute precision xml file");
        return false;
    }
    for(pAttributeElement = pRootElement->FirstChild()->ToElement();pAttributeElement != 0 ; pAttributeElement = pAttributeElement->NextSiblingElement())
    {
        string strAttributeName = pAttributeElement->FirstAttribute()->Value();
        if(m_mapAttributePrecision.find(strAttributeName) != m_mapAttributePrecision.end())
        {
            ERR_LOG("attribute name exists return ");
            return false;
        }
        m_mapAttributePrecision.insert(StrDoubleHashDataPair(strAttributeName,strtod(pAttributeElement->GetText(),NULL)));
    }
    return true;
    
}
bool Ys_BayesClassifier::GetAttributeWordNumFromXml()
{
    string fullPath =YS_ConfigData::GetInstance()->m_strAttributeWordNumPath;
    fullPath = fullPath + "/attribute_word_number.xml";
    TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
    if(!(myDocument->LoadFile()))
    {
        ERR_LOG("Load attribute word number from xml file failed");
        return false;
    }

    TiXmlElement *pRootElement = myDocument->RootElement();
    TiXmlElement *pAttributeElement = NULL;
    if(pRootElement->FirstChild() == NULL)
    {
        INF_LOG("there is no class record in attribute word number xml file");
        return false;
    }
    for(pAttributeElement = pRootElement->FirstChild()->ToElement();pAttributeElement != 0 ; pAttributeElement = pAttributeElement->NextSiblingElement())
    {
        string strAttributeName = pAttributeElement->FirstAttribute()->Value();
        if(m_mapAttributeWordNum.find(strAttributeName) != m_mapAttributeWordNum.end())
        {
            ERR_LOG("attribute name exists then return \n");
            return false;
        }
        StrDoubleHashData tmpStrDouble;
        TiXmlElement *pClassElement = NULL;
        if(pAttributeElement->FirstChild() == NULL)
        {
            INF_LOG("there is no word number record in %s attribute in word number xml file",strAttributeName.c_str());
            continue;
        }
        for(pClassElement = pAttributeElement->FirstChild()->ToElement();pClassElement != 0 ; pClassElement = pClassElement->NextSiblingElement())
        {
            string tmpWordName = pClassElement->FirstAttribute()->Value();
            tmpStrDouble.insert(StrDoubleHashDataPair(tmpWordName,strtod(pClassElement->GetText(),NULL)));
        }
        m_mapAttributeWordNum.insert(AttributeHashPair(strAttributeName,tmpStrDouble));
    }
    return true;
}
//将训练的结果写入Xml文件中
bool Ys_BayesClassifier::WriteWordProbabilityToXml()
{
    string fullPath = YS_ConfigData::GetInstance()->m_strWordProbabilityPath;
    fullPath = fullPath + "/word_probability.xml";
    TiXmlDocument *myDocument = new TiXmlDocument();
    //建立整个xml文件系统的根节点
    TiXmlElement *RootElement = new TiXmlElement("attributes");
    myDocument->LinkEndChild(RootElement);
    AttributeClassWordIter itAttr;
    //遍历每一个类
    for(itAttr = m_mapWordProbability.begin(); itAttr != m_mapWordProbability.end(); ++itAttr)
    {
        TiXmlElement *pAttributeElement = new TiXmlElement("attribute");
        pAttributeElement->SetAttribute("Name",itAttr->first.c_str());
        RootElement->LinkEndChild(pAttributeElement);
        AttributeHashDataIter itClass;
        AttributeHashData ClassHashDatatmp = itAttr->second;
        for(itClass = ClassHashDatatmp.begin();itClass != ClassHashDatatmp.end();++itClass)
        {
            TiXmlElement *pClassElement = new TiXmlElement("Class");
            pClassElement->SetAttribute("Name",itClass->first.c_str());
            pAttributeElement->LinkEndChild(pClassElement);
            StrDoubleHashDataIter itWord ;
            StrDoubleHashData StrDoubleHashDatatmp = itClass->second;
            for(itWord = StrDoubleHashDatatmp.begin() ; itWord != StrDoubleHashDatatmp.end();++itWord)
            {
                TiXmlElement *pWordElement = new TiXmlElement("Word");
                pWordElement->SetAttribute("Name",itWord->first.c_str());
                pClassElement->LinkEndChild(pWordElement);
                char buffer[15];
                sprintf(buffer,"%e",itWord->second);
                TiXmlText *pWordProbability = new TiXmlText(buffer);
                pWordElement->LinkEndChild(pWordProbability);
            }
        }
    }
    if(myDocument->SaveFile(fullPath.c_str()))
        return true;
    ERR_LOG("Save word probability to xml failed!");
    return false;

}
bool Ys_BayesClassifier::WriteClassProbabilityToXml()
{
    string fullPath = YS_ConfigData::GetInstance()->m_strClassProbabilityPath;
    fullPath = fullPath + "/class_probability.xml";
    TiXmlDocument *myDocument = new TiXmlDocument();
    TiXmlElement *RootElement = new TiXmlElement("Classes");
    myDocument->LinkEndChild(RootElement);
    StrDoubleHashDataIter itClassProbability;
    for(itClassProbability = m_mapClassDataMap.begin();itClassProbability != m_mapClassDataMap.end();++itClassProbability)
    {
        TiXmlElement *pClassElement = new TiXmlElement("Class");
        RootElement->LinkEndChild(pClassElement);
        pClassElement->SetAttribute("Name",itClassProbability->first.c_str());
        char buffer[15];
        sprintf(buffer,"%e",itClassProbability->second);
        TiXmlText *pClassProbability = new TiXmlText(buffer);
        pClassElement->LinkEndChild(pClassProbability);
    }
    if(myDocument->SaveFile(fullPath.c_str()))
        return true;
    ERR_LOG("Write class probability to xml failed !");
    return false;
}

bool Ys_BayesClassifier::WriteAttributeWordListToXml(const StrMap m_mAttributeWordList,const string fullPath)
{
    //string fullPath = YS_ConfigData::GetInstance()->m_strAttributeWordListPath;
    //fullPath = fullPath + "/attribute_word_list.xml";
    TiXmlDocument *myDocument = new TiXmlDocument();
    TiXmlElement *RootElement = new TiXmlElement("Attributes");
    myDocument->LinkEndChild(RootElement);
    StrMap::const_iterator itAttributetmp;
    for(itAttributetmp = m_mAttributeWordList.begin();itAttributetmp != m_mAttributeWordList.end();++itAttributetmp)
    {
        TiXmlElement *pAttributeElement = new TiXmlElement("Attribute");
        RootElement->LinkEndChild(pAttributeElement);
        pAttributeElement->SetAttribute("Name",itAttributetmp->first.c_str());
        StrSetIter itWordSet;
        StrSet tmpStrSet = itAttributetmp->second;
        for(itWordSet = tmpStrSet.begin(); itWordSet != tmpStrSet.end();++itWordSet)
        {
            TiXmlElement *pWordElement = new TiXmlElement("Word");
            pAttributeElement->LinkEndChild(pWordElement);
            TiXmlText *pWordName = new TiXmlText((*itWordSet).c_str());

            pWordElement->LinkEndChild(pWordName);
        }
    }
    if(myDocument->SaveFile(fullPath.c_str()))
        return true;
    ERR_LOG("Write attribute word list to xml file failed!");
    return false;
}
bool Ys_BayesClassifier::WriteAttributePrecisionToXml()
{
    string fullPath = YS_ConfigData::GetInstance()->m_strAttributePrecisionPath;
    fullPath =fullPath + "/attribute_precision.xml";
    TiXmlDocument *myDocument = new TiXmlDocument();
    TiXmlElement *RootElement = new TiXmlElement("Attributes");
    myDocument->LinkEndChild(RootElement);
    StrDoubleHashDataIter itStrDouble;
    for(itStrDouble = m_mapAttributePrecision.begin();itStrDouble != m_mapAttributePrecision.end();++itStrDouble)
    {
        TiXmlElement *pAttributeElement = new TiXmlElement("Attribute");
        RootElement->LinkEndChild(pAttributeElement);
        pAttributeElement->SetAttribute("Name",itStrDouble->first.c_str());
        char buffer[15];
        sprintf(buffer,"%e",itStrDouble->second);
        TiXmlText *pAttributePrecision = new TiXmlText(buffer);
        pAttributeElement->LinkEndChild(pAttributePrecision);
    }
    if(myDocument->SaveFile(fullPath.c_str()))
        return true;
    ERR_LOG("Write attribute precision to xml file failed!");
    return false;
}

bool Ys_BayesClassifier::WriteAttributeWordNumToXml()
{
    string fullPath = YS_ConfigData::GetInstance()->m_strAttributeWordNumPath;
    fullPath = fullPath + "/attribute_word_number.xml";
    TiXmlDocument *myDocument = new TiXmlDocument();
    TiXmlElement *RootElement = new TiXmlElement("attributes");
    myDocument->LinkEndChild(RootElement);
    AttributeHashDataIter itAttr;
    for(itAttr = m_mapAttributeWordNum.begin();itAttr != m_mapAttributeWordNum.end();++itAttr)
    {
        TiXmlElement *pAttributeElement = new TiXmlElement("Attribute");
        RootElement->LinkEndChild(pAttributeElement);
        pAttributeElement->SetAttribute("Name",itAttr->first.c_str());
        StrDoubleHashDataIter itStrDouble;
        StrDoubleHashData tmpStrDouble = itAttr->second;
        for(itStrDouble = tmpStrDouble.begin() ; itStrDouble != tmpStrDouble.end() ;++itStrDouble)
        {
            TiXmlElement *pClassElement = new TiXmlElement("Class");
            pAttributeElement->LinkEndChild(pClassElement);
            pClassElement->SetAttribute("Name",itStrDouble->first.c_str());
            char buffer[BUFFER_SIZE];
            sprintf(buffer,"%e",itStrDouble->second);
            TiXmlText *pWordNum = new TiXmlText(buffer);
            pClassElement->LinkEndChild(pWordNum);
        }
    }
    if(myDocument->SaveFile(fullPath.c_str()))
        return true;
    ERR_LOG("Write attribute word number to xml file !");
    return false;
}

bool Ys_BayesClassifier::TrainingDataInfo()
{
    for(StrDoubleHashData::iterator itClassPrecision = m_mapClassDataMap.begin();itClassPrecision != m_mapClassDataMap.end();++itClassPrecision){
        cout<<itClassPrecision->first<<" : "<<itClassPrecision->second<<endl;
    }

    for(AttributeClassWordIter itWordProbability = m_mapWordProbability.begin(); itWordProbability !=m_mapWordProbability.end(); ++itWordProbability){
        cout<<"attribute:"<<itWordProbability->first<<endl;
        for(AttributeHashData::iterator itClass = itWordProbability->second.begin(); itClass!= itWordProbability->second.end();++itClass){
            cout<<"class:"<<itClass->first<<endl;
            for(StrDoubleHashData::iterator itWord = itClass->second.begin();itWord!= itClass->second.end();++itWord){
                cout<<itWord->first<<":"<<itWord->second<<";";
            }
            cout<<endl;
        }
    }
    cout<<"m_iAllRecordNum:"<<m_iAllRecordNum<<endl;
    return true;
}

bool Ys_BayesClassifier::TrainingWeightDataInfo()
{
    cout<<"attribute weight data as follows: "<<endl;
	for(StrDoubleHashData::iterator itWord = m_mapAttributePrecision.begin(); itWord!= m_mapAttributePrecision.end(); ++itWord){
        cout<<itWord->first<<" "<<itWord->second<<endl;
    }
	return true;
}

bool Ys_BayesClassifier::AddWordToList(const string& word, const string& attribute)
{
	static string lastAttr;
	static StrMap::iterator mmit;
		
	if(lastAttr != attribute)
	{
		lastAttr = attribute;
		mmit = m_mAttributeWordList.find(attribute);
		if(mmit == m_mAttributeWordList.end())
		{
			StrSet strset;
			strset.insert(word).second;
			std::pair<StrMap::iterator, bool>  ret = m_mAttributeWordList.insert(StrMap::value_type(attribute,strset));
			mmit = ret.first;
			return ret.second;
		}
	}
	
	StrSet::iterator iter = mmit->second.find(word);
	if(iter == mmit->second.end())
		return (mmit->second.insert(word)).second;
	return true;
}

bool Ys_BayesClassifier::findWord(const string& word, const string& attribute)
{
	static string lastAttr_;
	static StrMap::iterator mmit_;

	if(lastAttr_ != attribute)
	{
		lastAttr_ = attribute;
		mmit_ = m_mAttributeWordList.find(attribute);
		if(mmit_ == m_mAttributeWordList.end())
		{
			return false;
		}
	}
	StrSet::iterator iter = mmit_->second.find(word);
	if(iter == mmit_->second.end())
		return false;
	return true;
}

bool Ys_BayesClassifier::addWord(const string& word, StrSet& strSet)
{
	StrSet::iterator iter = strSet.find(word);
	if(iter == strSet.end())
		return (strSet.insert(word)).second;
	return true;
}

bool Ys_BayesClassifier::findWord(const string& word, StrSet& strSet)
{
	StrSet::iterator iter = strSet.find(word);
	if(iter == strSet.end())
		return false;
	return true;
}
	
bool Ys_BayesClassifier::printWordListInfo()
{
	for(StrMap::iterator mIter = m_mAttributeWordList.begin(); mIter!=m_mAttributeWordList.end(); ++mIter)	
	{
		int co=0;
		for(StrSet::iterator sIter = mIter->second.begin(); sIter!=mIter->second.end(); ++sIter)
		{
			++co;
			if(co%100 == 0)
				printf("\n");
			printf("%s|", sIter->c_str());
		}	
	}
	return true;
}

bool Ys_BayesClassifier::printAveragePro()
{
	INF_LOG("==================printAveragePro====================");
	for(AttributeHashDataIter mIter = m_averagePro.begin(); mIter!=m_averagePro.end(); ++mIter)	
	{
		INF_LOG("==============================attribute:%s====", mIter->first.c_str());
		int co=0;
		for(StrDoubleHashDataIter sIter = mIter->second.begin(); sIter!=mIter->second.end(); ++sIter)
		{
			++co;
			if(co%100 == 0)
				printf("\n");
			INF_LOG("%s|%f", sIter->first.c_str(), sIter->second);
		}	
	}
	INF_LOG("==================printAveragePro====================\n");
	return true;
}

template <class T>
bool addHashData(const string& key, T& value, StrDoubleHashData& dataSet)
{
	StrDoubleHashDataIter iter = dataSet.find(key);
	if(iter == dataSet.end())
		return dataSet.insert( StrDoubleHashDataPair( key, value ) ).second;
	iter->second += value;
	return true;
}
