#ifndef __YS_CHAR_MEM_POOL_H__
#define __YS_CHAR_MEM_POOL_H__
#include "YssTypeDef.h"
#define CHARGROUP (1024*1024)
class YS_CharMemPool{
public:
	YS_CharMemPool(UINT32 uiPoolSize){
		if(CHARGROUP > uiPoolSize)
			m_uiPoolSize = CHARGROUP;
		else
			m_uiPoolSize = (uiPoolSize+1023)/1024*1024;    //M
		m_uiLeft = 0;
		m_szFree = NULL;
	}
	~YS_CharMemPool(){
		free();
	}
	char* malloc(UINT32 uiNum)
	{
		if (m_uiLeft < uiNum) {
			if(uiNum > m_uiPoolSize){
				m_szFree = new char[(uiNum+m_uiPoolSize-1)/m_uiPoolSize*m_uiPoolSize];
				m_lstChar.push_back(m_szFree);
				m_uiLeft = (uiNum+m_uiPoolSize-1)/m_uiPoolSize*m_uiPoolSize;
			}else{
				m_szFree = new char[m_uiPoolSize];
				m_lstChar.push_back(m_szFree);
				m_uiLeft = m_uiPoolSize;
			}
		}
		m_uiLeft -= uiNum;
		m_szFree += uiNum;
		return m_szFree - uiNum;
	}
	char* malloc(const char* szBuf)
	{
		if(NULL == szBuf)
			return NULL;
		char* szTemp = malloc(strlen(szBuf)+1); 
		strcpy(szTemp, szBuf);
		return szTemp;
	}
	void free(){
		for(list<char*>::iterator iter=m_lstChar.begin(); iter!=m_lstChar.end(); iter++)
			delete[] (*iter);
		m_lstChar.clear();
		m_uiLeft = 0;
		m_szFree = NULL;
	}

private:
	UINT32 m_uiLeft;
	char *m_szFree;
	list<char*> m_lstChar;
	UINT32 m_uiPoolSize;
};
#endif
