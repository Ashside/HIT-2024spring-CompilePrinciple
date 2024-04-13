#ifndef TOKEN_TYPE_ENUM_H
#define TOKEN_TYPE_ENUM_H

/*
 * TokenTypeEnum
 * 根据词法分析的结果，将词法单元的类型进行分类
 * 主要依据是实验指导书中的打印要求
 */
typedef enum _TokenTypeEnum
{
	TOKEN_INT,
	TOKEN_FLOAT,
	TOKEN_ID,
	TOKEN_TYPE,
	TOKEN_SYMBOL,
	TOKEN_NOTTOKEN,
} TokenTypeEnum;

#endif // TOKEN_TYPE_ENUM_H