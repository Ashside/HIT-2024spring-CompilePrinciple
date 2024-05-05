#ifndef SEMANTYPEENUM_H
#define SEMANTYPEENUM_H

// 错误类型
typedef enum _ErrorTypeEnum
{
	UNDEF_VAR = 1,         // Undefined Variable
    UNDEF_FUNC,            // Undefined Function
    REDEF_VAR,             // Redefined Variable
    REDEF_FUNC,            // Redefined Function
    TYPE_MISMATCH_ASSIGN,  // Type mismatchedfor assignment.
    LEFT_VAR_ASSIGN,  // The left-hand side of an assignment must be a variable.
    TYPE_MISMATCH_OP,      // Type mismatched for operands.
    TYPE_MISMATCH_RETURN,  // Type mismatched for return.
    FUNC_AGRC_MISMATCH,    // Function is not applicable for arguments
    NOT_A_ARRAY,           // Variable is not a Array
    NOT_A_FUNC,            // Variable is not a Function
    NOT_A_INT,             // Variable is not a Integer
    ILLEGAL_USE_DOT,       // Illegal use of "."
    NONEXISTFIELD,         // Non-existentfield
    REDEF_FEILD,           // Redefined field
    DUPLICATED_NAME,       // Duplicated name
    UNDEF_STRUCT       // 结构体在使用时未经定义
	// 仅使用1，3，5，6，7，10，12
} ErrorType;
/*
 * 数组等价 数组的基类型相同，维数相同
 * 结构体赋值 对应的域相应赋值
 * 结构体等价 进行名等价判定
 */

typedef enum _Kind {
	BASIC, 
	ARRAY, 
	STRUCTURE,
	FUNCTION 
} Kind;

typedef enum _BasicTypeEnum
{
	INT_TYPE = 1,
	FLOAT_TYPE
} BasicType;
#endif // SEMANTYPEENUM_H