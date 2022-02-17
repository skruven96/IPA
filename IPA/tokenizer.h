#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "common.h"

#include <queue>
#include <stack>
#include <string>


class Source;
class ModuleCompiler;
class CompilerError;

enum class TokenType : u8 {
	ErrorToken,

	EOFToken,
	StmtEndToken,
	ScopeBegToken,
	ScopeEndToken,

	StringToken,
	NumberToken,

	IdentifierToken,
	KeywordToken,
	OperandToken,
	PrimitiveToken,
};

enum class Keyword : u8 {
	NoKeyword,
	ImportKeyword,
	FromKeyword,
	AsKeyword,

	TrueKeyword,
	FalseKeyword,

	StructKeyword,
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

	NewKeyword,
};

enum class Operand : u8 {
	NoOperand,

	SetOperand,
	AddOperand,
	SubOperand,
	MulOperand,
	DivOperand,
	ModOperand,

	LtOperand,
	GtOperand,
	NotOperand,

	BinaryAndOperand,
	BinaryOrOperand,
	BinaryXorOperand,
	BinaryNotOperand,

	ColonOperand,
	CommaOperand,
	DotOperand,
	AtOperand,

	LSquareBracketOperand,
	RSquareBracketOperand,
	LCurlyBracketOperand,
	RCurlyBracketOperand,
	LPharenthesesOperand,
	RPharenthesesOperand,

	// Double operands
	FirstSetOperand,
	AddSetOperand = FirstSetOperand,
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

	LShiftOperand,
	RShiftOperand,

	DotDotOperand,
	ArrowOperand,

	OperandCount,

	// Keyword operands
	AndOperand,
	OrOperand,
};

enum class Primitive : u8 {
	NoPrimitive,
	VoidPrimitive,
	BoolPrimitive,

	S8Primitive,
	U8Primitive,
	S16Primitive,
	U16Primitive,
	S32Primitive,
	U32Primitive,
	S64Primitive,
	U64Primitive,
	F32Primitive,
	F64Primitive,

	PrimtiveCount,
};


class Token {
public:

	static const char* TokenTypeToString(TokenType type);
	static const char* OperandToString(Operand operand);
	static const char* KeywordToString(Keyword keyword);
	static const char* PrimitiveToString(Primitive primtive);

	Token() = default;
	Token(TokenType type, const char* start, int length, int line_number)
		: m_type(type), m_first_char(start), m_length(length), m_line_number(line_number)
	{}
	Token(Operand operand, const char* start, int length, int line_number)
		: m_type(TokenType::OperandToken), m_operand(operand), m_first_char(start), m_length(length), m_line_number(line_number)
	{}
	Token(Keyword keyword, const char* start, int length, int line_number)
		: m_type(TokenType::KeywordToken), m_keyword(keyword), m_first_char(start), m_length(length), m_line_number(line_number)
	{}
	Token(Primitive primitive, const char* start, int length, int line_number)
		: m_type(TokenType::PrimitiveToken), m_primitive(primitive), m_first_char(start), m_length(length), m_line_number(line_number)
	{}

	TokenType type() const { return m_type; }
	Operand operand() const { return m_operand; }
	Keyword keyword() const { return m_keyword; }
	Primitive primitive() const { return m_primitive; }

	std::string to_str() const;

	bool equals(const Token& other) const { 
		return m_length == other.m_length ? std::memcmp(m_first_char, other.m_first_char, m_length) == 0 : false;  
	}

	const char* first_char_ptr() const { return m_first_char; }
	const char* one_past_last_char_ptr() const { return m_first_char + m_length; }
	int length() const { return m_length; }
	int line_number() const { return m_line_number; }

	bool is(TokenType type)      const { return m_type == type; }
	bool is(Keyword keyword)     const { return m_keyword == keyword; }
	bool is(Operand operand)     const { return m_operand == operand; }
	bool is(Primitive primitive) const { return m_primitive == primitive; }

	operator bool() const { return m_type != TokenType::ErrorToken; }

private:
	TokenType m_type    = TokenType::ErrorToken;
	Keyword   m_keyword = Keyword::NoKeyword;
	Operand   m_operand = Operand::NoOperand;
	Primitive m_primitive = Primitive::NoPrimitive;

	int m_length = 0;
	int m_line_number = -1;
	const char* m_first_char = 0;
};


class Source {
public:
	Source(const std::string& path);
	~Source() = default;

	std::string path() { return m_path; }
	const std::string& path() const { return m_path; }
	bool is_loaded() const { return m_start != nullptr; }

	const char* start() const { return m_start; }
	const char* end()   const { return m_end;   }

private:

	std::string m_path;

	const char* m_start = 0;
	const char* m_end = 0;

};


class Tokenizer
{
public:
	Tokenizer(ModuleCompiler* module_compiler);
	~Tokenizer() = default;

	Token peek(i32 ahead) const { return (ahead + m_current_token) >= m_tokens.size() ? m_tokens.back() : m_tokens[m_current_token + ahead]; }
	Token peek() const { return m_tokens[m_current_token]; }
	Token eat();
	
private:

	void tokenize();

	void tokenize_scopes();
	void tokenize_identifier();
	void tokenize_number();
	void tokenize_operand();
	void tokenize_string();
	void tokenize_eol();
	void tokenize_eof();

	bool is_identifier();
	bool is_digit();
	bool is_whitespace();

	void skip_new_line();
	void skip_whitespace_and_comments();

	CompilerError* raise_error();

	bool m_stop_tokenizing = false;

	int m_indention_level = 0;
	
	ModuleCompiler* m_module_compiler;

	const char* m_start   = 0;
	const char* m_end     = 0;
	const char* m_current = 0;

	int m_current_line_number = 1;

	int m_current_token = 0;
	std::vector<Token> m_tokens;
	std::stack<Token> m_opening_brackets;
};


template<> struct std::equal_to<Token> {
	bool operator()(const Token& a, const Token& b) const noexcept {
		return a.equals(b);
	}
};
template<> struct std::hash<Token> {
	std::size_t operator()(const Token& s) const noexcept {
		std::size_t hash = 5381;
		const char* str = s.first_char_ptr();
		for (int i = 0; i < s.length(); i++)
			hash = ((hash << 5) + hash) + (int)str[i]; /* hash * 33 + c */
		return hash;
	}
};

#endif // TOKENIZER_H