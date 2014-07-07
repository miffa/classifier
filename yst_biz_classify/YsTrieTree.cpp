#include "YsTrieTree.h"


YsTreeMapOper::YsTreeMapOper()
{

}

YsTreeMapOper::~YsTreeMapOper()
{
    ClearMap();
}

YsTreeNode* YsTreeMapOper::FindPrefix(const string& str)
{
   YsTree_Map_Iter iter = m_hashMap.find(str);
   if(iter == m_hashMap.end())
        return NULL;
   else
        return iter->second;
}

bool YsTreeMapOper::InsertPrefix(const string& name, YsTreeNode* value)
{
    return m_hashMap.insert(YsTree_Map_Pair(name, value)).second;
}

bool YsTreeMapOper::ClearMap()
{
    for(YsTree_Map_Iter iter = m_hashMap.begin(); iter != m_hashMap.end(); ++iter)
    {
        if(iter->second != NULL)
        {
            delete iter->second;
            iter->second = NULL;
        }
    }
}

YsTireTree::YsTireTree()
{

}

YsTireTree::~YsTireTree()
{
    Delete();
}

YsTreeNode* YsTireTree::CreateNewNode (const string& str)// 创建一个新结点
{
    YsTreeNode* treeNode = new (nothrow) YsTreeNode();
    if(treeNode == NULL)
        return NULL;
    treeNode->COMPLETED = false;
    treeNode->strValue = str;
    treeNode->strClassName = "";
    return treeNode;
}

bool YsTireTree::InitTrie ()//初始化 Trie树
{
    m_treeRoot = CreateNewNode("");
	return (m_treeRoot==NULL)?false:true  ;
}

bool YsTireTree::Find (const STR_LIST& strNameList, string& value)// 找到长度为 len的字符串
{
    if(m_treeRoot == NULL)
        return false;
    YsTreeNode* ptrNode = m_treeRoot;
    for(STR_LIST_CONST_ITER it = strNameList.begin(); it!=strNameList.end(); ++it)
    {
        YsTreeNode* ptrTmpNode = ptrNode->m_child.FindPrefix(*it);
        if(ptrTmpNode == NULL)
        {
            //没找到，停止
            //cout<<"find "<<*it<<" failure "<<endl;
            break;
        }
        //cout<<"find "<<*it<<" success, will find next word in its child "<<endl;
        //继续在相应节点下寻找
        ptrNode = ptrTmpNode;
    }
    //if(ptrNode == NULL)
    //   return false;
    if(!ptrNode->COMPLETED)
        return false;
    value = ptrNode->strClassName;
    return true;
}
bool YsTireTree::Insert (const STR_LIST& strNameList, const string& value)
{
    YsTreeNode* ptrNode = m_treeRoot;
    for(STR_LIST_CONST_ITER it = strNameList.begin(); it!=strNameList.end(); ++it)
    {
        YsTreeNode* ptrTmpNode = ptrNode->m_child.FindPrefix(*it);
        if(ptrTmpNode == NULL)
        {
           // cout<< "insert action : not find  ["<<*it<<"]  in hash , will insert it"<<endl;
            //hashMap中没有， 新建节点，insert
            ptrTmpNode = CreateNewNode(*it);
            if(ptrTmpNode == NULL)
                return false;
            if(!ptrNode->m_child.InsertPrefix(*it, ptrTmpNode))
            {
                delete ptrTmpNode;
                //cout<< "insert ["<<*it<<"]  failure"<<endl;
            }
            //else
                //cout<< "insert ["<<*it<<"]  success"<<endl;
        }
        //else
             //cout<< "insert action : find  ["<<*it<<"]  in hash, will insert next word in its child"<<endl;
        //继续在相应节点下寻找
        ptrNode = ptrTmpNode;
    }
    ptrNode->COMPLETED = true;
    ptrNode->strClassName = value;
    return true;
}
void YsTireTree::Delete ()//删除整棵树
{
    DeleteTree(m_treeRoot);
}


void YsTireTree::DeleteTree (YsTreeNode* item)
{
    item->m_child.ClearMap();
}
