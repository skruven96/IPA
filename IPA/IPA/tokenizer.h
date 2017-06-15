#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "types.h"

#include <queue>
#include <istream>


class Source;


class SourcePosition
{
public:
	SourcePosition()
		: m_offset(-1)
	{}

	SourcePosition(int offset)
		: m_offset(offset)
	{
		
	}

	int row(Source* source) const { return 123; }
	int column(Source* source) const { return 123; }
	int offset() const { return m_offset; }

private:
	int m_offset;
};


class Token
{
public:

	enum TokenType: u8
	{
		EmptyToken,
		IllegalToken,

		EOFToken,
		NewLineToken,
		ScopeBegToken,
		ScopeEndToken,

		StringToken,
		NumberToken,

		IdentifierToken,
		KeywordToken,
		OperandToken,
		PrimitiveToken,
	};


	enum Keyword : u8
	{
		NoKeyword,
		ImportKeyword,
		FromKeyword,
		AsKeyword,

		ClassKeyword,
		DefKeyword,
		PassKeyword,
		ReturnKeyword,

		WhileKeyword,
		ForKeyword,
		InKeyword,
		BreakKeyword,
		ContinueKeyword,

		IfKeyword,
		ElifKeyword,
		ElseKeyword,
		AndKeyword,
		OrKeyword,

		NewKeyword,
		TryKeyword,
		CatchKeyword,
		RaiseKeyword
	};


	enum Operand : u8
	{
		NoOperand,

		SetOperand,
		AddOperand,
		SubOperand,
		MulOperand,
		DivOperand,
		ModOperand,

		AtOperand,
		IdOperand,
		LtOperand,
		GtOperand,
		NotOperand,

		BinaryAndOperand,
		BinaryOrOperand,
		BinaryXorOperand,
		BinaryNotOperand,

		LSquareOperand,
		RSquareOperand,
		LCurlyOperand,
		RCurlyOperand,
		LPharenthesesOperand,
		RPharenthesesOperand,

		ColonOperand,
		CommaOperand,
		DotOperand,
		
		// Double operands
		AddSetOperand,
		SubSetOperand,
		MulSetOperand,
		DivSetOperand,
		ModSetOperand,

		IncrementOperand,
		DecrementOperand,

		EqualsOperand,
		NotEqualsOperand,
		LesserEqualsOperand,
		GreaterEqualsOperand,

		BinaryLShiftOperand,
		BinaryRShiftOperand,
		PointerOperand,
	};


	enum Primitive : u8
	{
		NoPrimitive,
		VoidPrimitive,
		BoolPrimitive,

		I8Primitive,
		U8Primitive,
		I16Primitive,
		U16Primitive,
		I32Primitive,
		U32Primitive,
		I64Primitive,
		U64Primitive,
		F32Primitive,
		F64Primitive
	};


	Token()
		: m_type(EmptyToken), m_keyword(NoKeyword), m_operand(NoOperand), m_primitive(NoPrimitive)
	{}

	Token(TokenType type, SourcePosition start, SourcePosition end)
		: m_type(type), m_keyword(NoKeyword), m_operand(NoOperand), m_primitive(NoPrimitive), m_start(start), m_end(end)
	{}

	Token(Operand operand, SourcePosition start, SourcePosition end)
		: m_type(OperandToken), m_keyword(NoKeyword), m_operand(operand), m_primitive(NoPrimitive), m_start(start), m_end(end)
	{}

	Token(Keyword keyword, SourcePosition start, SourcePosition end)
		: m_type(KeywordToken), m_keyword(keyword), m_operand(NoOperand), m_primitive(NoPrimitive), m_start(start), m_end(end)
	{}

	Token(Primitive primitive, SourcePosition start, SourcePosition end)
		: m_type(PrimitiveToken), m_keyword(NoKeyword), m_operand(NoOperand), m_primitive(primitive), m_start(start), m_end(end)
	{}


	SourcePosition start() const { return m_start; }
	SourcePosition end() const	 { return m_end; }

	std::string type() const;
	TokenType token_type() const { return m_type; }
	Operand operand() const { return m_operand; }
	Keyword keyword() const { return m_keyword; }
	Primitive primitive() const { return m_primitive; }

	std::string value(Source* source) const;


	// Types
	bool is_empty() const	  { return m_type == EmptyToken; }
	bool is_eof() const		  { return m_type == EOFToken; }
	bool is_new_line() const  { return m_type == NewLineToken; }
	bool is_scope_beg() const { return m_type == ScopeBegToken; }
	bool is_scope_end() const { return m_type == ScopeEndToken; }

	bool is_number() const { return m_type == NumberToken; }
	bool is_string() const { return m_type == StringToken; }

	bool is_identififer() const { return m_type == IdentifierToken; }
	bool is_keyword() const		{ return m_type == KeywordToken; }
	bool is_operand() const		{ return m_type == OperandToken; }
	bool is_primitive() const	{ return m_type == PrimitiveToken; }


	// Keywords
	bool is_import() const	 { return m_keyword == ImportKeyword; }
	bool is_from() const	 { return m_keyword == FromKeyword; }
	bool is_as() const		 { return m_keyword == AsKeyword; }

	bool is_class() const	 { return m_keyword == ClassKeyword; }
	bool is_def() const		 { return m_keyword == DefKeyword; }
	bool is_pass() const	 { return m_keyword == PassKeyword; }
	bool is_return() const	 { return m_keyword == ReturnKeyword; }

	bool is_while() const	 { return m_keyword == WhileKeyword; }
	bool is_for() const		 { return m_keyword == ForKeyword; }
	bool is_in() const		 { return m_keyword == InKeyword; }
	bool is_break() const	 { return m_keyword == BreakKeyword; }
	bool is_continue() const { return m_keyword == ContinueKeyword; }

	bool is_if() const		 { return m_keyword == IfKeyword; }
	bool is_elif() const	 { return m_keyword == ElifKeyword; }
	bool is_else() const	 { return m_keyword == ElseKeyword; }
	bool is_and() const		 { return m_keyword == AndKeyword; }
	bool is_or() const		 { return m_keyword == OrKeyword; }

	bool is_new() const		 { return m_keyword == NewKeyword; }
	bool is_try() const		 { return m_keyword == TryKeyword; }
	bool is_catch() const	 { return m_keyword == CatchKeyword; }
	bool is_raise() const	 { return m_keyword == RaiseKeyword; }


	// Operands
	bool is_set() const				{ return m_operand == SetOperand; }
	bool is_add() const				{ return m_operand == AddOperand; }
	bool is_sub() const				{ return m_operand == SubOperand; }
	bool is_mul() const				{ return m_operand == MulOperand; }
	bool is_div() const				{ return m_operand == DivOperand; }
	bool is_mod() const				{ return m_operand == ModOperand; }

	bool is_at() const				{ return m_operand == AtOperand; }
	bool is_id() const				{ return m_operand == IdOperand; }
	bool is_lt() const				{ return m_operand == LtOperand; }
	bool is_gt() const				{ return m_operand == GtOperand; }
	bool is_not() const				{ return m_operand == NotOperand; }

	bool is_binary_and() const		{ return m_operand == BinaryAndOperand; }
	bool is_binary_or() const		{ return m_operand == BinaryOrOperand; }
	bool is_binary_xor() const		{ return m_operand == BinaryXorOperand; }
	bool is_binary_not() const		{ return m_operand == BinaryNotOperand; }

	bool is_lsquare_bracket() const { return m_operand == LSquareOperand; }
	bool is_rsquare_bracket() const { return m_operand == RSquareOperand; }
	bool is_lcurly_bracket() const	{ return m_operand == LCurlyOperand; }
	bool is_rcurly_bracket() const	{ return m_operand == RCurlyOperand; }
	bool is_lpharentheses() const	{ return m_operand == LPharenthesesOperand; }
	bool is_rpharentheses() const	{ return m_operand == RPharenthesesOperand; }

	bool is_colon() const			{ return m_operand == ColonOperand; }
	bool is_comma() const			{ return m_operand == CommaOperand; }
	bool is_dot() const				{ return m_operand == DotOperand; }

	bool is_add_set() const			{ return m_operand == AddSetOperand; }
	bool is_sub_set() const			{ return m_operand == SubSetOperand; }
	bool is_mul_set() const			{ return m_operand == MulSetOperand; }
	bool is_div_set() const			{ return m_operand == DivSetOperand; }
	bool is_mod_set() const			{ return m_operand == ModSetOperand; }

	bool is_increment() const		{ return m_operand == IncrementOperand; }
	bool is_decrement() const		{ return m_operand == DecrementOperand; }

	bool is_equals() const			{ return m_operand == EqualsOperand; }
	bool is_not_equals() const		{ return m_operand == NotEqualsOperand; }
	bool is_lesser_equals() const	{ return m_operand == LesserEqualsOperand; }
	bool is_greater_equals() const	{ return m_operand == GreaterEqualsOperand; }

	bool is_binary_lshift() const	{ return m_operand == BinaryLShiftOperand; }
	bool is_binary_rshift() const	{ return m_operand == BinaryRShiftOperand; }
	bool is_pointer() const			{ return m_operand == PointerOperand; }


	// Primitives
	bool is_void() const { return m_primitive == VoidPrimitive; }
	bool is_bool() const { return m_primitive == BoolPrimitive; }

	bool is_i8() const  { return m_primitive == I8Primitive; }
	bool is_u8() const  { return m_primitive == U8Primitive; }
	bool is_i16() const { return m_primitive == I16Primitive; }
	bool is_u16() const { return m_primitive == U16Primitive; }
	bool is_i32() const { return m_primitive == I32Primitive; }
	bool is_u32() const { return m_primitive == U32Primitive; }
	bool is_i64() const { return m_primitive == I64Primitive; }
	bool is_u64() const { return m_primitive == U64Primitive; }
	bool is_f32() const { return m_primitive == F32Primitive; }
	bool is_f64() const { return m_primitive == F64Primitive; }


private:
	TokenType m_type;
	Keyword m_keyword;
	Operand m_operand;
	Primitive m_primitive;

	SourcePosition m_start;
	SourcePosition m_end;
};


class Tokenizer
{
public:
	Tokenizer(Source* source)
		: m_source(source), m_reached_eof(false)
	{	tokenize_line(); }

	Token peek() const { return m_tokens.front(); }
	Token next();
	
private:

	void tokenize_line();

	bool tokenize_scopes();
	void tokenize_identifier();
	void tokenize_number();
	void tokenize_operand();
	void tokenize_string();
	void tokenize_eol();
	void tokenize_eof();

	Source* m_source;
	std::queue<Token> m_tokens;
	bool m_reached_eof;

	int m_indention_level;

};


#include "source.h"


#endif // TOKENIZER_H