#ifndef __YS_WORD_SEGMENT_LR_H__
#define __YS_WORD_SEGMENT_LR_H__
#include "YssGbkUcsConv.h"
#include <ext/hash_set>
#include <list>
#include "YssCommonStructDef.h"
#include "YssCommon.h"
#include "YsGbkT2SConv.h"
#include "YsCharMemPool.h"

#define MAX_FIELD_LEN 8096

class YS_WordSegmentLR{
public:
    YS_WordSegmentLR(){	    
	    m_pWordArray = NULL;
		m_CharMemPool = NULL;
        m_uiWordNum = 0;
    }
    ~YS_WordSegmentLR(){        
	    clear();
    }
public:
    int Init(const list<string>& lstDictFiles);
	void Init(vector<YSS_Word>& vecWord);
    void ExtractWordSegment(const char* szKeyword, list<YSS_Word>& lstWord);
    void DestroyWordSegment(list<YSS_Word>& lstWord);	
    inline UINT32 GetWordNum()const{
        return m_uiWordNum;
    }
    inline const YSS_Word* GetWord(UINT32 i) const{
        return m_pWordArray + i + 1;
    }
private:
	//create the filter machine.
	void AddStr(char const* szUtf8, const UINT32 uiID);
    YS_MatchCharItem const* IsExistWord(YS_MatchCharItem const *pFilter, char const* szUtf8, unsigned char ucLen) const;
    void clear(){
	    if(m_pWordArray)
		    delete[] m_pWordArray;
	    m_pWordArray = NULL;
		if(m_CharMemPool)
			delete m_CharMemPool;
		m_CharMemPool = NULL;
    }
private:
    YS_MatchCharItem  m_filter;   
    YSS_Word* m_pWordArray;
	YS_CharMemPool* m_CharMemPool;
    UINT32 m_uiWordNum;
};

#endif

