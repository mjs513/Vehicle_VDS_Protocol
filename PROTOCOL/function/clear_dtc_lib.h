/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明清除故障码处理函数
History:
	<author>	<time>		<desc>
************************************************************************/
#ifndef _CLEAR_DTC_LIB_H
#define _CLEAR_DTC_LIB_H
#include "../interface/protocol_define.h"

enum CLEAR_DTC_TYPE
{
    GENERAL_CLEAR_DTC = 0,
	GENERAL_CLEAR_1939_DTC,
};
void process_clear_Dtc( void* pIn, void* pOut );

void process_general_clear_Dtc( void* pIn, void* pOut );
void process_general_clear_1939_Dtc( void* pIn, void* pOut );
#endif

