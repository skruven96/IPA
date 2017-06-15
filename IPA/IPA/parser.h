#ifndef PARSER_H
#define PARSER_H
#include "error.h"
#include "tokenizer.h"
#include "nodes.h"
#include "stmt.h"

#include <stack>
#include <forward_list>


class Parser
{
public:
	Parser(Tokenizer& tokenizer, Module* module);

	void parse();

private:
	void parse_imports();
	Variable* parse_variable(Token name = Token());
	Function* parse_function();
	Class* parse_class();
	Widget* parse_widget();

	void parse_block(Block* block);

	void parse_if_stmt(bool is_elif = false);
	void parse_for_stmt();
	void parse_while_stmt();
	void parse_return_stmt();
	void parse_try_stmt();
	void parse_raise_stmt();

	Value* parse_term(Value* term);
	Value* parse_value(Value* target = nullptr);
	Value* parse_identifier();
	Value* parse_number();
	Value* parse_call(Value* callable);

	Type* parse_type();
	std::vector<Token> parse_path();


	void push_node(Node* node)
	{
		m_nodes.push(m_current_node);
		m_current_node = node;
		m_current_block = nullptr;

		switch (node->node_type())
		{
		case(Node::BlockType):
			m_current_block = static_cast<Block*>(node);
		}
		
	}
	void pop_node()
	{
		m_current_node = m_nodes.top();
		m_current_block = nullptr;

		m_nodes.pop();

		switch (m_current_node->node_type())
		{
		case(Node::BlockType):
			m_current_block = static_cast<Block*>(m_current_node);
		}
	}


	Token required(Token::TokenType data);
	Token required_keyword(Token::Keyword data);
	Token required_operand(Token::Operand data);

	bool optional(Token::TokenType data);
	bool optional_keyword(Token::Keyword data);
	bool optional_operand(Token::Operand data);

	bool optional(Token::TokenType data, Token& token);


	bool m_found_colon;

	Project* m_project;
	Module* m_module;
	Tokenizer& m_tokenizer;

	std::stack<Node*> m_nodes;

	Block* m_current_block;
	Node* m_current_node;
};


#endif // PARSER_H