/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明数据流数据处理函数
History:
	<author>	<time>		<desc>
************************************************************************/
#ifndef _DS_LIB_H
#define _DS_LIB_H
#include "../interface/protocol_define.h"
#include "../public/protocol_config.h"

typedef struct _STRUCT_DS_STORAGE_BUFFER
{
	int iCmdID;
	bool bNegativeFlag;
	int iValidLen;
	byte cBuffer[512];
} STRUCT_DS_STORAGE_BUFFER;

enum READ_DATA_STREAM_TYPE
{
    READ_GENERAL_DATA_STREAM = 0,
	TOGETHER_READ_GENERAL_DATA_STREAM,
};

enum FORMULA_TYPE
{
    FORMULA_PARSER = 0,
    PROTOCOL_PARSER,
};

byte get_current_screen_DS_config( void* pIn );
void free_general_DS_formula_config_space( void );
void process_read_data_stream( void* pIn, void* pOut );
int get_ds_config( STRUCT_DS_FORMULA_CONFIG * pstDSConfig, const byte * pcSource );
byte get_DS_buffer_offset( int iDSCmdID );
int send_receive_ds_cmd( byte * ReceiveBuffer, int * iValidByteNum, int DsId, int CmdSum );

void process_read_general_data_stream( void* pIn, void* pOut );
void process_general_data_stream_data( STRUCT_DS_FORMULA_GROUP_CONFIG * pstDSGroupConfig, byte cItemSum,
                                       STRUCT_CHAIN_DATA_OUTPUT* pstDataStreamOut );
void process_together_read_general_data_stream( void* pIn, void* pOut );
extern bool process_Multi_frame_cmd_To_VDI_processing(int CmdSum,byte* piCmd,void* pOut);

#endif