/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明常用公式计算函数和版本信息显示函数
History:
	<author>	<time>		<desc>
************************************************************************/
#ifndef _FORMULA_H
#define _FORMULA_H
#include "../interface/protocol_define.h"

/*定义的几个结构体，用来组合字符总数目和组合字符的组合*/
typedef struct _STRUCT_STRING_UNIT
{
	byte cCompareData;		//要比较的数据
	byte* pcOutputString;	//要输出的字符串
} STRUCT_STRING_UNIT;

typedef struct _STRUCT_DIS_STRING
{
	byte cCompareDataSum;					//需要比较数据个数
	byte* pcDefaultOutputString;			//不满足要求时输出的字符串
	STRUCT_STRING_UNIT *pstStringUnitGroup;	//定义在上面
} STRUCT_DIS_STRING;

void OneByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, const byte* pcFormat, byte *pcDataOut );
void TwoByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront, const byte* pcFormat, byte *pcDataOut );
void FourByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, bool Hfront, const byte* pcFormat, byte *pcDataOut );
void ThreeByteOperationd( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront, const byte* pcFormat, byte *pcDataOut );
void DisplayString( const byte *pcDsSource, STRUCT_DIS_STRING* pstDisString, byte cDisStringOffset, byte cAndNum, byte cMoveToRightTimes, byte *pcDataOut );

void DisASCII( const byte *pcDsSource, int iValidLen, byte *pcDataOut );
void DisHex( const byte *pcDsSource, int iValidLen, byte *pcDataOut );


#endif

