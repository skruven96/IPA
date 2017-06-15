#ifndef SOURCE_H
#define SOURCE_H
#include "tokenizer.h"
#include <array>
#include <string>
#include <fstream>


struct CharNode
{
	CharNode() {}

	CharNode(Token::TokenType type)
		: m_type(type)
	{}

	CharNode(Token::Operand operand)
		: m_type(Token::OperandToken), m_operand(operand)
	{}


	Token::TokenType m_type;
	Token::Keyword m_keyword;
	Token::Operand m_operand;
	Token::Primitive m_primitive;
};


inline Token::Keyword operator|(Token::Keyword lhs, Token::Keyword rhs)
{
	return static_cast<Token::Keyword>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
}
inline Token::Keyword operator&(Token::Keyword lhs, Token::Keyword rhs)
{
	return static_cast<Token::Keyword>(static_cast<u8>(lhs) & static_cast<u8>(rhs));
}

inline Token::Primitive operator|(Token::Primitive lhs, Token::Primitive rhs)
{
	return static_cast<Token::Primitive>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
}
inline Token::Primitive operator&(Token::Primitive lhs, Token::Primitive rhs)
{
	return static_cast<Token::Primitive>(static_cast<u8>(lhs) & static_cast<u8>(rhs));
}


extern std::array<CharNode, (char)127> s_chars_map;


class Source
{
public:
	Source(const std::string& path)
		: m_path(path)
	{}

	std::string path() { return m_path; }
	const std::string& path() const { return m_path; }

	std::string read(SourcePosition start, SourcePosition end);

	void open();
	void close();

	bool is_open() const { return m_stream.is_open(); }

	// Indicates if the source file has been modified since last opened.
	bool has_changed() const { return false; }

	char current() const { return *m_current; }
	CharNode data() const { return m_data; }
	SourcePosition position() const { return SourcePosition(m_current - m_start); }

	// Eats and returns the current char
	char eat();

	bool is_number() const	   { return m_data.m_type == Token::NumberToken; }
	bool is_string() const	   { return m_data.m_type == Token::StringToken; }
	bool is_operand() const	   { return m_data.m_type == Token::OperandToken; }
	bool is_identifier() const { return m_data.m_type == Token::IdentifierToken; }
	bool is_whitespace() const { return m_data.m_type == Token::ScopeBegToken; }

	bool is_eol() const		   { return m_data.m_type == Token::NewLineToken; }
	bool is_eof() const		   { return m_data.m_type == Token::EOFToken; }

private:
	std::ifstream m_stream;
	std::string m_path;

	char* m_start;
	char* m_end;
	char* m_current;
	CharNode m_data;
};


#endif // SOURCE_H