
#include "base64.h"
#include "string.h"

char GetB64Char(int index)
{
	const char szBase64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	if (index >= 0 && index < 64)
		return szBase64Table[index];
	
	return '=';
}

unsigned long GetBase64EncodeSize(unsigned long src_len)
{
	return ((src_len / 3) + 1 ) * 4;
}

unsigned long GetBase64DecodeSize(unsigned long src_len)
{
	return (src_len / 4) * 3;
}

unsigned long Base64Encode(char * base64code, const char * src, unsigned long src_len)
{   
	if (src_len == 0)
		src_len = strlen(src);
	
	unsigned long len = 0;
	unsigned char* psrc = (unsigned char*)src;
	char * p64 = base64code;
	int i = 0;
	for (i = 0; i < src_len - 3; i += 3)
	{
		unsigned long ulTmp = *(unsigned long*)psrc;
		int b0 = GetB64Char((B0(ulTmp) >> 2) & 0x3F);
		int b1 = GetB64Char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
		int b2 = GetB64Char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F);
		int b3 = GetB64Char((B2(ulTmp) << 2 >> 2) & 0x3F); 
		*((unsigned long*)p64) = b0 | b1 << 8 | b2 << 16 | b3 << 24;
		len += 4;
		p64  += 4; 
		psrc += 3;
	}
	
	if (i < src_len)
	{
		unsigned long rest = src_len - i;
		unsigned long ulTmp = 0;
		for (int j = 0; j < rest; ++j)
		{
			*(((unsigned char*)&ulTmp) + j) = *psrc++;
		}
		
		p64[0] = GetB64Char((B0(ulTmp) >> 2) & 0x3F); 
		p64[1] = GetB64Char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F); 
		p64[2] = rest > 1 ? GetB64Char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F) : '='; 
		p64[3] = rest > 2 ? GetB64Char((B2(ulTmp) << 2 >> 2) & 0x3F) : '='; 
		p64 += 4; 
		len += 4;
	}
	
	*p64 = '\0'; 
	
	return len;
}

//Base64Encode have a bug in ppc, encode and decode will not be the original.
//jacobd
unsigned long Base64Encode1(char * base64code, const char * src, unsigned long src_len)
{   
	if (src_len == 0)
		src_len = strlen(src);
 
	int i = 0;
	
	char* lpCurData = (char*)src;
    char* lpCurBuf = base64code;

	for(i = 0; i < src_len/3; i++)
    {
        *lpCurBuf  = GetB64Char((lpCurData[0] & 0xFC) >> 2);
        *(lpCurBuf+1) = GetB64Char(((lpCurData[0] & 0x03) << 4) | ((lpCurData[1] & 0xF0) >> 4));
        *(lpCurBuf+2) = GetB64Char(((lpCurData[1] & 0x0F) << 2) | ((lpCurData[2] & 0xC0) >> 6));
		*(lpCurBuf+3) = GetB64Char(lpCurData[2] & 0x3F);
		
        lpCurBuf  += 4;
        lpCurData += 3;
    }
	
    switch(src_len % 3)
    {
		case 0 :
			break;
			
		case 1 :
			lpCurBuf[0] = GetB64Char((lpCurData[0] & 0xFC) >> 2);
			lpCurBuf[1] = GetB64Char((lpCurData[0] & 0x03) << 4);
			lpCurBuf[2] = (char)'=';
			lpCurBuf[3] = (char)'=';
			
			lpCurBuf += 4;
			break;
			
		case 2 :
			lpCurBuf[0] = GetB64Char((lpCurData[0] & 0xFC) >> 2);
			lpCurBuf[1] = GetB64Char(((lpCurData[0] & 0x03) << 4) | ((lpCurData[1] & 0xF0) >> 4));
			lpCurBuf[2] = GetB64Char((lpCurData[1] & 0x0F) << 2);
			lpCurBuf[3] = (char)'=';
			
			lpCurBuf += 4;
			break;
    }
	
	*lpCurBuf = '\0'; 
	
	return lpCurBuf - base64code;
}



int GetB64Index(char ch)
{
	int index = -1;
	if (ch >= 'A' && ch <= 'Z')
	{
		index = ch - 'A';
	}
	else if (ch >= 'a' && ch <= 'z')
	{
		index = ch - 'a' + 26;
	}
	else if (ch >= '0' && ch <= '9')
	{
		index = ch - '0' + 52;
	}
	else if (ch == '+')
	{
		index = 62;
	}
	else if (ch == '/')
	{
		index = 63;
	}

	return index;
}

unsigned long Base64Decode(char * buf, const char * base64code, unsigned long src_len)
{   
	if (!base64code)
		return 0;
	if (src_len == 0)
		src_len = strlen(base64code);

	int len = 0;
	unsigned char* psrc = (unsigned char*)base64code;
	char * pbuf = buf;
	int i = 0;
	for (i = 0; i < src_len - 4; i += 4)
	{
		//unsigned long ulTmp = *(unsigned long*)psrc;
		char ch[4] = {0};
		for (int j = 0; j < 4; ++j)
		{
			ch[j] = *(psrc + j);
		}
		
		int b0 = (GetB64Index(ch[0]) << 2 | GetB64Index(ch[1]) << 2 >> 6) & 0xFF;
		int b1 = (GetB64Index(ch[1]) << 4 | GetB64Index(ch[2]) << 2 >> 4) & 0xFF;
		int b2 = (GetB64Index(ch[2]) << 6 | GetB64Index(ch[3]) << 2 >> 2) & 0xFF;
		
		/*
		register int b0 = (GetB64Index((char)B0(ulTmp)) << 2 | GetB64Index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
		register int b1 = (GetB64Index((char)B1(ulTmp)) << 4 | GetB64Index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
		register int b2 = (GetB64Index((char)B2(ulTmp)) << 6 | GetB64Index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;
		*/
		
		*pbuf = (char)b0;
		*(pbuf + 1) = (char)b1;
		*(pbuf + 2) = (char)b2;
		//*((unsigned long*)pbuf) = bc0 | (bc1 << 8) | (bc2 << 16);
		psrc  += 4; 
		pbuf += 3;
		len += 3;
	}

	if (i < src_len)
	{
		unsigned long rest = src_len - i;
		char ch[4] = {0};
		for (int j = 0; j < rest; ++j)
		{
			ch[j] = *psrc++;
		}
		/*
		unsigned long ulTmp = 0;
		for (int j = 0; j < rest; ++j)
		{
			*(((unsigned char*)&ulTmp) + j) = *psrc++;
		}
		*/
		
		int b0 = (GetB64Index(ch[0]) << 2 | GetB64Index(ch[1]) << 2 >> 6) & 0xFF;
		*pbuf++ = b0;
		len  ++;

		if ('=' != ch[1] && '=' != ch[2])
		{
			int b1 = (GetB64Index(ch[1]) << 4 | GetB64Index(ch[2]) << 2 >> 4) & 0xFF;
			*pbuf++ = b1;
			len  ++;
		}
		
		if ('=' != ch[2] && '=' != ch[3])
		{
			int b2 = (GetB64Index(ch[2]) << 6 | GetB64Index(ch[3]) << 2 >> 2) & 0xFF;
			*pbuf++ = b2;
			len  ++;
		}

	}
		
	*pbuf = '\0'; 
	
	return len;
} 
