#include "YsWordSegmentLR.h"

int YS_WordSegmentLR::Init(const list<string>& lstDictFiles){
	clear();
	YS_GbkT2SConv::Init();
	m_CharMemPool = new YS_CharMemPool(1000000);
	vector<YSS_Word> vecWord;
	UINT32 uiLen;
	for(list<string>::const_iterator iterDictFile = lstDictFiles.begin(); iterDictFile != lstDictFiles.end(); iterDictFile++){
		string strData;
		if(false == CCommon::LoadFile(*iterDictFile, strData)){
			ERROR_LOG("can not load file: %s\n", (*iterDictFile).c_str());
			return -1;
		}
		list<string> listLine;
		list<string> lstValue;
		list<string>::iterator iterValue;
		CCommon::splitIgnoreSpace(strData, listLine, '\n');	
		YSS_Word word;
		for(list<string>::iterator iterLine=listLine.begin(); iterLine!=listLine.end(); iterLine++){
			CCommon::ReplaceAll(*iterLine, "\r", "");
			CCommon::trimIncludeTab(*iterLine);
			if((*iterLine).empty())
				continue;
			CCommon::splitIgnoreSpace(*iterLine, lstValue, '|');
			iterValue = lstValue.begin();
			char* szBuf = m_CharMemPool->malloc((*iterValue).length()+1);
			strcpy(szBuf, (*iterValue).c_str());
			CCommon::SBCToDBC(szBuf);
			uiLen = strlen(szBuf);
			YS_GbkT2SConv::GbkT2S(szBuf, uiLen);
			word.m_szWord = szBuf;
			iterValue++;
			if(lstValue.end() == iterValue ){
				word.m_szAttr = NULL;
			}else{
				char* szBuf = m_CharMemPool->malloc((*iterValue).length()+1);
				strcpy(szBuf, (*iterValue).c_str());
				word.m_szAttr = szBuf;
			}			
			vecWord.push_back(word);
		}
	}	
	m_pWordArray = new YSS_Word[vecWord.size()+1];
	for(UINT32 i=0; i < vecWord.size(); i++){
		vecWord[i].m_uiOffset = 0;
		vecWord[i].m_bAlloc = false;
		if(2 == strlen(vecWord[i].m_szWord) && YS_GbkUcsConv::IsGbkCode(vecWord[i].m_szWord))
			vecWord[i].m_bSingle = true;
		else if(1 == strlen(vecWord[i].m_szWord))
			vecWord[i].m_bSingle = true;
		else
			vecWord[i].m_bSingle = false;
		vecWord[i].m_uiWordLen = strlen(vecWord[i].m_szWord);
		m_pWordArray[i+1] = vecWord[i];
		AddStr(vecWord[i].m_szWord, i+1);
	}	
    m_uiWordNum = vecWord.size();
	return 0;
}

void YS_WordSegmentLR::Init(vector<YSS_Word>& vecWord){
	clear();
	YS_GbkT2SConv::Init();
	m_CharMemPool = new YS_CharMemPool(1000000);	
	m_pWordArray = new YSS_Word[vecWord.size()+1];
	for(UINT32 i=0; i < vecWord.size(); i++){
		m_pWordArray[i+1] = vecWord[i];
		char* szBuf = m_CharMemPool->malloc(strlen(m_pWordArray[i+1].m_szWord)+1);
		strcpy(szBuf, m_pWordArray[i+1].m_szWord);
		CCommon::SBCToDBC(szBuf);
		UINT32 uiLen = strlen(szBuf);
		YS_GbkT2SConv::GbkT2S(szBuf, uiLen);
		m_pWordArray[i+1].m_szWord = szBuf;
        if(NULL != m_pWordArray[i+1].m_szAttr){
            char* szAttr = m_CharMemPool->malloc(m_pWordArray[i+1].m_szAttr);
            m_pWordArray[i+1].m_szAttr = szAttr;
        }
		m_pWordArray[i+1].m_uiOffset = 0;
		m_pWordArray[i+1].m_bAlloc = false;
		if(2 == strlen(m_pWordArray[i+1].m_szWord) && YS_GbkUcsConv::IsGbkCode(m_pWordArray[i+1].m_szWord))
			m_pWordArray[i+1].m_bSingle = true;
		else if(1 == strlen(m_pWordArray[i+1].m_szWord))
			m_pWordArray[i+1].m_bSingle = true;
		else
			m_pWordArray[i+1].m_bSingle = false;
		m_pWordArray[i+1].m_uiWordLen = strlen(m_pWordArray[i+1].m_szWord);		
		AddStr(m_pWordArray[i+1].m_szWord, i+1);
	}
    m_uiWordNum = vecWord.size();
}

//create the filter machine.
void YS_WordSegmentLR::AddStr(char const* szUtf8, const UINT32 uiID)
{
    UINT32  uiLen = strlen(szUtf8);
	if(uiLen == 0){
		ERROR_LOG("can not add an empty str into match machine.\n");
		return;
	}
	
    UINT32 i;
    unsigned char uc, ucUpper, ucLower;
    YS_MatchCharItem**  pCur = m_filter.m_arrNextCharItem;
	YS_MatchCharItem*  pParent = NULL;    

    for (i=0; i<uiLen; i++){
        uc = szUtf8[i];        
		ucUpper = (uc&(0xF0))>>4;
        if (NULL == pCur[ucUpper]){
            pCur[ucUpper] = new YS_MatchCharItem;
			if (pParent){
                pParent->m_bHasChild = true; //it has child
			}
            pParent = pCur[ucUpper];
            pCur = pCur[ucUpper]->m_arrNextCharItem;
        }else{
			pParent = pCur[ucUpper];
			pCur = pCur[ucUpper]->m_arrNextCharItem;
		}

		ucLower = (uc&(0x0F));
		if (NULL == pCur[ucLower]){
			pCur[ucLower] = new YS_MatchCharItem;
			if (pParent){
				pParent->m_bHasChild = true; //it has child
			}
			pParent = pCur[ucLower];
			pCur = pCur[ucLower]->m_arrNextCharItem;
		}else{
			pParent = pCur[ucLower];
			pCur = pCur[ucLower]->m_arrNextCharItem;
		}
    }
	pParent->m_uiID = uiID;	
}

void YS_WordSegmentLR::ExtractWordSegment(const char* szKeyword, list<YSS_Word>& lstWord){
	lstWord.clear();
	if(NULL == szKeyword)
		return;
	char szKeywordBuf[MAX_FIELD_LEN];
	snprintf(szKeywordBuf, MAX_FIELD_LEN, "%s", szKeyword);
	CCommon::SBCToDBC(szKeywordBuf);
	UINT32  uiLen = strlen(szKeywordBuf);    
	YS_GbkT2SConv::GbkT2S(szKeywordBuf, uiLen);
    
    UINT32  uiCheckedPos=0;//finish position
    UINT32  uiCurPos=0; //checking postion	
	UINT32  uiNextPos;
    unsigned char  ucFirstWordLen = 0, ucFirstWordLenOrg=0;

    YS_MatchCharItem const*   pCur = NULL;
	YSS_Word word;
	bool bWord;
    while(true){//out loop
        if (uiCheckedPos >= uiLen) break;		     
		pCur = &m_filter;
		uiCurPos = uiCheckedPos;		 
		if(YS_GbkUcsConv::IsGbkCode(szKeywordBuf + uiCurPos))
			ucFirstWordLen = 2;
		else
			ucFirstWordLen = 1;  
		ucFirstWordLenOrg = ucFirstWordLen;

		UINT32 uiID = 0;        
       	while(true){			
			//check the first word
			pCur = IsExistWord(pCur, szKeywordBuf + uiCurPos, ucFirstWordLen);
			if(NULL == pCur){
				break;
			}
			if(0 != pCur->m_uiID){
				uiID = pCur->m_uiID;
				uiNextPos = uiCurPos + ucFirstWordLen;
				//for(list<UINT32>::iterator iter = lstIDs.begin(); iter != lstIDs.end(); iter++)
				//	printf("%u\n", *iter);
			}
			uiCurPos+=ucFirstWordLen;
			if (uiCurPos >= uiLen) 
			    break; //reach the end
			if(YS_GbkUcsConv::IsGbkCode(szKeywordBuf + uiCurPos))
				ucFirstWordLen = 2;
			else
				ucFirstWordLen = 1; 
		}
		bWord = true;
		if(0 != uiID){
			if('a' <= szKeywordBuf[uiNextPos] && 'z' >= szKeywordBuf[uiNextPos] && 'a' <= szKeywordBuf[uiNextPos-1] && 'z' >= szKeywordBuf[uiNextPos-1])
				bWord = false;
			if('0' <= szKeywordBuf[uiNextPos] && '9' >= szKeywordBuf[uiNextPos] && '0' <= szKeywordBuf[uiNextPos-1] && '9' >= szKeywordBuf[uiNextPos-1])
				bWord = false;
		}

		if(0 != uiID && bWord){
			word = m_pWordArray[uiID];
			word.m_uiOffset = uiCheckedPos;
			lstWord.push_back(word);
			uiCheckedPos = uiNextPos;			
		}else{
			if(2 == ucFirstWordLenOrg){
				word.m_uiOffset = uiCheckedPos;
				word.m_uiWordLen = 2;
				word.m_bAlloc = true;
				word.m_bSingle = true;
				word.m_szAttr = NULL;
				char* szBuf = new char[3];
				snprintf(szBuf, 3, "%s", szKeywordBuf+uiCheckedPos);				
				word.m_szWord = szBuf;
				lstWord.push_back(word);
				uiCheckedPos+=ucFirstWordLenOrg;
			}else{
				if(szKeywordBuf[uiCheckedPos] >= 'a' && szKeywordBuf[uiCheckedPos] <= 'z' ){
					uiNextPos = uiCheckedPos+1;
					while(szKeywordBuf[uiNextPos] >= 'a' && szKeywordBuf[uiNextPos] <= 'z' )
						uiNextPos ++;
					word.m_uiOffset = uiCheckedPos;
					word.m_uiWordLen = uiNextPos-uiCheckedPos;
					word.m_bAlloc = true;					
					if(1 == uiNextPos - uiCheckedPos)
						word.m_bSingle = true;
					else
						word.m_bSingle = false;
					word.m_szAttr = NULL;
					char* szBuf = new char[uiNextPos-uiCheckedPos+1];
					snprintf(szBuf, uiNextPos-uiCheckedPos+1, "%s", szKeywordBuf+uiCheckedPos);
					word.m_szWord = szBuf;
					lstWord.push_back(word);
					uiCheckedPos = uiNextPos;
				}else if(szKeywordBuf[uiCheckedPos] >= '0' && szKeywordBuf[uiCheckedPos] <= '9' ){
					uiNextPos = uiCheckedPos+1;
					while(szKeywordBuf[uiNextPos] >= '0' && szKeywordBuf[uiNextPos] <= '9' )
						uiNextPos ++;
					word.m_uiOffset = uiCheckedPos;
					word.m_uiWordLen = uiNextPos-uiCheckedPos;
					word.m_bAlloc = true;					
					if(1 == uiNextPos - uiCheckedPos)
						word.m_bSingle = true;
					else
						word.m_bSingle = false;
					word.m_szAttr = NULL;
					char* szBuf = new char[uiNextPos-uiCheckedPos+1];
					snprintf(szBuf, uiNextPos-uiCheckedPos+1, "%s", szKeywordBuf+uiCheckedPos);
					word.m_szWord = szBuf;
					lstWord.push_back(word);
					uiCheckedPos = uiNextPos;
				}else
					uiCheckedPos++;
			}
		}
	}
}

void YS_WordSegmentLR::DestroyWordSegment(list<YSS_Word>& lstWord){
	for(list<YSS_Word>::iterator iter = lstWord.begin(); iter!= lstWord.end(); iter++)
		if((*iter).m_bAlloc)
			delete[] (*iter).m_szWord;
	lstWord.clear();
}

//NULL:not exist, otherwize, it's the match filter
YS_MatchCharItem const* YS_WordSegmentLR::IsExistWord(
                          YS_MatchCharItem const *pFilter,
                          char const* szUtf8,
                          unsigned char ucLen) const
{
    unsigned char uc, ucUpper, ucLower;
    if (NULL == pFilter) return NULL;
    for (unsigned char i=0; i<ucLen; i++){
        uc = szUtf8[i];       
		ucUpper = (uc&(0xF0))>>4;
        if (NULL == pFilter->m_arrNextCharItem[ucUpper]) return NULL;
        pFilter = pFilter->m_arrNextCharItem[ucUpper];
		ucLower = (uc&(0x0F));
		if (NULL == pFilter->m_arrNextCharItem[ucLower]) return NULL;
		pFilter = pFilter->m_arrNextCharItem[ucLower];
    }
    return pFilter;
}

