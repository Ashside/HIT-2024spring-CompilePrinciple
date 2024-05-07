#ifndef INTERCODEENUM_H
#define INTERCODEENUM_H

typedef enum _InterCodeEnum
{
	IR_LABEL, // label x :
	IR_FUNCTION, // function x :
	IR_ASSIGN, // x := y
	IR_ADD, // x := y + z
	IR_SUB, // x := y - z
	IR_MUL, // x := y * z
	IR_DIV, // x := y / z
	IR_GET_ADDR, // x := &y
	IR_READ_ADDR, // x := *y
	IR_WRITE_ADDR, // [*x := y
	IR_GOTO, // GOTO x
	IR_IF_GOTO, // IF x [relop] y GOTO z
	IR_RETURN, // RETURN x
	IR_DEC, // DEC x [size]
	IR_ARG, // ARG x
	IR_CALL, // x := CALL f
	IR_PARAM, // PARAM x
	IR_READ, // READ x
	IR_WRITE, // WRITE x
} InterCodeEnum;

typedef enum _OperandEnum
{
	OP_VARIABLE,
	OP_CONSTANT,
	OP_ADDRESS,
	OP_LABEL,
	OP_FUNCTION,
	OP_RELOP,
} OperandEnum;

#endif