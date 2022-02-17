#include "runtime.h"
#include "compiler.h"

#include <assert.h>


void Runtime::initialize()
{
	
}


Runtime& Runtime::start_call(ast::Function* function) {
	m_call = function;
	m_current_argument_index = 0;

	return *this;
}


Runtime& Runtime::arg(u8 value) {
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::U8() && "Argument is off wrong type. ");

	*m_stack_ptr++ = value;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(i8 value) {
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::S8() && "Argument is off wrong type. ");

	*m_stack_ptr = value;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(u16 value)
{
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::U16() && "Argument is off wrong type. ");

	*m_stack_ptr++ = value;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(i16 value)
{
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::S16() && "Argument is off wrong type. ");

	*m_stack_ptr++ = value;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(u32 value)
{
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::U32() && "Argument is off wrong type. ");

	*m_stack_ptr++ = value;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(i32 value)
{
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::S32() && "Argument is off wrong type. ");

	*m_stack_ptr++ = value;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(u64 value)
{
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::U64() && "Argument is off wrong type. ");

	m_stack_ptr = (i32*)(((uintptr_t)m_stack_ptr + (alignof(u64) - 1)) & (alignof(u64) - 1));
	*((u64*)m_stack_ptr) = value;
	m_stack_ptr += 2;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(i64 value)
{
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::S64() && "Argument is off wrong type. ");

	m_stack_ptr = (i32*)(((uintptr_t)m_stack_ptr + (alignof(u64) - 1)) & (alignof(u64) - 1));
	*((i64*)m_stack_ptr) = value;
	m_stack_ptr += 2;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(f32 value)
{
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::F32() && "Argument is off wrong type. ");

	*((f32*)m_stack_ptr) = value;
	m_stack_ptr += 1;
	++m_current_argument_index;

	return *this;
}


Runtime& Runtime::arg(f64 value)
{
	assert(m_call && m_current_argument_index != m_call->arguments_count && "To many arguments given. ");
	// assert(m_call->arguments[m_current_argument_index]->type == ast::Type::F64() && "Argument is off wrong type. ");

	m_stack_ptr = (i32*)(((uintptr_t)m_stack_ptr + (alignof(f64) - 1)) & (alignof(f64) - 1));
	*((f64*)m_stack_ptr) = value;
	m_stack_ptr += 2;
	++m_current_argument_index;

	return *this;
}


void Runtime::call(void* return_value, ast::Type* return_type)
{
	assert(m_call->arguments_count == m_current_argument_index && "To few arguments given. ");
	assert(return_type == m_call->type->return_type && "Return type dosen't match. ");

	m_call = nullptr;

	// call(return_value, code);
}

/*
void Runtime::call(void* return_value, OpCodes code)
{
	u8* constants = code.constants();
	Op* op = code.start_operation();

	while (true)
	{
		switch (op->code())
		{
		case OpRETURN_VOID:
			return;

		case OpIF:
			if (*((bool*)(m_stack + op->a())))
				++op;
			break;
		case OpJUMP:
			op += op->sbx();
			break;

		case OpCALLLocal:
			call(m_stack + op->b(), *(OpCodes*)(m_stack + op->a()));
			break;
		case OpCALLStatic:
			break;
		case OpCALLVirtual:
			break;

		// =================================================================================
		// i8 instructions
		// =================================================================================
		case OpI8toI8:
			*((i8*)(m_stack + op->b()))  = *(i8*)(m_stack + op->a());
			break;
		case OpI8toU8:
			*((u8*)(m_stack + op->b()))  = *(i8*)(m_stack + op->a());
			break;
		case OpI8toI16:
			*((i16*)(m_stack + op->b())) = *(i8*)(m_stack + op->a());
			break;
		case OpI8toU16:
			*((u16*)(m_stack + op->b())) = *(i8*)(m_stack + op->a());
			break;
		case OpI8toI32:
			*((i32*)(m_stack + op->b())) = *(i8*)(m_stack + op->a());
			break;
		case OpI8toU32:
			*((u32*)(m_stack + op->b())) = *(i8*)(m_stack + op->a());
			break;
		case OpI8toI64:
			*((i64*)(m_stack + op->b())) = *(i8*)(m_stack + op->a());
			break;
		case OpI8toU64:
			*((u64*)(m_stack + op->b())) = *(i8*)(m_stack + op->a());
			break;
		case OpI8toF32:
			*((f32*)(m_stack + op->b())) = *(i8*)(m_stack + op->a());
			break;
		case OpI8toF64:
			*((f64*)(m_stack + op->b())) = *(i8*)(m_stack + op->a());
			break;

		case OpI8ADD:
			*((i8*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) + *((i8*)(m_stack + op->b()));
			break;
		case OpI8SUB:
			*((i8*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) - *((i8*)(m_stack + op->b()));
			break;
		case OpI8MUL:
			*((i8*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) * *((i8*)(m_stack + op->b()));
			break;
		case OpI8DIV:
			*((i8*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) / *((i8*)(m_stack + op->b()));
			break;
		case OpI8MOD:
			*((i8*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) % *((i8*)(m_stack + op->b()));
			break;

		case OpI8INC:
			*((i8*)(m_stack + op->b())) = (*((i8*)(m_stack + op->a())))++;
			break;
		case OpI8DEC:
			*((i8*)(m_stack + op->b())) = (*((i8*)(m_stack + op->a())))++;
			break;

		case OpI8LT:
			*((bool*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) < *((i8*)(m_stack + op->b()));
			break;
		case OpI8LE:
			*((bool*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) <= *((i8*)(m_stack + op->b()));
			break;
		case OpI8GT:
			*((bool*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) > *((i8*)(m_stack + op->b()));
			break;
		case OpI8GE:
			*((bool*)(m_stack + op->c())) = *((i8*)(m_stack + op->a())) >= *((i8*)(m_stack + op->b()));
			break;

		// =================================================================================
		// u8 instructions
		// =================================================================================
		case OpU8toI8:
			*(i8*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toU8:
			*(u8*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toI16:
			*(i16*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toU16:
			*(u16*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toI32:
			*(i32*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toU32:
			*(u32*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toI64:
			*(i64*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toU64:
			*(u64*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toF32:
			*(f32*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;
		case OpU8toF64:
			*(f64*)(m_stack + op->b()) = *(u8*)(m_stack + op->a());
			break;

		case OpU8ADD:
			*(u8*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) + *(u8*)(m_stack + op->b());
			break;
		case OpU8SUB:
			*(u8*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) - *(u8*)(m_stack + op->b());
			break;
		case OpU8MUL:
			*(u8*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) * *(u8*)(m_stack + op->b());
			break;
		case OpU8DIV:
			*(u8*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) / *(u8*)(m_stack + op->b());
			break;
		case OpU8MOD:
			*(u8*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) % *(u8*)(m_stack + op->b());
			break;

		case OpU8INC:
			*(u8*)(m_stack + op->b()) = (*(u8*)(m_stack + op->a()))++;
			break;
		case OpU8DEC:
			*(u8*)(m_stack + op->b()) = (*(u8*)(m_stack + op->a()))--;
			break;

		case OpU8LT:
			*(bool*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) < *(u8*)(m_stack + op->b());
			break;
		case OpU8LE:
			*(bool*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) <= *(u8*)(m_stack + op->b());
			break;
		case OpU8GT:
			*(bool*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) > *(u8*)(m_stack + op->b());
			break;
		case OpU8GE:
			*(bool*)(m_stack + op->c()) = *(u8*)(m_stack + op->a()) >= *(u8*)(m_stack + op->b());
			break;

		// =================================================================================
		// 8 bit instructions
		// =================================================================================
		case Op8EQ:
			*((bool*)(m_stack + op->c())) = *(u8*)(m_stack + op->a()) == *(u8*)(m_stack + op->b());
			break;
		case Op8NOT:
			*((bool*)(m_stack + op->c())) = !*(u8*)(m_stack + op->a());
			break;
		case Op8NEQ:
			*((bool*)(m_stack + op->c())) = *(u8*)(m_stack + op->a()) != *(u8*)(m_stack + op->b());
			break;

		case Op8BOR:
			*((u8*)(m_stack + op->c())) = *(u8*)(m_stack + op->a()) | *(u8*)(m_stack + op->b());
			break;
		case Op8BAND:
			*((u8*)(m_stack + op->c())) = *(u8*)(m_stack + op->a()) & *(u8*)(m_stack + op->b());
			break;
		case Op8BXOR:
			*((u8*)(m_stack + op->c())) = *(u8*)(m_stack + op->a()) ^ *(u8*)(m_stack + op->b());
			break;
		case Op8BNOT:
			*((u8*)(m_stack + op->c())) = ~*(u8*)(m_stack + op->a());
			break;

		case Op8BLSHIFT:
			*((u8*)(m_stack + op->c())) = *(u8*)(m_stack + op->a()) << *(u8*)(m_stack + op->b());
			break;
		case Op8BRSHIFT:
			*((u8*)(m_stack + op->c())) = *(u8*)(m_stack + op->a()) >> *(u8*)(m_stack + op->b());
			break;

		case Op8LOAD_CONSTANT:
			*(u8*)(m_stack + op->c()) = *(u8*)(constants + op->a());
			break;
		case Op8LOAD_STATIC:
			*(u8*)(m_stack + op->a()) = *(u8*)(m_heap + op->bx());
			break;
		case Op8LOAD_MEMBER:
			*(u8*)(m_stack + op->c()) = ((u8*)(m_heap + op->a()))[op->b()];
			break;

		case Op8SET_LOCAL:
			*(u8*)(m_stack + op->c()) = *(u8*)(m_stack + op->a());
			break;
		case Op8SET_STATIC:
			*(u8*)(m_heap + op->bx()) = *(u8*)(m_stack + op->a());
			break;
		case Op8SET_MEMBER:
			((u8*)(m_heap + op->b()))[op->c()] = *(u8*)(m_stack + op->a());
			break;

		case Op8RETURN:
			*(u8*)return_value = *(u8*)(m_stack + op->a());
			return;

		// =================================================================================
		// i16 instructions
		// =================================================================================
		case OpI16toI8:
			*(i8*)(m_stack + op->b()) = static_cast<i8>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toU8:
			*(u8*)(m_stack + op->b()) = static_cast<u8>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toI16:
			*(i16*)(m_stack + op->b()) = static_cast<i16>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toU16:
			*(u16*)(m_stack + op->b()) = static_cast<u16>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toI32:
			*(i32*)(m_stack + op->b()) = static_cast<i32>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toU32:
			*(u32*)(m_stack + op->b()) = static_cast<u32>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toI64:
			*(i64*)(m_stack + op->b()) = static_cast<i64>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toU64:
			*(u64*)(m_stack + op->b()) = static_cast<u64>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toF32:
			*(f32*)(m_stack + op->b()) = static_cast<f32>(*(i16*)(m_stack + op->a()));
			break;
		case OpI16toF64:
			*(f64*)(m_stack + op->b()) = static_cast<f64>(*(i16*)(m_stack + op->a()));
			break;

		case OpI16ADD:
			*(i16*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) + *(i16*)(m_stack + op->b());
			break;
		case OpI16SUB:
			*(i16*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) - *(i16*)(m_stack + op->b());
			break;
		case OpI16MUL:
			*(i16*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) * *(i16*)(m_stack + op->b());
			break;
		case OpI16DIV:
			*(i16*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) / *(i16*)(m_stack + op->b());
			break;
		case OpI16MOD:
			*(i16*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) % *(i16*)(m_stack + op->b());
			break;

		case OpI16INC:
			*(i16*)(m_stack + op->b()) = (*(i16*)(m_stack + op->a()))++;
			break;
		case OpI16DEC:
			*(i16*)(m_stack + op->b()) = (*(i16*)(m_stack + op->a()))--;
			break;

		case OpI16LT:
			*(bool*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) < *(i16*)(m_stack + op->b());
			break;
		case OpI16LE:
			*(bool*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) <= *(i16*)(m_stack + op->b());
			break;
		case OpI16GT:
			*(bool*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) > *(i16*)(m_stack + op->b());
			break;
		case OpI16GE:
			*(bool*)(m_stack + op->c()) = *(i16*)(m_stack + op->a()) >= *(i16*)(m_stack + op->b());
			break;

		// =================================================================================
		// u16 instructions
		// =================================================================================
		case OpU16toI8:
			*(i8*)(m_stack + op->b()) = static_cast<i8>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toU8:
			*(u8*)(m_stack + op->b()) = static_cast<u8>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toI16:
			*(i16*)(m_stack + op->b()) = static_cast<i16>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toU16:
			*(u16*)(m_stack + op->b()) = static_cast<u16>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toI32:
			*(i32*)(m_stack + op->b()) = static_cast<i32>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toU32:
			*(u32*)(m_stack + op->b()) = static_cast<u32>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toI64:
			*(i64*)(m_stack + op->b()) = static_cast<i64>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toU64:
			*(u64*)(m_stack + op->b()) = static_cast<u64>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toF32:
			*(f32*)(m_stack + op->b()) = static_cast<f32>(*(u16*)(m_stack + op->a()));
			break;
		case OpU16toF64:
			*(f64*)(m_stack + op->b()) = static_cast<f64>(*(u16*)(m_stack + op->a()));
			break;

		case OpU16ADD:
			*(u16*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) + *(u16*)(m_stack + op->b());
			break;
		case OpU16SUB:
			*(u16*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) - *(u16*)(m_stack + op->b());
			break;
		case OpU16MUL:
			*(u16*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) * *(u16*)(m_stack + op->b());
			break;
		case OpU16DIV:
			*(u16*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) / *(u16*)(m_stack + op->b());
			break;
		case OpU16MOD:
			*(u16*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) % *(u16*)(m_stack + op->b());
			break;

		case OpU16INC:
			*(u16*)(m_stack + op->b()) = (*(u16*)(m_stack + op->a()))++;
			break;
		case OpU16DEC:
			*(u16*)(m_stack + op->b()) = (*(u16*)(m_stack + op->a()))--;
			break;

		case OpU16LT:
			*(bool*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) < *(u16*)(m_stack + op->b());
			break;
		case OpU16LE:
			*(bool*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) <= *(u16*)(m_stack + op->b());
			break;
		case OpU16GT:
			*(bool*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) > *(u16*)(m_stack + op->b());
			break;
		case OpU16GE:
			*(bool*)(m_stack + op->c()) = *(u16*)(m_stack + op->a()) >= *(u16*)(m_stack + op->b());
			break;

		// =================================================================================
		// 16 bit instructions
		// =================================================================================
		case Op16EQ:
			*((bool*)(m_stack + op->c())) = *(u16*)(m_stack + op->a()) == *(u16*)(m_stack + op->b());
			break;
		case Op16NOT:
			*((bool*)(m_stack + op->c())) = !*(u16*)(m_stack + op->a());
			break;
		case Op16NEQ:
			*((bool*)(m_stack + op->c())) = *(u16*)(m_stack + op->a()) != *(u16*)(m_stack + op->b());
			break;

		case Op16BOR:
			*((u16*)(m_stack + op->c())) = *(u16*)(m_stack + op->a()) | *(u16*)(m_stack + op->b());
			break;
		case Op16BAND:
			*((u16*)(m_stack + op->c())) = *(u16*)(m_stack + op->a()) & *(u16*)(m_stack + op->b());
			break;
		case Op16BXOR:
			*((u16*)(m_stack + op->c())) = *(u16*)(m_stack + op->a()) ^ *(u16*)(m_stack + op->b());
			break;
		case Op16BNOT:
			*((u16*)(m_stack + op->c())) = ~*(u16*)(m_stack + op->a());
			break;

		case Op16BLSHIFT:
			*((u16*)(m_stack + op->c())) = *(u16*)(m_stack + op->a()) << *(u16*)(m_stack + op->b());
			break;
		case Op16BRSHIFT:
			*((u16*)(m_stack + op->c())) = *(u16*)(m_stack + op->a()) >> *(u16*)(m_stack + op->b());
			break;

		case Op16LOAD_CONSTANT:
			*(u16*)(m_stack + op->c()) = *(u16*)(constants + op->a());
			break;
		case Op16LOAD_STATIC:
			*(u16*)(m_stack + op->a()) = *(u16*)(m_heap + op->bx());
			break;
		case Op16LOAD_MEMBER:
			*(u16*)(m_stack + op->c()) = ((u16*)(m_heap + op->a()))[op->b()];
			break;

		case Op16SET_LOCAL:
			*(u16*)(m_stack + op->c()) = *(u16*)(m_stack + op->a());
			break;
		case Op16SET_STATIC:
			*(u16*)(m_heap + op->bx()) = *(u16*)(m_stack + op->a());
			break;
		case Op16SET_MEMBER:
			((u16*)(m_heap + op->b()))[op->c()] = *(u16*)(m_stack + op->a());
			break;

		case Op16RETURN:
			*(u16*)return_value = *(u16*)(m_stack + op->a());
			return;


		// =================================================================================
		// i32 instructions
		// =================================================================================
		case OpI32toI8:
			*(i8*)(m_stack + op->b()) = static_cast<i8>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toU8:
			*(u8*)(m_stack + op->b()) = static_cast<u8>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toI16:
			*(i16*)(m_stack + op->b()) = static_cast<i16>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toU16:
			*(u16*)(m_stack + op->b()) = static_cast<u16>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toI32:
			*(i32*)(m_stack + op->b()) = static_cast<i32>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toU32:
			*(u32*)(m_stack + op->b()) = static_cast<u32>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toI64:
			*(i64*)(m_stack + op->b()) = static_cast<i64>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toU64:
			*(u64*)(m_stack + op->b()) = static_cast<u64>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toF32:
			*(f32*)(m_stack + op->b()) = static_cast<f32>(*(i32*)(m_stack + op->a()));
			break;
		case OpI32toF64:
			*(f64*)(m_stack + op->b()) = static_cast<f64>(*(i32*)(m_stack + op->a()));
			break;

		case OpI32ADD:
			*(i32*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) + *(i32*)(m_stack + op->b());
			break;
		case OpI32SUB:
			*(i32*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) - *(i32*)(m_stack + op->b());
			break;
		case OpI32MUL:
			*(i32*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) * *(i32*)(m_stack + op->b());
			break;
		case OpI32DIV:
			*(i32*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) / *(i32*)(m_stack + op->b());
			break;
		case OpI32MOD:
			*(i32*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) % *(i32*)(m_stack + op->b());
			break;

		case OpI32INC:
			*(i32*)(m_stack + op->b()) = (*(i32*)(m_stack + op->a()))++;
			break;
		case OpI32DEC:
			*(i32*)(m_stack + op->b()) = (*(i32*)(m_stack + op->a()))--;
			break;

		case OpI32LT:
			*(bool*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) < *(i32*)(m_stack + op->b());
			break;
		case OpI32LE:
			*(bool*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) <= *(i32*)(m_stack + op->b());
			break;
		case OpI32GT:
			*(bool*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) > *(i32*)(m_stack + op->b());
			break;
		case OpI32GE:
			*(bool*)(m_stack + op->c()) = *(i32*)(m_stack + op->a()) >= *(i32*)(m_stack + op->b());
			break;


		// =================================================================================
		// u32 instructions
		// =================================================================================
		case OpU32toI8:
			*(i8*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toU8:
			*(u8*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toI16:
			*(i16*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toU16:
			*(u16*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toI32:
			*(i32*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toU32:
			*(u32*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toI64:
			*(i64*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toU64:
			*(u64*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toF32:
			*(f32*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;
		case OpU32toF64:
			*(f64*)(m_stack + op->b()) = *(u32*)(m_stack + op->a());
			break;

		case OpU32ADD:
			*(u32*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) + *(u32*)(m_stack + op->b());
			break;
		case OpU32SUB:
			*(u32*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) - *(u32*)(m_stack + op->b());
			break;
		case OpU32MUL:
			*(u32*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) * *(u32*)(m_stack + op->b());
			break;
		case OpU32DIV:
			*(u32*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) / *(u32*)(m_stack + op->b());
			break;
		case OpU32MOD:
			*(u32*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) % *(u32*)(m_stack + op->b());
			break;

		case OpU32INC:
			*(u32*)(m_stack + op->b()) = (*(u32*)(m_stack + op->a()))++;
			break;
		case OpU32DEC:
			*(u32*)(m_stack + op->b()) = (*(u32*)(m_stack + op->a()))--;
			break;

		case OpU32LT:
			*(bool*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) < *(u32*)(m_stack + op->b());
			break;
		case OpU32LE:
			*(bool*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) <= *(u32*)(m_stack + op->b());
			break;
		case OpU32GT:
			*(bool*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) > *(u32*)(m_stack + op->b());
			break;
		case OpU32GE:
			*(bool*)(m_stack + op->c()) = *(u32*)(m_stack + op->a()) >= *(u32*)(m_stack + op->b());
			break;


		// =================================================================================
		// f32 instructions
		// =================================================================================
		case OpF32toI8:
			*(i8*)(m_stack + op->b()) = static_cast<i8>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toU8:
			*(u8*)(m_stack + op->b()) = static_cast<u8>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toI16:
			*(i16*)(m_stack + op->b()) = static_cast<i16>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toU16:
			*(u16*)(m_stack + op->b()) = static_cast<u16>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toI32:
			*(i32*)(m_stack + op->b()) = static_cast<i32>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toU32:
			*(u32*)(m_stack + op->b()) = static_cast<u32>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toI64:
			*(i64*)(m_stack + op->b()) = static_cast<i64>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toU64:
			*(u64*)(m_stack + op->b()) = static_cast<u64>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toF32:
			*(f32*)(m_stack + op->b()) = static_cast<f32>(*(f32*)(m_stack + op->a()));
			break;
		case OpF32toF64:
			*(f64*)(m_stack + op->b()) = static_cast<f64>(*(f32*)(m_stack + op->a()));
			break;

		case OpF32ADD:
			*(f32*)(m_stack + op->c()) = *(f32*)(m_stack + op->a()) + *(f32*)(m_stack + op->b());
			break;
		case OpF32SUB:
			*(f32*)(m_stack + op->c()) = *(f32*)(m_stack + op->a()) - *(f32*)(m_stack + op->b());
			break;
		case OpF32MUL:
			*(f32*)(m_stack + op->c()) = *(f32*)(m_stack + op->a()) * *(f32*)(m_stack + op->b());
			break;
		case OpF32DIV:
			*(f32*)(m_stack + op->c()) = *(f32*)(m_stack + op->a()) / *(f32*)(m_stack + op->b());
			break;
		case OpF32MOD:
			assert(false && "Can't use modulus on floats. ");
			break;

		case OpF32INC:
			*(f32*)(m_stack + op->b()) = (*(f32*)(m_stack + op->a()))++;
			break;
		case OpF32DEC:
			*(f32*)(m_stack + op->b()) = (*(f32*)(m_stack + op->a()))--;
			break;

		case OpF32LT:
			*(bool*)(m_stack + op->c()) = *(f32*)(m_stack + op->a()) < *(f32*)(m_stack + op->b());
			break;
		case OpF32LE:
			*(bool*)(m_stack + op->c()) = *(f32*)(m_stack + op->a()) <= *(f32*)(m_stack + op->b());
			break;
		case OpF32GT:
			*(bool*)(m_stack + op->c()) = *(f32*)(m_stack + op->a()) > *(f32*)(m_stack + op->b());
			break;
		case OpF32GE:
			*(bool*)(m_stack + op->c()) = *(f32*)(m_stack + op->a()) >= *(f32*)(m_stack + op->b());
			break;


		// =================================================================================
		// 32 bit instructions
		// =================================================================================
		case Op32EQ:
			*((bool*)(m_stack + op->c())) = *(u32*)(m_stack + op->a()) == *(u32*)(m_stack + op->b());
			break;
		case Op32NOT:
			*((bool*)(m_stack + op->c())) = !*(u32*)(m_stack + op->a());
			break;
		case Op32NEQ:
			*((bool*)(m_stack + op->c())) = *(u32*)(m_stack + op->a()) != *(u32*)(m_stack + op->b());
			break;

		case Op32BOR:
			*((u32*)(m_stack + op->c())) = *(u32*)(m_stack + op->a()) | *(u32*)(m_stack + op->b());
			break;
		case Op32BAND:
			*((u32*)(m_stack + op->c())) = *(u32*)(m_stack + op->a()) & *(u32*)(m_stack + op->b());
			break;
		case Op32BXOR:
			*((u32*)(m_stack + op->c())) = *(u32*)(m_stack + op->a()) ^ *(u32*)(m_stack + op->b());
			break;
		case Op32BNOT:
			*((u32*)(m_stack + op->c())) = ~*(u32*)(m_stack + op->a());
			break;

		case Op32BLSHIFT:
			*((u32*)(m_stack + op->c())) = *(u32*)(m_stack + op->a()) << *(u32*)(m_stack + op->b());
			break;
		case Op32BRSHIFT:
			*((u32*)(m_stack + op->c())) = *(u32*)(m_stack + op->a()) >> *(u32*)(m_stack + op->b());
			break;

		case Op32LOAD_CONSTANT:
			*(u32*)(m_stack + op->c()) = *(u32*)(constants + op->a());
			break;
		case Op32LOAD_STATIC:
			*(u32*)(m_stack + op->a()) = *(u32*)(m_heap + op->bx());
			break;
		case Op32LOAD_MEMBER:
			*(u32*)(m_stack + op->c()) = ((u32*)(m_heap + op->a()))[op->b()];
			break;

		case Op32SET_LOCAL:
			*(u32*)(m_stack + op->c()) = *(u32*)(m_stack + op->a());
			break;
		case Op32SET_STATIC:
			*(u32*)(m_heap + op->bx()) = *(u32*)(m_stack + op->a());
			break;
		case Op32SET_MEMBER:
			((u32*)(m_heap + op->b()))[op->c()] = *(u32*)(m_stack + op->a());
			break;

		case Op32RETURN:
			*(u32*)return_value = *(u32*)(m_stack + op->a());
			return;


		// =================================================================================
		// i64 instructions
		// =================================================================================
		case OpI64toI8:
			*(i8*)(m_stack + op->b()) = static_cast<i8>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toU8:
			*(u8*)(m_stack + op->b()) = static_cast<u8>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toI16:
			*(i16*)(m_stack + op->b()) = static_cast<i16>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toU16:
			*(u16*)(m_stack + op->b()) = static_cast<u16>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toI32:
			*(i32*)(m_stack + op->b()) = static_cast<i32>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toU32:
			*(u32*)(m_stack + op->b()) = static_cast<u32>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toI64:
			*(i64*)(m_stack + op->b()) = static_cast<i64>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toU64:
			*(u64*)(m_stack + op->b()) = static_cast<u64>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toF32:
			*(f32*)(m_stack + op->b()) = static_cast<f32>(*(i64*)(m_stack + op->a()));
			break;
		case OpI64toF64:
			*(f64*)(m_stack + op->b()) = static_cast<f64>(*(i64*)(m_stack + op->a()));
			break;

		case OpI64ADD:
			*(i64*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) + *(i64*)(m_stack + op->b());
			break;
		case OpI64SUB:
			*(i64*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) - *(i64*)(m_stack + op->b());
			break;
		case OpI64MUL:
			*(i64*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) * *(i64*)(m_stack + op->b());
			break;
		case OpI64DIV:
			*(i64*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) / *(i64*)(m_stack + op->b());
			break;
		case OpI64MOD:
			*(i64*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) % *(i64*)(m_stack + op->b());
			break;

		case OpI64INC:
			*(i64*)(m_stack + op->b()) = (*(i64*)(m_stack + op->a()))++;
			break;
		case OpI64DEC:
			*(i64*)(m_stack + op->b()) = (*(i64*)(m_stack + op->a()))--;
			break;

		case OpI64LT:
			*(bool*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) < *(i64*)(m_stack + op->b());
			break;
		case OpI64LE:
			*(bool*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) <= *(i64*)(m_stack + op->b());
			break;
		case OpI64GT:
			*(bool*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) > *(i64*)(m_stack + op->b());
			break;
		case OpI64GE:
			*(bool*)(m_stack + op->c()) = *(i64*)(m_stack + op->a()) >= *(i64*)(m_stack + op->b());
			break;


		// =================================================================================
		// u64 instructions
		// =================================================================================
		case OpU64toI8:
			*(i8*)(m_stack + op->b()) = static_cast<i8>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toU8:
			*(u8*)(m_stack + op->b()) = static_cast<u8>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toI16:
			*(i16*)(m_stack + op->b()) = static_cast<i16>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toU16:
			*(u16*)(m_stack + op->b()) = static_cast<u16>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toI32:
			*(i32*)(m_stack + op->b()) = static_cast<i32>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toU32:
			*(u32*)(m_stack + op->b()) = static_cast<u32>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toI64:
			*(i64*)(m_stack + op->b()) = static_cast<i64>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toU64:
			*(u64*)(m_stack + op->b()) = static_cast<u64>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toF32:
			*(f32*)(m_stack + op->b()) = static_cast<f32>(*(u64*)(m_stack + op->a()));
			break;
		case OpU64toF64:
			*(f64*)(m_stack + op->b()) = static_cast<f64>(*(u64*)(m_stack + op->a()));
			break;

		case OpU64ADD:
			*(u64*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) + *(u64*)(m_stack + op->b());
			break;
		case OpU64SUB:
			*(u64*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) - *(u64*)(m_stack + op->b());
			break;
		case OpU64MUL:
			*(u64*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) * *(u64*)(m_stack + op->b());
			break;
		case OpU64DIV:
			*(u64*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) / *(u64*)(m_stack + op->b());
			break;
		case OpU64MOD:
			*(u64*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) % *(u64*)(m_stack + op->b());
			break;

		case OpU64INC:
			*(u64*)(m_stack + op->b()) = (*(u64*)(m_stack + op->a()))++;
			break;
		case OpU64DEC:
			*(u64*)(m_stack + op->b()) = (*(u64*)(m_stack + op->a()))--;
			break;

		case OpU64LT:
			*(bool*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) < *(u64*)(m_stack + op->b());
			break;
		case OpU64LE:
			*(bool*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) <= *(u64*)(m_stack + op->b());
			break;
		case OpU64GT:
			*(bool*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) > *(u64*)(m_stack + op->b());
			break;
		case OpU64GE:
			*(bool*)(m_stack + op->c()) = *(u64*)(m_stack + op->a()) >= *(u64*)(m_stack + op->b());
			break;

		// =================================================================================
		// f64 instructions
		// =================================================================================
		case OpF64toI8:
			*(i8*)(m_stack + op->b()) = static_cast<i8>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toU8:
			*(u8*)(m_stack + op->b()) = static_cast<u8>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toI16:
			*(i16*)(m_stack + op->b()) = static_cast<i16>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toU16:
			*(u16*)(m_stack + op->b()) = static_cast<u16>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toI32:
			*(i32*)(m_stack + op->b()) = static_cast<i32>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toU32:
			*(u32*)(m_stack + op->b()) = static_cast<u32>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toI64:
			*(i64*)(m_stack + op->b()) = static_cast<i64>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toU64:
			*(u64*)(m_stack + op->b()) = static_cast<u64>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toF32:
			*(f32*)(m_stack + op->b()) = static_cast<f32>(*(f64*)(m_stack + op->a()));
			break;
		case OpF64toF64:
			*(f64*)(m_stack + op->b()) = static_cast<f64>(*(f64*)(m_stack + op->a()));
			break;

		case OpF64ADD:
			*(f64*)(m_stack + op->c()) = *(f64*)(m_stack + op->a()) + *(f64*)(m_stack + op->b());
			break;
		case OpF64SUB:
			*(f64*)(m_stack + op->c()) = *(f64*)(m_stack + op->a()) - *(f64*)(m_stack + op->b());
			break;
		case OpF64MUL:
			*(f64*)(m_stack + op->c()) = *(f64*)(m_stack + op->a()) * *(f64*)(m_stack + op->b());
			break;
		case OpF64DIV:
			*(f64*)(m_stack + op->c()) = *(f64*)(m_stack + op->a()) / *(f64*)(m_stack + op->b());
			break;
		case OpF64MOD:
			assert(false && "Can't use modulus on floats. ");
			break;

		case OpF64INC:
			*(f64*)(m_stack + op->b()) = (*(f64*)(m_stack + op->a()))++;
			break;
		case OpF64DEC:
			*(f64*)(m_stack + op->b()) = (*(f64*)(m_stack + op->a()))--;
			break;

		case OpF64LT:
			*(bool*)(m_stack + op->c()) = *(f64*)(m_stack + op->a()) < *(f64*)(m_stack + op->b());
			break;
		case OpF64LE:
			*(bool*)(m_stack + op->c()) = *(f64*)(m_stack + op->a()) <= *(f64*)(m_stack + op->b());
			break;
		case OpF64GT:
			*(bool*)(m_stack + op->c()) = *(f64*)(m_stack + op->a()) > *(f64*)(m_stack + op->b());
			break;
		case OpF64GE:
			*(bool*)(m_stack + op->c()) = *(f64*)(m_stack + op->a()) >= *(f64*)(m_stack + op->b());
			break;

		// =================================================================================
		// 64 bit instructions
		// =================================================================================
		case Op64EQ:
			*((bool*)(m_stack + op->c())) = *(u64*)(m_stack + op->a()) == *(u64*)(m_stack + op->b());
			break;
		case Op64NOT:
			*((bool*)(m_stack + op->c())) = !*(u64*)(m_stack + op->a());
			break;
		case Op64NEQ:
			*((bool*)(m_stack + op->c())) = *(u64*)(m_stack + op->a()) != *(u64*)(m_stack + op->b());
			break;

		case Op64BOR:
			*((u64*)(m_stack + op->c())) = *(u64*)(m_stack + op->a()) | *(u64*)(m_stack + op->b());
			break;
		case Op64BAND:
			*((u64*)(m_stack + op->c())) = *(u64*)(m_stack + op->a()) & *(u64*)(m_stack + op->b());
			break;
		case Op64BXOR:
			*((u64*)(m_stack + op->c())) = *(u64*)(m_stack + op->a()) ^ *(u64*)(m_stack + op->b());
			break;
		case Op64BNOT:
			*((u64*)(m_stack + op->c())) = ~*(u64*)(m_stack + op->a());
			break;

		case Op64BLSHIFT:
			*((u64*)(m_stack + op->c())) = *(u64*)(m_stack + op->a()) << *(u64*)(m_stack + op->b());
			break;
		case Op64BRSHIFT:
			*((u64*)(m_stack + op->c())) = *(u64*)(m_stack + op->a()) >> *(u64*)(m_stack + op->b());
			break;

		case Op64LOAD_CONSTANT:
			*(u64*)(m_stack + op->c()) = *(u64*)(constants + op->a());
			break;
		case Op64LOAD_STATIC:
			*(u64*)(m_stack + op->a()) = *(u64*)(m_heap + op->bx());
			break;
		case Op64LOAD_MEMBER:
			*(u64*)(m_stack + op->c()) = ((u64*)(m_heap + op->a()))[op->b()];
			break;

		case Op64SET_LOCAL:
			*(u64*)(m_stack + op->c()) = *(u64*)(m_stack + op->a());
			break;
		case Op64SET_STATIC:
			*(u64*)(m_heap + op->bx()) = *(u64*)(m_stack + op->a());
			break;
		case Op64SET_MEMBER:
			((u64*)(m_heap + op->b()))[op->c()] = *(u64*)(m_stack + op->a());
			break;

		case Op64RETURN:
			*(u64*)return_value = *(u64*)(m_stack + op->a());
			return;

		default:
			assert(false);
		}

		++op;
	}
}

*/