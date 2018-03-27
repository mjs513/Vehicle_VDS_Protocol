#ifndef __FORMULA_PARSER_PARSER_H__
#define __FORMULA_PARSER_PARSER_H__

#include <stdio.h> 
#include <stdlib.h>
#include <math.h>  
#include <string.h>

#define MAX_SIZE 512

enum BOOL
{
    FALSE,
    TRUE
};

typedef struct tagOPERATE
{
	double Operand;//操作数
	char Operator;//操作符
} OPERATE, * P_OPERATE;

int IsDigit( char ch, int bHex );//判断是否为数字

//将公式中的A-Z置换为有效数据
void PutDataToFormula( const char* pcDataSource, const int nDataSourceLen, const char* pSrcFormula, int nFormulaLen, char* pDestFormula );

//处理公式中的负号
void HandleMinus( char* pcSrc );

//用单个字母替换数学表达式
void FormulaProc( char* pSrc );

//根据运算符的优先级格式化公式
void FormatFormula( char* pSrc, P_OPERATE pOperate );

//获得操作数
int GetOperand( char* pSrc, int * nLen, double *pOperand );

//
int Locate( char ch );

//获得操作符进栈的优先级
int GetPushStackPriority( char ch );

//获得操作符出栈的优先级
int GetPopStackPriority( char ch );

//计算数值
double CalculateValue( P_OPERATE pOperator, double x );

//拆分公式和Switch
void ParseFormulaAndSwitch( const char* pstrFormula, int nLen, char* pOutStrFormula, char* pOutSwitch );

//SwitchFunction
void SwitchFunction( int nValue, char* pSwitchFunction, int nLen, char* pOutValue );

//分段函数公式分析
void PieceSwitchFunction(unsigned int nValue, char* pSwitchFunction, int nLen, char* pOutValue );




/************************************************************************/
/* 获得Format的类型
0：ASCII  不需要计算
1：HEX 十六进制显示	不需要计算
2：DECIMAL 十进制显示	不需要计算
3：INTEGER 整数格式化显示	需要计算
4：DOUBLE 浮点型数据格式化显示	需要计算
5：STRING 根据数值找到对应的字符串ID                                                                     */
/************************************************************************/
int GetFormatType( char *pStrFormat );

#endif