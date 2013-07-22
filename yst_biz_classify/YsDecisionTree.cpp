/**/

#include "YsDecisionTree.h"
#include "YsTemplateHashTable.h"
#include "YsTools.h"
#include "YsHashData.h"
#include "YsClassifierManager.h"
#include <math.h>
#include <deque>
using namespace std;

#define IF_ZERO(a) ((a)-0.0 < 0.0000001)

YS_DescisionTree::YS_DescisionTree()
{
	m_wordsInDocs = new YS_KeyHashMap();
	// �ĵ���������
	m_docsHaveWords = new YS_KeyHashMap();
	// �ĵ��������
	m_allDocsInfo = new YS_StrHashMap<string>();
	m_allWords = new YS_KeyHashSet();
	m_allDocs = new YS_KeyHashSet();
	m_treeRoot = NULL;
	m_iDepth = 0;
}

void YS_DescisionTree::PrintInfo()
{
	printf("�������ڼ�¼��Ϣ��������س��鿴\n");
	TAP_ENTER;
	m_wordsInDocs->PrintInfo();
	printf("��¼���е�����Ϣ��������س��鿴\n");
	TAP_ENTER;
	m_docsHaveWords->PrintInfo();
	printf("��¼���������Ϣ��������س��鿴\n");
	TAP_ENTER;
	
	for(m_allDocsInfo->Begin(); !m_allDocsInfo->IsEnd(); m_allDocsInfo->Next())
	{
		printf("%-10s  belongs to %-10s\n", m_allDocsInfo->IterFirst().c_str(),m_allDocsInfo->IterSecond().c_str());
	}
	
	printf("������Ϣ��������س��鿴\n");
	TAP_ENTER;
	m_allWords->PrintInfo();
	printf("��¼��Ϣ��������س��鿴\n");
	TAP_ENTER;
	m_allDocs->PrintInfo();
	printf("������\n");
	TAP_ENTER;
}

void YS_DescisionTree::PrintTreeInfo(YS_DescitionTreeNode* node, int value)
{
	int va = 0;
	//while(va++<value)PRINT("-->");
	if(node->m_eType == T_ATTR)
		PRINT("[%s]->", node->m_strValue.c_str());
	else
	{
		PRINT("label [%s] is final  leaf \n", node->m_strValue.c_str());
		return;
	}
	
	if(node->m_pYesRight != NULL)
	{
		PRINT("YES->\n", node->m_strValue.c_str());
		PrintTreeInfo(node->m_pYesRight, value+1);
	}
	else
	{
		PRINT("right END NULL\n");
		return;
	}

	if(node->m_pNoLeft!=NULL) 
	{
		//while(va++<value)PRINT("-->");
		PRINT("No ->\n", node->m_strValue.c_str());
		PrintTreeInfo(node->m_pNoLeft, value+1);
	}
	else
	{
		PRINT("left END NULL\n");
		return;
	}
}
void YS_DescisionTree::PtrNull()
{
	m_allWords = NULL;     
	m_allDocs = NULL;      
	m_docsHaveWords = NULL;
	m_allDocsInfo = NULL;  
	m_wordsInDocs = NULL;
	m_treeRoot = NULL;
}

YS_DescisionTree::~YS_DescisionTree()
{
	if(m_treeRoot != NULL)
		delete m_treeRoot;
	if(m_allWords != NULL)
		delete m_allWords;
	if(m_allDocs != NULL)
		delete m_allDocs;
	delete m_docsHaveWords;
	delete m_allDocsInfo;
	delete m_wordsInDocs;
}
	
bool YS_DescisionTree::StartUpRecursion()
{
	list<string> words;
	list<string> docs;
	m_allWords->ChangeToArray(words);
	delete m_allWords;
	m_allDocs->ChangeToArray(docs);
	delete m_allDocs;
	INF_LOG("BulidDescisionTree begin");
	m_treeRoot = BulidDescisionTreeRecursion(m_treeRoot, docs, words, m_iDepth);
	INF_LOG3("BulidDescisionTree over and StartUp ok depth %d", m_iDepth);
#ifdef _DEBUG
	PrintTreeInfo(m_treeRoot, 0);
#endif	
	return true;
}

bool YS_DescisionTree::StartUpLoop()
{
	
	PrintInfo();
	list<string>* words = new list<string>();
	m_allWords->ChangeToArray(*words);
	delete m_allWords;
	m_allWords = NULL;
	YS_DataNode* docListHead = NULL;
	docListHead = m_allDocs->ChangeToArray(docListHead);
	delete m_allDocs;
	m_allDocs = NULL;
	INF_LOG("BulidDescisionTree begin");
	if(BulidDescisionTreeLoop(docListHead, words))
	INF_LOG3("BulidDescisionTree over and StartUp ok depth %d", m_iDepth);
	
#ifdef _DEBUG
	PrintTreeInfo(m_treeRoot, 0);
#endif	
	return true;
}

bool YS_DescisionTree::SetData(const string& className, const string& document, const string& word)
{
	// ���ʹ����ĵ�
	m_wordsInDocs->AddKeyValue(word, document);
	// �ĵ���������
	m_docsHaveWords->AddKeyValue(document, word);
	m_allWords->AddLable(word);
	m_allDocs->AddLable(document);
	m_allDocsInfo->AddKeyValue(document, className);	
}

YS_DescitionTreeNode* YS_DescisionTree::BulidDescisionTreeRecursion(YS_DescitionTreeNode*& treeNode, list<string>& docs, list<string>& words, long& iLevel)
{
	if(docs.size() == 0)
	{
		ERR_LOG("record num is zero");
		iLevel = 0;
		return NULL;
	}
	
	if(treeNode == NULL)
		treeNode = new YS_DescitionTreeNode();
		
	//��������
	if(words.size() == 0)
	{
		treeNode->m_strValue = IsMostTheSame(docs);
		treeNode->m_eType = T_LABEL;
		iLevel =1;
		return treeNode;
	}
	////�ж��Ƿ�����ֹͣ����
	string className;
	if(IsAllTheSame(docs, className))
	{
		treeNode->m_strValue = className;
		treeNode->m_eType = T_LABEL;
		iLevel = 1;
		return treeNode;
	}
	
	////��ȡ�����Ϣ����������������������
	INF_LOG("call GetMaxGainInfo ");
	string strKey = GetMaxGainInfo(docs, words);
	INF_LOG("call GetMaxGainInfo over [%s]", strKey.c_str());
	
	if(strKey.empty())
	{
		ERR_LOG("GainInfo no result to return");
		delete treeNode;
		treeNode = NULL;
		iLevel = 0;
		return NULL;
	}
	
	treeNode->m_strValue = strKey;
	treeNode->m_eType = T_ATTR;
	//ɾ������strKey
	list<string> keyList;
	for(list<string>::iterator wit = words.begin(); wit!=words.end(); ++wit)
	{
		if( *wit == strKey)
			continue;
		else
			keyList.push_back(*wit);
	}
	
	list<string> hasKey;
	list<string> noKey;
	const YS_KeyHashSet* hasKeyDocSet = m_wordsInDocs->getHashSet(strKey);
	for(list<string>::iterator dit = docs.begin(); dit!=docs.end(); ++dit)
	{
		if(hasKeyDocSet->FindLabel(*dit))
			hasKey.push_back(*dit);
		else
			noKey.push_back(*dit);
	}
	//�ݹ齨��
	long iLSonlevel = 0;
	long iRSonlevel = 0;
	INF_LOG("build Yes right tree node");
	treeNode->m_pYesRight = BulidDescisionTreeRecursion(treeNode->m_pYesRight, hasKey, keyList,iRSonlevel);
	INF_LOG("build Yes left tree node");
	treeNode->m_pNoLeft = BulidDescisionTreeRecursion(treeNode->m_pNoLeft, noKey, keyList,iLSonlevel);
	iLevel = ((iLSonlevel>iRSonlevel) ? iLSonlevel:iRSonlevel) + 1;
	INF_LOG("now tree depth is %d", iLevel);
	return treeNode;
}

bool YS_DescisionTree::BulidDescisionTreeLoop(YS_DataNode* docList, list<string>* wordList)
{
	INF_LOG("in BulidDescisionTreeLoop");
	typedef list<string> TStrList;
	deque<YS_DescitionTreeNode**> ptrNodeFifoList;  //�������ڵ���У�ÿ��Ԫ��Ϊһ�����ڵ�
	deque<YS_DataNode*> docNodeFifoList;            //ѵ����¼���У�ÿ��Ԫ��Ϊһ����¼����
	deque<TStrList*> wordNodeFifoList;              //���ʼ�¼���У�ÿ��Ԫ��Ϊһ�����ʼ���
	
	
	ptrNodeFifoList.push_back(&m_treeRoot);
	docNodeFifoList.push_back(docList);
	wordNodeFifoList.push_back(wordList);
	
	while(ptrNodeFifoList.size()!=0 && docNodeFifoList.size()!=0 &&wordNodeFifoList.size()!=0 )
	{
		if(ptrNodeFifoList.size() != docNodeFifoList.size()) 
		{
			ERR_LOG("FATAL ERROR in BulidDescisionTreeLoop  ptrNodeFifoList.size:%d docNodeFifoList.size: %d ", ptrNodeFifoList.size(),docNodeFifoList.size() );
			return false;
		}
		if(ptrNodeFifoList.size() != wordNodeFifoList.size()) 
		{
			ERR_LOG("FATAL ERROR in BulidDescisionTreeLoop  ptrNodeFifoList.size:%d wordNodeFifoList.size: %d ", ptrNodeFifoList.size(),wordNodeFifoList.size() );
			return false;
		}
		++m_iDepth;
		DEBUG_LOG("in BulidDescisionTreeLoop while %ld", m_iDepth);
		///1 ��ȡ��ǰ�ڵ�����
		YS_DescitionTreeNode** treeNode = ptrNodeFifoList.front();
		ptrNodeFifoList.pop_front();
		
		YS_DataNode* docNode = docNodeFifoList.front();  //���treeNodeΪҶ�ӽڵ� ��Ҫ�ͷ� docNode�е�����
		docNodeFifoList.pop_front();
		
		TStrList* words = wordNodeFifoList.front();  //words��Ҫ�ͷ�mem
		wordNodeFifoList.pop_front();
		
		///2 ��¼Ϊ��,�ͷŹؼ����б�
		if(docNode == NULL)
		{
			words->clear();
			delete words;
			words = NULL;
			ERR_LOG("record num is zero");
			continue;
		}
		
		///3 �����¼��
		//Ϊ�½��ڵ�����ڴ�
		if(*treeNode == NULL)
			*treeNode = new YS_DescitionTreeNode();
		
		///4 �������� ����Ҷ�ӽڵ�
		//if(words->size() == 0 || lLength < MIN_RECORD_QUANTITY)
		if(words->size() == 0)
		{
			(*treeNode)->m_strValue = IsMostTheSame(docNode);
			(*treeNode)->m_eType = T_LABEL;
			//�ͷż�¼�ڴ�
			for(YS_DataNode* docItem = docNode; docItem!=NULL; docItem=docItem->m_next)
			{
				delete docItem;
			}
			//ɾ��words mem
			delete words;
			words = NULL;
			INF_LOG(" leaf node is most same [%s]", (*treeNode)->m_strValue.c_str());
			continue;
		}
		
		///5 �ж��Ƿ�ʣ���¼�Ƿ�����ͬһ��
		string className;
		if(IsAllTheSame(docNode, className))
		{
			(*treeNode)->m_strValue = className;
			(*treeNode)->m_eType = T_LABEL;
			words->clear();
			delete words;
			words = NULL;
			for(YS_DataNode* docItem = docNode; docItem!=NULL; docItem=docItem->m_next)
			{
				delete docItem;
			}
			INF_LOG("leaf node is all same [%s]", className.c_str());
			continue;
		}
	
		///6 ��ȡ�����Ϣ����������������������
		DEBUG_LOG("call GetMaxGainInfo ");
		string strKey = GetMaxGainInfo(docNode, *words);
		DEBUG_LOG("call GetMaxGainInfo over [%s]", strKey.c_str());
		//��ȡʧ�ܣ��ͷ��ڴ�
		if(strKey.empty())
		{
			ERR_LOG("GainInfo no result to return");
			//delete *treeNode;
			//*treeNode = NULL;
			(*treeNode)->m_strValue = IsMostTheSame(docNode);
			(*treeNode)->m_eType = T_LABEL;
			words->clear();
			delete words;
			words = NULL;
			for(YS_DataNode* docItem = docNode; docItem!=NULL; docItem=docItem->m_next)
			{
				delete docItem;
			}
			continue;
		}
		//�ɹ�  ������������
		(*treeNode)->m_strValue = strKey;
		(*treeNode)->m_eType = T_ATTR;
		
		//���ʷ���
		list<string>* keyListY = new list<string>();
		list<string>* keyListN = new list<string>();
		for(list<string>::iterator wit = words->begin(); wit!=words->end(); ++wit)
		{
			if( *wit == strKey)
				continue;
			else
			{
				keyListN->push_back(*wit);
				keyListY->push_back(*wit);
			}
		}
		
		//��¼����
		YS_DataNode* hasKeyDocHead = NULL;
		YS_DataNode* noKeyDocHead = NULL;
		int hasKeyNum = 0;
		int noKeyNum = 0;
		
		YS_DataNode* docHead = docNode;
		YS_DataNode* newHead = NULL;
		const YS_KeyHashSet* hasKeyDocSet = m_wordsInDocs->getHashSet(strKey);
		if(hasKeyDocSet == NULL)
		{
			ERR_LOG("no %s   in hasKeyDocSet", strKey.c_str());
			continue;
		}
		
		while(docHead != NULL)
		{
			newHead = docHead->m_next;
			if(hasKeyDocSet->FindLabel(docHead->m_strValue))
			{
				//��ͷ������hasKeyDocHead
				docHead->m_next = hasKeyDocHead;
				hasKeyDocHead = docHead;
				docHead = newHead;
				++hasKeyNum;
			}
			else
			{
				docHead->m_next = noKeyDocHead;
				noKeyDocHead = docHead;
				docHead = newHead;
				++noKeyNum;
			}
		}
		
		words->clear();
		delete words;
		words = NULL;
		//���������ڵ�
		if(hasKeyNum != 0 && hasKeyDocHead != NULL)
		{
			ptrNodeFifoList.push_back( &((*treeNode)->m_pYesRight) );
			docNodeFifoList.push_back(hasKeyDocHead);
			wordNodeFifoList.push_back(keyListY);
			INF_LOG(" ����strKey[%s] �ļ�¼�� %d  right ��֧ go", strKey.c_str(),hasKeyNum);
		}
		else
		{
			INF_LOG(" ����[%s]�ļ�¼��[%d] right ��֧ stop", strKey.c_str(),hasKeyNum);
		}
		
		if(noKeyNum != 0 && noKeyDocHead != NULL)
		{
			ptrNodeFifoList.push_back( &((*treeNode)->m_pNoLeft) );
			docNodeFifoList.push_back(noKeyDocHead);
			wordNodeFifoList.push_back(keyListN);
			INF_LOG(" ������[%s]�ļ�¼��[%d]  left ��֧ go", strKey.c_str(),noKeyNum);
		}
		else
		{
			INF_LOG(" ������[%s]�ļ�¼��[%d]  left ��֧ stop", strKey.c_str(),noKeyNum);
		}
		/****************************************/
	}
	m_iDepth = static_cast<long> (log(m_iDepth+1)/log(2));
	
	return true;
}

bool YS_DescisionTree::Predict(const list<string>& item, string& className)
{
	DEBUG_LOG("YS_DescisionTree::Predict ");
	list<string> items = item;
	return PredictFromTree(m_treeRoot, items, className);
};

bool YS_DescisionTree::PredictFromTree(YS_DescitionTreeNode * node, list<string>& item, string& className)
{
	DEBUG_LOG(">>>>PredictFromTree son tree");
	if(node == NULL)
	{
		DEBUG_LOG("no result, classify failure");
		return false;
	}
	if(node->m_eType == T_LABEL)
	{
		className = node->m_strValue;
		DEBUG_LOG("result, classify success %s ",node->m_strValue.c_str());
		return true;
	}
	
	bool ifHasFlag = false;
	T_StrListIter iter = find(item.begin(), item.end(), node->m_strValue);
	if(iter != item.end())
	{
		ifHasFlag = true;
		item.erase(iter);
	}
	if(ifHasFlag)
		return PredictFromTree(node->m_pYesRight, item, className);
	else
		return PredictFromTree(node->m_pNoLeft, item, className);
}

string YS_DescisionTree::GetMaxGainInfo(const list<string>& docList, const list<string>& words)
{
	///�÷�֧�ĵ�����
	int docCounter = docList.size();
	if(docCounter == 0)
		return string("");
	if(words.size() == 0)
		return string("");
		
	////����H(X)
	double dHXinfo = 0.0;
	YS_StrHashMap<double> calssInfoList; //��������µ��ĵ�����
	for(T_StrConstListIter it = docList.begin(); it!=docList.end(); ++it)
	{
		string className("");
		if(!m_allDocsInfo->FindKeyValue(*it, className)) //�����ĵ��������
			continue;

		double counter = 1.0;
		if(!calssInfoList.FindKeyValue(className, counter))// not exist
		{
			calssInfoList.AddKeyValue(className, counter);
		}
		else
		{
			counter += 1.0;
			calssInfoList.ReplaceKeyValue(className, counter); //exist
		}
	}
	
	for(calssInfoList.Begin(); !calssInfoList.IsEnd(); calssInfoList.Next())
	{
		dHXinfo += Entropy(calssInfoList.IterSecond(), docCounter);
	}
		
	////����GAIN(wid)
	double dMaxGainInfoWord = 0.0;
	string dMaxHXWord = "";
	YS_StrHashMap<double> calssWordInfoList; //��������µ��ĵ�����
	for(T_StrConstListIter wit = words.begin(); wit!=words.end(); ++wit)
	{
		//���Һ���wid�ļ��ĵ��б�
		const YS_KeyHashSet* docSet = m_wordsInDocs->getHashSet(*wit);
		if(docSet == NULL)
			return false;
		int wordYCounter = 0; //��docList�к���wit�ؼ��ֵ��ĵ�����
		int wordNCounter = 0; //��docList�в�����wit�ؼ��ֵ��ĵ�����
		calssWordInfoList.Clear(); //��¼�������к���wid�ĸ���
		for(T_StrConstListIter dit = docList.begin(); dit!=docList.end(); ++dit)
		{
			if(docSet->FindLabel(*dit)) //dit�Ƿ���wid����
			{
				++wordYCounter;
				string className("");
				if(!m_allDocsInfo->FindKeyValue(*dit, className)) //�����ĵ��������
					continue;

				double counter = 1.0;
				if(!calssWordInfoList.FindKeyValue(className, counter))
				{
					calssWordInfoList.AddKeyValue(className, counter);
				}
				else
				{
					counter += 1.0;
					calssWordInfoList.ReplaceKeyValue(className, counter);
				}
			}
			else
				++wordNCounter;
			
		}
		
		double dHXWordInfo = 0.0;
		for(calssInfoList.Begin(); !calssInfoList.IsEnd(); calssInfoList.Next())
		{
			///����H(X|wid)
			// IG(�г�) = H(��ҵ) + H(����) �C H(����|�г�) �C H(��ҵ|�г�)
			//H(����|�г�)= P(����|�г�)log(P(����|�г�)) + P(����|^�г�)log(P(����|^�г�))
			//H(��ҵ|�г�)= P(��ҵ|�г�)log(P(��ҵ|�г�)) + P(��ҵ|^�г�)log(P(��ҵ|^�г�))
			double wordInClass = 0.0;
			double noWidInClassCounter = 0.0;
			calssWordInfoList.FindKeyValue(calssInfoList.IterFirst(), wordInClass);
			noWidInClassCounter = calssInfoList.IterSecond() - wordInClass;
			
			dHXWordInfo += wordYCounter/docCounter * Entropy(calssInfoList.IterSecond(), wordYCounter) 
							+ wordNCounter/docCounter * Entropy(noWidInClassCounter, wordNCounter);
		}
		
		//����GAIN(wid)
		double dGainInfoWord = dHXinfo - dHXWordInfo;
		if(dGainInfoWord > dMaxGainInfoWord)
		{
			dMaxGainInfoWord = dGainInfoWord;
			dMaxHXWord = *wit;
		}
	}	//end for wid
	return dMaxHXWord;
}

//ID3
string YS_DescisionTree::GetMaxGainInfo(YS_DataNode* docList, const list<string>& words)
{
	/**1. �÷�֧�ĵ����� **/
	if(words.size() == 0)
	{
		ERR_LOG("word num is 0 ,make an error");
		return string("");
	}
	
	if(docList == NULL)
	{
		ERR_LOG("docList is 0 ,make an error");
		return string("");
	}
	
	/**2. ����H(X) ��Ϣ��**/
	double dHXinfo = 0.0;
	double docCounter = 0.0;
	YS_StrHashMap<double> calssRecordNum; //���������¼��
	///2.1����������ຬ�м�¼��
	for(YS_DataNode* docNode = docList; docNode!=NULL; docNode=docNode->m_next)
	{
		docCounter += 1.0;
		string className("");
		if(!m_allDocsInfo->FindKeyValue(docNode->m_strValue, className)) //�����ĵ��������
			continue;

		double counter = 1.0;
		if(!calssRecordNum.FindKeyValue(className, counter))// not exist
		{
			calssRecordNum.AddKeyValue(className, counter);
		}
		else
		{
			counter += 1.0;
			calssRecordNum.ReplaceKeyValue(className, counter); //exist
		}
	}
	if(docCounter < MIN_RECORD_QUANTITY ) return string("");
	
	///2.2 ����H(X)
	for(calssRecordNum.Begin(); !calssRecordNum.IsEnd(); calssRecordNum.Next())
	{
		dHXinfo += Entropy(calssRecordNum.IterSecond(), docCounter);
	}	
	if( IF_ZERO(dHXinfo) ) return string("");
	/** 3.����GAIN(wid)**/
	double dMaxGainInfoWord = 0.0; //max GainInfo value
	string dMaxHXWord = "";        //max GainInfo keyword
	YS_StrHashMap<double> keyCalssRecordNum; //�����ؼ����·����¼��
	///3.1����ÿ��������������Ϣ��
	for(T_StrConstListIter wit = words.begin(); wit!=words.end(); ++wit)
	{
		///3.2��ȡ����wid�ļ�¼�б�
		const YS_KeyHashSet* docSet = m_wordsInDocs->getHashSet(*wit);
		if(docSet == NULL)
		{
			ERR_LOG("fatal error %s is not in any files", wit->c_str());
			return false;
		}
		double wordYCounter = 0.0; //��docList�к���wit�ؼ��ֵ��ĵ�����
		double wordNCounter = 0.0; //��docList�в�����wit�ؼ��ֵ��ĵ�����
		keyCalssRecordNum.Clear(); //��¼�������к���wid�ĸ���
		///3.3 �鿴��ǰ��¼�Ƿ�����ں���wid�ļ�¼�б���, ��������Ͳ�����wid�ļ�¼��
		for(YS_DataNode* docNode = docList; docNode!=NULL; docNode=docNode->m_next)
		{
			if(docSet->FindLabel(docNode->m_strValue)) //dit����wid����
			{
				wordYCounter += 1.0;
				string className("");
				if(!m_allDocsInfo->FindKeyValue(docNode->m_strValue, className)) //�����ĵ��������
				{
					ERR_LOG("fatal error %s does not belong to  any classes", docNode->m_strValue.c_str());
					continue;
				}

				double counter = 1.0;
				if(!keyCalssRecordNum.FindKeyValue(className, counter))
				{
					keyCalssRecordNum.AddKeyValue(className, counter);
				}
				else
				{
					counter += 1.0;
					keyCalssRecordNum.ReplaceKeyValue(className, counter);
				}
			}
		}
		wordNCounter = docCounter - wordYCounter;
		///3.4����H(X|wid) ������
		double dHXWordInfo = 0.0;
		for(calssRecordNum.Begin(); !calssRecordNum.IsEnd(); calssRecordNum.Next())
		{
			
			// IG(�г�) = H(��ҵ) + H(����) �C H(����|�г�) �C H(��ҵ|�г�)
			//H(����|�г�)= P(����|�г�)log(P(����|�г�)) + P(����|^�г�)log(P(����|^�г�))
			//H(��ҵ|�г�)= P(��ҵ|�г�)log(P(��ҵ|�г�)) + P(��ҵ|^�г�)log(P(��ҵ|^�г�))
			double keyclassCounter = 0.0;  //����wid��calssRecordNum.IterFirst���¼��
			keyCalssRecordNum.FindKeyValue(calssRecordNum.IterFirst(), keyclassCounter);
			double noWidInClassCounter = calssRecordNum.IterSecond() - keyclassCounter;
			
			DEBUG_LOG("H(%s|%s) docList�ĵ��ܸ��� %f "
						"����[%s]�ؼ��ֵ��ĵ����� %f, ������[%s]�ĸ��� %f " 
						" ����[%s]��[%s]�����ĵ����� %f 	"
						" ������[%s]��[%s]�����ĵ����� %f  H(Y|X)=%f" ,
							calssRecordNum.IterFirst().c_str(),wit->c_str() ,docCounter, 
							wit->c_str(),wordYCounter,wit->c_str(),wordNCounter ,
							wit->c_str(),calssRecordNum.IterFirst().c_str(),keyclassCounter,
							wit->c_str(),calssRecordNum.IterFirst().c_str(),noWidInClassCounter, dHXWordInfo );
			//P(����|�г�)log(P(����|�г�))
			double dHaveHX = wordYCounter/docCounter * Entropy(keyclassCounter, wordYCounter) ;
			dHXWordInfo += dHaveHX;
			//P(����|^�г�)log(P(����|^�г�))
			double dNoHX = wordNCounter/docCounter * Entropy(noWidInClassCounter, wordNCounter);
			dHXWordInfo += dNoHX;
			DEBUG_LOG("H(%s|%s)=%e(%e + %e)", calssRecordNum.IterFirst().c_str(),wit->c_str() ,dHXWordInfo, dHaveHX, dNoHX);
		}
		///3.5����GAIN(wid) ��Ϣ����
		double dGainInfoWord = dHXinfo - dHXWordInfo;
		DEBUG_LOG(">>>>>>>>>>>GAIN_INFO(%s)=%e(%e-%e)  max %s:%e", wit->c_str(), dGainInfoWord, dHXinfo ,dHXWordInfo, dMaxHXWord.c_str(), dMaxGainInfoWord);
		//����GainInfo���ĵ���
		if(dGainInfoWord > dMaxGainInfoWord)
		{
			dMaxGainInfoWord = dGainInfoWord;
			dMaxHXWord = *wit;
		}
	}	//end for wid
	return dMaxHXWord;
}

string YS_DescisionTree::GetMaxGainRate(YS_DataNode* docList, const list<string>& words )//���������
{
	/**1. �÷�֧�ĵ����� **/
	if(words.size() == 0)
	{
		ERR_LOG("word num is 0 ,make an error");
		return string("");
	}
		
	/**2. ����splitInfo ��Ϣ��**/
	double docCounter = 0.0;
	YS_StrHashMap<double> calssRecordNum; //���������¼��
	///2.1����������ຬ�м�¼��
	for(YS_DataNode* docNode = docList; docNode!=NULL; docNode=docNode->m_next)
	{
		docCounter += 1.0;
		string className("");
		if(!m_allDocsInfo->FindKeyValue(docNode->m_strValue, className)) //�����ĵ��������
			continue;

		double counter = 1.0;
		if(!calssRecordNum.FindKeyValue(className, counter))// not exist
		{
			calssRecordNum.AddKeyValue(className, counter);
		}
		else
		{
			counter += 1.0;
			calssRecordNum.ReplaceKeyValue(className, counter); //exist
		}
	}
		
	/** 3.����GAIN(wid)**/
	double dMaxGainInfoWord = 0.0; //max GainInfo value
	string dMaxHXWord = "";        //max GainInfo keyword
	YS_StrHashMap<double> keyCalssRecordNum; //�����ؼ����·����¼��
	///3.1����ÿ��������������Ϣ��
	for(T_StrConstListIter wit = words.begin(); wit!=words.end(); ++wit)
	{
		///3.2��ȡ����wid�ļ�¼�б�
		const YS_KeyHashSet* docSet = m_wordsInDocs->getHashSet(*wit);
		if(docSet == NULL)
		{
			ERR_LOG("fatal error %s is not in any files", wit->c_str());
			return false;
		}
		double wordYCounter = 0.0; //��docList�к���wit�ؼ��ֵ��ĵ�����
		double wordNCounter = 0.0; //��docList�в�����wit�ؼ��ֵ��ĵ�����
		double dHXinfo = 0.0; //splitInfo
		keyCalssRecordNum.Clear(); //��¼�������к���wid�ĸ���
		///3.3 �鿴��ǰ��¼�Ƿ�����ں���wid�ļ�¼�б���, ��������Ͳ�����wid�ļ�¼��
		for(YS_DataNode* docNode = docList; docNode!=NULL; docNode=docNode->m_next)
		{
			if(docSet->FindLabel(docNode->m_strValue)) //dit����wid����
			{
				wordYCounter += 1.0;
				string className("");
				if(!m_allDocsInfo->FindKeyValue(docNode->m_strValue, className)) //�����ĵ��������
				{
					ERR_LOG("fatal error %s does not belong to  any classes", docNode->m_strValue.c_str());
					continue;
				}

				double counter = 1.0;
				if(!keyCalssRecordNum.FindKeyValue(className, counter))
				{
					keyCalssRecordNum.AddKeyValue(className, counter);
				}
				else
				{
					counter += 1.0;
					keyCalssRecordNum.ReplaceKeyValue(className, counter);
				}
			}
		}
		wordNCounter = docCounter - wordYCounter;
		dHXinfo = Entropy(wordYCounter, docCounter) + Entropy(wordNCounter, docCounter);
		if(dHXinfo - 0 < 0.000001 ) continue;
		///3.4����H(X|wid) ������
		double dHXWordInfo = 0.0;
		for(calssRecordNum.Begin(); !calssRecordNum.IsEnd(); calssRecordNum.Next())
		{
			// IG(�г�) = H(��ҵ) + H(����) �C H(����|�г�) �C H(��ҵ|�г�)
			//H(����|�г�)= P(����|�г�)log(P(����|�г�)) + P(����|^�г�)log(P(����|^�г�))
			//H(��ҵ|�г�)= P(��ҵ|�г�)log(P(��ҵ|�г�)) + P(��ҵ|^�г�)log(P(��ҵ|^�г�))
			double keyclassCounter = 0.0;  //����wid��calssRecordNum.IterFirst���¼��
			keyCalssRecordNum.FindKeyValue(calssRecordNum.IterFirst(), keyclassCounter);
			double noWidInClassCounter = calssRecordNum.IterSecond() - keyclassCounter;
			
			DEBUG_LOG("H(%s|%s) docList�ĵ��ܸ��� %f "
						"����[%s]�ؼ��ֵ��ĵ����� %f, ������[%s]�ĸ��� %f " 
						" ����[%s]��[%s]�����ĵ����� %f 	"
						" ������[%s]��[%s]�����ĵ����� %f  H(Y|X)=%f" ,
							calssRecordNum.IterFirst().c_str(),wit->c_str() ,docCounter, 
							wit->c_str(),wordYCounter,wit->c_str(),wordNCounter ,
							wit->c_str(),calssRecordNum.IterFirst().c_str(),keyclassCounter,
							wit->c_str(),calssRecordNum.IterFirst().c_str(),noWidInClassCounter, dHXWordInfo );
			//P(����|�г�)log(P(����|�г�))
			double dHaveHX = wordYCounter/docCounter * Entropy(keyclassCounter, wordYCounter) ;
			dHXWordInfo += dHaveHX;
			//P(����|^�г�)log(P(����|^�г�))
			double dNoHX = wordNCounter/docCounter * Entropy(noWidInClassCounter, wordNCounter);
			dHXWordInfo += dNoHX;
			DEBUG_LOG("H(%s|%s)=%e(%e + %e)", calssRecordNum.IterFirst().c_str(),wit->c_str() ,dHXWordInfo, dHaveHX, dNoHX);
		}
		///3.5����GAINRate(wid) ������
		double dGainInfoWord = dHXWordInfo/dHXinfo;
		DEBUG_LOG(">>>>>>>>>>>GAIN_INFO(%s)=%e(%e/%e)  max %s:%e", wit->c_str(), dGainInfoWord, dHXWordInfo, dHXinfo ,dMaxHXWord.c_str(), dMaxGainInfoWord);
		//����GainInfo���ĵ���
		if(dGainInfoWord > dMaxGainInfoWord)
		{
			dMaxGainInfoWord = dGainInfoWord;
			dMaxHXWord = *wit;
		}
	}	//end for wid
	return dMaxHXWord;	
}
///��Ϣ�ؼ��㺯��
double YS_DescisionTree::Entropy(double dOkdata, double dTotaldata)
{
	if( IF_ZERO(dTotaldata) || IF_ZERO(dOkdata) )
	{
		return -0.0;
	}
	double dPropo = dOkdata/dTotaldata;
	return -(dPropo*log(dPropo)/log(2));
}

///�Ƿ�ͬ��
bool YS_DescisionTree::IsAllTheSame(const list<string>& docList, string& value)
{
	if(docList.size() == 0)
	{
		ERR_LOG("IsAllTheSame docList size ==0 ");
		return false;
	}
	
	for(T_StrConstListIter it = docList.begin(); it!=docList.end(); ++it)
	{
		string strClassName("");
		if(!m_allDocsInfo->FindKeyValue(*it, strClassName))
		{
			//err 
			return false;
		}
		if(value.empty())
		{
			value = strClassName;
			continue;
		}
		if(value==strClassName)
			continue; 
		else
		{
			DEBUG_LOG("not same  [%s:%s]", strClassName.c_str(), value.c_str());
			return false;
		}
	}
	return true;
}

string YS_DescisionTree::IsMostTheSame(const list<string>& docList)
{
	if(docList.size() == 0)
	{
		ERR_LOG("IsAllTheSame docList size ==0 ");
		return false;
	}
	YS_StrHashMap<int> calssList;
	
	int iMaxClassCounter = 0;
	string strMaxClassName("");
	for(T_StrConstListIter it = docList.begin(); it!=docList.end(); ++it)
	{
		string className("");
		if(!m_allDocsInfo->FindKeyValue(*it, className))
			continue;

		int counter = 1;
		if(!calssList.FindKeyValue(className, counter))
			calssList.AddKeyValue(className, counter);
		else
		{
			counter += 1;
			calssList.ReplaceKeyValue(className, counter);
		}
		
		if(counter > iMaxClassCounter)
		{
			strMaxClassName = className;
			iMaxClassCounter = counter;
		}	
	}
	
	return strMaxClassName;
}

bool YS_DescisionTree::IsAllTheSame(YS_DataNode* docList, string& value)
{
	if(docList == NULL)
	{
		ERR_LOG("IsAllTheSame docList size ==0 ");
		return false;
	}
	for(YS_DataNode* docNode = docList; docNode!=NULL; docNode=docNode->m_next)
	{
		string strClassName("");
		if(!m_allDocsInfo->FindKeyValue(docNode->m_strValue, strClassName))
		{
			//err 
			return false;
		}
		if(value.empty())
		{
			value = strClassName;
			continue;
		}
		if(value==strClassName)
			continue; 
		else
		{
			DEBUG_LOG("not same  [%s:%s]", strClassName.c_str(), value.c_str());
			return false;
		}
	}
	return true;
}

string YS_DescisionTree::IsMostTheSame(YS_DataNode* docList)
{
	if(docList == NULL)
	{
		ERR_LOG("IsMostTheSame docList size ==0 ");
		return false;
	}
	YS_StrHashMap<int> calssList;
	int iMaxClassCounter = 1;
	string strMaxClassName("");
	for(YS_DataNode* docNode = docList; docNode!=NULL; docNode=docNode->m_next)
	{
		string className("");
		if(!m_allDocsInfo->FindKeyValue(docNode->m_strValue, className))
			continue;

		int counter = 1;
		if(!calssList.FindKeyValue(className, counter))
			calssList.AddKeyValue(className, counter);
		else
		{
			counter += 1;
			calssList.ReplaceKeyValue(className, counter);
		}
		
		if(counter > iMaxClassCounter)
		{
			strMaxClassName = className;
			iMaxClassCounter = counter;
		}
	}
	
	return strMaxClassName;
}
/****************************/
/***  YS_DecisionForest   ***/
/****************************/
YS_DecisionForest::YS_DecisionForest()
{
	m_decisionForest = new YS_StrHashMap<YS_DescisionTree*>();
}
YS_DecisionForest::~YS_DecisionForest()
{
	Clear();
}
bool YS_DecisionForest::StartUp()
{
	bool bFlag = true;
	m_lstTitle = YS_ConfigData::GetInstance()->m_attributeTitle;
	m_attributeKey = YS_ConfigData::GetInstance()->m_attributeKey;
	for(list<string>::iterator iter=m_lstTitle.begin(); iter!=m_lstTitle.end(); ++iter )
	{
		YS_DescisionTree* item = new YS_DescisionTree();
		m_decisionForest->AddKeyValue(*iter, item);
	}
	
	bFlag &= ReadFile();
	for(list<string>::iterator iter=m_lstTitle.begin(); iter!=m_lstTitle.end(); ++iter )
	{
		YS_DescisionTree* treeNode = NULL;
		if(!m_decisionForest->FindKeyValue(*iter, treeNode))
		{
			bFlag &= false;
			continue;
		}
		//bFlag &= treeNode->StartUpRecursion();
		bFlag &= treeNode->StartUpLoop();
	}
	return 	bFlag;
}

bool YS_DecisionForest::Clear()
{
	for(list<string>::iterator iter=m_lstTitle.begin(); iter!=m_lstTitle.end(); ++iter )
	{
		YS_DescisionTree* treeNode = NULL;
		if(!m_decisionForest->FindKeyValue(*iter, treeNode))
			continue;
		delete treeNode;
		treeNode = NULL;
	}
	delete m_decisionForest;
	return true;
}

YS_DescisionTree* YS_DecisionForest::FindMyAttributeTree(const string& item)
{
	YS_DescisionTree* myTree = NULL;
	if(m_decisionForest->FindKeyValue(item, myTree))
		return myTree;
	return NULL;
}

bool YS_DecisionForest::SetAttrData(const string& className, const string& attr, const list<string>& value, const string& documentId)
{
	YS_DescisionTree* treeNode = FindMyAttributeTree(attr);
	if(treeNode != NULL)
	{
		for(T_StrConstListIter wid=value.begin(); wid!=value.end(); ++wid)
			treeNode->SetData(className, documentId, *wid);
	}
	else
	{
		ERR_LOG("no this tree node [%s]", attr.c_str());
		return false;
	}
	return true;
}

bool YS_DecisionForest::ReadFile()
{
	//get data Reource
	list<string> flieList;
	if( !CCommon::GetDirFile(YS_ConfigData::GetInstance()->m_strTrainPath, flieList))
	{
		ERR_LOG("%s[%d]GetDirFile failure:%s ", __FILE__, __LINE__, YS_ConfigData::GetInstance()->m_strTrainPath.c_str());
		return false;
	}
	
	if(flieList.size() == 0)
	{
		ERR_LOG("GetDirFile empty:%s ", YS_ConfigData::GetInstance()->m_strTrainPath.c_str());
		return false;
	}
	
	char cLineBuf[MAX_BUFFER_LEN];
	cLineBuf[0] = 0x00;
	//process by row
	YS_WordSegmentLR& wordSegmentLR = Ys_ClassifierManager::GetInstance()->GetWordSegment();
	memset(cLineBuf, 0, MAX_BUFFER_LEN);
	for(list<string>::iterator fit=flieList.begin(); fit!=flieList.end(); ++fit)
	{
		size_t iFirstPos = (*fit).find_first_of("0123456789");
		if(iFirstPos == std::string::npos)
		{
			ERR_LOG("can't find number in file name");
			continue;
		}
        string strClassName = fit->substr(0,iFirstPos);
		
		string strFile = YS_ConfigData::GetInstance()->m_strTrainPath + "/" + *fit;
		YS_FileProcessor filePtr(strFile.c_str());
		if(!filePtr.OpenForRead())
		{
			ERR_LOG("open file err:%s ", strFile.c_str());
			continue;
		}
		//INF_LOG("open file  %s  ",strFile.c_str());
		unsigned int fileSize = 0;
		while(!filePtr.IsEnd())
		{
			filePtr.ReadLine(cLineBuf, MAX_BUFFER_LEN);
			cLineBuf[MAX_BUFFER_LEN-1]= 0x00;
			
			if(cLineBuf[0]==0x00) //empty row
			{
				INF_LOG("    empty column  ");
				continue;
			}
			++fileSize;
			list<string> attrList;
			CCommon::split(cLineBuf, attrList, SPLIT_CHAR);
			
			//���Ը�������
			if(attrList.size() < m_lstTitle.size() )
			{
				ERR_LOG("    error column real size %d need size %d  ", attrList.size(), m_lstTitle.size());
				continue;
			}
			
			list<string>::iterator iPos = m_lstTitle.begin();
			for( list<string>::iterator it=attrList.begin(); it!=attrList.end()&& iPos!=m_lstTitle.end(); ++it,++iPos)
			{
				
				if(m_attributeKey == *iPos)
				{
					Ys_BadCharacter::GetInstance()->earseInvalidChar(*it);
				}
				//
				list<YSS_Word> lstWord;
				list<string> listWord; 
				wordSegmentLR.ExtractWordSegment(it->c_str(), lstWord);
				for(list<YSS_Word>::iterator pos = lstWord.begin(); pos!=lstWord.end(); ++pos)
				{
					if(Ys_BadCharacter::GetInstance()->findBadChar(pos->m_szWord))
					{
						//INF_LOG("delete bad character %s ", pos->m_szWord);
						continue;
					}
					listWord.push_back( pos->m_szWord );
				}
				//�ͷ���Դ	
				wordSegmentLR.DestroyWordSegment(lstWord);
				
				cLineBuf[0]=0x00;
				sprintf(cLineBuf, "%s%u", fit->c_str(), fileSize);
				SetAttrData(strClassName, *iPos, listWord, cLineBuf);
			}//end of attribute i	
		}//end for read line
		filePtr.CloseFile();
	}//end process file

	return true;
}

bool YS_DecisionForest::PredictingByAttribute(const string& attri, const list<string>& item, string& ret)
{
	YS_DescisionTree* treeNode = NULL;
	if(!m_decisionForest->FindKeyValue(attri, treeNode))
		return false;
	//Predict(const list<string>& item, string className)
	return treeNode->Predict(item, ret);
}
