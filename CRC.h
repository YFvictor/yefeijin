#ifdef CRC_API
#else
#define CRC_API _declspec(dllexport)
#endif
#include "stdafx.h"
CRC_API unsigned long  Crc32Calculate(unsigned char *buffer, unsigned int size);
CRC_API unsigned short CRC16(unsigned char * puchmsg,unsigned short usdatalen);
