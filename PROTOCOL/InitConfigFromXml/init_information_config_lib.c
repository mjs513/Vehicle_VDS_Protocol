/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义从xml获取版本信息配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>
#include "..\function\ds_lib.h"


/*************************************************
Description:	获取版本信息配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据不同的配置模版进相应的分支
*************************************************/
void get_information_config( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cTemp	= 0;
	byte * pcTemp = NULL;

	byte i = 0;
	byte j = 0;

	int iLen = 0;
	int temp = 0;

	byte cGroupSum = 0;
	byte cGroupID = 0;
	byte cItemSum = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pcTemp = pstParam->pcData;

	iLen = get_command_one_block_config_data( &cTemp, &temp, pcTemp );
	cGroupSum = cTemp;//获得组的总数
	pcTemp += iLen;

	for( i = 0; i < cGroupSum; i++ )
	{
		iLen = get_command_one_block_config_data( &cTemp, &temp, pcTemp );
		cGroupID = cTemp;//获得本组的组号
		pcTemp += iLen;

		g_p_stInformationGroupConfigGroup[cGroupID] = ( STRUCT_DS_FORMULA_GROUP_CONFIG * )malloc( sizeof( STRUCT_DS_FORMULA_GROUP_CONFIG ) );
		g_p_stInformationGroupConfigGroup[cGroupID]->pstDSFormulaConfig = NULL;

		iLen = get_command_one_block_config_data( &cTemp, &temp, pcTemp );
		cItemSum = cTemp;//获得本组中版本信息条数
		pcTemp += iLen;

		g_p_stInformationGroupConfigGroup[cGroupID]->cItemSum = cItemSum;
		g_p_stInformationGroupConfigGroup[cGroupID]->pstDSFormulaConfig =
		    ( STRUCT_DS_FORMULA_CONFIG * )calloc( cItemSum, sizeof( STRUCT_DS_FORMULA_CONFIG ) );

		for( j = 0; j < cItemSum; j++ )
		{
			iLen = get_ds_config( ( STRUCT_DS_FORMULA_CONFIG * ) & ( g_p_stInformationGroupConfigGroup[cGroupID]->pstDSFormulaConfig[j] ),
			                      pcTemp );
			pcTemp += iLen;
		}

	}

}

/*************************************************
Description:	释放存放具体命令配置的空间
Input:	无
Output:	保留
Return:	无
Others: 每添加一类配置就在该函数中添加相应的释放代码
并在quit_system_lib.c的free_xml_config_space
函数中调用该函数。
*************************************************/
void free_information_config_space( void )
{
	byte i = 0;
	byte j = 0;

	for( i = 0; i < sizeof( g_p_stInformationGroupConfigGroup ) / sizeof( g_p_stInformationGroupConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stInformationGroupConfigGroup[i] )
		{
			if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig )
			{
				for( j = 0; j < g_p_stInformationGroupConfigGroup[i]->cItemSum; j++ )
				{
					if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcValidByteNumber )
					{
						free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcValidByteNumber );
						g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcValidByteNumber = NULL;
					}

					if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcValidByteOffset )
					{
						free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcValidByteOffset );
						g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcValidByteOffset = NULL;
					}

					if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].piDSCmdID )
					{
						free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].piDSCmdID );
						g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].piDSCmdID = NULL;
					}

					if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcFormula )
					{
						free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcFormula );
						g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcFormula = NULL;
					}

					if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pStrFormat )
					{
						free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pStrFormat );
						g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pStrFormat = NULL;
					}
				}

				free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig );
				g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig = NULL;
			}

			free( g_p_stInformationGroupConfigGroup[i] );
			g_p_stInformationGroupConfigGroup[i] = NULL;

		}
	}

}