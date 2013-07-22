/*
 * YsGetRecord.cpp
 *  Created on 2013-1-16
 *      Author:miffa chandler
 *
 * */
#include "YssCommon.h"
#include "YsWordSegmentLR.h"
#include "YsClassifierManager.h"
#include "YsGetRecord.h"
#include "YsTools.h"
#include <fstream>
#include <cstdlib>

Ys_GetRecord::Ys_GetRecord(const string &dir){
    m_strPath = dir;
    m_boolHaveFile = false ;
    m_vRecordFileList.clear();
}

bool Ys_GetRecord::StartGet( YS_WordSegmentLR &wordSegment,const bool bIfWordSegment){
    list<string> lFileNames;
    if(!CCommon::GetDirFile(m_strPath,lFileNames)){
        ERR_LOG("Error happened when getting records from path\n");
        return false;
    }
    
    for(list<string>::iterator itListString = lFileNames.begin();itListString != lFileNames.end();++itListString)
    {
        char cRecord[MAX_LENGTH]; 
       //假定文件名的格式为分类名后跟编号名,表示某一属于一个分类的文件的编号
        int iFirstPos = (*itListString).find_first_of("0123456789");
        if(iFirstPos == std::string::npos)
        {
            ERR_LOG("can't find number in file name");
            continue;
        }
        string strClassName = itListString->substr(0,iFirstPos);
        string strFileNumber = itListString->substr(iFirstPos,std::string::npos);
        //获取改路径下的带路径文件名
        string strFileFullPath;
        strFileFullPath = m_strPath;
        strFileFullPath.append("/" + *itListString);

        Ys_FileRecordList* Filetmp = new Ys_FileRecordList(strClassName,strFileNumber);
        ifstream ifReadFile;
        ifReadFile.open(strFileFullPath.c_str());
        
        //打开用ifstream结构打开文件
        ifReadFile.getline(cRecord,MAX_LENGTH);//读取一行记录
        string strRecord = cRecord;
        strRecord = trim(strRecord);
        while(strRecord.size() != 0){
            vector<string> strAttributeWords;
            GetWordList(strRecord,strAttributeWords,SPLIT_CHAR);
            Ys_Record *temp = new Ys_Record(strClassName);
            
            //添加每一个属性,以及与之相关的关键词
            for(unsigned int i=0;i<strAttributeWords.size();i++){ 
                //属性名获取 
                string strAttributeNametmp = Ys_ClassifierManager::GetInstance()->getAttributeName(i);
				temp->AddAttribute(strAttributeNametmp);
				if( Ys_ClassifierManager::GetInstance()->getKeyAttributeName() == strAttributeNametmp ){
					Ys_BadCharacter::GetInstance()->earseInvalidChar(strAttributeWords[i]);
				}
				
                vector<string> strWordListTemp;
                strWordListTemp.clear();

                list<YSS_Word> lstWord; 
                wordSegment.ExtractWordSegment((strAttributeWords[i]).c_str(),lstWord);
					
				for(list<YSS_Word>::iterator pos = lstWord.begin() ; pos != lstWord.end();++pos){
					if(Ys_BadCharacter::GetInstance()->findBadChar(pos->m_szWord))
						continue;
						strWordListTemp.push_back(pos->m_szWord);
                }	
			    wordSegment.DestroyWordSegment(lstWord);
               temp->AddWordToAttribute(strAttributeNametmp,strWordListTemp);
			
			#ifdef _DEBUG
				//INF_LOG(" record as follows: \n");
				//temp->ViewRecord(); 
			#endif
            }
            Filetmp->AddRecord(temp);  
            ifReadFile.getline(cRecord,MAX_LENGTH);
            strRecord = trim(string(cRecord));
        }//end for
		ifReadFile.close();
        m_vRecordFileList.push_back(Filetmp);
    }
    return true;
}

Ys_GetRecord::~Ys_GetRecord(){
    m_strPath.clear();
    m_boolHaveFile = false;
    for(unsigned int i=0;i<m_vRecordFileList.size();i++){
        delete m_vRecordFileList[i];
    }
    m_vRecordFileList.clear();       
}
const vector<Ys_FileRecordList*>& Ys_GetRecord::GetFileRecordList(){
    return m_vRecordFileList;
}


void Ys_GetRecord::GetWordList(const string &strWhole,vector<string> &strWordList,char delimiter){
	CCommon::split(strWhole, strWordList, delimiter);
    return;
}

string Ys_GetRecord::trim(const string &s){
    static const string delims ="/t/r/n ";
    string::size_type last=s.find_last_not_of(delims);
    if(last !=string::npos){
        return s.substr(s.find_first_not_of(delims),last+1);
    }
    return s;
}

Ys_FileRecordList::Ys_FileRecordList(const string strClassName,const string strFileNumber)
{
    m_strClassName = strClassName;
    m_strFileNumber = strFileNumber;
    m_vRecordList.clear();
}

Ys_FileRecordList::~Ys_FileRecordList()
{
    m_strClassName.clear();
    for(unsigned int i=0;i<m_vRecordList.size();i++)
    {
        delete m_vRecordList[i];
    }
    m_vRecordList.clear();
}

const vector<Ys_Record*>& Ys_FileRecordList::GetRecordList()
{
    return m_vRecordList;
}

bool Ys_FileRecordList::AddRecord(Ys_Record* Recordtmp)
{
    m_vRecordList.push_back(Recordtmp);
    return true;
}
