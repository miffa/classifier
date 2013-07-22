#ifndef __YSS_COMMON_STRUCT_DEF_H__
#define __YSS_COMMON_STRUCT_DEF_H__
#include "stdio.h"
#include "YssStd.h"
#include "YssTypeDef.h"
#include "YssCommon.h"
#include <ext/hash_set>
#ifdef ERROR_LOG
#undef ERROR_LOG
#endif
#define ERROR_LOG(fmt, arg...) do{ fprintf(stderr,"%s:%d "fmt, __FILE__, __LINE__, ##arg); fflush(stderr); }while(0)

class YSS_Word{
public:
	char const* m_szWord;
	char const* m_szAttr;
	UINT32 m_uiOffset;
	UINT32 m_uiWordLen;
	bool m_bAlloc;
	bool m_bSingle;
};

class YSS_CityWord: public YSS_Word{
public:	
	UINT16 m_unCityCode;
};

class YS_MatchCharItem{
public:
    YS_MatchCharItem(){
        memset(m_arrNextCharItem, 0x00, sizeof(YS_MatchCharItem*) * 16);
        m_uiID = 0;
        m_bHasChild = false;
    }
    ~YS_MatchCharItem(){
        for (int i=0; i<16; i++){
            if (m_arrNextCharItem[i])  delete m_arrNextCharItem[i];
        }
    }
public:
    YS_MatchCharItem*    m_arrNextCharItem[16];
    UINT32               m_uiID;
    bool                 m_bHasChild;
};

class YS_SavedMatchCharItem{
public:
        YS_SavedMatchCharItem(){
                m_lstIDs.clear();
                m_bHasChild = false;
                m_arrNextCharItem = NULL;
                m_unMask = 0;
        }
        ~YS_SavedMatchCharItem(){
                m_lstIDs.clear();
        }
public:
        YS_SavedMatchCharItem**  m_arrNextCharItem;
        list<UINT32>         m_lstIDs;
        bool                 m_bHasChild;
        UINT16               m_unMask;
};

typedef struct YS_ADDR_COORDINATE {
    bool operator < (const YS_ADDR_COORDINATE& item) const{
        return (m_uiRefNum > item.m_uiRefNum);
    }
		double m_lfLon;
		double m_lfLat;
		UINT32 m_uiRefNum;
        const char* m_szCounty;
} YS_ADDR_COORDINATE;

#endif
