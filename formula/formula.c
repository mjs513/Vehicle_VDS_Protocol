/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义常用公式计算函数和版本信息显示函数
History:
	<author>	<time>		<desc>
************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include "formula.h"
#include "../interface/protocol_define.h"

/*************************************************
Description:	一个字节的加减乘除运算
Input:
	pcDsSource	取值起始地址
	addnum		加数
	subnum		减数
	mulnum		乘数
	divnum		除数
	pcFormat	打印格式

Output:	pcDataOut	计算结果输出地址
Return:	void
Others:
*************************************************/

void OneByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum,
                       const byte* pcFormat, byte *pcDataOut )
{
	int onebyte = *pcDsSource;
	double disnum = 0;

	if( divnum == 1 && mulnum == 1 )
		disnum = ( double )onebyte + addnum - subnum;
	else if( divnum != 1 && mulnum == 1 )
		disnum = ( double )onebyte / divnum + addnum - subnum;
	else if( mulnum != 1 && divnum == 1 )
		disnum = ( double )onebyte * mulnum + addnum - subnum;
	else
		disnum = ( double )onebyte * mulnum / divnum + addnum - subnum;

	/*sprintf_s( pcDataOut, 20, pcFormat, disnum );*/
	sprintf( pcDataOut, pcFormat, disnum );
}
/*************************************************
Description:	两个字节的加减乘除运算
Input:
	pcDsSource	取值起始地址
	addnum		加数
	subnum		减数
	mulnum		乘数
	divnum		除数
	Hfront		高低位标志
					0:小端，高位在高地址
					1:大端，高位在低地址
	pcFormat	打印格式

Output:	pcDataOut	计算结果输出地址
Return:	void
Others:
*************************************************/

void TwoByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront,
                       const byte* pcFormat, byte *pcDataOut )
{
	uint16 twobyte = 0;
	double disnum = 0;

	if( Hfront == 0 ) //小端，高位在高地址
	{
		twobyte = ( uint16 ) * ( ++pcDsSource ) << 8;
		twobyte |= ( uint8 ) * ( --pcDsSource );
	}
	else if( Hfront == 1 )
	{
		twobyte = ( uint16 ) * pcDsSource << 8;
		twobyte |= ( uint8 ) * ( ++pcDsSource );
	}

	if( divnum == 1 && mulnum == 1 )
		disnum = ( double )twobyte + addnum - subnum;
	else if( divnum != 1 && mulnum == 1 )
		disnum = ( double )twobyte / divnum + addnum - subnum;
	else if( mulnum != 1 && divnum == 1 )
		disnum = ( double )twobyte * mulnum + addnum - subnum;
	else
		disnum = ( double )twobyte * mulnum / divnum + addnum - subnum;

	/*sprintf_s( pcDataOut, 20, pcFormat, disnum );*/
	sprintf( pcDataOut, pcFormat, disnum );
}
/*************************************************
Description:	四个字节的加减乘除运算
Input:
	pcDsSource	取值起始地址
	addnum		加数
	subnum		减数
	mulnum		乘数
	divnum		除数
	Hfront		高低位标志
					0:小端，高位在高地址
					1:大端，高位在低地址
	pcFormat	打印格式

Output:	pcDataOut	计算结果输出地址
Return:	void
Others:	结果不超过32位 eg.XYZW*2/65536
*************************************************/
void FourByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, bool Hfront,
                        const byte* pcFormat, byte *pcDataOut )
{
	UNN_2WORD_4BYTE num;
	double dinum = 0;

	if( Hfront )
	{
		num.u8Bit[3] = pcDsSource[3];
		num.u8Bit[2] = pcDsSource[2];
		num.u8Bit[1] = pcDsSource[1];
		num.u8Bit[0] = pcDsSource[0];
	}
	else
	{
		num.u8Bit[0] = pcDsSource[3];
		num.u8Bit[1] = pcDsSource[2];
		num.u8Bit[2] = pcDsSource[1];
		num.u8Bit[3] = pcDsSource[0];		 
	}

	dinum = ( double )num.u32Bit;

	if( divnum == 1 && mulnum == 1 )
		dinum = dinum + addnum - subnum;
	else if( divnum != 1 && mulnum == 1 )
		dinum = dinum / divnum + addnum - subnum;
	else if( mulnum != 1 && divnum == 1 )
		dinum = dinum * mulnum + addnum - subnum;
	else
		dinum = dinum * mulnum / divnum + addnum - subnum;

	/*sprintf_s( pcDataOut, 20, pcFormat, dinum );*/
	sprintf( pcDataOut, pcFormat, dinum );
}
/*************************************************
Description:	三个字节的加减乘除运算
Input:
	pcDsSource	取值起始地址
	addnum		加数
	subnum		减数
	mulnum		乘数
	divnum		除数
	Hfront		高低位标志
					0:小端，高位在高地址
					1:大端，高位在低地址
	pcFormat	打印格式

Output:	pcDataOut	计算结果输出地址
Return:	void
Others:	结果不超过32位
*************************************************/
void ThreeByteOperationd( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront,
                          const byte* pcFormat, byte *pcDataOut )
{
	byte i;
	double dinum = 0;
	UNN_2WORD_4BYTE num;

	num.u32Bit = 0;

	if( Hfront )
	{
		i = 3;

		while( i-- )
			num.u8Bit[i] = pcDsSource[i];
	}
	else
	{
		i = 0;

		while( i < 3 )
		{
			num.u8Bit[i] = pcDsSource[2 - i];
			i++;
		}

	}

	dinum = ( double )num.u32Bit;

	if( divnum == 1 && mulnum == 1 )
		dinum = dinum + addnum - subnum;
	else if( divnum != 1 && mulnum == 1 )
		dinum = dinum / divnum + addnum - subnum;
	else if( mulnum != 1 && divnum == 1 )
		dinum = dinum * mulnum + addnum - subnum;
	else
		dinum = dinum * mulnum / divnum + addnum - subnum;

	/*sprintf_s( pcDataOut, 20, pcFormat, dinum );*/
	sprintf( pcDataOut, pcFormat, dinum );
}

/*************************************************
Description:	字符串显示运算
Input:
	pcDsSource			取值起始地址
	pstDisString		显示字符串的数组
	cDisStringOffset	显示字符串偏移
	cAndNum				与数据
	cMoveToRightTimes	右移次数

Output:	pcDataOut	计算结果输出地址
Return:	void
Others:
*************************************************/
void DisplayString( const byte *pcDsSource, STRUCT_DIS_STRING* pstDisString, byte cDisStringOffset, byte cAndNum,
                    byte cMoveToRightTimes, byte *pcDataOut )
{
	STRUCT_STRING_UNIT *pstStringUnitTemp;
	byte i = 0;
	byte cNeedCompareData = 0;
	bool bFind = false;

	pstStringUnitTemp = pstDisString[cDisStringOffset].pstStringUnitGroup;
	cNeedCompareData = *pcDsSource & cAndNum;
	cNeedCompareData >>= cMoveToRightTimes;

	for( i = 0; i < pstDisString[cDisStringOffset].cCompareDataSum; i++ )
	{
		if( cNeedCompareData == pstStringUnitTemp->cCompareData )
		{
			bFind = true;
			break;
		}

		pstStringUnitTemp++;
	}

	if( bFind ) //判断是否找到
	{
		memcpy( pcDataOut, pstStringUnitTemp->pcOutputString, strlen( pstStringUnitTemp->pcOutputString ) );

		pcDataOut[ strlen( pstStringUnitTemp->pcOutputString ) ] = '\0';
	}
	else
	{
		memcpy(	pcDataOut,
		        pstDisString[cDisStringOffset].pcDefaultOutputString,
		        strlen( pstDisString[cDisStringOffset].pcDefaultOutputString )
		      );

		pcDataOut[ strlen( pstDisString[cDisStringOffset].pcDefaultOutputString ) ] = '\0';
	}

}

/*************************************************
Description:	以ASII码方式显示
Input:
	pcDsSource	取值起始地址
	iValidLen	有效长度（需格式化的长度）

Output:	pcDataOut	结果输出地址
Return:	void
Others:
*************************************************/
void DisASCII( const byte *pcDsSource, int iValidLen, byte *pcDataOut )
{
	memcpy( pcDataOut, pcDsSource, iValidLen );
	pcDataOut[iValidLen] = '\0';
}
/*************************************************
Description:	以DisHex码方式显示
Input:
	pcDsSource	取值起始地址
	iValidLen	有效长度（需格式化的长度）

Output:	pcDataOut	结果输出地址
Return:	void
Others:
*************************************************/

void DisHex( const byte *pcDsSource, int iValidLen, byte *pcDataOut )
{
	int i = 0;

	for( ; i < iValidLen; i++ )
	{
		/*sprintf_s( &pcDataOut[i * 2], ( iValidLen * 2 + 1 - i * 2 ), "%02X", pcDsSource[i] );*/
		sprintf( &pcDataOut[i * 2], "%02X", pcDsSource[i] );
	}
}

