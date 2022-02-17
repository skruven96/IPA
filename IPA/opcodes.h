#ifndef OPCODES_H
#define OPCODES_H
#include "common.h"


class Function;
class ServerFunctionCompiler;


enum class OpCode : u8
{
	OpNop,

	OpCall,
	OpJump8,
	OpJump16,

	OpReturn,
	OpReturnVoid,

	OpPushConst32, OpPushConst64, OpPushRef, OpPushNull,
	OpPop32, OpPop64,

	OpLoadGlobalS8,  OpLoadGlobalS16, OpLoadGlobalU8,  OpLoadGlobalU16,
	OpLoadGlobalI32, OpLoadGlobalI64, OpLoadGlobalF32, OpLoadGlobalF64, OpLoadGlobalRef, 
	OpLoadLocalS8,   OpLoadLocalS16,  OpLoadLocalU8,   OpLoadLocalU16,
	OpLoadLocalI32,  OpLoadLocalI64,  OpLoadLocalF32,  OpLoadLocalF64,  OpLoadLocalRef,

	OpLoadS8,  OpLoadS16, OpLoadU8, OpLoadU16, OpLoadI32, OpLoadI64,
	OpLoadF32, OpLoadF64,

	OpStoreI8, OpStoreI16, OpStoreI32, OpStoreI64, OpStoreF32, OpStoreF64,

	OpArrayLoadS8,  OpArrayLoadS16,  OpArrayLoadI32,  OpArrayLoadI64,
	OpArrayLoadU8,  OpArrayLoadU16,  OpArrayLoadF32,  OpArrayLoadF64,
	OpArrayStoreI8, OpArrayStoreI16, OpArrayStoreI32, OpArrayStoreI64, OpArrayStoreF32, OpArrayStoreF64,

	OpS64toS32, OpS32toS64,
	OpU64toU32, OpU32toU64,
	OpF64toF32, OpF32toF64,

	OpAddI32, OpSubI32,
	OpMulS32, OpDivS32, OpModS32,
	OpMulU32, OpDivU32, OpModU32,
	OpMulS64, OpDivS64, OpModS64,
	OpMulU64, OpDivU64, OpModU64,
	OpAddF32, OpSubF32, OpDivF32, OpMulF32,
	OpAddF64, OpSubF64, OpDivF64, OpMulF64,

	OpAnd32, OpOr32, OpXor32, OpNot32,
	OpAnd64, OpOr64, OpXor64, OpNot64,

	OpLtS32, OpGtS32, OpLteS32, OpGteS32, OpLtU32, OpGtU32, OpLteU32, OpGteU32, OpEq32, OpNeq32,
	OpLtS64, OpGtS64, OpLteS64, OpGteS64, OpLtU64, OpGtU64, OpLteU64, OpGteU64, OpEq64, OpNeq64,
	OpLtF32, OpGtF32, OpLteF32, OpGteF32,
	OpLtF64, OpGtF64, OpLteF64, OpGteF64,
};


#endif // OPCODES_H