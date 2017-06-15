#include "source.h"
#include "tokenizer.h"

#include <array>
#include <string>
#include <assert.h>


std::array<CharNode, (char)127> s_chars_map
{
	CharNode(Token::IllegalToken),		// NULL '\0'
	CharNode(Token::IllegalToken),		// SOH
	CharNode(Token::IllegalToken),		// STX
	CharNode(Token::IllegalToken),		// ETX
	CharNode(Token::IllegalToken),		// EOT
	CharNode(Token::IllegalToken),		// ENQ
	CharNode(Token::IllegalToken),		// ACK
	CharNode(Token::IllegalToken),		// BEL
	CharNode(Token::IllegalToken),		// BS
	CharNode(Token::ScopeBegToken),		// HT '\t'
	CharNode(Token::NewLineToken),		// LF '\n'
	CharNode(Token::IllegalToken),		// VT
	CharNode(Token::IllegalToken),		// FF
	CharNode(Token::NewLineToken),		// CR '\r'
	CharNode(Token::IllegalToken),		// SO
	CharNode(Token::IllegalToken),		// IT

	CharNode(Token::IllegalToken), // DLE
	CharNode(Token::IllegalToken), // DC1
	CharNode(Token::IllegalToken), // DC2
	CharNode(Token::IllegalToken), // DC3
	CharNode(Token::IllegalToken), // DC4
	CharNode(Token::IllegalToken), // NAK
	CharNode(Token::IllegalToken), // SYN
	CharNode(Token::IllegalToken), // ETB
	CharNode(Token::IllegalToken), // CAN
	CharNode(Token::IllegalToken), // EM
	CharNode(Token::IllegalToken), // SUB
	CharNode(Token::IllegalToken), // ESC
	CharNode(Token::IllegalToken), // FS
	CharNode(Token::IllegalToken), // GS
	CharNode(Token::IllegalToken), // RS
	CharNode(Token::IllegalToken), // US

	CharNode(Token::ScopeBegToken),			// SP ' '
	CharNode(Token::NotOperand),			// !
	CharNode(Token::StringToken),			// "
	CharNode(Token::IdOperand),				// #
	CharNode(Token::IllegalToken),			// $
	CharNode(Token::ModOperand),			// %
	CharNode(Token::BinaryAndOperand),		// &
	CharNode(Token::StringToken),			// '
	CharNode(Token::LPharenthesesOperand),	// (
	CharNode(Token::RPharenthesesOperand),	// )
	CharNode(Token::MulOperand),			// *
	CharNode(Token::AddOperand),			// +
	CharNode(Token::CommaOperand),			// ,
	CharNode(Token::SubOperand),			// -
	CharNode(Token::NumberToken),			// .
	CharNode(Token::DivOperand),			// /

	CharNode(Token::NumberToken),  // 0
	CharNode(Token::NumberToken),  // 1
	CharNode(Token::NumberToken),  // 2
	CharNode(Token::NumberToken),  // 3
	CharNode(Token::NumberToken),  // 4
	CharNode(Token::NumberToken),  // 5
	CharNode(Token::NumberToken),  // 6
	CharNode(Token::NumberToken),  // 7
	CharNode(Token::NumberToken),  // 8
	CharNode(Token::NumberToken),  // 9
	CharNode(Token::ColonOperand), // :
	CharNode(Token::IllegalToken), // ;
	CharNode(Token::LtOperand),	   // <
	CharNode(Token::SetOperand),   // =
	CharNode(Token::GtOperand),	   // >
	CharNode(Token::IllegalToken), // ?

	CharNode(Token::AtOperand),		   // @
	CharNode(Token::IdentifierToken),  // A
	CharNode(Token::IdentifierToken),  // B
	CharNode(Token::IdentifierToken),  // C
	CharNode(Token::IdentifierToken),  // D
	CharNode(Token::IdentifierToken),  // E
	CharNode(Token::IdentifierToken),  // F
	CharNode(Token::IdentifierToken),  // G
	CharNode(Token::IdentifierToken),  // H
	CharNode(Token::IdentifierToken),  // I
	CharNode(Token::IdentifierToken),  // J
	CharNode(Token::IdentifierToken),  // K
	CharNode(Token::IdentifierToken),  // L
	CharNode(Token::IdentifierToken),  // M
	CharNode(Token::IdentifierToken),  // N
	CharNode(Token::IdentifierToken),  // O

	CharNode(Token::IdentifierToken),  // P
	CharNode(Token::IdentifierToken),  // Q
	CharNode(Token::IdentifierToken),  // R
	CharNode(Token::IdentifierToken),  // S
	CharNode(Token::IdentifierToken),  // Y
	CharNode(Token::IdentifierToken),  // U
	CharNode(Token::IdentifierToken),  // V
	CharNode(Token::IdentifierToken),  // W
	CharNode(Token::IdentifierToken),  // X
	CharNode(Token::IdentifierToken),  // Y
	CharNode(Token::IdentifierToken),  // Z
	CharNode(Token::LSquareOperand),   // [
	CharNode(Token::IllegalToken),	   // \ 
	CharNode(Token::RSquareOperand),   // ]
	CharNode(Token::BinaryXorOperand), // ^
	CharNode(Token::IdentifierToken),  // _

	CharNode(Token::IllegalToken),	  // `
	CharNode(Token::IdentifierToken), // a
	CharNode(Token::IdentifierToken), // b
	CharNode(Token::IdentifierToken), // c
	CharNode(Token::IdentifierToken), // d
	CharNode(Token::IdentifierToken), // e
	CharNode(Token::IdentifierToken), // f
	CharNode(Token::IdentifierToken), // g
	CharNode(Token::IdentifierToken), // h
	CharNode(Token::IdentifierToken), // i
	CharNode(Token::IdentifierToken), // j
	CharNode(Token::IdentifierToken), // k
	CharNode(Token::IdentifierToken), // l
	CharNode(Token::IdentifierToken), // m
	CharNode(Token::IdentifierToken), // n
	CharNode(Token::IdentifierToken), // o

	CharNode(Token::IdentifierToken),  // p
	CharNode(Token::IdentifierToken),  // q
	CharNode(Token::IdentifierToken),  // r
	CharNode(Token::IdentifierToken),  // s
	CharNode(Token::IdentifierToken),  // t
	CharNode(Token::IdentifierToken),  // u
	CharNode(Token::IdentifierToken),  // v
	CharNode(Token::IdentifierToken),  // w
	CharNode(Token::IdentifierToken),  // x
	CharNode(Token::IdentifierToken),  // y
	CharNode(Token::IdentifierToken),  // z
	CharNode(Token::LCurlyOperand),    // {
	CharNode(Token::BinaryOrOperand),  // |
	CharNode(Token::RCurlyOperand),    // }
	CharNode(Token::BinaryNotOperand), // ~
};


std::string Source::read(SourcePosition start, SourcePosition end)
{
	const u32 SIZE = end.offset() - start.offset() + 1;
	return std::string(m_start + start.offset(), SIZE);
}


void Source::open()
{
	m_stream.open(m_path, std::ios::binary);
	m_stream.seekg(0, std::ios::end);
	const u32 SIZE = m_stream.tellg();

	m_start = new char[SIZE + 1];
	m_current = m_start;
	m_end = m_start + SIZE;
	*m_end = '\0';

	m_stream.seekg(0, std::ios::beg);
	m_stream.read(m_start, SIZE);

	m_stream.close();

	m_data = s_chars_map[*m_current];
}


void Source::close()
{
	
}


char Source::eat()
{
	char c = *m_current;

	++m_current;

	if (m_current == m_end)
	{
		m_data.m_type = Token::EOFToken;
		return c;
	}

	m_data = s_chars_map[*m_current];
	return c;
}

