#ifndef SEMANTYPEENUM_H
#define SEMANTYPEENUM_H

// 错误类型
typedef enum _ErrorTypeEnum
{
	NOT_DEFINED_VARIABLE = 1,  // 变量在使用时未经定义
	NOT_DEFINED_FUNCTION,  // 函数在调用时未经定义
	RE_DEFINED_VARIABLE,   // 变量出现重复定义，或变量与前面定义过的结构体名字重复
	RE_DEFINED_FUNCTION,   // 函数出现重复定义
	MISMATCHED_ASSIGNMENT, // 赋值号两边的类型不匹配
	NOT_LEFT_ASSIGNMENT,   // 赋值号左边出现一个只有右值的表达式
	MISMATCHED_OPERAND,	   // 操作数类型不匹配或操作数类型与操作符不匹配（例如整型变量与数组变量相加减，或数组（或结构体）变量与数组（或结构体）变量相加减）
	MISMATCHED_RETURN,	   // 函数返回值类型与函数声明的返回值类型不匹配
	MISMATCHED_PARAMETER,  // 函数调用时实参与形参的数目和类型不匹配
	NOT_ARRAY,			   // 对非数组变量使用数组访问操作符
	NOT_FUNCTION,		   // 对非函数变量使用函数调用操作符
	NOT_INTEGER_INDEX,	   // 数组访问操作符中的下标表达式不是整型表达式
	NOT_STRUCT,			   // 对非结构体变量使用结构体成员访问操作符
	NOT_DEFINED_SCOPE,	   // 访问结构体中未定义过的域。
	RE_DEFINED_SCOPE,	   // 结构体中域名重复定义（指同一结构体中），或在定义时对域进行初始化
	RE_DEFINED_STRUCT,	   // 结构体名字重复定义
	NOT_DEFINED_STRUCT,	   // 结构体在使用时未经定义
	// 仅使用1，3，5，6，7，10，12
} ErrorTypeEnum;
/*
 * 数组等价 数组的基类型相同，维数相同
 * 结构体赋值 对应的域相应赋值
 * 结构体等价 进行名等价判定
 */

typedef enum _Kind {
	BASIC_KIND, 
	ARRAY_KIND, 
	STRUCTURE_KIND,
	FUNCTION_KIND 
} Kind;

typedef enum _BasicTypeEnum
{
	INT_TYPE = 1,
	FLOAT_TYPE
} BasicTypeEnum;
#endif // SEMANTYPEENUM_H