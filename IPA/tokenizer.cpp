#include "tokenizer.h"
#include "project.h"
#include "compiler.h"

#include <fstream>
#include <iostream>
#include <assert.h>


Source::Source(const std::string& path) 
	: m_path(path), m_start(nullptr), m_end(nullptr) {
	std::ifstream stream(path, std::ios::binary);
	if (!stream)
		return;

	stream.seekg(0, std::ios::end);
	const auto file_size = (i64)stream.tellg();

	char* file_data = new char[file_size + 2];

	m_start = file_data + 1;
	m_end   = file_data + file_size + 1;

	stream.seekg(0, std::ios::beg);
	stream.read(file_data + 1, file_size);
	file_data[0] = '\n';
	file_data[file_size + 1] = '\0';

	stream.close();
}

const char* Token::TokenTypeToString(TokenType type) {
	switch (type)
	{
	case TokenType::ErrorToken:      return "<error>";
	case TokenType::EOFToken:        return "<eof>";
	case TokenType::StmtEndToken:    return "stmt-end";
	case TokenType::ScopeBegToken:   return "scope-beg";
	case TokenType::ScopeEndToken:   return "scope-end";
	case TokenType::StringToken:     return "string-literal";
	case TokenType::NumberToken:     return "number-literal";
	case TokenType::IdentifierToken: return "identifier";
	case TokenType::KeywordToken:    return "keyword";
	case TokenType::OperandToken:    return "operand";
	case TokenType::PrimitiveToken:  return "primitive-type";
	default:
		assert(false);
		break;
	}
	return "";
}

const char* Token::OperandToString(Operand operand) {
	switch (operand)
	{
	case Operand::NoOperand:   return "<no-operand>";
	case Operand::SetOperand:  return "=";
	case Operand::AddOperand:  return "+";
	case Operand::SubOperand:  return "-";
	case Operand::MulOperand:  return "*";
	case Operand::DivOperand:  return "/";
	case Operand::ModOperand:  return "%";
	case Operand::LtOperand:   return "<";
	case Operand::GtOperand:   return ">";
	case Operand::NotOperand:  return "!";
	case Operand::BinaryAndOperand: return "&";
	case Operand::BinaryOrOperand:  return "|";
	case Operand::BinaryXorOperand: return "^";
	case Operand::BinaryNotOperand: return "~";
	case Operand::ColonOperand: return ":";
	case Operand::CommaOperand: return ",";
	case Operand::DotOperand:   return ".";
	case Operand::LSquareBracketOperand: return "[";
	case Operand::RSquareBracketOperand: return "]";
	case Operand::LCurlyBracketOperand:  return "{";
	case Operand::RCurlyBracketOperand:  return "}";
	case Operand::LPharenthesesOperand:  return "(";
	case Operand::RPharenthesesOperand:  return ")";

	case Operand::AddSetOperand: return "+=";
	case Operand::SubSetOperand: return "-=";
	case Operand::MulSetOperand: return "*=";
	case Operand::DivSetOperand: return "/=";
	case Operand::ModSetOperand: return "%=";
	case Operand::IncrementOperand: return "++";
	case Operand::DecrementOperand: return "--";
	case Operand::EqualsOperand: return "==";
	case Operand::NotEqualsOperand: return "!=";
	case Operand::LesserEqualsOperand: return "<=";
	case Operand::GreaterEqualsOperand: return ">=";
	case Operand::LShiftOperand: return "<<";
	case Operand::RShiftOperand: return ">>";
	case Operand::ArrowOperand: return "->";

	case Operand::AndOperand: return "and";
	case Operand::OrOperand:  return "or";
	default:
		assert(false);
		break;
	}
	return "<error-operand>";
}

const char* Token::KeywordToString(Keyword keyword) {
	switch (keyword)
	{
	case Keyword::NoKeyword:       return "<no-keyword>";
	case Keyword::ImportKeyword:   return "import";
	case Keyword::FromKeyword:     return "from";
	case Keyword::AsKeyword:       return "as";
	case Keyword::TrueKeyword:     return "true";
	case Keyword::FalseKeyword:    return "false";
	case Keyword::StructKeyword:   return "struct";
	case Keyword::PassKeyword:     return "pass";
	case Keyword::ReturnKeyword:   return "return";
	case Keyword::WhileKeyword:    return "while";
	case Keyword::ForKeyword:      return "for";
	case Keyword::InKeyword:       return "in";
	case Keyword::BreakKeyword:    return "break";
	case Keyword::ContinueKeyword: return "continue";
	case Keyword::IfKeyword:       return "if";
	case Keyword::ElifKeyword:     return "elif";
	case Keyword::ElseKeyword:     return "else";
	case Keyword::NewKeyword:      return "new";
	default:
		assert(false);
		break;
	}
	return "<error-keyword>";
}

const char* Token::PrimitiveToString(Primitive primitive) {
	switch (primitive)
	{
	case Primitive::NoPrimitive:   return "<no-primitive>";
	case Primitive::VoidPrimitive: return "void";
	case Primitive::BoolPrimitive: return "bool";
	case Primitive::S8Primitive:   return "s8";
	case Primitive::U8Primitive:   return "u8";
	case Primitive::S16Primitive:  return "s16";
	case Primitive::U16Primitive:  return "u16";
	case Primitive::S32Primitive:  return "s32";
	case Primitive::U32Primitive:  return "u32";
	case Primitive::S64Primitive:  return "s64";
	case Primitive::U64Primitive:  return "u64";
	case Primitive::F32Primitive:  return "f32";
	case Primitive::F64Primitive:  return "f64";
	default:
		assert(false);
		break;
	}
	return "<error-primitive>";
}

std::string Token::to_str() const {
	std::string result;
	switch (m_type)
	{
	case TokenType::ErrorToken:
		if (m_length > 0)
			result.append(m_first_char, m_length);
		else
			result = "<error-token>";
		break;
	case TokenType::EOFToken:
		result = "<eof>";
		break;
	case TokenType::StmtEndToken:
		result = "<stmt-end>";
		break;
	case TokenType::ScopeBegToken:
		result = "<scope-beg>";
		break;
	case TokenType::ScopeEndToken:
		result = "<scope-end>";
		break;
	default:
		result.append(m_first_char, m_length);
		break;
	}

	return result;
}


Tokenizer::Tokenizer(ModuleCompiler* module_compiler)
	: m_module_compiler(module_compiler), m_stop_tokenizing(false) {
	m_start   = module_compiler->source()->start(); 
	m_current = module_compiler->source()->start(); 
	m_end     = module_compiler->source()->end();
	tokenize();
}

Token Tokenizer::eat() {
	if (m_current_token == m_tokens.size())
		return m_tokens.back();
	return m_tokens[m_current_token++];
}

void Tokenizer::tokenize() {
	while (!m_stop_tokenizing) {
		tokenize_scopes();

		while (!m_stop_tokenizing) {
			skip_whitespace_and_comments();

			if (*m_current == '\n' || *m_current == '\r') {
				tokenize_eol();
				break;
			} else if (*m_current == '\0') {
				tokenize_eof();
			} else if (is_digit() || 
				(*m_current == '.' && m_current[1] >= '0' && m_current[1] <= '9')) {
				tokenize_number();
			} else if (is_identifier()) {
				tokenize_identifier();
			} else
				tokenize_operand();
		}
	}

	m_tokens.push_back(Token(TokenType::EOFToken, m_current, 1, m_current_line_number));
}

void Tokenizer::tokenize_scopes() {
	if (!m_opening_brackets.empty()) {
		skip_whitespace_and_comments();
		if (*m_current == '\r' || *m_current == '\n') {
			skip_new_line();
			tokenize_scopes();
		} else if (*m_current == '\0') {
			tokenize_eof();
		}
	} else {
		const char* first_char_of_line = m_current;
		const char* first_char_of_indent = m_current;
		const char* first_char_of_illegal_spaces = nullptr;
		int space_count = 0;
		int new_indention = 0;
		while (true) {
			if (*m_current == ' ') {
				first_char_of_indent = m_current;
				++m_current;
				space_count = 1;
				while (space_count != 4 && *m_current == ' ') {
					++space_count; ++m_current;
				}

				if (space_count != 4) {
					first_char_of_illegal_spaces = first_char_of_indent;
					break;
				} else {
					++new_indention;
				}
			} else if (*m_current == '\t') {
				first_char_of_indent = m_current;
				++m_current;
				++new_indention;
			} else
				break;
		}

		skip_whitespace_and_comments();
		if (*m_current == '\r' || *m_current == '\n') {
			skip_new_line();
			tokenize_scopes();
		} else if (*m_current == '\0') {
			tokenize_eof();
		} else if (first_char_of_illegal_spaces) {
			raise_error()
				->message("Illegal number of spaces, needs to be a multiple of 4. ")
				->highlight_token(Token(TokenType::ScopeBegToken, first_char_of_illegal_spaces, space_count, m_current_line_number));
		} else if (new_indention > m_indention_level + 1) {
			raise_error()
				->message("To much indentation, can only increase by 4 spaces or one tab at a time. ")
				->highlight_token(Token(TokenType::ScopeBegToken, first_char_of_indent, m_current - first_char_of_indent, m_current_line_number));
		} else {
			if (new_indention == m_indention_level + 1) {
				m_indention_level = new_indention;
				m_tokens.push_back(Token(TokenType::ScopeBegToken, first_char_of_indent, m_current - first_char_of_indent, m_current_line_number));
			} else {
				while (new_indention < m_indention_level) {
					--m_indention_level;
					m_tokens.push_back(Token(TokenType::ScopeEndToken, first_char_of_line, m_current - first_char_of_line, m_current_line_number));
				}
			}
		}
	}
}

void Tokenizer::tokenize_number() {
	const char* start = m_current;
	bool found_decimal = false;

	while (is_digit() || *m_current == '.') {
		if (*m_current == '.') {
			if (found_decimal || m_current[1] == '.') 
				break;
			found_decimal = true;
		}
		++m_current;
	}

	m_tokens.push_back(Token(TokenType::NumberToken, start, m_current - start, m_current_line_number));
}

void Tokenizer::tokenize_identifier() {
	const char* start = m_current++;

	while (is_identifier() || is_digit()) {
		++m_current;
	}

	int length = m_current - start;
	Primitive primitive    = Primitive::NoPrimitive;
	Keyword keyword = Keyword::NoKeyword;
	Operand operand = Operand::NoOperand;
	switch (length)
	{
	case(2):
		if      (memcmp(start, "if", length) == 0) keyword = Keyword::IfKeyword;
		else if (memcmp(start, "in", length) == 0) keyword = Keyword::InKeyword;
		else if (memcmp(start, "as", length) == 0) keyword = Keyword::AsKeyword;
		else if (memcmp(start, "or", length) == 0) operand = Operand::OrOperand;
		else if (memcmp(start, "s8", length) == 0) primitive = Primitive::S8Primitive;
		else if (memcmp(start, "u8", length) == 0) primitive = Primitive::U8Primitive;
		break;
	case(3):
		if      (memcmp(start, "for", length) == 0) keyword = Keyword::ForKeyword;
		else if (memcmp(start, "and", length) == 0) operand = Operand::AndOperand;
		else if (memcmp(start, "new", length) == 0) keyword = Keyword::NewKeyword;
		else if (memcmp(start, "s16", length) == 0) primitive = Primitive::S16Primitive;
		else if (memcmp(start, "u16", length) == 0) primitive = Primitive::U16Primitive;
		else if (memcmp(start, "s32", length) == 0) primitive = Primitive::S32Primitive;
		else if (memcmp(start, "u32", length) == 0) primitive = Primitive::U32Primitive;
		else if (memcmp(start, "s64", length) == 0) primitive = Primitive::S64Primitive;
		else if (memcmp(start, "u64", length) == 0) primitive = Primitive::U64Primitive;
		else if (memcmp(start, "f32", length) == 0) primitive = Primitive::F32Primitive;
		else if (memcmp(start, "f64", length) == 0) primitive = Primitive::F64Primitive;
		break;
	case(4):
		if      (memcmp(start, "elif", length) == 0) keyword = Keyword::ElifKeyword;
		else if (memcmp(start, "else", length) == 0) keyword = Keyword::ElseKeyword;
		else if (memcmp(start, "pass", length) == 0) keyword = Keyword::PassKeyword;
		else if (memcmp(start, "from", length) == 0) keyword = Keyword::FromKeyword;
		else if (memcmp(start, "true", length) == 0) keyword = Keyword::TrueKeyword;
		else if (memcmp(start, "void", length) == 0) primitive = Primitive::VoidPrimitive;
		else if (memcmp(start, "bool", length) == 0) primitive = Primitive::BoolPrimitive;
		break;
	case(5):
		if      (memcmp(start, "while", length) == 0) keyword = Keyword::WhileKeyword;
		else if (memcmp(start, "break", length) == 0) keyword = Keyword::BreakKeyword;
		else if (memcmp(start, "false", length) == 0) keyword = Keyword::FalseKeyword;
		break;
	case(6):
		if      (memcmp(start, "import", length) == 0) keyword = Keyword::ImportKeyword;
		else if (memcmp(start, "return", length) == 0) keyword = Keyword::ReturnKeyword;
		else if (memcmp(start, "struct", length) == 0) keyword = Keyword::StructKeyword;
		break;
	case(8):
		if (memcmp(start, "continue", length) == 0) keyword = Keyword::ContinueKeyword;
		break;
	}

	if (keyword != Keyword::NoKeyword) {
		m_tokens.push_back(Token(keyword, start, length, m_current_line_number));
	} else if (primitive != Primitive::NoPrimitive) {
		m_tokens.push_back(Token(primitive, start, length, m_current_line_number));
	} else if (operand != Operand::NoOperand) {
		m_tokens.push_back(Token(operand, start, length, m_current_line_number));
	} else {
		m_tokens.push_back(Token(TokenType::IdentifierToken, start, length, m_current_line_number));
	}
}

void Tokenizer::tokenize_operand() {
	const char* start = m_current;
	char c0 = *m_current++;
	char c1 = *m_current;

	bool is_opening_bracket = false, is_closing_bracket = false;
	Operand operand = Operand::NoOperand;
	Operand double_operand = Operand::NoOperand;

	switch (c0) {
	case('='): {
		operand = Operand::SetOperand;
		if (c1 == '=') double_operand = Operand::EqualsOperand;
	} break;
	case('+'): {
		operand = Operand::AddOperand;
		if (c1 == '=') double_operand = Operand::AddSetOperand;
		if (c1 == '+') double_operand = Operand::IncrementOperand;
	} break;
	case('-'): {
		operand = Operand::SubOperand;
		if (c1 == '=') double_operand = Operand::SubSetOperand;
		if (c1 == '-') double_operand = Operand::DecrementOperand;
		if (c1 == '>') double_operand = Operand::ArrowOperand;
	} break;
	case('*'): {
		operand = Operand::MulOperand;
		if (c1 == '=') double_operand = Operand::MulSetOperand;
	} break;
	case('/'): {
		operand = Operand::DivOperand;
		if (c1 == '=') double_operand = Operand::DivSetOperand;
	} break;
	case('%'): {
		operand = Operand::ModOperand;
		if (c1 == '=') double_operand = Operand::ModSetOperand;
	} break;

	case('<'): {
		operand = Operand::LtOperand;
		if (c1 == '=') double_operand = Operand::LesserEqualsOperand;
		if (c1 == '<') double_operand = Operand::LShiftOperand;
	} break;
	case('>'): {
		operand = Operand::GtOperand;
		if (c1 == '=') double_operand = Operand::GreaterEqualsOperand;
		if (c1 == '>') double_operand = Operand::RShiftOperand;
	} break;
	case('!'): {
		operand = Operand::NotOperand;
		if (c1 == '=') double_operand = Operand::NotEqualsOperand;
	} break;

	case('&'): {
		operand = Operand::BinaryAndOperand;
		if (c1 == '=') double_operand = Operand::AddSetOperand;
	} break;
	case('|'): {
		operand = Operand::BinaryOrOperand;
		if (c1 == '=') double_operand = Operand::AddSetOperand;
	} break;
	case('^'): {
		operand = Operand::BinaryXorOperand;
		if (c1 == '=') double_operand = Operand::AddSetOperand;
	} break;
	case('~'): {
		operand = Operand::BinaryNotOperand;
		if (c1 == '=') double_operand = Operand::AddSetOperand;
	} break;

	case('['): operand = Operand::LSquareBracketOperand; is_opening_bracket = true; break;
	case('{'): operand = Operand::LCurlyBracketOperand;  is_opening_bracket = true; break;
	case('('): operand = Operand::LPharenthesesOperand;  is_opening_bracket = true; break;
	case(']'): operand = Operand::RSquareBracketOperand; is_closing_bracket = true; break;
	case('}'): operand = Operand::RCurlyBracketOperand;  is_closing_bracket = true; break;
	case(')'): operand = Operand::RPharenthesesOperand;  is_closing_bracket = true; break;

	case(','): operand = Operand::CommaOperand; break;
	case('.'): {
		operand = Operand::DotOperand;
		if (c1 == '.') double_operand = Operand::DotDotOperand;
	} break;
	case(':'): operand = Operand::ColonOperand; break;
	case('@'): operand = Operand::AtOperand;    break;
	case(';'): {
		m_tokens.push_back(Token(TokenType::StmtEndToken, start, m_current - start, m_current_line_number));
		return;
	}

	default:
		raise_error()
			->message("Illegal token encountered, found here:")
			->highlight_token(Token(TokenType::ErrorToken, start, 1, m_current_line_number));
		return;	
	}

	if (double_operand != Operand::NoOperand) {
		++m_current;
		operand = double_operand;
	}

	m_tokens.push_back(Token(operand, start, m_current - start, m_current_line_number));
	if (is_opening_bracket) {
		m_opening_brackets.push(m_tokens.back());
	} else if (is_closing_bracket) {
		if (m_opening_brackets.empty()) {
			raise_error()
				->message("No opening bracket found, the closing bracket was found here: ")
				->highlight_token(m_tokens.back());
		} else {
			bool matched = (operand == Operand::RSquareBracketOperand && m_opening_brackets.top().operand() == Operand::LSquareBracketOperand) ||
				           (operand == Operand::RCurlyBracketOperand  && m_opening_brackets.top().operand() == Operand::LCurlyBracketOperand ) || 
				           (operand == Operand::RPharenthesesOperand  && m_opening_brackets.top().operand() == Operand::LPharenthesesOperand );
			if (!matched) {
				raise_error()
					->message("Mismatched brackets, opening and closing bracket found here: ")
					->highlight_token(m_opening_brackets.top())
					->highlight_token(m_tokens.back());
			}
			m_opening_brackets.pop();
		}
	}
}

void Tokenizer::tokenize_string() {
	const char* start = m_current++;
	
	while (*m_current != '"') {
		if (*m_current == '\\') {
			++m_current;
			switch (*m_current) {
				case('n'):  break;
				case('t'):  break;
				case('v'):  break;
				case('b'):  break;
				case('r'):  break;
				case('f'):  break;
				case('a'):  break;
				case('\\'): break;
				case('\''): break;
				case('"'):  break;
				case('0'):  break;
				default: {
					raise_error()
						->message("Illegal escape character encountered. ")
						->highlight_token(Token(TokenType::ErrorToken, m_current - 1, 2, m_current_line_number));
				}
			}
		}

		if (*m_current == '\r' || *m_current == '\n') {
			raise_error()
				->message("Encountered new line before closing of string literal. ")
				->highlight_token(Token(TokenType::StringToken, start, 1, m_current_line_number));
			return;
		} else if (*m_current == '\0') {
			raise_error()
				->message("Encountered eof before closing of string. ")
				->highlight_token(Token(TokenType::StringToken, start, 1, m_current_line_number));
			return;
		}
	}
	++m_current;

	m_tokens.push_back(Token(TokenType::StringToken, start, m_current - start, m_current_line_number));
}

void Tokenizer::tokenize_eol() {
	const char* first = m_current;
	if (*m_current == '\r') ++m_current;
	if (*m_current == '\n') ++m_current;
	if (m_opening_brackets.empty())
		m_tokens.push_back(Token(TokenType::StmtEndToken, first, m_current - first, m_current_line_number));
	++m_current_line_number;
}

void Tokenizer::tokenize_eof() {
	while (!m_opening_brackets.empty()) {
		raise_error()
			->message("No closing bracket found, the bracket was opened here: ")
			->highlight_token(m_opening_brackets.top());
		m_opening_brackets.pop();
	}

	if (!m_tokens.back().is(TokenType::ScopeEndToken) && !m_tokens.back().is(TokenType::StmtEndToken))
		m_tokens.push_back(Token(TokenType::StmtEndToken, m_current, 1, m_current_line_number));

	while (m_indention_level > 0) {
		m_tokens.push_back(Token(TokenType::ScopeEndToken, m_current, 1, m_current_line_number));
		--m_indention_level;
	}

	m_tokens.push_back(Token(TokenType::EOFToken, m_current, 1, m_current_line_number));
	m_stop_tokenizing = true;
}

void Tokenizer::skip_new_line() {
	if (*m_current == '\r') ++m_current;
	if (*m_current == '\n') ++m_current;
	++m_current_line_number;
}

void Tokenizer::skip_whitespace_and_comments() {
	while (is_whitespace())
		++m_current;
	while (m_current[0] == '/' && (m_current[1] == '/' || m_current[1] == '*')) {
		if (m_current[1] == '/') {
			m_current += 2;
			while (*m_current) {
				if (*m_current == '\n' || *m_current == '\r') {
					skip_new_line();
					break;
				}
				++m_current;
			}
		} else {
			const char* start = m_current;
			int nested_comments = 1;
			m_current += 2;
			while (nested_comments) {
				if (m_current[0] == '/' && m_current[1] == '*') {
					m_current += 2;
					++nested_comments;
				} else if (m_current[0] == '*' && m_current[1] == '/') {
					m_current += 2;
					--nested_comments;
				} else if (*m_current == '\0') {
					raise_error()
						->message("Reached eof before closing comment.")
						->highlight_token(Token(TokenType::ErrorToken, m_current, 2, m_current_line_number));
					break;
				} else if (*m_current == '\n' || *m_current == '\r')
					skip_new_line();
				else
					++m_current;
			}
		}
	
		while (is_whitespace())
			++m_current;
	}
	
}

bool Tokenizer::is_identifier() {
	char c = *m_current;
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Tokenizer::is_digit() {
	char c = *m_current;
	return (c >= '0' && c <= '9');
}

bool Tokenizer::is_whitespace() {
	char c = *m_current;
	return c == ' ' || c == '\t';
}

CompilerError* Tokenizer::raise_error() {
	m_stop_tokenizing = true;
	return m_module_compiler->raise_error();
}
