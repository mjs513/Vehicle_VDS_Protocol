/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明特殊功能函数、定义相关宏和数据类型
History:
<author>	<time>		<desc>

************************************************************************/
#ifndef _SCAN_ECU_VERSION_FUNCTION
#define _SCAN_ECU_VERSION_FUNCTION

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include "..\SpecialFunction\special_function.h"
#include "active_ecu_lib.h"
#include "idle_link_lib.h"


/********************************通用函数声明**************************************/
void Read_ECU_Version( STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut );
void Check_ECU_Version(STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut);
void process_ECU_version_function( void* pIn, void* pOut );

#endif