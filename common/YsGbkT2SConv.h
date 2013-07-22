#ifndef __YS_GBYS2S_CONV_H__
#define __YS_GBYS2S_CONV_H__

class YS_GbkT2SConv{
public:
	enum{
		GBYS_COUNT = 3145
		};
public:
	static void Init();
	static void GbkT2S(char* szGbk, unsigned int uiLen);
private:
	static unsigned short 		m_unGbkT2SMap[GBYS_COUNT][2];
	static char			 		m_szGbkArr[0x10000][2];
};

#endif

