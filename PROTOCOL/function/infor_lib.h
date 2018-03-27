/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明版本信息数据处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#ifndef _INFOR_LIB_H
#define _INFOR_LIB_H
#include "../interface/protocol_define.h"
#include "../public/protocol_config.h"

enum READ_INFORMATION_TYPE
{
    GENERAL_READ_INFORMATION = 0,
};

void process_read_ECU_information( void* pIn, void* pOut );

void process_read_general_ECU_information( void* pIn, void* pOut );
void process_general_infor_data( STRUCT_DS_FORMULA_GROUP_CONFIG *pstInformationGroupConfig, STRUCT_CHAIN_DATA_OUTPUT* pstInformOut
                                 , int iReceiveStatus );
#endif