#ifndef OPCODES_H
#define OPCODES_H
#include "types.h"

#include <vector>


class Function;
class ServerFunctionCompiler;


/* An OpCode consist of an unsigned short to describe the action followed by varing data.
 * 
 * The types of OpCodes:
 *     ABC  OpCode, a:u16, b:u16, c:u16
 *     ABx	OpCode, a:u16, bx:u32
 *     AsBx	OpCode, a:u16, bx:s32
 */
typedef u16 ABC;
typedef u32 Bx;
typedef i32 SBx;


enum OpTypeSpecificOffsets : u16
{
	OpxtoI8,  // target:a <- from:b
	OpxtoU8,  // target:a <- from:b
	OpxtoI16, // target:a <- from:b
	OpxtoU16, // target:a <- from:b
	OpxtoI32, // target:a <- from:b
	OpxtoU32, // target:a <- from:b
	OpxtoI64, // target:a <- from:b
	OpxtoU64, // target:a <- from:b
	OpxtoF32, // target:a <- from:b
	OpxtoF64, // target:a <- from:b

	OpxADD, // target:a <- lhs:b + rhs:c
	OpxSUB, // target:a <- lhs:b - rhs:c
	OpxMUL, // target:a <- lhs:b * rhs:c
	OpxDIV, // target:a <- lhs:b / rhs:c
	OpxMOD, // target:a <- lhs:b % rhs:c

	OpxINC, // target:a <- val:b
	OpxDEC, // target:a <- val:b

	OpxLT,  // target:a <- lhs:b < rhs:c
	OpxLE,  // target:a <- lhs:b > rhs:c

	OpxGT,  // target:a <- lhs:b >= rhs:c
	OpxGE,  // target:a <- lhs:b <= rhs:c
};


enum OpSizeSpecificOffsets : u16
{
	OpxEQ,  // target:a <- lhs:b == rhs:c
	OpxNOT, // target:a <-        ! val:b
	OpxNEQ, // target:a <- lhs:b != rhs:c

	OpxBOR,  // target:a <- lhs:b | rhs:c
	OpxBAND, // target:a <- lhs:b & rhs:c
	OpxBXOR, // target:a <- lhs:b ^ rhs:c
	OpxBNOT, // target:a <-       ~ val:b
	OpxBLSHIFT, // target:a <- lhs:b << rhs:c
	OpxBRSHIFT, // target:a <- lhs:b >> rhs:c


	OpxLOAD_CONSTANT, // target:a <- index:b
	OpxLOAD_STATIC,   // target:a <- address:bx
	OpxLOAD_MEMBER,   // target:a <- obj:b, offset:c

	OpxSET_LOCAL,  // val:a -> target:b
	OpxSET_STATIC, // val:a -> target:bx
	OpxSET_MEMBER, // val:a -> obj:b, offset:c

	OpxRETURN, // value:a
};


enum OpCode : u16
{
	OpEmptyCode,
	OpRETURN_VOID,

	OpIF,   // condition:a
	OpJUMP, // jump:sbx

	OpCALLLocal,   // a:return_value, b:callable
	OpCALLStatic,  // a:return_value, bx:callable
	OpCALLVirtual, // a:return_value, b:object, c:callable

	// ==========================================================================
	// I8 Opcodes
	// ==========================================================================
	I8Codes,

	OpI8toI8 = I8Codes + OpxtoI8,
	OpI8toU8 = I8Codes + OpxtoU8,
	OpI8toI16 = I8Codes + OpxtoI16,
	OpI8toU16 = I8Codes + OpxtoU16,
	OpI8toI32 = I8Codes + OpxtoI32,
	OpI8toU32 = I8Codes + OpxtoU32,
	OpI8toI64 = I8Codes + OpxtoI64,
	OpI8toU64 = I8Codes + OpxtoU64,
	OpI8toF32 = I8Codes + OpxtoF32,
	OpI8toF64 = I8Codes + OpxtoF64,

    OpI8ADD = I8Codes + OpxADD,
    OpI8SUB = I8Codes + OpxSUB,
    OpI8MUL = I8Codes + OpxMUL,
    OpI8DIV = I8Codes + OpxDIV,
    OpI8MOD = I8Codes + OpxMOD,

	OpI8INC = I8Codes + OpxINC,
	OpI8DEC = I8Codes + OpxDEC,

    OpI8LT = I8Codes + OpxLT,
    OpI8LE = I8Codes + OpxLE,

	OpI8GT = I8Codes + OpxGT,
	OpI8GE = I8Codes + OpxGE,


	// ==========================================================================
	// U8 Opcodes
	// ==========================================================================
	U8Codes,

	OpU8toI8 = U8Codes + OpxtoI8,
	OpU8toU8 = U8Codes + OpxtoU8,
	OpU8toI16 = U8Codes + OpxtoI16,
	OpU8toU16 = U8Codes + OpxtoU16,
	OpU8toI32 = U8Codes + OpxtoI32,
	OpU8toU32 = U8Codes + OpxtoU32,
	OpU8toI64 = U8Codes + OpxtoI64,
	OpU8toU64 = U8Codes + OpxtoU64,
	OpU8toF32 = U8Codes + OpxtoF32,
	OpU8toF64 = U8Codes + OpxtoF64,

	OpU8ADD = U8Codes + OpxADD,
	OpU8SUB = U8Codes + OpxSUB,
	OpU8MUL = U8Codes + OpxMUL,
	OpU8DIV = U8Codes + OpxDIV,
	OpU8MOD = U8Codes + OpxMOD,

	OpU8INC = U8Codes + OpxINC,
	OpU8DEC = U8Codes + OpxDEC,

	OpU8LT = U8Codes + OpxLT,
	OpU8LE = U8Codes + OpxLE,

	OpU8GT = U8Codes + OpxGT,
	OpU8GE = U8Codes + OpxGE,
	

	// ==========================================================================
	// 8 bit Opcodes
	// ==========================================================================
	Size8Codes,

    Op8EQ = Size8Codes + OpxEQ,
    Op8NOT = Size8Codes + OpxNOT,
    Op8NEQ = Size8Codes + OpxNEQ,

    Op8BOR = Size8Codes + OpxBOR,
    Op8BAND = Size8Codes + OpxBAND,
	Op8BXOR = Size8Codes + OpxBXOR,
    Op8BNOT = Size8Codes + OpxBNOT,
    Op8BLSHIFT = Size8Codes + OpxBLSHIFT,
    Op8BRSHIFT = Size8Codes + OpxBRSHIFT,


    Op8LOAD_CONSTANT = Size8Codes + OpxLOAD_CONSTANT,
    Op8LOAD_STATIC = Size8Codes + OpxLOAD_STATIC,
    Op8LOAD_MEMBER = Size8Codes + OpxLOAD_MEMBER,

    Op8SET_LOCAL = Size8Codes + OpxSET_LOCAL,
    Op8SET_STATIC = Size8Codes + OpxSET_STATIC,
    Op8SET_MEMBER = Size8Codes + OpxSET_MEMBER,

    Op8RETURN = Size8Codes + OpxRETURN,


	// ==========================================================================
	// I16 Opcodes
	// ==========================================================================
	I16Codes,

	OpI16toI8 = I16Codes + OpxtoI8,
	OpI16toU8 = I16Codes + OpxtoU8,
	OpI16toI16 = I16Codes + OpxtoI16,
	OpI16toU16 = I16Codes + OpxtoU16,
	OpI16toI32 = I16Codes + OpxtoI32,
	OpI16toU32 = I16Codes + OpxtoU32,
	OpI16toI64 = I16Codes + OpxtoI64,
	OpI16toU64 = I16Codes + OpxtoU64,
	OpI16toF32 = I16Codes + OpxtoF32,
	OpI16toF64 = I16Codes + OpxtoF64,

	OpI16ADD = I16Codes + OpxADD,
	OpI16SUB = I16Codes + OpxSUB,
	OpI16MUL = I16Codes + OpxMUL,
	OpI16DIV = I16Codes + OpxDIV,
	OpI16MOD = I16Codes + OpxMOD,

	OpI16INC = I16Codes + OpxINC,
	OpI16DEC = I16Codes + OpxDEC,

	OpI16LT = I16Codes + OpxLT,
	OpI16LE = I16Codes + OpxLE,

	OpI16GT = I16Codes + OpxGT,
	OpI16GE = I16Codes + OpxGE,


	// ==========================================================================
	// U16 Opcodes
	// ==========================================================================
	U16Codes,

	OpU16toI8 = U16Codes + OpxtoI8,
	OpU16toU8 = U16Codes + OpxtoU8,
	OpU16toI16 = U16Codes + OpxtoI16,
	OpU16toU16 = U16Codes + OpxtoU16,
	OpU16toI32 = U16Codes + OpxtoI32,
	OpU16toU32 = U16Codes + OpxtoU32,
	OpU16toI64 = U16Codes + OpxtoI64,
	OpU16toU64 = U16Codes + OpxtoU64,
	OpU16toF32 = U16Codes + OpxtoF32,
	OpU16toF64 = U16Codes + OpxtoF64,

	OpU16ADD = U16Codes + OpxADD,
	OpU16SUB = U16Codes + OpxSUB,
	OpU16MUL = U16Codes + OpxMUL,
	OpU16DIV = U16Codes + OpxDIV,
	OpU16MOD = U16Codes + OpxMOD,

	OpU16INC = U16Codes + OpxINC,
	OpU16DEC = U16Codes + OpxDEC,

	OpU16LT = U16Codes + OpxLT,
	OpU16LE = U16Codes + OpxLE,

	OpU16GT = U16Codes + OpxGT,
	OpU16GE = U16Codes + OpxGE,


	// ==========================================================================
	// 16 bit Opcodes
	// ==========================================================================
	Size16Codes,

	Op16EQ = Size16Codes + OpxEQ,
	Op16NOT = Size16Codes + OpxNOT,
	Op16NEQ = Size16Codes + OpxNEQ,

	Op16BOR = Size16Codes + OpxBOR,
	Op16BAND = Size16Codes + OpxBAND,
	Op16BXOR = Size16Codes + OpxBXOR,
	Op16BNOT = Size16Codes + OpxBNOT,
	Op16BLSHIFT = Size16Codes + OpxBLSHIFT,
	Op16BRSHIFT = Size16Codes + OpxBRSHIFT,


	Op16LOAD_CONSTANT = Size16Codes + OpxLOAD_CONSTANT,
	Op16LOAD_STATIC = Size16Codes + OpxLOAD_STATIC,
	Op16LOAD_MEMBER = Size16Codes + OpxLOAD_MEMBER,

	Op16SET_LOCAL = Size16Codes + OpxSET_LOCAL,
	Op16SET_STATIC = Size16Codes + OpxSET_STATIC,
	Op16SET_MEMBER = Size16Codes + OpxSET_MEMBER,

	Op16RETURN = Size16Codes + OpxRETURN,


	// ==========================================================================
	// I32 Opcodes
	// ==========================================================================
	I32Codes,

	OpI32toI8 = I32Codes + OpxtoI8,
	OpI32toU8 = I32Codes + OpxtoU8,
	OpI32toI16 = I32Codes + OpxtoI16,
	OpI32toU16 = I32Codes + OpxtoU16,
	OpI32toI32 = I32Codes + OpxtoI32,
	OpI32toU32 = I32Codes + OpxtoU32,
	OpI32toI64 = I32Codes + OpxtoI64,
	OpI32toU64 = I32Codes + OpxtoU64,
	OpI32toF32 = I32Codes + OpxtoF32,
	OpI32toF64 = I32Codes + OpxtoF64,

	OpI32ADD = I32Codes + OpxADD,
	OpI32SUB = I32Codes + OpxSUB,
	OpI32MUL = I32Codes + OpxMUL,
	OpI32DIV = I32Codes + OpxDIV,
	OpI32MOD = I32Codes + OpxMOD,

	OpI32INC = I32Codes + OpxINC,
	OpI32DEC = I32Codes + OpxDEC,

	OpI32LT = I32Codes + OpxLT,
	OpI32LE = I32Codes + OpxLE,

	OpI32GT = I32Codes + OpxGT,
	OpI32GE = I32Codes + OpxGE,


	// ==========================================================================
	// U32 Opcodes
	// ==========================================================================
	U32Codes,

	OpU32toI8 = U32Codes + OpxtoI8,
	OpU32toU8 = U32Codes + OpxtoU8,
	OpU32toI16 = U32Codes + OpxtoI16,
	OpU32toU16 = U32Codes + OpxtoU16,
	OpU32toI32 = U32Codes + OpxtoI32,
	OpU32toU32 = U32Codes + OpxtoU32,
	OpU32toI64 = U32Codes + OpxtoI64,
	OpU32toU64 = U32Codes + OpxtoU64,
	OpU32toF32 = U32Codes + OpxtoF32,
	OpU32toF64 = U32Codes + OpxtoF64,

	OpU32ADD = U32Codes + OpxADD,
	OpU32SUB = U32Codes + OpxSUB,
	OpU32MUL = U32Codes + OpxMUL,
	OpU32DIV = U32Codes + OpxDIV,
	OpU32MOD = U32Codes + OpxMOD,

	OpU32INC = U32Codes + OpxINC,
	OpU32DEC = U32Codes + OpxDEC,

	OpU32LT = U32Codes + OpxLT,
	OpU32LE = U32Codes + OpxLE,

	OpU32GT = U32Codes + OpxGT,
	OpU32GE = U32Codes + OpxGE,


	// ==========================================================================
	// F32 Opcodes
	// ==========================================================================
	F32Codes,

	OpF32toI8 = F32Codes + OpxtoI8,
	OpF32toU8 = F32Codes + OpxtoU8,
	OpF32toI16 = F32Codes + OpxtoI16,
	OpF32toU16 = F32Codes + OpxtoU16,
	OpF32toI32 = F32Codes + OpxtoI32,
	OpF32toU32 = F32Codes + OpxtoU32,
	OpF32toI64 = F32Codes + OpxtoI64,
	OpF32toU64 = F32Codes + OpxtoU64,
	OpF32toF32 = F32Codes + OpxtoF32,
	OpF32toF64 = F32Codes + OpxtoF64,

	OpF32ADD = F32Codes + OpxADD,
	OpF32SUB = F32Codes + OpxSUB,
	OpF32MUL = F32Codes + OpxMUL,
	OpF32DIV = F32Codes + OpxDIV,
	OpF32MOD = F32Codes + OpxMOD,

	OpF32INC = F32Codes + OpxINC,
	OpF32DEC = F32Codes + OpxDEC,

	OpF32LT = F32Codes + OpxLT,
	OpF32LE = F32Codes + OpxLE,

	OpF32GT = F32Codes + OpxGT,
	OpF32GE = F32Codes + OpxGE,


	// ==========================================================================
	// 32 bit Opcodes
	// ==========================================================================
	Size32Codes,

	Op32EQ = Size32Codes + OpxEQ,
	Op32NOT = Size32Codes + OpxNOT,
	Op32NEQ = Size32Codes + OpxNEQ,

	Op32BOR = Size32Codes + OpxBOR,
	Op32BAND = Size32Codes + OpxBAND,
	Op32BXOR = Size32Codes + OpxBXOR,
	Op32BNOT = Size32Codes + OpxBNOT,
	Op32BLSHIFT = Size32Codes + OpxBLSHIFT,
	Op32BRSHIFT = Size32Codes + OpxBRSHIFT,


	Op32LOAD_CONSTANT = Size32Codes + OpxLOAD_CONSTANT,
	Op32LOAD_STATIC = Size32Codes + OpxLOAD_STATIC,
	Op32LOAD_MEMBER = Size32Codes + OpxLOAD_MEMBER,

	Op32SET_LOCAL = Size32Codes + OpxSET_LOCAL,
	Op32SET_STATIC = Size32Codes + OpxSET_STATIC,
	Op32SET_MEMBER = Size32Codes + OpxSET_MEMBER,

	Op32RETURN = Size32Codes + OpxRETURN,


	// ==========================================================================
	// I64 Opcodes
	// ==========================================================================
	I64Codes,

	OpI64toI8 = I64Codes + OpxtoI8,
	OpI64toU8 = I64Codes + OpxtoU8,
	OpI64toI16 = I64Codes + OpxtoI16,
	OpI64toU16 = I64Codes + OpxtoU16,
	OpI64toI32 = I64Codes + OpxtoI32,
	OpI64toU32 = I64Codes + OpxtoU32,
	OpI64toI64 = I64Codes + OpxtoI64,
	OpI64toU64 = I64Codes + OpxtoU64,
	OpI64toF32 = I64Codes + OpxtoF32,
	OpI64toF64 = I64Codes + OpxtoF64,

	OpI64ADD = I64Codes + OpxADD,
	OpI64SUB = I64Codes + OpxSUB,
	OpI64MUL = I64Codes + OpxMUL,
	OpI64DIV = I64Codes + OpxDIV,
	OpI64MOD = I64Codes + OpxMOD,

	OpI64INC = I64Codes + OpxINC,
	OpI64DEC = I64Codes + OpxDEC,

	OpI64LT = I64Codes + OpxLT,
	OpI64LE = I64Codes + OpxLE,

	OpI64GT = I64Codes + OpxGT,
	OpI64GE = I64Codes + OpxGE,


	// ==========================================================================
	// U64 Opcodes
	// ==========================================================================
	U64Codes,

	OpU64toI8 = U64Codes + OpxtoI8,
	OpU64toU8 = U64Codes + OpxtoU8,
	OpU64toI16 = U64Codes + OpxtoI16,
	OpU64toU16 = U64Codes + OpxtoU16,
	OpU64toI32 = U64Codes + OpxtoI32,
	OpU64toU32 = U64Codes + OpxtoU32,
	OpU64toI64 = U64Codes + OpxtoI64,
	OpU64toU64 = U64Codes + OpxtoU64,
	OpU64toF32 = U64Codes + OpxtoF32,
	OpU64toF64 = U64Codes + OpxtoF64,

	OpU64ADD = U64Codes + OpxADD,
	OpU64SUB = U64Codes + OpxSUB,
	OpU64MUL = U64Codes + OpxMUL,
	OpU64DIV = U64Codes + OpxDIV,
	OpU64MOD = U64Codes + OpxMOD,

	OpU64INC = U64Codes + OpxINC,
	OpU64DEC = U64Codes + OpxDEC,

	OpU64LT = U64Codes + OpxLT,
	OpU64LE = U64Codes + OpxLE,

	OpU64GT = U64Codes + OpxGT,
	OpU64GE = U64Codes + OpxGE,


	// ==========================================================================
	// F64 Opcodes
	// ==========================================================================
	F64Codes,

	OpF64toI8 = F64Codes + OpxtoI8,
	OpF64toU8 = F64Codes + OpxtoU8,
	OpF64toI16 = F64Codes + OpxtoI16,
	OpF64toU16 = F64Codes + OpxtoU16,
	OpF64toI32 = F64Codes + OpxtoI32,
	OpF64toU32 = F64Codes + OpxtoU32,
	OpF64toI64 = F64Codes + OpxtoI64,
	OpF64toU64 = F64Codes + OpxtoU64,
	OpF64toF32 = F64Codes + OpxtoF32,
	OpF64toF64 = F64Codes + OpxtoF64,

	OpF64ADD = F64Codes + OpxADD,
	OpF64SUB = F64Codes + OpxSUB,
	OpF64MUL = F64Codes + OpxMUL,
	OpF64DIV = F64Codes + OpxDIV,
	OpF64MOD = F64Codes + OpxMOD,

	OpF64INC = F64Codes + OpxINC,
	OpF64DEC = F64Codes + OpxDEC,

	OpF64LT = F64Codes + OpxLT,
	OpF64LE = F64Codes + OpxLE,

	OpF64GT = F64Codes + OpxGT,
	OpF64GE = F64Codes + OpxGE,


	// ==========================================================================
	// 64 bit Opcodes
	// ==========================================================================
	Size64Codes,

	Op64EQ = Size64Codes + OpxEQ,
	Op64NOT = Size64Codes + OpxNOT,
	Op64NEQ = Size64Codes + OpxNEQ,

	Op64BOR = Size64Codes + OpxBOR,
	Op64BAND = Size64Codes + OpxBAND,
	Op64BXOR = Size64Codes + OpxBXOR,
	Op64BNOT = Size64Codes + OpxBNOT,
	Op64BLSHIFT = Size64Codes + OpxBLSHIFT,
	Op64BRSHIFT = Size64Codes + OpxBRSHIFT,


	Op64LOAD_CONSTANT = Size64Codes + OpxLOAD_CONSTANT,
	Op64LOAD_STATIC = Size64Codes + OpxLOAD_STATIC,
	Op64LOAD_MEMBER = Size64Codes + OpxLOAD_MEMBER,

	Op64SET_LOCAL = Size64Codes + OpxSET_LOCAL,
	Op64SET_STATIC = Size64Codes + OpxSET_STATIC,
	Op64SET_MEMBER = Size64Codes + OpxSET_MEMBER,

	Op64RETURN = Size64Codes + OpxRETURN,
	
};


class Op
{
public:
	Op()
		: m_code(OpEmptyCode), m_a(0), m_b(0), m_c(0)
	{}

	Op(OpCode code)
		: m_code(code), m_a(0), m_b(0), m_c(0)
	{}

	Op(OpCode code, ABC a)
		: m_code(code), m_a(a), m_b(0), m_c(0)
	{}

	Op(OpCode code, ABC a, ABC b, ABC c)
		: m_code(code), m_a(a), m_b(b), m_c(c)
	{}

	Op(OpCode code, ABC a, Bx bx)
		: m_code(code), m_a(a), m_bx(bx)
	{}

	Op(OpCode code, ABC a, SBx sbx)
		: m_code(code), m_a(a), m_sbx(sbx)
	{}

	OpCode code() const { return m_code; }

	ABC a() const { return m_a; }
	ABC b() const { return m_b; }
	ABC c() const { return m_c; }

	Bx bx() const { return m_bx; }
	SBx sbx() const { return m_sbx; }

	void sbx(SBx bx) { m_sbx = bx; }

private:
	OpCode m_code;
	ABC m_a;

	union
	{
		struct
		{
			ABC m_b;
			ABC m_c;
		};

		Bx m_bx;
		SBx m_sbx;
	};

	friend class ServerFunctionCompiler;
};


static_assert(sizeof(Op) == 8, "Weird size of Op");


/*
 *
 */
class OpCodes
{
private:
	struct Header
	{
		ABC m_stack_size;
		u8* m_constants;
		Function* m_function;
	};

	static const u32 s_header_size = (sizeof(Header) + (sizeof(Header) % sizeof(Op))) / sizeof(Op);

public:
	OpCodes()
		: m_code(nullptr)
	{}

	OpCodes(Function* function, u32 stack_size, const std::vector<Op>& code, const std::vector<u8>& constants)
	{
		const u32 SIZE = code.size() + s_header_size + 1;

		m_code = new Op[SIZE];
		m_code[SIZE - 1] = Op();

		Header* header = (Header*)m_code;
		header->m_stack_size = stack_size;
		header->m_function = function;
		header->m_constants = new u8[constants.size()];

		Op* op = start_operation();
		for (auto it = code.begin(); it != code.end(); ++it)
		{
			*op = *it;
			++op;
		}

		u8* constant = this->constants();
		for (auto it = constants.begin(); it != constants.end(); ++it)
		{
			*constant = *it;
			++constant;
		}
	}

	bool is_compiled() const { return m_code != nullptr; }
	
	Function* function() const { return ((Header*)m_code)->m_function; }
	u32 stack_size() const { return ((Header*)m_code)->m_stack_size; }
	u8* constants() const { return ((Header*)m_code)->m_constants; }

	Op* start_operation() { return m_code + s_header_size; }

private:
	Op* m_code;
};


#endif // OPCODES_H