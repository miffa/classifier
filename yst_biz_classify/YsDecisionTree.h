/*
    Decision  Tree algorithm
*/

#ifndef _YS_DECISION_TREE_H_
#define _YS_DECISION_TREE_H_

#include <string>
#include <list>
using namespace std;

enum TreeNodeType { T_ATTR, T_LABEL, T_UNKNOWN};//node type 属性分支,叶子节点
struct YS_DescitionTreeNode
{
	YS_DescitionTreeNode* m_pNoLeft;
	YS_DescitionTreeNode* m_pYesRight;
	string m_strValue;
	TreeNodeType m_eType;

	YS_DescitionTreeNode():m_pNoLeft(NULL),m_pYesRight(NULL),m_eType(T_UNKNOWN),m_strValue(""){}
	~YS_DescitionTreeNode()
	{ 
		if(m_pNoLeft != NULL) {delete m_pNoLeft; m_pNoLeft=NULL;}
		if(m_pYesRight != NULL) {delete m_pYesRight; m_pYesRight=NULL;}
	}	
};

class YS_KeyHashMap;
class YS_KeyHashSet;
template <class T>
class YS_StrHashMap;
struct YS_DataNode;

class YS_DescisionTree
{
public:
	YS_DescisionTree();
	~YS_DescisionTree();
	void PtrNull();
	bool StartUpRecursion();
	bool StartUpLoop();
	bool SetData(const string& className, const string& document, const string& word);
	bool Predict(const list<string>& item, string& className);
	void PrintInfo();
	void PrintTreeInfo(YS_DescitionTreeNode*, int);
private:
	string GetMaxGainInfo(YS_DataNode* docList, const list<string>& attr );//最大信息增益
	string GetMaxGainRate(YS_DataNode* docList, const list<string>& attr );//最大增益率
	string GetMaxGainInfo(const list<string>& docList, const list<string>& attr );
	bool PredictFromTree(YS_DescitionTreeNode * node, list<string>& item, string& className);
	double Entropy(double dOkdata, double dTotaldata);
	bool IsAllTheSame(const list<string>& docList, string& value);
	bool IsAllTheSame(YS_DataNode* docList, string& value);
	string IsMostTheSame(YS_DataNode* docList);
	string IsMostTheSame(const list<string>& docList);
	YS_DescitionTreeNode* BulidDescisionTreeRecursion(YS_DescitionTreeNode*& tree,list<string>& docs, list<string>& words, long& level);
	bool BulidDescisionTreeLoop(YS_DataNode* docs, list<string>* words);
	YS_DescisionTree(const YS_DescisionTree&);
	YS_DescisionTree& operator=(const YS_DescisionTree&);
private:
	//决策树
	YS_DescitionTreeNode* m_treeRoot;   
	// 单词关联文档
	YS_KeyHashMap* m_wordsInDocs;
	// 文档关联单词
	YS_KeyHashMap* m_docsHaveWords;
	// 文档所属类别
	YS_StrHashMap<string>* m_allDocsInfo;
	//空间单词集合
	YS_KeyHashSet* m_allWords;
	YS_KeyHashSet* m_allDocs;
	long m_iDepth;
	static const double MIN_RECORD_QUANTITY = 10.0;
};

class YS_DecisionForest
{
public:
	YS_DecisionForest();
	~YS_DecisionForest();
	
	bool StartUp();
	bool PredictingByAttribute(const string& attri, const list<string>& item, string& ret);
	private:
	bool ReadFile();
	bool Clear();
	YS_DescisionTree* FindMyAttributeTree(const string& item);
	/**********************  类别名      ******属性     **********属性单词集合 **************记录编号*/
	bool SetAttrData(const string& className, const string& attr, const list<string>& value, const string& documentId);
private:
	YS_StrHashMap<YS_DescisionTree*>* m_decisionForest;
	list<string> m_lstTitle;
	string m_attributeKey;
};

#endif


