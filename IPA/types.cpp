#include "compiler.h"
#include "tokenizer.h"

ast::Type* s_primitive_types = nullptr;
void ast::Type::InitializePrimitveTypes() {
	s_primitive_types = (ast::Type*)calloc((i32)Primitive::PrimtiveCount, sizeof(ast::Type));

	s_primitive_types[(i32)Primitive::VoidPrimitive].size  = 0;
	s_primitive_types[(i32)Primitive::VoidPrimitive].flags = (u32)Primitive::VoidPrimitive;

	s_primitive_types[(i32)Primitive::BoolPrimitive].size  = sizeof(bool);
	s_primitive_types[(i32)Primitive::BoolPrimitive].flags = (u32)Primitive::BoolPrimitive;

	s_primitive_types[(i32)Primitive::U8Primitive].size   = sizeof(u8);
	s_primitive_types[(i32)Primitive::U8Primitive].flags  = UNSIGNED | (u32)Primitive::U8Primitive;

	s_primitive_types[(i32)Primitive::U16Primitive].size  = sizeof(u16);
	s_primitive_types[(i32)Primitive::U16Primitive].flags = UNSIGNED | (u32)Primitive::U16Primitive;

	s_primitive_types[(i32)Primitive::U32Primitive].size  = sizeof(u32);
	s_primitive_types[(i32)Primitive::U32Primitive].flags = UNSIGNED | (u32)Primitive::U32Primitive;

	s_primitive_types[(i32)Primitive::U64Primitive].size  = sizeof(u64);
	s_primitive_types[(i32)Primitive::U64Primitive].flags = UNSIGNED | (u32)Primitive::U64Primitive;

	s_primitive_types[(i32)Primitive::S8Primitive].size  = sizeof(i8);
	s_primitive_types[(i32)Primitive::S8Primitive].flags = SIGNED | (u32)Primitive::S8Primitive;

	s_primitive_types[(i32)Primitive::S16Primitive].size  = sizeof(i16);
	s_primitive_types[(i32)Primitive::S16Primitive].flags = SIGNED | (u32)Primitive::S16Primitive;

	s_primitive_types[(i32)Primitive::S32Primitive].size  = sizeof(i32);
	s_primitive_types[(i32)Primitive::S32Primitive].flags = SIGNED | (u32)Primitive::S32Primitive;

	s_primitive_types[(i32)Primitive::S64Primitive].size  = sizeof(i64);
	s_primitive_types[(i32)Primitive::S64Primitive].flags = SIGNED | (u32)Primitive::S64Primitive;

	s_primitive_types[(i32)Primitive::F32Primitive].size  = sizeof(f32);
	s_primitive_types[(i32)Primitive::F32Primitive].flags = DECIMAL | (u32)Primitive::F32Primitive;

	s_primitive_types[(i32)Primitive::F64Primitive].size  = sizeof(f64);
	s_primitive_types[(i32)Primitive::F64Primitive].flags = DECIMAL | (u32)Primitive::F64Primitive;
}

ast::Type* ast::Type::GetPrimitiveOrAssert(Primitive primitive) {
	assert(primitive != Primitive::NoPrimitive && (i32)primitive < (i32)Primitive::PrimtiveCount);
	return &s_primitive_types[(i32)primitive];
}