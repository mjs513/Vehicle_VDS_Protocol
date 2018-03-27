#ifndef __FORMULA_PARSER_INTERFACE_H__
#define __FORMULA_PARSER_INTERFACE_H__



/*
***********************************************************************
** Function             : calculate
** Input                :
**		pcDataSource	: 数据源，为数组指针
**		nDataSourceLen	: 数据源的长度
**		pcFormula		: 公式
**		nFormulaLen		: 公式长度
**		pStrFormat		: 格式化数值的字符串表达式 例如 "%d"
** Output               : 输出参数
**		pOutStringData	: 返回字符串类型的数据
** Return               :
** Description          :
**       --
	ASCII、HEX、DECIMAL显示方式是不需要计算公式的。

** Calls                :
**       -- 调用的函数
***********************************************************************
*/
int calculate( const char* pcDataSource, const int nDataSourceLen,
               const char* pcFormula, const int nFormulaLen, char *pStrFormat, char* pOutStringData );




#endif