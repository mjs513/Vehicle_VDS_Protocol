/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义获取激活配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>

/*************************************************
Description:	获取发送特殊命令的配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据不同的配置模版进相应的分支
*************************************************/
void get_special_config( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ConfigTemp[50] = {0};
	int    i = 0;
	int    j = 0;
	byte cConfigOffset = 0;

	//pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	i = get_config_data( u32ConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cConfigOffset = ( byte )u32ConfigTemp[0];

	for( j = 0; j < i - 2; j++ )
	{
		u32Config_fuc_Group[cConfigOffset][j] = u32ConfigTemp[j + 2];
	}

	//memcpy(u32Config_fuc_Group, u32ConfigTemp+2, i-2);

}




