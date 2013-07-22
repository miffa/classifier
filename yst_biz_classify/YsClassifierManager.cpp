#include "YsClassifierManager.h"
#include "YsTools.h"
#include "YssCommon.h"
#include "YsAttributeRecord.h"
#include "YsWordSegmentLR.h"

#include <list>
using namespace std;

Ys_ClassifierManager* Ys_ClassifierManager::m_instance = NULL;
string Ys_ClassifierManager::m_strNullStr = "";

Ys_ClassifierManager* Ys_ClassifierManager::GetInstance()
{
	if(m_instance == NULL)
		m_instance = new Ys_ClassifierManager();
	return m_instance;
}

bool Ys_ClassifierManager::StartUp()
{
	m_attributeTitle = YS_ConfigData::GetInstance()->m_attributeTitle;
	m_attributeKey = YS_ConfigData::GetInstance()->m_attributeKey;
	
	m_attributeKeyPos = 0;
	string keyAttribute = *(m_attributeTitle.begin());
	for( list<string>::iterator it=m_attributeTitle.begin(); it!=m_attributeTitle.end();++it)
	{
		if(*it==m_attributeKey)
		{
			INF_LOG("    key attribute is [%s:%d] ", m_attributeKey.c_str(), m_attributeKeyPos);
			break;
		}
		m_attributeKeyPos++;
	}
			
	if(m_attributeTitle.size() ==0)
	{
		ERR_LOG("fatal error no attribute in config");
		return false;
	}
	m_adapterItem.clear();
	
	list<string> flieList;
	if( !CCommon::GetDirFilePath(YS_ConfigData::GetInstance()->m_strDictPath, flieList))
	{
		ERR_LOG("GetDirFilePath is error ");
		return false;
	}

	if(-1 == m_wordSegmentLR.Init(flieList)){
		ERR_LOG("Load lr dict %s failed. ", m_strDictPath.c_str());
		return false;;
	}
	return true;
}

bool Ys_ClassifierManager::ProcessDataFile()
{
	//get data Reource
	list<string> flieList;
	if( !CCommon::GetDirFile(YS_ConfigData::GetInstance()->m_strSourcePath, flieList))
	{
		ERR_LOG("%s[%d]GetDirFile failure:%s ", __FILE__, __LINE__, YS_ConfigData::GetInstance()->m_strSourcePath.c_str());
		return false;
	}
	
	if(flieList.size() == 0)
	{
		ERR_LOG("GetDirFile empty:%s ", YS_ConfigData::GetInstance()->m_strSourcePath.c_str());
		return false;
	}
	
	char cLineBuf[MAX_BUFFER_LEN];
	memset(cLineBuf, 0, MAX_BUFFER_LEN);
	
	//output file good file
	sprintf(cLineBuf, "%s/result_good_%d", YS_ConfigData::GetInstance()->m_strGoodDataPath.c_str(), getpid());
	YS_FileProcessor fResultGoodFile(cLineBuf);
	fResultGoodFile.OpenForWrite();
	//INF_LOG("open %s  ",cLineBuf);
	
	//output file bad file
	sprintf(cLineBuf, "%s/result_bad_%d", YS_ConfigData::GetInstance()->m_strBadDataPath.c_str(), getpid());
	YS_FileProcessor fResultBadFile(cLineBuf);
	fResultBadFile.OpenForWrite();
	//INF_LOG("open %s  ",cLineBuf);
	//备份文件夹
	string strDate;
	CCommon::GetY4MD2(time(NULL), strDate);
	string strBakFilePrefix = YS_ConfigData::GetInstance()->m_strBakPath + "/" + strDate;	
	INF_LOG("%s ", strBakFilePrefix.c_str());
	CCommon::CreateNoExistDirectory(strBakFilePrefix);
	//INF_LOG("dir is ok %s  ",strBakFilePrefix.c_str());
	
	//process by row
#ifdef _DEBUG	 
	long totalRows = 0;
	long accrRows = 0;
	long failRows = 0;
#endif
	
	memset(cLineBuf, 0, MAX_BUFFER_LEN);
	for(list<string>::iterator it=flieList.begin(); it!=flieList.end(); ++it)
	{
		string strFile = YS_ConfigData::GetInstance()->m_strSourcePath + "/" + *it;
		YS_FileProcessor filePtr(strFile.c_str());
		filePtr.OpenForRead();
		//INF_LOG("open file  %s  ",strFile.c_str());
		while(!filePtr.IsEnd())
		{
			filePtr.ReadLine(cLineBuf, MAX_BUFFER_LEN);
			cLineBuf[MAX_BUFFER_LEN-1]= 0x00;
			
			if(cLineBuf[0]==0x00) //empty row
			{
				INF_LOG("    empty column  ");
				continue;
			}
			INF_LOG("    column:[%s]  ",cLineBuf);
			list<string> attrList;
			CCommon::split(cLineBuf, attrList, SPLIT_CHAR);
			
			//属性个数不对
			if(attrList.size()< m_attributeTitle.size() )
			{
				INF_LOG("    error column real size %d need size %d  ", attrList.size(), m_attributeTitle.size()-1);
				continue;
			}
			
			Ys_Record item; //行记录
			list<string>::iterator iPos = m_attributeTitle.begin();
			for( list<string>::iterator it=attrList.begin(); it!=attrList.end()&& iPos!=m_attributeTitle.end(); ++it,++iPos)
			{
				
				if(m_attributeKey == *iPos)
				{
					Ys_BadCharacter::GetInstance()->earseInvalidChar(*it);
				}
				item.AddAttribute(*iPos);
				vector<string> itemList;
				//
				list<YSS_Word> lstWord;
				m_wordSegmentLR.ExtractWordSegment(it->c_str(), lstWord);
				
				for(list<YSS_Word>::iterator pos = lstWord.begin(); pos!=lstWord.end(); ++pos)
				{
					if(Ys_BadCharacter::GetInstance()->findBadChar(pos->m_szWord))
					{
						//INF_LOG("delete bad character %s ", pos->m_szWord);
						continue;
					}
					itemList.push_back(pos->m_szWord);
				}
					
				item.AddWordToAttribute(*iPos, itemList);	
				//释放资源	
				m_wordSegmentLR.DestroyWordSegment(lstWord);
			}
			//////////////////////////////////
		#ifdef _DEBUG	 
			++totalRows;
			item.ViewRecord();
		#endif
		
			char cRetBuffer[MAX_BUFFER_LEN];
			memset(cRetBuffer, 0x00, MAX_BUFFER_LEN);
			
			char cTmpBuffer[BUFFER_LEN];
			//memset(cTmpBuffer, 0x00, BUFFER_LEN);
			cTmpBuffer[0] = 0x00;
			char* beginStr = cTmpBuffer;
			
			list<string> aimClassName;
			
			//获取记录主键 key Attribute
			string keyAttribute = *(m_attributeTitle.begin());
			list<string>::iterator iter=attrList.begin();
			for( list<string>::iterator ait=m_attributeTitle.begin(); ait!=m_attributeTitle.end() && iter!=attrList.end();++ait, ++iter)
			{
				if(*ait==m_attributeKey)
				{
					keyAttribute = *iter;
					INF_LOG("    key attribute is [%s] ", keyAttribute.c_str());
					break;
				}
			}
			
			INF_LOG("-------->begining classify---------------> ");
			//白名单分类
			Ys_ClassifierAdapter* itemAdapter = FindAdapter(T_WHITE);
			if(itemAdapter == NULL)
			{
				ERR_LOG("no white list adapter in Adapter list ");
			}
			else{
			if(itemAdapter->ProcessClassifier(keyAttribute, item, aimClassName))
			{
				INF_LOG("      %s in whiteList  ",keyAttribute.c_str());
				for(list<string>::iterator pos=aimClassName.begin(); pos!=aimClassName.end(); ++pos)
				{
					sprintf(beginStr,"%s,", pos->c_str());
					beginStr = beginStr + strlen(pos->c_str()+1);
				}
				int iLen = strlen(cTmpBuffer);
				cTmpBuffer[iLen>0? (iLen-1):0 ] = 0x00; //earse ","
				//sprintf(cRetBuffer,"%s$%s$",cLineBuf,cTmpBuffer);
			#ifdef _DEBUG	 
				if( it->find(cTmpBuffer) != string::npos)
					++accrRows;
				sprintf(cRetBuffer,"%s$%s$%s$whitelist",cLineBuf,cTmpBuffer, it->c_str());
			#else
				sprintf(cRetBuffer,"%s$%s$whitelist",cLineBuf,cTmpBuffer);
			#endif
				fResultGoodFile.WriteLine(cRetBuffer);
				continue;
			}
			}
			//后缀树
			itemAdapter = FindAdapter(T_TIRETREE);
			if(itemAdapter == NULL)
			{
				ERR_LOG("no white list adapter in Adapter list ");
			}
			else{
			if(itemAdapter->ProcessClassifier(keyAttribute, item, aimClassName))
			{
				INF_LOG("      %s in sufixtree  ",keyAttribute.c_str());
				for(list<string>::iterator pos=aimClassName.begin(); pos!=aimClassName.end(); ++pos)
				{
					sprintf(beginStr,"%s,", pos->c_str());
					beginStr = beginStr + strlen(pos->c_str()+1);
				}
				int iLen = strlen(cTmpBuffer);
				cTmpBuffer[iLen>0? (iLen-1):0 ] = 0x00; //earse ","
				//sprintf(cRetBuffer,"%s$%s$",cLineBuf,cTmpBuffer);
			#ifdef _DEBUG	 
				if( it->find(cTmpBuffer) != string::npos)
					++accrRows;
				sprintf(cRetBuffer,"%s$%s$%s$sufixtree",cLineBuf,cTmpBuffer, it->c_str());
			#else
				sprintf(cRetBuffer,"%s$%s$sufixtree",cLineBuf,cTmpBuffer);
			#endif
				fResultGoodFile.WriteLine(cRetBuffer);
				continue;
			}
			}
			
			//tagwhite
			itemAdapter = FindAdapter(T_TAG_WHITE);
			if(itemAdapter == NULL)
			{
				ERR_LOG("no white list adapter in Adapter list ");
			}
			else{
			if(itemAdapter->ProcessClassifier(keyAttribute, item, aimClassName))
			{
				INF_LOG("      %s in tagwhite  ",keyAttribute.c_str());
				for(list<string>::iterator pos=aimClassName.begin(); pos!=aimClassName.end(); ++pos)
				{
					sprintf(beginStr,"%s,", pos->c_str());
					beginStr = beginStr + strlen(pos->c_str()+1);
				}
				int iLen = strlen(cTmpBuffer);
				cTmpBuffer[iLen>0? (iLen-1):0 ] = 0x00; //earse ","
				//sprintf(cRetBuffer,"%s$%s$",cLineBuf,cTmpBuffer);
			#ifdef _DEBUG	 
				if( it->find(cTmpBuffer) != string::npos)
					++accrRows;
				sprintf(cRetBuffer,"%s$%s$%s$tagwhite",cLineBuf,cTmpBuffer, it->c_str());
			#else
				sprintf(cRetBuffer,"%s$%s$tagwhite",cLineBuf,cTmpBuffer);
			#endif
				fResultGoodFile.WriteLine(cRetBuffer);
				continue;
			}
			}
			//决策树分类
			/*itemAdapter = FindAdapter(T_JUECESHU);
			if(itemAdapter == NULL)
			{
				ERR_LOG("no decision tree adapter in Adapter list ");
			}
			else
			{
			if(itemAdapter->ProcessClassifier(keyAttribute, item, aimClassName))
			{
				INF_LOG("      %s indecison tree  ",keyAttribute.c_str());
				for(list<string>::iterator pos=aimClassName.begin(); pos!=aimClassName.end(); ++pos)
				{
					sprintf(beginStr,"%s,", pos->c_str());
					beginStr = beginStr + strlen(pos->c_str()+1);
				}
				int iLen = strlen(cTmpBuffer);
				cTmpBuffer[iLen>0? (iLen-1):0 ] = 0x00; //earse ","
				//sprintf(cRetBuffer,"%s$%s$",cLineBuf,cTmpBuffer);
			#ifdef _DEBUG	 
				if( it->find(cTmpBuffer) != string::npos)
					++accrRows;
				sprintf(cRetBuffer,"%s$%s$%s$",cLineBuf,cTmpBuffer, it->c_str());
			#else
				sprintf(cRetBuffer,"%s$%s$",cLineBuf,cTmpBuffer);
			#endif
				fResultGoodFile.WriteLine(cRetBuffer);
				continue;
			}
			}*/
			//贝叶斯分类
			itemAdapter = FindAdapter(T_BEIYES);
			if(itemAdapter == NULL)
			{
				ERR_LOG("no bayes classify  adapter in Adapter list ");
			}
			else{
			if(itemAdapter->ProcessClassifier(keyAttribute,item, aimClassName))
			{
				INF_LOG("  %s in bayes classfier success  ",keyAttribute.c_str());
				for(list<string>::iterator pos=aimClassName.begin(); pos!=aimClassName.end(); ++pos)
				{
					sprintf(beginStr,"%s,", pos->c_str());
					beginStr = beginStr + strlen(pos->c_str()) + 1;
				}
				int iLen = strlen(cTmpBuffer);
				cTmpBuffer[iLen>0? (iLen-1):0 ] = 0x00; //earse ","
			#ifdef _DEBUG	 
				if( it->find(cTmpBuffer) != string::npos)
				{
					++accrRows;
					sprintf(cRetBuffer,"%s$%s$%s$bayes",cLineBuf,cTmpBuffer, it->c_str());
				}
				else
				{
					sprintf(cRetBuffer,"%s$%s$%s$bayes",cLineBuf,cTmpBuffer, it->c_str());
					fResultGoodFile.WriteLine(cRetBuffer);
					continue;
				}
			#else
				sprintf(cRetBuffer,"%s$%s$",cLineBuf,cTmpBuffer);
			#endif
				fResultGoodFile.WriteLine(cRetBuffer);
				continue;
			}
			}

			//classify failure
			for(list<string>::iterator pos=aimClassName.begin(); pos!=aimClassName.end(); ++pos)
			{
				sprintf(beginStr,"%s,", pos->c_str());
				beginStr = beginStr + strlen(pos->c_str()) +1;
			}
			int iLen = strlen(cTmpBuffer);
			cTmpBuffer[iLen>0? (iLen-1):0 ] = 0x00; //earse ","
			sprintf(cRetBuffer,"%s$%s$",cLineBuf,cTmpBuffer);
		#ifdef _DEBUG
			++failRows;
		#endif
			fResultBadFile.WriteLine(cRetBuffer);
			continue;
			
		}//end for read line
		filePtr.CloseFile();
        //backup file
		string strBakFile = strBakFilePrefix +  "/" + *it;
		CCommon::MoveFile(strFile, strBakFile, false);
		INF_LOG("bak file %s-->%s ", strFile.c_str(), strBakFile.c_str());
	}//end process file
	
	#ifdef _DEBUG
		INF_LOG(" total :%ld ", totalRows);
		INF_LOG(" accur :%ld ",accrRows);
		INF_LOG(" fail  :%ld ",failRows);
		INF_LOG(" fail %lf  success:%lf ", 100*float(failRows)/totalRows, 100*float(accrRows)/totalRows);
		
	#endif
	return true;
}

bool Ys_ClassifierManager::AddAdapter(YS_CLASSIFIER_TYPE_T ty, Ys_ClassifierAdapter* item)
{
	if(item == NULL)
	{
		ERR_LOG("Ys_ClassifierAdapter ptr is null  ");
		return false;
	}
	return (m_adapterItem.insert(map<YS_CLASSIFIER_TYPE_T, Ys_ClassifierAdapter*>::value_type(ty, item))).second;
}

Ys_ClassifierAdapter* Ys_ClassifierManager::FindAdapter(YS_CLASSIFIER_TYPE_T type)
{
	if(m_adapterItem.size() == 0)
	{
		ERR_LOG("m_adapterlist  ptr is empty  ");
		return NULL;
	}
	
	map<YS_CLASSIFIER_TYPE_T, Ys_ClassifierAdapter*>::iterator it = m_adapterItem.find(type);
	if(it != m_adapterItem.end())
	{
		string name = (type==T_BEIYES ? "bayes" : "whitelist" );
		INF_LOG("%s is in adapter list  ", name.c_str());
		return it->second;
	}
	
	ERR_LOG("can not find  YS_CLASSIFIER_TYPE_T in m_adapterlist  ");
	return NULL;
}

const string& Ys_ClassifierManager::getAttributeName(int serno)const
{
	if(serno > m_attributeTitle.size())
	{
		ERR_LOG(" serno is too larger than titlelist in  getAttributeNamebyserno ");
		return m_strNullStr;
	}
		
	int iPos = 0;
	for(list<string>::const_iterator it=m_attributeTitle.begin(); it!=m_attributeTitle.end(); ++it)
	{
		if(iPos == serno)
			return *it;
		++iPos;
	}
	ERR_LOG("getAttributeName by serno error ");
	return m_strNullStr;
}

char* Ys_ClassifierManager::FormatOutPut(const list<string>& className, const string& fileName, const char* cLineBuf, char* cRetBuffer, int len)
{
	char cTmpBuffer[BUFFER_LEN];
	cTmpBuffer[0] = 0x00;
	char* beginStr = cTmpBuffer;
	for(list<string>::const_iterator pos=className.begin(); pos!=className.end(); ++pos)
	{
		sprintf(beginStr,"%s,", pos->c_str());
		beginStr = beginStr + strlen(pos->c_str()) + 1;
	}
	int iLen = strlen(cTmpBuffer);
	cTmpBuffer[iLen>0? (iLen-1):0 ] = 0x00; //earse ","
	
	if(strlen(cLineBuf)+strlen(cTmpBuffer) >= len)
		return NULL;
#ifdef _DEBUG	 
	sprintf(cRetBuffer,"%s$%s$%s$",cLineBuf,cTmpBuffer, fileName.c_str());
#else
	sprintf(cRetBuffer,"%s$%s$",cLineBuf,cTmpBuffer);
#endif
	
	return cRetBuffer;
}
