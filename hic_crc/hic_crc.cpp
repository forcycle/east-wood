// hic_crc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "Windows.h"


typedef signed char  int8_t;
typedef signed short int16_t;
typedef signed int   int32_t;
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

typedef enum{
	ERROR_TYPE = 0x0,
	ERROR_MAX,
	ERROR_MIN,
	ERROR_UNKNOW,
}_ERROR_TYPING_;

//#define CRC_A 0
#define CRC_B 1


static void _hci_byte_Crc(
    uint8_t     crcByte, 
    uint16_t    *pCrc
)
{
    crcByte = (crcByte ^ (uint8_t)((*pCrc) & 0x00FF));
    crcByte = (crcByte ^ (uint8_t)(crcByte << 4));
    *pCrc = (*pCrc >> 8) ^ ((uint16_t)crcByte << 8) ^
                ((uint16_t)crcByte << 3) ^
                ((uint16_t)crcByte >> 4);
}

void _hci_Crc(
    uint8_t     *pData, 
    uint8_t     length,
    uint8_t     *pCrc1, 
    uint8_t     *pCrc2
)
{
    uint8_t     crc_byte = 0, 
                index = 0;
    uint16_t    crc = 0;

#ifdef CRC_A
        crc = 0x6363; /* ITU-V.41 */
#else
        crc = 0xFFFF; /* ISO/IEC 13239 (formerly ISO/IEC 3309) */
#endif /* #ifdef CRC_A */

    do 
    {
        crc_byte = pData[index];
        _hci_byte_Crc(crc_byte, &crc);
        index++;
    } while (index < length);

#ifndef INVERT_CRC
    crc = ~crc; /* ISO/IEC 13239 (formerly ISO/IEC 3309) */
#endif /* #ifndef INVERT_CRC */

    *pCrc1 = (uint8_t) (crc & 0xFF);
    *pCrc2 = (uint8_t) ((crc >> 8) & 0xFF);
    return;
}

char ConvertHexChar(char ch)     
{   
  if((ch>='0')&&(ch<='9'))   
  return   ch-0x30;   
  else   if((ch>='A')&&(ch<='F'))   
  return   ch-'A'+10;   
  else   if((ch>='a')&&(ch<='f'))   
  return   ch-'a'+10;   
  else   return   (-1);   
}

void SetColor(unsigned short color)
{
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hcon,color);
}
#define RELEASE 1
static void usage(void)
{
	printf( "Usage:\n"
	"\tDescription:[Length] + [Header] + [Payload] + [CRC16]\n"
	"\tFor example input [05 f9 04 00],return [0xc3,e5] is ok\n"
#if defined RELEASE
	"\tIf you meet some problem, please contact nfcsphinx@gamil.com\n\n");
#else
	"\tIf you meet some problem, please contact xiaohua.wang@nxp.com\n"
	"\tCopyright (c) 2011, NXP Semiconductors N.V.\n\n");
#endif
}

static void error(_ERROR_TYPING_ arg)
{
	if(arg == ERROR_TYPE)
	printf( "\tError:Argument incorrect!!!\n");
	else if(arg == ERROR_MAX || arg == ERROR_MIN)
	printf( "\tError:Over 32 argument or less than 3 argument!!!\n");
	else
	printf( "\tError:Unknow!!!\n");

	printf( "\tPlease input again!!!\n");
}
//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char* argv[])
{
	uint8_t crc1 = 0x0;
	uint8_t crc2 = 0x0;
	uint8_t str[200] = {0x0};
	uint8_t i = 0x0;

	usage();
	if(argc > 32 || argc < 3)
	{error(ERROR_MAX);return 0;}

	for(i=0;i<argc - 1;i++)
	{
		if((((char) *(argv[i+1]+2)) != 0x0) || ConvertHexChar((char) *argv[i+1]) == -1 || ConvertHexChar((char) *(argv[i+1]+1)) == -1)
		{error(ERROR_TYPE);return 0;}

		str[i] = ((ConvertHexChar((char) *argv[i+1])&0x0f) << 0x4) | (ConvertHexChar((char) *(argv[i+1]+1))&0x0f);
	}

	_hci_Crc(str,argc - 1,&crc1,&crc2);
	SetColor(12);
	printf("\tCRC:[0x%x,0x%x]\n",crc1,crc2);
	SetColor(7);

	return 0;
}

