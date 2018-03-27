/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义从xml获取初始化配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "init_config_from_xml_lib.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "..\public\protocol_config.h"
#include <ctype.h>


STRUCT_INIT_XML_GLOBAL_VARIABLE g_stInitXmlGobalVariable =
{
	0,
	0,
	NULL,
	0,
	NULL,
	0,
};
STRUCT_INIT_PROTOCOL_GLOBAL_VARIABLE g_stInitProtocolGobalVariable=
{
	false,
	false,
	0,
	0,

};

/*************************************************
Description:	处理从xml获取配置数据
Input:
	iConfigType		配置类型
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据配置类型进相应分支
*************************************************/
void process_init_config_from_xml( int iConfigType, void* pIn )
{
	switch( iConfigType )
	{
	case VCI_CONFIG:
	{
		get_vci_config_data( pIn );
	}
	break;

	case IDLE_LINK_CONFIG:
	{
		get_idle_link_config_data( pIn );
	}
	break;

	case ACTIVE_ECU_CONFIG:
	{
		get_active_config_data( pIn );
	}
	break;

	case PROTOCOL_CONFIG:
	{
		get_protocol_config_data( pIn );
	}
	break;

	case SPECIFIC_COMMAND_CONFIG:
	{
		get_specific_command_config_data( pIn );
	}
	break;

	case DTC_CONFIG:
	{
		get_dtc_config_data( pIn );
	}
	break;

	case FREEZE_DTC_CONFIG:
	{
		get_freeze_dtc_config_data( pIn );
	}
	break;

	case FREEZE_DS_CONFIG:
	{
		get_freeze_ds_config_data( pIn );
	}
	break;

	case PROCESS_FUN_CONFIG:
	{
		get_process_fun_config_data( pIn );
	}
	break;

	case SECURITY_ACCESS_CONFIG:
	{
		get_security_access_config( pIn );
	}
	break;

	case ECU_INFORMATION_FORMULA:
	{
		get_information_config( pIn );
	}
	break;

	case FREEZE_DATA_STREAM_FORMULA:
	{
		get_freeze_DS_formula_config( pIn );
	}
	break;

	case SPECIAL_CONFIG:
	{
		get_special_config( pIn );
	}
	break;



	default:
		break;
	}
}
/*************************************************
Description:	获得xml中配置的命令
Input:
	pcSource	取用配置命令的地址（字符串）
	iMaxLen		取数据的长度限制

Output:	pu32Dest	保存命令数据的地址
Return:	int			命令个数
Others:	在xml配置中有0X的表示16进制否则是10进制
		以后会用专门工具添加配置内容，这里没有进行
		错误判断、处理 2014-5-28 12:10:54
		数据最终保存在uint32类型的数组中。
		pcSource中数据格式类似于00090x01,0xBB，其中0009表
		示长度（16进制），0x01,0xBB为xml中一个元素的内容
*************************************************/
int get_config_data( uint32 *pu32Dest, const byte* pcSource, int iMaxLen )
{
	byte cTemp[15] = {0};
	int num = 0;
	int i = 0, j = 0, k = 0, m = 0;
	int iScale = 16;

	if( 0 == iMaxLen )
		return iMaxLen;

	while( pcSource[i] != '\0' && i != iMaxLen )
	{
		memcpy( cTemp, &pcSource[i], 4 );
		cTemp[4] = '\0';
		iScale = 16;
		num = strtol( cTemp, NULL, iScale );
		i += 4;
		k = 0;

		if( 0 == num )
			continue;

		if( num < 3 )
		{
			iScale = 10;

			memcpy( cTemp, &pcSource[i], num );

			cTemp[num] = '\0';

			pu32Dest[m] = strtol( cTemp, NULL, iScale );
			m += 1;

			i += num;

			continue;
		}

		for( j = 0; j < num + 1; j++ )
		{
			if( isspace( pcSource[i + j] ) ) //处理空格、换行、制表符等
				continue;

			if( ',' == pcSource[i + j] )
			{
				cTemp[k] = '\0';
				pu32Dest[m] = strtol( cTemp, NULL, iScale );
				m += 1;
				k = 0;
				continue;
			}
			else if( j == num )
			{
				cTemp[k] = '\0';
				pu32Dest[m] = strtol( cTemp, NULL, iScale );
				m += 1;
				k = 0;
				break;
			}

			cTemp[k] = pcSource[i + j];

			if( 1 == k ) //在xml中0和x中间有空格其实是错误的，这里添加这种容错
			{
				if( cTemp[k] == 'x' || cTemp[k] == 'X' )
					iScale = 16;
				else
					iScale = 10;
			}

			k += 1;

		}

		i += num;
	}

	return m;

}

/*************************************************
Description:	获得xml中command一块配置命令数据
Input:
	pcSource	取用配置命令的地址（字符串）

Output:	pu32Dest	保存命令数据的地址
Return:	int			跟这个配置命令相关的字符个数
Others:
pcSource中数据格式类似于00090x01,0xBB，其中0009表
示长度（16进制），0x01,0xBB为xml中一个元素的内容
*************************************************/
int get_command_one_block_config_data( byte *pcDest, int *iNum, const byte* pcSource )
{
	byte cTemp[15] = {0};
	int iLen = 0;

	memcpy( cTemp, pcSource, 4 );
	cTemp[4] = '\0';

	iLen = 4;
	iLen += strtol( cTemp, NULL, 16 );

	if( 4 == iLen ) //长度为4表示没有内容
		return iLen;

	*iNum = get_cmd_config_content_data( pcDest , pcSource, iLen );

	return iLen;
}

/*************************************************
Description:	获得xml中配置cmmand中具体命令内容
Input:
	pcSource	取用配置命令的地址（字符串）
	iMaxLen		取数据的长度限制

Output:	pu32Dest	保存命令数据的地址
Return:	int			命令个数
Others: 数据最终保存在byte类型的数组中
*************************************************/
int get_cmd_config_content_data( byte *pcDest, const byte* pcSource, int iMaxLen )
{
	byte cTemp[256] = {0};
	int num = 0;
	int i = 0, j = 0, k = 0, m = 0;
	int iScale = 16;

	if( 0 == iMaxLen )
		return iMaxLen;

	while( pcSource[i] != '\0' && i != iMaxLen )
	{
		memcpy( cTemp, &pcSource[i], 4 );
		cTemp[4] = '\0';
		iScale = 16;

		num = strtol( cTemp, NULL, iScale );
		i += 4;
		k = 0;

		if( 0 == num )
			continue;

		if( num < 3 )
		{
			iScale = 10;

			memcpy( cTemp, &pcSource[i], num );

			cTemp[num] = '\0';

			pcDest[m] = ( byte )strtol( cTemp, NULL, iScale );
			m += 1;

			i += num;

			continue;
		}

		for( j = 0; j < num + 1; j++ )
		{
			if( isspace( pcSource[i + j] ) ) //处理空格、换行、制表符等
				continue;

			if( ',' == pcSource[i + j] )
			{
				cTemp[k] = '\0';
				pcDest[m] = ( byte )strtol( cTemp, NULL, iScale );
				m += 1;
				k = 0;
				continue;
			}
			else if( j == num )
			{
				cTemp[k] = '\0';
				pcDest[m] = ( byte )strtol( cTemp, NULL, iScale );
				m += 1;
				k = 0;
				break;
			}

			cTemp[k] = pcSource[i + j];

			if( 1 == k ) //在xml中0和x中间有空格其实是错误的，这里添加这种容错
			{
				if( cTemp[k] == 'x' || cTemp[k] == 'X' )
					iScale = 16;
				else
					iScale = 10;
			}

			k += 1;

		}

		i += num;
	}

	return m;

}
