#ifndef __YS_GBK_UCS_CONV_H__
#define __YS_GBK_UCS_CONV_H__

#include "YssStd.h"

class YS_GbkUcsConv{
public:
    static int Init(char const* szFileName);
    static void GbkToUtf8(char const* src, size_t& src_len, char* desc, size_t& dest_len);
    static void Utf8ToGbk(char const* src, size_t& src_len, char* desc, size_t& dest_len);
    static void GbkToUtf16(char const* src, size_t& src_len, char* desc, size_t& dest_len);
    static void GbkToUtf16BE(char const* src, size_t& src_len, char* desc, size_t& dest_len);
    static void Utf16ToGbk(char const* src, size_t& src_len, char* desc, size_t& dest_len);
    static void Utf16BEToGbk(char const* src, size_t& src_len, char* desc, size_t& dest_len);
    static void Utf16ToUtf8(unsigned short c16, char* c8, size_t& c8_len);
    static void Utf8ToUtf16(char const* c8, size_t c8_len, unsigned short& c16);
    static size_t GetUtf8PosByByteLen(char const* src, size_t src_len, size_t get_len);
    static size_t GetUtf8PosByCharLen(char const* src, size_t src_len, size_t get_len);
    static size_t GetGbkPosByByteLen(char const* src, size_t src_len, size_t get_len);
    static size_t GetGbkPosByCharLen(char const* src, size_t src_len, size_t get_len);
    static size_t GetGbkPosByCharLen(char const* src, size_t get_len);
    static size_t GetGbkStrlen(char const* src, size_t src_len);
    static bool IsGbkCode(char const* src);
	static bool IsReverseGbkCode(char const* src);
    static size_t GetUtf8Len(char const* src);
	static void Utf8ToGbk(string& strOrg, char* szBuf, size_t buf_len);

private:
    static unsigned short  m_arrGbkToUtf32[65535];
    static unsigned short  m_arrUtf32ToGbk[65535];
};

#endif
