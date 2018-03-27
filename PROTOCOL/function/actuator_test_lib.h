/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明执行器测试处理函数
History:
	<author>	<time>		<desc>
************************************************************************/
#ifndef _ACTUATOR_TEST_LIB
#define _ACTUATOR_TEST_LIB
#include "../interface/protocol_define.h"

enum ACTUATOR_TEST_TYPE
{
    GENERAL_ACTUATOR_TEST = 0,
};
void process_actuator_test( void* pIn, void* pOut );

void process_general_actuator_test( void* pIn, void* pOut );
bool actuator_just_send_cmd( uint32* u32ActuatorTestCmdData, STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut );
bool actuator_input_type( uint32* u32ActuatorTestCmdData, STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut );
bool actuator_process_exit( uint32* u32ActuatorTestCmdData, STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut );

#endif
