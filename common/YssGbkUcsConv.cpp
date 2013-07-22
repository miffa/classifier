#include "YssGbkUcsConv.h"
#include "YssCommon.h"

unsigned short  YS_GbkUcsConv::m_arrGbkToUtf32[65535];
unsigned short  YS_GbkUcsConv::m_arrUtf32ToGbk[65535];


int YS_GbkUcsConv::Init(char const* szFileName){
    string line;
    list<string> value;
    list<string>::iterator iter;
    FILE* fd=fopen(szFileName, "rb");
    unsigned short g_code;
    unsigned short u_code;
    if (NULL == fd){
        line = "Can't open file:";
        line += szFileName;
//        Error_Log(line.c_str());
        return -1;
    }
    memset(m_arrGbkToUtf32, 0x00, sizeof(short) * 65535);
    memset(m_arrUtf32ToGbk, 0x00, sizeof(short) * 65535);
    while(CCommon::ReadLine(fd, line)){
        if (line.empty()) return 0;
//it's gbk
        CCommon::split(line, value, ' ');
        iter = value.begin();
        g_code = (unsigned short)strtol((*iter).c_str(), 0, 16);
        iter ++;
        u_code = (unsigned short)strtol((*iter).c_str()+2, 0, 16);

//it's gb2312
/*        CCommon::split(line, value, '\t');
        iter = value.begin();
        g_code = (unsigned short)strtol((*iter).c_str(), 0, 16) + 0x8080;
        iter ++;
        u_code = (unsigned short)strtol((*iter).c_str(), 0, 16);*/
        
        m_arrGbkToUtf32[g_code] = u_code;
        m_arrUtf32ToGbk[u_code] = g_code;
    }
	fclose(fd);
    return 0;
}

void YS_GbkUcsConv::GbkToUtf8(char const* src, size_t& src_len, char* desc, size_t& dest_len){
    unsigned short g_code;
    unsigned short u_code;
    char szUtf8[9];
    size_t utf8_len;
    size_t src_pos=0;
    size_t dest_pos=0;
    while(1){
        if (IsGbkCode(src + src_pos)){//must 2 byte
            if (src_pos + 2 > src_len){//exceed the src's range
                break;
            }
            g_code =(unsigned char)src[src_pos];
            g_code <<=8;
            g_code +=(unsigned char)src[src_pos+1];
            u_code = m_arrGbkToUtf32[g_code];
            if (0 == u_code) {
                break;
            }
            src_pos += 2;
            //convert utf16 to utf8
            Utf16ToUtf8(u_code, szUtf8, utf8_len);
            if (dest_pos + utf8_len > dest_len) break;//exceed the dest's range.
            memcpy(desc + dest_pos, szUtf8, utf8_len);
            dest_pos += utf8_len;
        }else{
            if (src[src_pos] < 0){//invalid char
                break;
            }
            if (src_pos + 1 > src_len){//exceed the src's rang
                break;
            }
            if (dest_pos + 1 > dest_len) {//exceed the dest's range
                break;
            }
            desc[dest_pos] = src[src_pos];
            src_pos++;
            dest_pos++;
        }
    }
    src_len = src_pos;
    if(dest_pos < dest_len)
		desc[dest_pos] = '\0';
    dest_len = dest_pos;
}
void YS_GbkUcsConv::Utf8ToGbk(char const* src, size_t& src_len, char* desc, size_t& dest_len){
    unsigned short g_code;
    unsigned short u_code;
//    char szUtf8[9];
    size_t utf8_len;
    size_t src_pos=0;
    size_t dest_pos=0;
    while(1){
        if (src_pos >= src_len) //end
            break;
        utf8_len = GetUtf8Len(src + src_pos);
        if (0 == utf8_len){//invalid utf8
            break;
        }
        if (src_pos + utf8_len > src_len){//exceed the max length
            break;
        }
        if (1 == utf8_len){
            if (dest_pos + 1 > dest_len){//exceed the max length
                break;
            }
            desc[dest_pos] = src[src_pos];
            dest_pos++;
            src_pos++;
            continue;//next

        }
        if (utf8_len > 3){//not gbk, break it
//            src_pos += utf8_len;
            break;;
        }
        //get utf16 code
        Utf8ToUtf16(src + src_pos, utf8_len, u_code);
        //get g_code
        g_code = m_arrUtf32ToGbk[u_code];
//        if (0 == g_code) {//not exist.
//            break;
//        }
        //exist.
        if (dest_pos + 2 > dest_len){//exceed the max length
            break;
        }
		if (0 != g_code){
	        desc[dest_pos] = (char)(g_code>>8);
    	    desc[dest_pos+1]=(char)(g_code);
			dest_pos +=2;
		}
		src_pos += utf8_len;
    }
    src_len = src_pos;
    if(dest_pos < dest_len)
		desc[dest_pos] = '\0';
    dest_len = dest_pos;
}
void YS_GbkUcsConv::GbkToUtf16(char const* src, size_t& src_len, char* desc, size_t& dest_len){
    unsigned short g_code;
    unsigned short u_code;
    size_t src_pos=0;
    size_t dest_pos=0;
    while(1){
        if (IsGbkCode(src + src_pos)){//must 2 byte
            if (src_pos + 2 > src_len){//exceed the src's range
                break;
            }
            g_code =(unsigned char)src[src_pos];
            g_code <<=8;   
            g_code +=(unsigned char)src[src_pos+1];   
            u_code = m_arrGbkToUtf32[g_code];
            if (0 == u_code) 
                break;
            if (dest_pos + 2 > dest_len) 
                break;//exceed the dest's range.
            desc[dest_pos] = (u_code & 0xFF);
            desc[dest_pos+1] = (u_code >> 8);
            src_pos += 2;
            dest_pos += 2;
        }else{
//            if (src[src_pos] < 0){//invalid char
//                break;
//            }
            if (src_pos + 1 > src_len){//exceed the src's rang
                break;
            }
            if (dest_pos + 2 > dest_len) {//exceed the dest's range
                break;
            }
            desc[dest_pos] = src[src_pos];
            desc[dest_pos+1] = 0;
            src_pos++;
            dest_pos += 2;
        }
    }
    src_len = src_pos;
    if(dest_pos < dest_len)
        desc[dest_pos] = '\0';
    dest_len = dest_pos;
}

void YS_GbkUcsConv::Utf16ToGbk(char const* src, size_t& src_len, char* desc, size_t& dest_len){
    unsigned short g_code;
    unsigned short u_code;
    size_t src_pos=0;
    size_t dest_pos=0;
    while(1){       
        if (src_pos + 2 > src_len){//exceed the max length
            break;
        }
        u_code =(unsigned char)src[src_pos+1];
        u_code <<=8;   
        u_code +=(unsigned char)src[src_pos];   
        g_code = m_arrUtf32ToGbk[u_code];
        if (0 == g_code) {//not exist.
            if (u_code < 0xff){//single char
                if (dest_pos + 1 > dest_len){//exceed length
                    break;
                }
                desc[dest_pos] = u_code;
                dest_pos++;
                src_pos += 2;
                continue;
            }else{
                break;//error
            }
        }
        //exist.
        if (dest_pos + 2 > dest_len){//exceed the max length
            break;
        }
        desc[dest_pos] = (g_code >> 8);
        desc[dest_pos+1] = (g_code & 0xFF);
        src_pos += 2;
        dest_pos +=2;
    }
    src_len = src_pos;
    if(dest_pos < dest_len)
        desc[dest_pos] = '\0';
    dest_len = dest_pos;
}

void YS_GbkUcsConv::Utf16ToUtf8(unsigned short c16, char* c8, size_t& c8_len){
/*
UCS-2编码(16进制) UTF-8 字节流(二进制) 
0000 - 007F       0xxxxxxx 
0080 - 07FF       110xxxxx 10xxxxxx 
0800 - FFFF       1110xxxx 10xxxxxx 10xxxxxx 
*/
    if (c16 < 0x80){//single
        c8_len = 1;
        c8[0] =(char)c16;
    }else if (c16 < 0x800){
        c8[0] =(char) 0xC0 | ((c16>>6)&0x1F);
        c8[1] =(char) 0x80 | (c16 & 0x3F);
        c8_len = 2;
    }else{
        c8[0] =(char) 0xE0 | ((c16>>12)&0xF);
        c8[1] =(char) 0x80 | ((c16>>6)& 0x3F);
        c8[2] =(char) 0x80 | (c16 & 0x3F);
        c8_len = 3;
    }
}
void YS_GbkUcsConv::Utf8ToUtf16(char const* c8, size_t c8_len, unsigned short& c16){    
    if (1 == c8_len){//single
        c16 = c8[0];
    }else if (2 == c8_len){
        c16 = (c8[0]&0x1F)<<6;
        c16 += c8[1]&0x3F;
    }else{
        c16 = (c8[0]&0x0F)<<12;
        c16 += (c8[1]&0x3F)<<6;
        c16 += (c8[2]&0x3F);
    }

}
size_t YS_GbkUcsConv::GetUtf8PosByByteLen(char const* src, size_t src_len, size_t get_len){
     size_t src_pos =0;
     size_t len;
     while(1){
         len =GetUtf8Len(src + src_pos);
         if (src_pos + len > src_len) break;
         if (src_pos + len > get_len) break;
         if (0 == len) break;
         src_pos +=len;
     }
     return src_pos;
}
size_t YS_GbkUcsConv::GetUtf8PosByCharLen(char const* src, size_t src_len, size_t get_len){
     size_t src_pos =0;
     size_t char_num =0;
     size_t len;
     while(1){
         len =GetUtf8Len(src + src_pos);
         if (src_pos + len > src_len) break;
         if (char_num + 1 > get_len) break;
         if (0 == len) break;
         char_num ++;
         src_pos +=len;
     }
     return src_pos;
}
size_t YS_GbkUcsConv::GetGbkPosByByteLen(char const* src, size_t src_len, size_t get_len){
     size_t src_pos =0;
     size_t len;
     while(1){
         len =IsGbkCode(src + src_pos)?2:1;
         if (src_pos + len > src_len) break;
         if (src_pos + len > get_len) break;
         src_pos +=len;
     }
     return src_pos;
}
size_t YS_GbkUcsConv::GetGbkPosByCharLen(char const* src, size_t src_len, size_t get_len){
     size_t src_pos =0;
     size_t char_num =0;
     size_t len;
     while(1){
         len =IsGbkCode(src + src_pos)?2:1;
         if (src_pos + len > src_len) break;
         if (char_num + 1 > get_len) break;
         char_num ++;
         src_pos +=len;
     }
     return src_pos;
}

size_t YS_GbkUcsConv::GetGbkPosByCharLen(char const* src, size_t get_len){
     size_t src_pos = 0;
     size_t char_num = 0;
     size_t len;
     while(src[src_pos]){
         len =IsGbkCode(src + src_pos)?2:1;
         if (char_num + 1 > get_len) break;
         char_num ++;
         src_pos +=len;
     }
     return src_pos;
}

size_t YS_GbkUcsConv::GetGbkStrlen(char const* src, size_t src_len){
    size_t src_pos =0;
    size_t count =0;
    size_t len;
    while (src_pos < src_len){
         len =IsGbkCode(src + src_pos)?2:1;
         count ++;
         src_pos +=len;
    }
    return count;
}

bool YS_GbkUcsConv::IsGbkCode(char const* src){
    if ( ((unsigned char)src[0] >=0x81) &&
         ((unsigned char)src[0] <=0xFE) &&
         ((unsigned char)src[1] >=0x40) &&
         ((unsigned char)src[1] <=0xFE) ) return true;
    return false;
}

bool YS_GbkUcsConv::IsReverseGbkCode(char const* src){
    if ( ((unsigned char)src[1] >=0x81) &&
         ((unsigned char)src[1] <=0xFE) &&
         ((unsigned char)src[0] >=0x40) &&
         ((unsigned char)src[0] <=0xFE) ) return true;
    return false;

}

size_t YS_GbkUcsConv::GetUtf8Len(char const* src){
    size_t i;
    unsigned char c = src[0];
    if ( (c & 0x80) == 0) return 1;
    for (i=1; i<8; i++){
        c <<= 1;
        if ( (c & 0x80) == 0) break;
        if ((src[i] &0xC0) != 0x80) return 0; //error, skip it.
    }
    return i;
}


void YS_GbkUcsConv::GbkToUtf16BE(char const* src, size_t& src_len, char* desc, size_t& dest_len){
    unsigned short g_code;
    unsigned short u_code;
    size_t src_pos=0;
    size_t dest_pos=0;
    while(1){
        if (IsGbkCode(src + src_pos)){//must 2 byte
            if (src_pos + 2 > src_len){//exceed the src's range
                break;
            }
            g_code =(unsigned char)src[src_pos];
            g_code <<=8;   
            g_code +=(unsigned char)src[src_pos+1];   
            u_code = m_arrGbkToUtf32[g_code];
            if (0 == u_code) 
                break;
            if (dest_pos + 2 > dest_len) 
                break;//exceed the dest's range.
            desc[dest_pos] = (u_code >> 8);
            desc[dest_pos+1] = (u_code & 0xFF);
            src_pos += 2;
            dest_pos += 2;
        }else{
//            if (src[src_pos] < 0){//invalid char
//                break;
//            }
            if (src_pos + 1 > src_len){//exceed the src's rang
                break;
            }
            if (dest_pos + 2 > dest_len) {//exceed the dest's range
                break;
            }
            desc[dest_pos] = 0;
            desc[dest_pos+1] = src[src_pos];
            src_pos++;
            dest_pos += 2;
        }
    }
    src_len = src_pos;
    if(dest_pos < dest_len)
        desc[dest_pos] = '\0';
    dest_len = dest_pos;
}

void YS_GbkUcsConv::Utf16BEToGbk(char const* src, size_t& src_len, char* desc, size_t& dest_len){
    unsigned short g_code;
    unsigned short u_code;
    size_t src_pos=0;
    size_t dest_pos=0;
    while(1){       
        if (src_pos + 2 > src_len){//exceed the max length
            break;
        }
        u_code =(unsigned char)src[src_pos];
        u_code <<=8;   
        u_code +=(unsigned char)src[src_pos+1];   
        g_code = m_arrUtf32ToGbk[u_code];
        if (0 == g_code) {//not exist.
            if (u_code < 0xff){//single char
                if (dest_pos + 1 > dest_len){//exceed length
                    break;
                }
                desc[dest_pos] = u_code;
                dest_pos++;
                src_pos += 2;
                continue;
            }else{
                break;//error
            }
        }
        //exist.
        if (dest_pos + 2 > dest_len){//exceed the max length
            break;
        }
        desc[dest_pos] = (g_code >> 8);
        desc[dest_pos+1] = (g_code & 0xFF);
        src_pos += 2;
        dest_pos +=2;
    }
    src_len = src_pos;
    if(dest_pos < dest_len)
        desc[dest_pos] = '\0';
    dest_len = dest_pos;
}

void YS_GbkUcsConv::Utf8ToGbk(string& strOrg, char* szBuf, size_t buf_len){
	size_t src_len = strOrg.length();
	Utf8ToGbk(strOrg.c_str(), src_len, szBuf, buf_len);
	strOrg = szBuf;
}

