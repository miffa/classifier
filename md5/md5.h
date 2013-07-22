/* MD5.H - header file for MD5C.C
 */
/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */
namespace	MD5
{
/* POINTER defines a generic pointer type */
typedef unsigned char	*POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short	UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int	UINT4;

/* MD5 context. */
typedef struct
{
	UINT4	state[4];			/* state (ABCD) */
	UINT4	count[2];			/* number of bits, modulo 2^64 (lsb first) */
	unsigned char	buffer[64]; /* input buffer */
} MD5_CTX;

void	MD5Init(MD5_CTX *);
void	MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void	MD5Final(unsigned char[16], MD5_CTX *);
};
using namespace MD5;

#include <string>
class YS_Encrypt
{
public:
	~YS_Encrypt(){}

	/// MD5加密算法,将输入字符串加密成32位密文
	static bool encryptMD5(const std::string &src, std::string &dest);
	static void StrToUpper(std::string& buffer);
	/// 获取唯一实例
	static YS_Encrypt *getInstance();

protected:
	YS_Encrypt() {}

private:
	static YS_Encrypt *instance;

};