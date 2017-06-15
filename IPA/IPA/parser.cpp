#include "parser.h"
#include "nodes.h"
#include "links.h"

#include <assert.h>
#include <iostream>


Parser::Parser(Tokenizer& tokenizer, Module* module)
	: m_tokenizer(tokenizer), m_module(module), m_current_node(module), m_current_block(nullptr), m_found_colon(false)
{
	m_project = module->project();
}


void Parser::parse()
{
	try
	{
		while (true)
		{
			switch (m_tokenizer.peek().keyword())
			{
			case(Token::FromKeyword):
			case(Token::ImportKeyword):
				parse_imports();
				break;
			case(Token::ClassKeyword):
				parse_class();
				break;
			case(Token::DefKeyword):
				parse_function();
				break;
			default:
				if (parse_variable() == nullptr)
				{
					required(Token::EOFToken);
					return;
				}
				else
					required(Token::NewLineToken);
			}
		}
	}
	catch (SyntaxError*)
	{}
}


void Parser::parse_imports()
{
	if (optional_keyword(Token::ImportKeyword))
	{
		Token as;

		bool relative = optional_operand(Token::DotOperand);

		std::vector<Token> import = parse_path();

		if (optional_keyword(Token::AsKeyword))
			as = required(Token::IdentifierToken);

		required(Token::NewLineToken);

		m_module->import(relative, import, as);
	}
	else
	{
		bool relative = optional_operand(Token::DotOperand);
		std::vector<Token> from, import;
		Token as;

		from = parse_path();

		required_keyword(Token::ImportKeyword);

		if (!optional_operand(Token::MulOperand))
		{
			do
				import.push_back(required(Token::IdentifierToken));
			while (optional_operand(Token::CommaOperand));
		}

		if (optional_keyword(Token::AsKeyword))
			as = required(Token::IdentifierToken);

		required(Token::NewLineToken);

		m_module->import(relative, from, as, import);
	}
}


Variable* Parser::parse_variable(Token name)
{
	if (name.is_empty())
	{
		if (!optional(Token::IdentifierToken, name))
			return nullptr;

		required_operand(Token::ColonOperand);
	}

	Type* type = nullptr;
	Variable* variable = nullptr;

	Token set_operand;
	if (m_tokenizer.peek().is_set())
		set_operand = m_tokenizer.next();
	else
	{
		type = parse_type();

		if (m_tokenizer.peek().is_set())
			set_operand = m_tokenizer.next();
	}

	if (m_current_block != nullptr)
		variable = Variable::CreateLocal(m_current_block, type, name.value(m_module->source()));
	else
		variable = Variable::CreateStatic(m_current_node, type, name.value(m_module->source()));

	if (!set_operand.is_empty())
	{
		push_node(m_project->initialization_function()->block());
		variable->m_default_value = parse_value();

		if (variable->default_value() == nullptr)
			throw new SyntaxError(m_module->error_context(), m_module, set_operand);

		OperandStmt::Create(m_current_block, set_operand, variable, variable->default_value(), variable);
		pop_node();
	}

	return variable;
}


Class* Parser::parse_class()
{
	return nullptr;
}


Function* Parser::parse_function()
{
	required_keyword(Token::DefKeyword);

	Token name = required(Token::IdentifierToken);				// <name>
	Type* return_type = nullptr;

	std::string str_name(name.value(m_module->source()));
	Function* func = Function::CreateStatic(m_current_node, str_name);
	push_node(func->block());

	required_operand(Token::LPharenthesesOperand);				// (
	if (!optional_operand(Token::RPharenthesesOperand))			// [)]
	{
		while (true)
		{
			func->add_argument(parse_variable());
			if (!optional_operand(Token::CommaOperand)) break;	 // ,
		}
		required_operand(Token::RPharenthesesOperand);			 // )
	}
	if (optional_operand(Token::PointerOperand))				 // ->
		func->return_type(parse_type());						 // <type>
	required_operand(Token::ColonOperand);						 // :

	parse_block(func->block());
	pop_node();
	return func;
}


Widget* Parser::parse_widget()
{
	return nullptr;
}


void Parser::parse_block(Block* block)
{
	push_node(block);

	if (optional(Token::NewLineToken))
	{
		required(Token::ScopeBegToken);
		Value* val = nullptr;

		bool running = true;
		while (running)
		{
			switch (m_tokenizer.peek().keyword())
			{
			case Token::ClassKeyword:
				parse_class();
				break;
			case Token::DefKeyword:
				parse_function();
				break;
			case Token::ReturnKeyword:
				parse_return_stmt();
				break;
			case Token::WhileKeyword:
				parse_while_stmt();
				break;
			case Token::ForKeyword:
				parse_for_stmt();
				break;
			case Token::BreakKeyword:
				break;
			case Token::ContinueKeyword:
				break;
			case Token::IfKeyword:
				parse_if_stmt();
				break;
			case Token::TryKeyword:
				parse_try_stmt();
				break;
			case Token::RaiseKeyword:
				parse_raise_stmt();
				break;
			default:
				if (parse_value() != nullptr)
					required(Token::NewLineToken);
				else
					running = false;
			}
		}

		required(Token::ScopeEndToken);
	}
	else
	{
		switch (m_tokenizer.peek().keyword())
		{
		case Token::ReturnKeyword:
			parse_return_stmt();
			break;
		case Token::RaiseKeyword:
			parse_raise_stmt();
			break;
		default:
			parse_value();
		}
		required(Token::NewLineToken);
	}

	pop_node();
}


Value* Parser::parse_term(Value* term)
{
	if (optional_operand(Token::LPharenthesesOperand))
	{
		if (term == nullptr)
		{
			term = parse_value();
			required_operand(Token::RPharenthesesOperand);
		}
		else
			term = parse_call(term);
	}
	else if (term == nullptr)
	{
		Token operand;
		switch (m_tokenizer.peek().token_type())
		{
		case(Token::StringToken):
			term = Constant::String(m_current_block, m_tokenizer.next().value(m_module->source()));
			break;
		case(Token::NumberToken):
			term = parse_number();
			break;
		case(Token::IdentifierToken):
			term = parse_identifier();
			break;
		default:
			switch (m_tokenizer.peek().operand())
			{
			case(Token::SubOperand):
			case(Token::IncrementOperand):
			case(Token::DecrementOperand):
			case(Token::NotOperand):
			case(Token::BinaryNotOperand):
				operand = m_tokenizer.next();
				break;
			default:
				return term;
			}
		}

		if (!operand.is_empty())
		{
			term = parse_term(nullptr);
			if (term == nullptr)
				throw(new SyntaxError(m_module->error_context(), m_module, operand));

			if (operand.is_sub())
			{
				OperandStmt* stmt = OperandStmt::Create(m_current_block, operand, Constant::Integer(m_current_block, 0), term, nullptr);
				return stmt->target();
			}
			else
			{
				OperandStmt* stmt = OperandStmt::Create(m_current_block, operand, nullptr, term, nullptr);
				return stmt->target();
			}
		}
	}
	else if (optional_operand(Token::DotOperand))
	{
		Token member = required(Token::IdentifierToken);
		assert(false && "Load member. ");
	}
	else
		return term;

	return parse_term(term);
}


Value* Parser::parse_value(Value* target)
{
	Value* lhs = parse_term(nullptr);

	if (lhs == nullptr)
		return lhs;

	struct Instruction
	{
		Token m_operand;
		Value* m_rhs;
	};

	std::vector<Instruction> instructions;

	while (true)
	{
		Token operand;
		switch (m_tokenizer.peek().operand())
		{
		case Token::SetOperand:
		case Token::AddOperand:
		case Token::SubOperand:
		case Token::MulOperand:
		case Token::DivOperand:
		case Token::ModOperand:
		case Token::LtOperand:
		case Token::GtOperand:
		case Token::BinaryAndOperand:
		case Token::BinaryOrOperand:
		case Token::BinaryXorOperand:
		case Token::AddSetOperand:
		case Token::SubSetOperand:
		case Token::MulSetOperand:
		case Token::DivSetOperand:
		case Token::ModSetOperand:
		case Token::EqualsOperand:
		case Token::NotEqualsOperand:
		case Token::LesserEqualsOperand:
		case Token::GreaterEqualsOperand:
		case Token::BinaryLShiftOperand:
		case Token::BinaryRShiftOperand:
			optional(Token::OperandToken, operand);
		}

		Value* rhs = parse_term(nullptr);

		if (rhs == nullptr)
		{
			if (operand.is_empty())
				break;
			throw(new SyntaxError(m_module->error_context(), m_module, operand));
		}

		Instruction inst;
		inst.m_rhs = rhs;
		inst.m_operand = operand;
		instructions.push_back(inst);
	}

	if (instructions.empty())
		return lhs;

	i16 operator_presedence = 0;
	auto it = instructions.begin();

	while (!instructions.empty())
	{
		bool should_parse = false;
		switch (it->m_operand.operand())
		{
		case Token::SetOperand:
			if (operator_presedence == 8)
				should_parse = true;
			break;

		case Token::AddOperand:
		case Token::SubOperand:
			if (operator_presedence == 1)
				should_parse = true;
			break;

		case Token::MulOperand:
		case Token::DivOperand:
		case Token::ModOperand:
			if (operator_presedence == 0)
				should_parse = true;
			break;

		case Token::LtOperand:
		case Token::GtOperand:
			if (operator_presedence == 3)
				should_parse = true;
			break;

		case Token::BinaryAndOperand:
			if (operator_presedence == 5)
				should_parse = true;
			break;

		case Token::BinaryOrOperand:
			if (operator_presedence == 7)
				should_parse = true;
			break;

		case Token::BinaryXorOperand:
			if (operator_presedence == 6)
				should_parse = true;
			break;

		case Token::AddSetOperand:
		case Token::SubSetOperand:
		case Token::MulSetOperand:
		case Token::DivSetOperand:
		case Token::ModSetOperand:
			if (operator_presedence == 8)
				should_parse = true;
			break;

		case Token::EqualsOperand:
		case Token::NotEqualsOperand:
			if (operator_presedence == 4)
				should_parse = true;
			break;

		case Token::LesserEqualsOperand:
		case Token::GreaterEqualsOperand:
			if (operator_presedence == 3)
				should_parse = true;
			break;

		case Token::BinaryLShiftOperand:
		case Token::BinaryRShiftOperand:
			if (operator_presedence == 2)
				should_parse = true;
			break;
			
		default:
			continue;
		}

		if (should_parse)
		{
			OperandStmt* stmt = nullptr;
			if (it != instructions.begin())
			{
				Value* left = (it - 1)->m_rhs;

				if (instructions.size() == 1) // Last instruction
				{
					stmt = OperandStmt::Create(m_current_block, it->m_operand, left, it->m_rhs, target);
					return stmt->target();
				}
				else
					stmt = OperandStmt::Create(m_current_block, it->m_operand, left, it->m_rhs, nullptr);
				(it - 1)->m_rhs = stmt->target();
			}
			else
			{
				if (instructions.size() == 1) // Last instruction
				{
					stmt = OperandStmt::Create(m_current_block, it->m_operand, lhs, it->m_rhs, target);
					return stmt->target();
				}
				else
					stmt = OperandStmt::Create(m_current_block, it->m_operand, lhs, it->m_rhs, nullptr);
				lhs = stmt->target();
			}

			it = instructions.erase(it);
		}
		else
			++it;

		if (it == instructions.end())
		{
			++operator_presedence;
			it = instructions.begin();
		}
	}

	assert(false && "Unreachable code. ");
	return nullptr;
}


Value* Parser::parse_identifier()
{
	std::vector<Token> path = parse_path();

	if (path.size() == 1 && optional_operand(Token::ColonOperand))
	{
		if (m_tokenizer.peek().is_new_line())
			m_found_colon = true;
		else
			return parse_variable(*path.begin());
	}
	
	return UnresolvedValueLink::Create(m_module, m_current_node, path);
}


Value* Parser::parse_number()
{
	Token number = m_tokenizer.next();
	std::string str = number.value(m_module->source());

	if (str.find('.') != std::string::npos)
	{
		if (!str.find('f'));
		else if (!str.find('F'));
		else
		{
			return Constant::Double(m_current_block, std::stod(str));
		}

		return Constant::Float(m_current_block, std::stof(str.substr(0, str.size() - 1)));
	}

	return Constant::Integer(m_current_block, std::stoi(str));
}


Value* Parser::parse_call(Value* callable)
{
	std::vector<Value*> arguments;

	if (!optional_operand(Token::RPharenthesesOperand))
	{
		while (true)
		{
			Value* argument = parse_value();

			if (argument == nullptr)
				throw new SyntaxError(m_module->error_context(), m_module, m_tokenizer.peek());

			arguments.push_back(argument);

			if (!optional_operand(Token::CommaOperand))
				break;
		}

		required_operand(Token::RPharenthesesOperand);
	}

	CallStmt* stmt = CallStmt::Create(m_current_block, callable, arguments, nullptr);
	return stmt->target();
}


void Parser::parse_if_stmt(bool is_elif)
{
	if (is_elif == false)
		required_keyword(Token::IfKeyword);

	Value* condition = parse_value();

	if (m_found_colon)
		m_found_colon = false;
	else
		required_operand(Token::ColonOperand);

	IfStmt* stmt = IfStmt::Create(m_current_block, condition);

	parse_block(stmt->true_block());

	if (optional_keyword(Token::ElseKeyword))
	{
		required_operand(Token::ColonOperand);

		parse_block(stmt->false_block());
	}
	else if (optional_keyword(Token::ElifKeyword))
	{
		push_node(stmt->m_false_block);
		parse_if_stmt(true);
		pop_node();
	}
}


void Parser::parse_for_stmt()
{
	
}


void Parser::parse_while_stmt()
{
	required_keyword(Token::WhileKeyword);

	WhileStmt* stmt = WhileStmt::Create(m_current_block);

	push_node(stmt->block());
	Value* condition = parse_value();

	IfStmt* break_stmt = IfStmt::Create(m_current_block, condition);
	ControlFlowStmt::CreateBreak(break_stmt->m_false_block);
	pop_node();

	required_operand(Token::ColonOperand);

	parse_block(stmt->block());
}


void Parser::parse_return_stmt()
{
	required_keyword(Token::ReturnKeyword);

	if (optional(Token::NewLineToken))
		ControlFlowStmt::CreateReturn(m_current_block);
	else
	{
		Value* return_value = parse_value();
		ControlFlowStmt::CreateReturn(m_current_block, return_value);
		required(Token::NewLineToken);
	}
}


void Parser::parse_try_stmt()
{
	
}


void Parser::parse_raise_stmt()
{

}


Type* Parser::parse_type()
{
	Token primitive;
	if (optional(Token::PrimitiveToken, primitive))
		return Type::Primitive(m_project, primitive.primitive());

	std::vector<Token> path(parse_path());
	return UnresolvedTypeLink::Create(m_module, m_current_node, path);
}


std::vector<Token> Parser::parse_path()
{
	std::vector<Token> path;

	do
		path.push_back(required(Token::IdentifierToken));
	while (optional_operand(Token::DotOperand));

	return path;
}


Token Parser::required(Token::TokenType data)
{
	Token token(m_tokenizer.next());
	if (token.token_type() != data)
		throw new SyntaxError(m_module->error_context(), m_module, token);
	return token;
}


Token Parser::required_keyword(Token::Keyword data)
{
	Token token(m_tokenizer.next());
	if (token.keyword() != data)
		throw new SyntaxError(m_module->error_context(), m_module, token);
	return token;
}


Token Parser::required_operand(Token::Operand data)
{
	Token token(m_tokenizer.next());
	if (token.operand() != data)
		throw new SyntaxError(m_module->error_context(), m_module, token);
	return token;
}


bool Parser::optional(Token::TokenType data)
{
	Token token(m_tokenizer.peek());
	if (token.token_type() == data)
	{
		m_tokenizer.next();
		return true;
	}
	return false;
}


bool Parser::optional_keyword(Token::Keyword data)
{
	Token token(m_tokenizer.peek());
	if (token.keyword() == data)
	{
		m_tokenizer.next();
		return true;
	}
	return false;
}


bool Parser::optional_operand(Token::Operand data)
{
	Token token(m_tokenizer.peek());
	if (token.operand() == data)
	{
		m_tokenizer.next();
		return true;
	}
	return false;
}


bool Parser::optional(Token::TokenType data, Token& token)
{
	Token optional(m_tokenizer.peek());
	if (optional.token_type() == data)
	{
		token = m_tokenizer.next();
		return true;
	}
	return false;
}

