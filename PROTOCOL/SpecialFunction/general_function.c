/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义特殊功能处理函数和通用的相关函数
History:
	<author>	<time>		<desc>

************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*#define NDEBUG*/
#include "special_function.h"
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include "..\interface\protocol_interface.h"


/*************************************************
Description:	获得特殊功能的命令数据
Input:
	pcSource	存放命令数据的地址（字符串）
	iMaxLen	命令数据长度（字符串）

Output:	pcCmdData	存放命令数据地址
Return:	int		命令数据个数
Others:	直接从xml获取的命令数据是string类型的，
		且格式类似"00,10,00,01,01,25,07,11"
*************************************************/
int get_string_type_data_to_byte( byte* pcCmdData, const byte* pcSource, const int iMaxLen )
{
	byte cTemp[15] = {0};
	int i = 0, k = 0, m = 0;
	int iScale = 16;

	if( 0 == iMaxLen )
		return iMaxLen;

	while( i != iMaxLen + 1 )
	{
		if( 0 == k )
			iScale = 10;

		if( isspace( pcSource[i] ) ) //处理空格、换行、制表符等
		{
			i += 1;
			continue;
		}

		if( ',' == pcSource[i] )
		{
			cTemp[k] = '\0';
			pcCmdData[m] = ( byte )strtol( cTemp, NULL, iScale );
			m += 1;
			k = 0;
			i += 1;
			continue;
		}
		else if( i == iMaxLen )
		{
			cTemp[k] = '\0';
			pcCmdData[m] = ( byte )strtol( cTemp, NULL, iScale );
			m += 1;
			break;
		}

		cTemp[k] = pcSource[i];

		if( 1 == k ) //在xml中0和x中间有空格其实是错误的，这里添加这种容错
		{
			if( cTemp[k] == 'x' || cTemp[k] == 'X' )
				iScale = 16;
		}

		k += 1;

		i += 1;
	}

	return m;
}

/*************************************************
Description:	获得类型为string的命令数据
Input:
	pcSource	存放命令数据的地址（字符串）
	iMaxLen		命令数据长度（字符串）

Output:	pu32Dest	存放命令数据地址
Return:	int		命令数据个数
Others:	直接从xml获取的命令数据是string类型的，
		且格式类似"00,10,0x00,01,01,25,07,11"
		把结果保存在uint32型地址中
*************************************************/
int get_string_type_data_to_uint32( uint32 *pu32Dest, const byte* pcSource, const int iMaxLen )
{
	byte cTemp[15] = {0};
	int i = 0, k = 0, m = 0;
	int iScale = 16;

	if( 0 == iMaxLen )
		return iMaxLen;

	while( i != iMaxLen + 1 )
	{
		if( 0 == k )
			iScale = 10;

		if( isspace( pcSource[i] ) ) //处理空格、换行、制表符等
		{
			i += 1;
			continue;
		}

		if( ',' == pcSource[i] )
		{
			cTemp[k] = '\0';
			pu32Dest[m] = strtol( cTemp, NULL, iScale );
			m += 1;
			k = 0;
			i += 1;
			continue;
		}
		else if( i == iMaxLen )
		{
			cTemp[k] = '\0';
			pu32Dest[m] = strtol( cTemp, NULL, iScale );
			m += 1;
			break;
		}

		cTemp[k] = pcSource[i];

		if( 1 == k ) //在xml中0和x中间有空格其实是错误的，这里添加这种容错
		{
			if( cTemp[k] == 'x' || cTemp[k] == 'X' )
				iScale = 16;
		}

		k += 1;

		i += 1;
	}

	return m;

}


/*************************************************
Description:	返回状态信息函数
Input:
	cTipMode		状态信息类型，可取下列3种值的或
						功能执行的结果:
								COMM_INTERRUPT(0X02)
								PROCESS_OK(0X01)
								PROCESS_FAIL(0X00)
						是否跳步:
								HAVE_JUMP(0X04)
								NO_JUMP(0X00)
						是否弹出提示框:
								HAVE_TIP_DATA(0X10)
								HAVE_TIP(0X08)
								NO_TIP(0X00)

	pcLable			要跳转的lable
	pcTipContent	提示信息内容

Output:	pOut	输出数据地址
Return:	int		输出地址中存放数据长度
Others:该函数只支持返回帧头信息，包括跳转和提示的信息，不包含追加数据
*************************************************/
int special_return_status( const byte cTipMode, const byte* pcLable, const byte* pcTipContent, byte cButtonNum, void* pOut )
{
	byte *pOutTemp = ( byte* )pOut;
	byte cProcessValue	= ( cTipMode >> 0 ) & 0x03;
	byte cJumpValue		= ( cTipMode >> 2 ) & 0x01;
	byte cTipValue		= ( cTipMode >> 3 ) & 0x03;
	
	byte cTipConentOffset	= 0;//提示内容保存偏移
	byte cAppendDataOffset	= 0;//附加数据保存偏移
	byte GetLogArr[] = {0xA0,0x00,0x05,0x80,0x25};

	int i = 0;
	int iDataLen = 0;
	if (cProcessValue == PROCESS_FAIL)
	{
		send_cmd(GetLogArr, 5);
	}

	pOutTemp[0] = cProcessValue;
	pOutTemp[1] = cJumpValue;
	pOutTemp[2] = cTipValue;

	pOutTemp[3] = 0;
	pOutTemp[4] = 0;
	pOutTemp[5] = 0;
	pOutTemp[6] = 0;

	//装载Lable
	pOutTemp[7] = 0;//跳转Lable长度高字节
	pOutTemp[8] = ( pcLable == NULL ) ? 0 : ( byte )strlen( pcLable ); //跳转Lable长度低字节

	memcpy( &pOutTemp[9], pcLable, pOutTemp[8] );

	//装载提示信息
	cTipConentOffset = 9 + pOutTemp[8];

	pOutTemp[ cTipConentOffset ] = 0;//提示信息长度高字节
	pOutTemp[ cTipConentOffset  + 1] = ( pcTipContent == NULL ) ? 0 : ( byte )strlen( pcTipContent ); //提示信息长度低字节

	memcpy( &pOutTemp[ cTipConentOffset + 2 ], pcTipContent, pOutTemp[ cTipConentOffset + 1] );


	//装载附加数据
	cAppendDataOffset = cTipConentOffset + 2 + pOutTemp[cTipConentOffset + 1];

	iDataLen = cAppendDataOffset;
	//带按钮，支持返回多个提示信息ID
	if (pOutTemp[2] == 3)
	{
		pOutTemp[ iDataLen ] = cButtonNum;
		iDataLen++;
	}

	pOutTemp[6] = iDataLen;

	return iDataLen;

}
/*************************************************
Description:	追加数据和控件ID
Input:
	iAppendDataOffset	存放附加数据的偏移，
							与输出总长度相同
	pcData				数据存放地址
	cDataLen			数据长度
	iUploadDataType		上传数据类型(HEX_PRINT,DEC_PRINT,ORIGINAL)
	pcControlsID		控件ID

Output:	pOut	输出数据地址
Return:	int		输出地址中存放数据长度
Others:第一次使用本函数前需先调用special_return_status
函数，本函数会修改输出数据总数字节
*************************************************/
int add_data_and_controlsID( const int iAppendDataOffset, const byte* pcData, const byte cDataLen,
                             const int iUploadDataType, const byte* pcControlsID, void* pOut )
{
	byte *pOutTemp = ( byte* )pOut;
	int i = 0;

	int iControlsIDOffset	= 0;//控件ID保存偏移
	byte cDecDataLen		= 0;//10进制数据长度
	uint32 iTemp = 0;

	UNN_2WORD_4BYTE uuControlsIDLen;

	UNN_2WORD_4BYTE DataLen;

	DataLen.u32Bit = 0;
	uuControlsIDLen.u32Bit = 0;

	DataLen.u32Bit += iAppendDataOffset;//获取之前输出总长度

	switch( iUploadDataType )
	{
	case HEX_PRINT://按16进制打印成字符的
	{
		pOutTemp[iAppendDataOffset] = 0;
		pOutTemp[iAppendDataOffset + 1] = cDataLen * 2;

		for( i = 0; i < cDataLen; i++ )
		{
			/*sprintf_s( &pOutTemp[iAppendDataOffset + 1 + i * 2], ( cDataLen * 2 + 1 - i * 2 ), "%02X", pcData[i] );*/
			sprintf( &pOutTemp[iAppendDataOffset + 2 + i * 2], "%02X", pcData[i] );
		}
	}
	break;
	case DEC_LITTLE_ENDIAN://小端模式多字节组合十进制显示，最多四个
	{
		for( i = 0; i < cDataLen; ++i)
		{
			iTemp |= (pcData[i] << (i * 8));
		}
		pOutTemp[iAppendDataOffset] = 0; 
		pOutTemp[iAppendDataOffset + 1] = sprintf( &pOutTemp[iAppendDataOffset + 2], "%d", iTemp );
	}		
	break;
	case DEC_BIG_ENDIAN://大端模式多字节组合十进制显示，最多四个
	{
		for( i = 0; i < cDataLen; ++i)
		{
			iTemp |= pcData[i];
			if ( i != cDataLen - 1 )
			{
				iTemp <<= 8;
			}
		}
		pOutTemp[iAppendDataOffset] = 0; 
		pOutTemp[iAppendDataOffset + 1] = sprintf( &pOutTemp[iAppendDataOffset + 2], "%d", iTemp );
	}		
	break;

	case DEC_PRINT://按10进制打印成字符的
	{
		cDecDataLen = 0;

		for( i = 0; i < cDataLen; i++ )
		{
			cDecDataLen += sprintf( &pOutTemp[iAppendDataOffset + 2 + cDecDataLen], "%d", pcData[i] );
		}

		pOutTemp[iAppendDataOffset] = 0;
		pOutTemp[iAppendDataOffset + 1] = cDecDataLen;
	}
	break;
	case J1939_SPN://1939的故障码SPN
		{
			for( i = 0; i < cDataLen; ++i)
			{
				iTemp |= (pcData[i] << (i * 8));
			}
			pOutTemp[iAppendDataOffset] = 0; 
			pOutTemp[iAppendDataOffset + 1] = sprintf( &pOutTemp[iAppendDataOffset + 2], "SPN%d", iTemp );
		}		
		break;
	case PCBU_PRINT://PCBU+十六进制
		pOutTemp[iAppendDataOffset] = 0;
		pOutTemp[iAppendDataOffset + 1] = cDataLen * 2 + 1;
		switch( (*pcData) & 0xC0 )
		{
		case 0:
			pOutTemp[iAppendDataOffset + 2] = 'P';
			break;
		case 0x40:
			pOutTemp[iAppendDataOffset + 2] = 'C';
			break;
		case 0x80:
			pOutTemp[iAppendDataOffset + 2] = 'B';
			break;
		case 0xC0:
			pOutTemp[iAppendDataOffset + 2] = 'U';
			break;
		}
		for( i = 0; i < cDataLen; i++ )
		{
			if( i == 0 )
				sprintf( &pOutTemp[iAppendDataOffset + 3 + i * 2], "%02X", pcData[i] & 0x3F );
			else
				sprintf( &pOutTemp[iAppendDataOffset + 3 + i * 2], "%02X", pcData[i] );
		}
		break;

	case ORIGINAL:
	default://其他情况按原样拷贝
	{
		pOutTemp[iAppendDataOffset] = 0;
		pOutTemp[iAppendDataOffset + 1] = cDataLen;

		memcpy( &pOutTemp[iAppendDataOffset + 2], pcData, cDataLen );
	}
	break;
	}

	DataLen.u32Bit += 2 + pOutTemp[iAppendDataOffset + 1];

	//装载controlsID
	iControlsIDOffset = iAppendDataOffset + 2 + pOutTemp[iAppendDataOffset + 1];

	uuControlsIDLen.u32Bit = ( pcControlsID == NULL ) ? 0 : ( uint32 )strlen( pcControlsID );

	pOutTemp[iControlsIDOffset] = uuControlsIDLen.u8Bit[1];
	pOutTemp[iControlsIDOffset + 1] = uuControlsIDLen.u8Bit[0];

	memcpy( &pOutTemp[iControlsIDOffset + 2], pcControlsID, uuControlsIDLen.u32Bit );

	DataLen.u32Bit += 2 + uuControlsIDLen.u32Bit;

	//装载总数
	pOutTemp[3] = DataLen.u8Bit[3];
	pOutTemp[4] = DataLen.u8Bit[2];
	pOutTemp[5] = DataLen.u8Bit[1];
	pOutTemp[6] = DataLen.u8Bit[0];

	return ( int )DataLen.u32Bit;
}
