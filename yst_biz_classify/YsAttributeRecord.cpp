/*
 *Ys_Attribute.cpp
 *
 * Created on:2013-1-15
 *   Author miffa chandler
 *
 * */

#include "YsAttributeRecord.h"
#include "YsTools.h"
#include <iostream>

string Ys_Attribute::GetName(){
    return m_strName;
}

Ys_Attribute::Ys_Attribute(const string &str ){
    m_strName = str;
}

bool Ys_Attribute::AddWord(const string &word){
    m_WordList.push_back(word);
    return true;
}

Ys_Attribute::~Ys_Attribute(){
    m_strName = "";
    m_WordList.clear();
}
int Ys_Attribute::GetWordNum(){
    return m_WordList.size();
}

int Ys_Attribute::ViewAttribute(){
    cout<<"        "<<m_strName<<":"<<endl;
	cout<<"                ";
    for(unsigned int i=0 ; i<m_WordList.size();i++){
        cout<<m_WordList[i]<<"|";
    }
    cout<<endl;
    return (1);
}
bool Ys_Attribute::GetWordList(vector<string> &vWordList){
    vWordList = m_WordList;
    return true;
}

vector<string>& Ys_Attribute::GetWordListReference(){
    return m_WordList;
}

string Ys_Attribute::GetAttributeName(){
    return m_strName;
}

Ys_Record::Ys_Record(){
    m_iID = 0;
    m_strClassName="";
}

Ys_Record::Ys_Record(const int &ID,const string &strClass){
    m_iID = ID;
    m_strClassName = strClass;
}

Ys_Record::Ys_Record(const string &strClass)
{
    m_iID = 0;
    m_strClassName = strClass;
}


Ys_Record::Ys_Record(const int &ID){
    m_iID = ID;
    m_strClassName="";
}

int Ys_Record::AddAttribute(const string &str){
    map<string,Ys_Attribute*>::iterator it;
    it = m_mapAttributeList.find(str);
    if(it != m_mapAttributeList.end()){
        cout<<"Attribute has been added to List!"<<endl;
        return(-1);
    }
    Ys_Attribute* temp = new Ys_Attribute(str);
    m_mapAttributeList.insert(pair<string,Ys_Attribute*>(str,temp));
    return 1;
}

int Ys_Record::AddWordToAttribute(const string &str,
                                  const vector<string> &WordList){
    map<string,Ys_Attribute*>::iterator it;
    it = m_mapAttributeList.find(str);
    if(it == m_mapAttributeList.end()){
        cout<<"Attribute not in the List";
        return (-1);
    }
    Ys_Attribute* temp = it->second;
    for(unsigned int i=0;i<WordList.size();i++){
        temp->AddWord(WordList[i]);    
    }
    return(-1);
}

Ys_Record::Ys_Record(const Ys_Record &source){
    
    for(map<string,Ys_Attribute*>::iterator it = m_mapAttributeList.begin();it!=m_mapAttributeList.end(); ++it){
        delete it->second;
    }
	m_mapAttributeList.clear();
    m_iID = source.m_iID;
    m_strClassName = source.m_strClassName;
    const map<string,Ys_Attribute*> &tmpAttributeList = source.GetAllAttribute();
    map<string,Ys_Attribute*>::const_iterator itconst;
    for(itconst = tmpAttributeList.begin();itconst!= tmpAttributeList.end();++itconst){
        string attribute = itconst->first;
        Ys_Attribute* temp = new Ys_Attribute(itconst->second->GetAttributeName());
        vector<string> vWordListtmp;
        itconst->second->GetWordList(vWordListtmp); 
        for(unsigned int i=0;i<vWordListtmp.size();i++){
            temp->AddWord(vWordListtmp[i]);
        }
        m_mapAttributeList.insert(pair<string,Ys_Attribute*>(attribute,temp));
    }
}

Ys_Record::~Ys_Record(){
    for(map<string,Ys_Attribute*>::iterator it = m_mapAttributeList.begin();it!=m_mapAttributeList.end(); ++it){
        delete it->second;
    }
	m_mapAttributeList.clear();
}

int Ys_Record::ViewRecord(){
   /* map<string,Ys_Attribute*>::iterator it;
    for(it=m_mapAttributeList.begin();it!=m_mapAttributeList.end();it++){
        it->second->ViewAttribute();
    } 
	*/
    return (1);
}

const Ys_Attribute* Ys_Record::GetAttributeByName(const string &AttributeName){

    map<string,Ys_Attribute*>::iterator it;
    if((it = m_mapAttributeList.find(AttributeName)) == m_mapAttributeList.end())
        return NULL;
    else 
        return it->second;
}

const map<string,Ys_Attribute*> &Ys_Record::GetAllAttribute()const{
    return  m_mapAttributeList; 
}

bool operator ==(const YsRetData& it1, const YsRetData& it2)
{
	return it1.m_className == it2.m_className;
};

//降序排列
bool YsSortComparer::operator() (const YsRetData& item1, const YsRetData& item2)
{
	return item1.m_posProb > item2.m_posProb;
}
