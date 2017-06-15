#include "tokenizer.h"
#include "source.h"

#include <iostream>
#include <assert.h>


std::string Token::type() const
{
	switch (m_type)
	{
	case(ScopeBegToken):
		return "Scope Start";
	case(ScopeEndToken):
		return "Scope End";
	case(IdentifierToken):
		return "Identifier";
	case(NumberToken):
		return "Number";
	case(StringToken):
		return "String";
	case(NewLineToken):
		return "New Line";
	case(EOFToken):
		return "End Of File";
	case(KeywordToken):
		return "Keyword";
	case(OperandToken):
		return "Operand";
	default:
		return "Token";
	}
}


std::string Token::value(Source* source) const
{
	switch (m_type)
	{
	case Token::EmptyToken:
		return "<empty-token>";
	case Token::IllegalToken:
		return "<illegal-token>";
	case Token::EOFToken:
		return "<eof>";
	case Token::NewLineToken:
	case Token::ScopeBegToken:
	case Token::ScopeEndToken:
	case Token::StringToken:
	case Token::NumberToken:
	case Token::IdentifierToken:
		return source->read(m_start, m_end);
	case Token::KeywordToken:
		switch (m_keyword)
		{
		case Token::NoKeyword:
			return "<no-keyword?>";
		case Token::ImportKeyword:
			return "import";
		case Token::FromKeyword:
			return "from";
		case Token::AsKeyword:
			return "as";
		case Token::ClassKeyword:
			return "class";
		case Token::DefKeyword:
			return "def";
		case Token::PassKeyword:
			return "pass";
		case Token::ReturnKeyword:
			return "return";
		case Token::WhileKeyword:
			return "while";
		case Token::ForKeyword:
			return "for";
		case Token::InKeyword:
			return "in";
		case Token::BreakKeyword:
			return "break";
		case Token::ContinueKeyword:
			return "continue";
		case Token::IfKeyword:
			return "if";
		case Token::ElifKeyword:
			return "elif";
		case Token::ElseKeyword:
			return "else";
		case Token::AndKeyword:
			return "and";
		case Token::OrKeyword:
			return "or";
		case Token::NewKeyword:
			return "new";
		case Token::TryKeyword:
			return "try";
		case Token::CatchKeyword:
			return "catch";
		case Token::RaiseKeyword:
			return "raise";
		default:
			assert(false && "Unimplemented Keyword in Token::value. ");
		}
	case Token::OperandToken:
		switch (m_operand)
		{
		case Token::NoOperand:
			return "<no-operand?>";
		case Token::SetOperand:
			return "=";
		case Token::AddOperand:
			return "+";
		case Token::SubOperand:
			return "-";
		case Token::MulOperand:
			return "*";
		case Token::DivOperand:
			return "/";
		case Token::ModOperand:
			return "%";
		case Token::AtOperand:
			return "@";
		case Token::IdOperand:
			return "#";
		case Token::LtOperand:
			return "<";
		case Token::GtOperand:
			return ">";
		case Token::NotOperand:
			return "!";
		case Token::BinaryAndOperand:
			return "&";
		case Token::BinaryOrOperand:
			return "|";
		case Token::BinaryXorOperand:
			return "^";
		case Token::BinaryNotOperand:
			return "~";
		case Token::LSquareOperand:
			return "[";
		case Token::RSquareOperand:
			return "]";
		case Token::LCurlyOperand:
			return "{";
		case Token::RCurlyOperand:
			return "}";
		case Token::LPharenthesesOperand:
			return "(";
		case Token::RPharenthesesOperand:
			return ")";
		case Token::ColonOperand:
			return ":";
		case Token::CommaOperand:
			return ",";
		case Token::DotOperand:
			return ".";
		case Token::AddSetOperand:
			return "+=";
		case Token::SubSetOperand:
			return "-=";
		case Token::MulSetOperand:
			return "*=";
		case Token::DivSetOperand:
			return "/=";
		case Token::ModSetOperand:
			return "%=";
		case Token::IncrementOperand:
			return "++";
		case Token::DecrementOperand:
			return "--";
		case Token::EqualsOperand:
			return "==";
		case Token::NotEqualsOperand:
			return "!=";
		case Token::LesserEqualsOperand:
			return "<=";
		case Token::GreaterEqualsOperand:
			return ">=";
		case Token::BinaryLShiftOperand:
			return "<<";
		case Token::BinaryRShiftOperand:
			return ">>";
		case Token::PointerOperand:
			return "->";
		default:
			assert(false && "Unimplemented operand in Token::value. ");
		}
	case Token::PrimitiveToken:
		switch (m_primitive)
		{
		case Token::NoPrimitive:
			return "<no-primitive?>";
		case Token::VoidPrimitive:
			return "void";
		case Token::BoolPrimitive:
			return "bool";
		case Token::I8Primitive:
			return "i8";
		case Token::U8Primitive:
			return "u8";
		case Token::I16Primitive:
			return "i16";
		case Token::U16Primitive:
			return "u16";
		case Token::I32Primitive:
			return "i32";
		case Token::U32Primitive:
			return "u32";
		case Token::I64Primitive:
			return "i64";
		case Token::U64Primitive:
			return "u64";
		case Token::F32Primitive:
			return "f32";
		case Token::F64Primitive:
			return "f64";
		default:
			assert(false && "Unimplemented primitive in Token::value. ");
		}
	default:
		assert(false && "Unimplemented TokenType in Token::value. ");
	}
}


Token Tokenizer::next()
{
	if (m_tokens.size() < 3 && !m_reached_eof)
		tokenize_line();

	Token token(m_tokens.front());

	if (!token.is_eof())
		m_tokens.pop();

	return token;
}


void Tokenizer::tokenize_line()
{
	if (tokenize_scopes())
		return;

	while (true)
	{
		switch (m_source->data().m_type)
		{
		case(Token::NumberToken):
			tokenize_number();
			break;
		case(Token::IdentifierToken):
			tokenize_identifier();
			break;
		case(Token::StringToken):
			tokenize_string();
			break;
		case(Token::NewLineToken):
			tokenize_eol();
			return;
		case(Token::ScopeBegToken):
			m_source->eat();
			break;
		case(Token::OperandToken):
			tokenize_operand();
			break;
		case(Token::EOFToken):
			tokenize_eof();
			return;
		default:
			assert(false && "Use of illegal token! ");
		}
	}
}


bool Tokenizer::tokenize_scopes()
{
	SourcePosition start = m_source->position();
	SourcePosition end = m_source->position();
	int indention_level = 0;

	bool legal_indention = true;
	while (m_source->is_whitespace())
	{
		end = m_source->position();
		char current = m_source->eat();

		if (current == ' ')
		{
			int num_spaces = 1;

			while (m_source->current() == ' ')
			{
				end = m_source->position();
				m_source->eat();
				num_spaces += 1;
			}

			if (num_spaces == 4)
				indention_level += 1;
			else
				legal_indention = true;
		}
		else
			indention_level += 1;
	}

	if (m_source->is_eol())
	{
		if (m_source->eat() == '\r')
		{
			if (m_source->current() == '\n')
				m_source->eat();
		}

		tokenize_line();

		return true;
	}

	assert(legal_indention && "Spaces needs to be in groups of four. ");

	if (indention_level == m_indention_level + 1)
	{
		++m_indention_level;
		m_tokens.push(Token(Token::ScopeBegToken, start, end));
	}
	else if (indention_level <= m_indention_level)
	{
		while (indention_level != m_indention_level)
		{
			m_tokens.push(Token(Token::ScopeEndToken, start, end));
			--m_indention_level;
		}
	}
	else
		assert(false && "Illegal number of indentions");

	return false;
}


void Tokenizer::tokenize_number()
{
	SourcePosition start = m_source->position();
	SourcePosition end = m_source->position();
	std::string number;
	bool found_decimal = false;

	while (m_source->is_number())
	{
		end = m_source->position();

		if (m_source->is_operand()) // Only char that is both number and opernad is .
		{
			if (found_decimal) break;
			found_decimal = true;
		}

		number += m_source->eat();
	}

	if (number == ".")
		m_tokens.push(Token(Token::DotOperand, start, end));
	else
		m_tokens.push(Token(Token::NumberToken, start, end));
}


void Tokenizer::tokenize_identifier()
{
	SourcePosition start(m_source->position());
	SourcePosition end(start);

	std::string identifier; 
	identifier += m_source->eat();
	
	while (m_source->is_identifier() || m_source->is_number())
	{
		end = m_source->position();
		identifier += m_source->eat();
	}

	Token::Primitive primitive = Token::NoPrimitive;
	Token::Keyword keyword = Token::NoKeyword;

	switch (identifier.size())
	{
	case(2):
		if (identifier == "if") keyword = Token::IfKeyword;
		else if (identifier == "in") keyword = Token::InKeyword;
		else if (identifier == "as") keyword = Token::AsKeyword;
		else if (identifier == "or") keyword = Token::OrKeyword;
		else if (identifier == "i8") primitive = Token::I8Primitive;
		else if (identifier == "u8") primitive = Token::U8Primitive;
		break;
	case(3):
		if (identifier == "def") keyword = Token::DefKeyword;
		else if (identifier == "for") keyword = Token::ForKeyword;
		else if (identifier == "and") keyword = Token::AndKeyword;
		else if (identifier == "new") keyword = Token::NewKeyword;
		else if (identifier == "try") keyword = Token::TryKeyword;
		else if (identifier == "i16") primitive = Token::I16Primitive;
		else if (identifier == "u16") primitive = Token::U16Primitive;
		else if (identifier == "i32") primitive = Token::I32Primitive;
		else if (identifier == "u32") primitive = Token::U32Primitive;
		else if (identifier == "i64") primitive = Token::I64Primitive;
		else if (identifier == "u64") primitive = Token::U64Primitive;
		else if (identifier == "f32") primitive = Token::F32Primitive;
		else if (identifier == "f64") primitive = Token::F64Primitive;
		break;
	case(4):
		if (identifier == "elif") keyword = Token::ElifKeyword;
		else if (identifier == "else") keyword = Token::ElseKeyword;
		else if (identifier == "pass") keyword = Token::PassKeyword;
		else if (identifier == "from") keyword = Token::FromKeyword;
		else if (identifier == "void") primitive = Token::VoidPrimitive;
		else if (identifier == "bool") primitive = Token::BoolPrimitive;
		break;
	case(5):
		if (identifier == "while") keyword = Token::WhileKeyword;
		else if (identifier == "break") keyword = Token::BreakKeyword;
		else if (identifier == "class") keyword = Token::ClassKeyword;
		else if (identifier == "catch") keyword = Token::CatchKeyword;
		else if (identifier == "raise") keyword = Token::RaiseKeyword;
		break;
	case(6):
		if (identifier == "import") keyword = Token::ImportKeyword;
		else if (identifier == "return") keyword = Token::ReturnKeyword;
		break;
	case(8):
		if (identifier == "continue") keyword = Token::ContinueKeyword;
		break;
	}

	if (keyword != Token::NoKeyword)
	{
		m_tokens.push(Token(keyword, start, end));
		return;
	}
	else if (primitive != Token::NoPrimitive)
	{
		m_tokens.push(Token(primitive, start, end));
		return;
	}
	else
		m_tokens.push(Token(Token::IdentifierToken, start, end));
}


void Tokenizer::tokenize_operand()
{
	SourcePosition start = m_source->position();
	CharNode data = m_source->data();
	char operand = m_source->eat();

	if (m_source->is_operand())
	{
		Token::Operand double_operand = Token::NoOperand;
		CharNode seconed_data = m_source->data();
		bool found_double = false;

		switch (seconed_data.m_operand)
		{
		case(Token::SetOperand):
			switch (data.m_operand)
			{
			case(Token::SetOperand):
				double_operand = Token::EqualsOperand;
				break;
			case(Token::AddOperand):
				double_operand = Token::AddSetOperand;
				break;
			case(Token::SubOperand):
				double_operand = Token::SubSetOperand;
				break;
			case(Token::MulOperand):
				double_operand = Token::MulSetOperand;
				break;
			case(Token::DivOperand):
				double_operand = Token::DivSetOperand;
				break;
			case(Token::ModOperand):
				double_operand = Token::ModSetOperand;
				break;
			case(Token::NotOperand):
				double_operand = Token::NotEqualsOperand;
				break;
			case(Token::LtOperand):
				double_operand = Token::LesserEqualsOperand;
				break;
			case(Token::GtOperand):
				double_operand = Token::GreaterEqualsOperand;
				break;
			}
			break;
		case(Token::AddOperand):
			if (operand == '+') double_operand = Token::IncrementOperand;
			break;
		case(Token::SubOperand):
			if (operand == '-') double_operand = Token::DecrementOperand;
			break;
		case(Token::LtOperand):
			if (operand == '<') double_operand = Token::BinaryLShiftOperand;
			break;
		case(Token::GtOperand):
			if (operand == '>')
				double_operand = Token::BinaryRShiftOperand;
			else if (operand == '-')
				double_operand = Token::PointerOperand;
			break;
		}

		if (double_operand != Token::NoOperand)
		{
			m_tokens.push(Token(double_operand, start, m_source->position()));
			m_source->eat();
			return;
		}
	}
	
	m_tokens.push(Token(data.m_operand, start, start));
}


void Tokenizer::tokenize_string()
{
	SourcePosition start = m_source->position();
	SourcePosition end = m_source->position();
	char end_char = m_source->eat();

	std::string str;

	while (true)
	{
		end = m_source->position();
		char character = m_source->eat();

		if (character == '\\')
		{
			character = m_source->eat();

			switch (character)
			{
			case('n'):
				str += '\n';
				break;
			case('t'):
				str += '\t';
				break;
			case('v'):
				str += '\v';
				break;
			case('b'):
				str += '\b';
				break;
			case('r'):
				str += '\r';
				break;
			case('f'):
				str += '\f';
				break;
			case('a'):
				str += '\a';
				break;
			case('\\'):
				str += '\\';
				break;
			case('\''):
				str += '\'';
				break;
			case('"'):
				str += '\"';
				break;
			case('0'):
				str += '\0';
				break;
			default:
				assert(false && "Invalid escape sequence. ");
			}
		}

		if (character == end_char)
			break;

		if (character == '\r' || character == '\n' || character == '\0')
			assert(false && "Unexpected end of line/file. ");
		
		str += character;
	}

	m_tokens.push(Token(Token::StringToken, start, end));
}


void Tokenizer::tokenize_eol()
{
	SourcePosition start(m_source->position());
	SourcePosition end(m_source->position());
	std::string value;
	value += m_source->eat();

	if (value[0] == '\r')
	{
		if (m_source->current() == '\n')
		{
			end = m_source->position();
			value += m_source->eat();
		}
	}
	
	m_tokens.push(Token(Token::NewLineToken, start, end));
}


void Tokenizer::tokenize_eof()
{
	if (!m_tokens.back().is_scope_end() && !m_tokens.back().is_new_line())
		m_tokens.push(Token(Token::NewLineToken, m_source->position(), m_source->position()));

	while (m_indention_level > 0)
	{
		m_tokens.push(Token(Token::ScopeEndToken, m_source->position(), m_source->position()));
		--m_indention_level;
	}

	m_tokens.push(Token(Token::EOFToken, m_source->position(), m_source->position()));
	m_reached_eof = true;
}

