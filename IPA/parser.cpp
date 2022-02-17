#include "compiler.h"
#include "tokenizer.h"
#include "project.h"

#include <assert.h>
#include <iostream>


Parser::Parser(ModuleCompiler* module_compiler)
	: m_tokenizer(module_compiler), m_project(module_compiler->module()->project()), m_module_compiler(module_compiler) {
	m_allocator = module_compiler->allocator();
}

void Parser::parse() {
	if (m_module_compiler->encountered_error())
		return;

	m_ctx.scope = m_module_compiler->scope();
	while (!m_tokenizer.peek().is(TokenType::EOFToken) && !m_should_stop) {
		Token token = m_tokenizer.peek();
		if (token.is(Keyword::ImportKeyword) || token.is(Keyword::FromKeyword))
			parse_imports();
		else if (token.is(TokenType::IdentifierToken) || token.is(Operand::AtOperand))
			parse_decleration();
		else {
			raise_error_and_stop()
				->message("Unexpected token in global scope, only imports and declerations allowed. ")
				->highlight_token(token);
		}
	}

	assert(m_ctx.first_stmt_in_block_index == 0);
	assert(m_ctx.first_decl_in_scope_index == 0);
	assert(m_ctx.first_local_variable_in_scope_index == 0);
	assert(m_ctx.first_local_variable_in_function_index == 0);
	m_module_compiler->scope()->fill_out_declerations(m_allocator, &m_declerations_stack[0], (i32)m_declerations_stack.size());
}

void Parser::parse_imports() {
	bool is_import = optional(Keyword::ImportKeyword);
	if (!is_import) required(Keyword::FromKeyword);

	bool first_iteration = true;
	bool is_relative = optional(Operand::DotOperand);
	std::string path;

	Token error_token;
	do {
		Token identifier = optional(TokenType::IdentifierToken);
		if (!identifier) error_token = m_tokenizer.peek();
		if (!first_iteration) path += '/';
		path += identifier.to_str();
		first_iteration = false;
	} while (optional(Operand::DotOperand) || error_token);
	Module* module = m_module_compiler->compiler()->resolve_module(path);

	if (is_import) {
		do {
			if (!error_token) {
				Token as;
				if (optional(Keyword::AsKeyword)) {
					as = optional(TokenType::IdentifierToken);
					if (!as) error_token = m_tokenizer.peek();
				}
				if (!error_token) {
					m_module_compiler->import(module, as);
				}
			}
			if (error_token) {
				raise_error_and_continue_after_line()
					->message("Unexpected ")->message(Token::TokenTypeToString(error_token.type()))->message(" token, import should be in the format:")
					->message("import <path>[as <identifier>][, <path>[as <identifier>]]...")
					->highlight_token(error_token);
				return;
			}
		} while (optional(Operand::CommaOperand));

	} else {
		if (!error_token) {
			if (!optional(Keyword::ImportKeyword))
				error_token = m_tokenizer.peek();
			else if (optional(Operand::MulOperand)) {
				m_module_compiler->import_everything(module);
			} else {
				do {
					Token what = optional(TokenType::IdentifierToken);
					Token as;
					if (!what) error_token = m_tokenizer.peek();
					else if (optional(Keyword::AsKeyword)){
						as = optional(TokenType::IdentifierToken);
						if (!as) error_token = m_tokenizer.peek();
					}
					if (error_token)
						break;
					else
						m_module_compiler->import_from(module, what, as);
				} while (optional(Operand::CommaOperand));
			}
		}
		if (error_token) {
			raise_error_and_continue_after_line()
				->message("Unexpected ")->message(Token::TokenTypeToString(error_token.type()))->message(" token, import from should be in the format:")
				->message("from <path> import <identifier>[as <identifier>][, <identifier>[as <identifier>]]...")
				->highlight_token(error_token);
		}
	}

	required_stmt_end_or_raise_garbage_error_and_continue_after_line();
	return;
}

void Parser::parse_decleration() {
	std::vector<Token> attributes;
	std::vector<bool> used_attributes;

	auto has_attribute = [&](const char* attr) -> Token {
		Token attr_token(TokenType::ErrorToken, attr, (i32)strlen(attr), -1);
		for (i32 i = 0; i < attributes.size(); i++) {
			if (attributes[i].equals(attr_token)) {
				used_attributes[i] = true;
				return attributes[i];
			}
		}
		return Token();
	};

	while (optional(Operand::AtOperand)) {
		attributes.push_back(required(TokenType::IdentifierToken));
		used_attributes.push_back(false);
		optional(TokenType::StmtEndToken);
	}

	Token name = required(TokenType::IdentifierToken);
	if (!name) 
		return;

	if (!required(Operand::ColonOperand))
		return;

	u32 flags = 0;
	bool is_constant = optional(Operand::ColonOperand);
	bool expect_default_value = is_constant ? true : optional(Operand::SetOperand);
	ast::Type* type = nullptr;
	if (!expect_default_value) {
		type = parse_type();
		is_constant = optional(Operand::ColonOperand);
		expect_default_value = is_constant ? true : optional(Operand::SetOperand);
	}

	if (is_constant)
		flags |= ast::Decl::CONST;
	if (!has_attribute("global")) {
		if (m_ctx.decl && m_ctx.decl->as_or_null<ast::Function>()) {
			flags |= ast::Decl::LOCAL;
		} else if (m_ctx.decl && m_ctx.decl->as_or_null<ast::Struct>()) {
			flags |= ast::Decl::MEMBER;
		} else {
			flags |= ast::Decl::GLOBAL;
		}
	} else {
		flags |= ast::Decl::GLOBAL;
	}

	if (Token token = has_attribute("export")) {
		if (m_ctx.decl) {
			raise_error_and_continue()
				->message("The 'export' attribute can only be used in file scope. ")
				->highlight_token(token);
		} else {
			// TODO: Add to export scope.
		}
	}

	ast::Decl* decl;
	if (expect_default_value) {
		if (optional(Keyword::StructKeyword)) {
			required(Operand::ColonOperand);
			required(TokenType::StmtEndToken);
			required(TokenType::ScopeBegToken);

			ast::Struct* structure = ast::Struct::Create(m_allocator, name);
			structure->scope = ast::Scope::Create(m_allocator, m_ctx.scope);
			Context ctx = update_context(structure);

			if (optional(Keyword::PassKeyword)) {
				required(TokenType::StmtEndToken);
			}
			else {
				do {
					parse_decleration();
				} while (!m_tokenizer.peek().is(TokenType::ScopeEndToken) && !m_should_stop);
			}

			required(TokenType::ScopeEndToken);

			ast::Decl** decls; i32 decls_count;
			get_decls(decls, decls_count);
			structure->scope->fill_out_declerations(m_allocator, decls, decls_count);

			restore_context(ctx);
			decl = structure;
		} else if (m_tokenizer.peek(0).is(Operand::LPharenthesesOperand) &&
			m_tokenizer.peek(1).is(TokenType::IdentifierToken) && 
			m_tokenizer.peek(2).is(Operand::ColonOperand)) {
			std::vector<ast::Variable*> arguments;
			required(Operand::LPharenthesesOperand);
			if (!optional(Operand::LPharenthesesOperand)) {
				do {
					Token name = required(TokenType::IdentifierToken);
					if (!name) return;
					if (!required(Operand::ColonOperand)) return;
					ast::Type* type = parse_type();
					arguments.push_back(ast::Variable::Create(m_allocator, type, name, nullptr, ast::Decl::LOCAL));
				} while (optional(Operand::CommaOperand));
				required(Operand::RPharenthesesOperand);
			}
			ast::Type* return_type = nullptr;
			if (optional(Operand::ArrowOperand)) {
				return_type = parse_type();
			}

			ast::Function* function = ast::Function::Create(m_allocator, name, &arguments[0], (i32)arguments.size(), return_type);
			Context ctx = update_context(function);

			for (auto arg : arguments)
				add_local_variables_or_return_false(arg);

			function->body = parse_block();

			restore_context(ctx);
			decl = function;
		} else {
			ast::Expr* expr = parse_expr();
			required_stmt_end_or_raise_garbage_error_and_continue_after_line();

			ast::Variable* var = ast::Variable::Create(m_allocator, type, name, expr, flags);

			if ((flags & ast::Variable::LOCAL) == ast::Variable::LOCAL) {
				ast::LoadExpr* var_expr = ast::LoadExpr::CreateLoadVariable(m_allocator, var);
				ast::OperandExpr* assign_expr = ast::OperandExpr::Create(m_allocator, Operand::SetOperand, var_expr, expr);
				ast::ExprStmt* stmt = ast::ExprStmt::Create(m_allocator, assign_expr);
				m_statements_stack.push_back(stmt);
			}

			decl = var;
		}
	} else {
		decl = ast::Variable::Create(m_allocator, type, name, nullptr, flags);

		required_stmt_end_or_raise_garbage_error_and_continue_after_line();
	}

	for (i32 i = 0; i < attributes.size(); i++) {
		if (!used_attributes[i]) {
			raise_error_and_continue()
				->message("Unrecognised attribute. ")
				->highlight_token(attributes[i]);
		}
	}

	if (decl->as_or_null<ast::Variable>() && decl->decl_flags & ast::Decl::LOCAL) {
		add_local_variables_or_return_false(decl->as_or_assert<ast::Variable>());
	} else {
		add_decleration_or_return_false(decl);
	}
}

ast::Type* Parser::parse_type() {
	ast::Type* result = nullptr;
	Token primitive = optional(TokenType::PrimitiveToken);
	if (primitive) {
		return ast::Type::GetPrimitiveOrAssert(primitive.primitive());
	} else {
		assert(false);
	}

	return result;
}

ast::Block* Parser::parse_block() {
	ast::Scope* scope = ast::Scope::Create(m_allocator, m_ctx.scope);
	Context old_ctx = update_context(scope);

	required(Operand::ColonOperand);
	required(TokenType::StmtEndToken);
	required(TokenType::ScopeBegToken);
	if (optional(Keyword::PassKeyword)) {
		required(TokenType::StmtEndToken);
	} else {
		while (!m_should_stop) {
			Token token = m_tokenizer.peek();
			if (token.is(Keyword::ReturnKeyword))
				parse_return_stmt();
			else if (token.is(Keyword::IfKeyword))
				parse_if_stmt();
			else if (token.is(Keyword::WhileKeyword))
				parse_while_stmt();
			else if (token.is(Keyword::ForKeyword))
				parse_for_stmt();
			else if ((token.is(TokenType::IdentifierToken) && m_tokenizer.peek(1).is(Operand::ColonOperand)) ||
				token.is(Operand::AtOperand)) {
				parse_decleration();
			} else if (token.is(TokenType::ScopeEndToken))
				break;
			else {
				ast::Expr* expr = parse_expr();
				required_stmt_end_or_raise_garbage_error_and_continue_after_line();
				if (expr) {
					m_statements_stack.push_back(ast::ExprStmt::Create(m_allocator, expr));
				}
			}
		}
	}
	required(TokenType::ScopeEndToken);

	ast::Decl** decls; i32 decls_count;
	get_decls(decls, decls_count);
	scope->fill_out_declerations(m_allocator, decls, decls_count);

	ast::Variable** local_variables; i32 local_variables_count;
	ast::Stmt** stmts; i32 stmts_count;
	get_local_variables(local_variables, local_variables_count);
	get_stmts(stmts, stmts_count);
	ast::Block* result = ast::Block::Create(m_allocator, scope, local_variables, local_variables_count, stmts, stmts_count);
	restore_context(old_ctx);
	return result;
}

void Parser::parse_if_stmt(bool is_elif) {
	required(is_elif ? Keyword::ElifKeyword : Keyword::IfKeyword);

	ast::Expr* condition = parse_expr();
	ast::Block* true_block = parse_block();
	ast::Block* false_block = nullptr;

	if (m_tokenizer.peek().is(Keyword::ElifKeyword)) {
		i32 index_of_stmts = (i32)m_statements_stack.size();

		parse_if_stmt(true);

		i32 stmts_count = (i32)m_statements_stack.size() - index_of_stmts;
		ast::Stmt** stmts = &m_statements_stack[index_of_stmts];
		assert(stmts_count == 1);

		false_block = ast::Block::Create(m_allocator, m_ctx.scope, nullptr, 0, stmts, stmts_count);
		m_statements_stack.resize(index_of_stmts);
	} else if (optional(Keyword::ElseKeyword)) {
		false_block = parse_block();
	}

	ast::IfStmt* stmt = ast::IfStmt::Create(m_allocator, condition, true_block, false_block);
	m_statements_stack.push_back(stmt);
}

void Parser::parse_for_stmt() {
	required(Keyword::ForKeyword);
	
	ast::Expr* first_expr = parse_expr();
	ast::Expr* upper_expr = nullptr;
	if (optional(Operand::DotDotOperand)) {
		upper_expr = parse_expr();
	}

	if (optional(Keyword::AsKeyword)) {
		Token it_name = required(TokenType::IdentifierToken);
		if (optional(Operand::CommaOperand))
			Token index_name = required(TokenType::IdentifierToken);
	}

	ast::Block* block = parse_block();

	ast::ForStmt* stmt = upper_expr ? 
		ast::ForStmt::Create(m_allocator, first_expr, upper_expr, block) :
		ast::ForStmt::Create(m_allocator, first_expr, block);
	m_statements_stack.push_back(stmt);
}

void Parser::parse_while_stmt() {
	required(Keyword::WhileKeyword);

	ast::Expr* condition = parse_expr();
	ast::Block* loop_body = parse_block();

	ast::WhileStmt* stmt = ast::WhileStmt::Create(m_allocator, condition, loop_body);
	m_statements_stack.push_back(stmt);
}

void Parser::parse_return_stmt() {
	required(Keyword::ReturnKeyword);

	ast::Expr* expr = nullptr;
	if (!optional(TokenType::StmtEndToken)) {
		expr = parse_expr();
		required(TokenType::StmtEndToken);
	}

	ast::ReturnStmt* stmt = ast::ReturnStmt::Create(m_allocator, expr);
	m_statements_stack.push_back(stmt);
}

ast::Expr* Parser::parse_expr(int* precedens_lvl_out) {
	ast::Expr* expr = parse_unary_prefix_operators();

	int precedens_level = -1;
	Operand operand = m_tokenizer.peek().operand();
	switch (operand) {
	case(Operand::MulOperand): case(Operand::DivOperand): case(Operand::ModOperand):
		precedens_level = 1; goto valid_operand;
	case(Operand::AddOperand): case(Operand::SubOperand):
		precedens_level = 2; goto valid_operand;
	case(Operand::LShiftOperand): case(Operand::RShiftOperand):
		precedens_level = 3; goto valid_operand;
	case(Operand::LtOperand): case(Operand::GtOperand): case(Operand::LesserEqualsOperand): case(Operand::GreaterEqualsOperand):
		precedens_level = 4; goto valid_operand;
	case(Operand::EqualsOperand):   case(Operand::NotEqualsOperand):
		precedens_level = 5; goto valid_operand;
	case(Operand::BinaryAndOperand):
		precedens_level = 6; goto valid_operand;
	case(Operand::BinaryXorOperand):
		precedens_level = 7; goto valid_operand;
	case(Operand::BinaryOrOperand): 
		precedens_level = 8; goto valid_operand;
	case(Operand::AndOperand):
		precedens_level = 9; goto valid_operand;
	case(Operand::OrOperand):
		precedens_level = 10; goto valid_operand;
	case(Operand::SetOperand):
	case(Operand::AddSetOperand):
	case(Operand::SubSetOperand):
	case(Operand::MulSetOperand):
	case(Operand::DivSetOperand):
	case(Operand::ModSetOperand):
		precedens_level = 11; goto valid_operand;

	valid_operand:
		m_tokenizer.eat();
		int rhs_precedens_level;
		ast::Expr* rhs = parse_expr(&rhs_precedens_level);
		ast::OperandExpr* this_operand = ast::OperandExpr::Create(m_allocator, operand, expr, rhs);
		if (precedens_level < rhs_precedens_level) {
			ast::OperandExpr* rhs_operand = rhs->as_or_assert<ast::OperandExpr>();
			ast::Expr* temp = rhs_operand->lhs;
			rhs_operand->lhs = this_operand;
			this_operand->rhs = temp;
			expr = rhs;
		} else {
			expr = this_operand;
		}
		break;
	}

	if (precedens_lvl_out)
		*precedens_lvl_out = precedens_level;
	return expr;
}

ast::Expr* Parser::parse_unary_prefix_operators() {
	ast::Expr* expr = nullptr;
	Token token = m_tokenizer.peek();
	if (token.is(Operand::LPharenthesesOperand)) {
		m_tokenizer.eat();
		expr = parse_expr();
		required(Operand::RPharenthesesOperand);
		expr = parse_unary_postfix_operators(expr);
	} else {
		switch (token.operand()) {
		case(Operand::AddOperand): case(Operand::SubOperand):
		case(Operand::IncrementOperand): case(Operand::DecrementOperand):
		case(Operand::NotOperand): case(Operand::BinaryNotOperand): {
			Token operand = m_tokenizer.eat();
			ast::Expr* value = parse_unary_prefix_operators();
			if (!value) return nullptr;
			expr = ast::OperandExpr::Create(m_allocator, operand.operand(), nullptr, value);
		} break;
		default:
			if (token.is(TokenType::IdentifierToken)) {
				Token identifier = required(TokenType::IdentifierToken);
				if (!identifier) return nullptr;
				ast::Variable* variable = get_local_variable_or_null(identifier);
				if (!variable) {
					ast::LoadExpr* load_expr = ast::LoadExpr::CreateLoadVariable(m_allocator, nullptr);
					m_module_compiler->add_link(load_expr, m_ctx.scope, identifier);
					expr = load_expr;
				} else {
					expr = ast::LoadExpr::CreateLoadVariable(m_allocator, variable);
				}
				expr = parse_unary_postfix_operators(expr);
			} else if (!token.is(TokenType::NumberToken) && !token.is(TokenType::StringToken) &&
				!token.is(Keyword::TrueKeyword) && !token.is(Keyword::FalseKeyword)) {
				raise_error_and_continue_after_line()
					->message("Unexpected token, expected a constant or identifier. ")
					->highlight_token(token);
				return nullptr;
			} else {
				expr = ast::LoadExpr::CreateLoadConstant(m_allocator, m_tokenizer.eat());
			}
			break;
		}
	}
	return expr;
}

ast::Expr* Parser::parse_unary_postfix_operators(ast::Expr* expr) {
	Token token = m_tokenizer.peek();
	if (token.is(Operand::IncrementOperand) || token.is(Operand::DecrementOperand)) {
		expr = ast::OperandExpr::Create(m_allocator, m_tokenizer.eat().operand(), expr, nullptr);
		expr = parse_unary_postfix_operators(expr);
	} else if (token.is(Operand::DotOperand)) {
		m_tokenizer.eat();
		Token member_name = required(TokenType::IdentifierToken);
		expr = ast::LoadExpr::CreateLoadMember(m_allocator, expr, nullptr);
		expr = parse_unary_postfix_operators(expr);
	} else if (token.is(Operand::LPharenthesesOperand)) {
		std::vector<ast::Expr*> arguments;
		required(Operand::LPharenthesesOperand);
		if (!optional(Operand::RPharenthesesOperand)) {
			do {
				arguments.push_back(parse_expr());
			} while (optional(Operand::CommaOperand));
			required(Operand::RPharenthesesOperand);
		}
		expr = ast::CallExpr::Create(m_allocator, expr, &arguments[0], (i32)arguments.size());
		expr = parse_unary_postfix_operators(expr);
	} else if (token.is(Operand::LSquareBracketOperand)) {
		required(Operand::LSquareBracketOperand);
		ast::Expr* index_expr = parse_expr();
		required(Operand::RSquareBracketOperand);

		expr = ast::ArrayAccessExpr::Create(m_allocator, expr, index_expr);
		expr = parse_unary_postfix_operators(expr);
	}
	return expr;
}

bool Parser::add_decleration_or_return_false(ast::Decl* decl) {
	bool is_local_scope = m_ctx.decl && m_ctx.decl->as_or_null<ast::Function>() != nullptr;
	ast::Variable* var = decl->as_or_null<ast::Variable>();
	if (is_local_scope && var) {
		return add_local_variables_or_return_false(var);
	} else {
		for (int i = m_ctx.first_decl_in_scope_index; i < m_declerations_stack.size(); i++) {
			if (m_declerations_stack[i]->name.equals(decl->name)) {
				raise_error_and_continue()
					->message("Redcleration of '")->message(decl->name.to_str().c_str())->message("', original decleration here:")
					->highlight_token(m_declerations_stack[i]->name)
					->highlight_token(decl->name);
				return false;
			}
		}
		m_declerations_stack.push_back(decl);
	}
	return true;
}

bool Parser::add_local_variables_or_return_false(ast::Variable* variable) {
	int index = m_ctx.first_local_variable_in_function_index;
	while (index < m_local_variables_stack.size()) {
		if (m_local_variables_stack[index]->name.equals(variable->name))
			break;
		++index;
	}
	if (index == m_local_variables_stack.size()) {
		m_local_variables_stack.push_back(variable);
		return true;
	}
	raise_error_and_continue()
		->message("Local variable with name '")->message(variable->name.to_str())->message("' already exists, original declared here:")
		->highlight_token(m_local_variables_stack[index]->name)->highlight_token(variable->name);
	return false;
}

ast::Variable* Parser::get_local_variable_or_null(const Token& identifier) {
	i32 index = (i32)m_local_variables_stack.size() - 1;
	while (index >= m_ctx.first_local_variable_in_function_index) {
		if (m_local_variables_stack[index]->name.equals(identifier))
			return m_local_variables_stack[index];
		--index;
	}
	return nullptr;
}

Token Parser::required(TokenType data) {
	Token token(m_tokenizer.eat());
	if (token.type() != data) {
		raise_error_and_stop()
			->message("Expected token '")->message(Token::TokenTypeToString(data))->message("' got ")->message(Token::TokenTypeToString(token.type()))->message(", error token detected here : ")
			->highlight_token(token);
		return Token();
	}
	return token;
}

Token Parser::required(Keyword data) {
	Token token(m_tokenizer.eat());
	if (token.keyword() != data) {
		raise_error_and_stop()
			->message("Expected keyword '")->message(Token::KeywordToString(data))->message("' got ")->message(Token::TokenTypeToString(token.type()))->message(", error token detected here : ")
			->highlight_token(token);
		return Token();
	}
	return token;
}

Token Parser::required(Operand data) {
	Token token(m_tokenizer.eat());
	if (token.operand() != data) {
		raise_error_and_stop()
			->message("Unexpected ")->token_type(token.type())->message(", expected ")->operand(data)
			->highlight_token(token);
		return Token();
	}
	return token;
}

Token Parser::optional(TokenType data) {
	return m_tokenizer.peek().type() == data ? m_tokenizer.eat() : Token();
}

Token Parser::optional(Keyword data) {
	return m_tokenizer.peek().keyword() == data ? m_tokenizer.eat() : Token();
}

Token Parser::optional(Operand data) {
	return m_tokenizer.peek().operand() == data ? m_tokenizer.eat() : Token();
}

CompilerError* Parser::raise_error_and_continue() {
	return m_module_compiler->raise_error();
}

CompilerError* Parser::raise_error_and_stop() {
	m_should_stop = true;
	return m_module_compiler->raise_error();
}

CompilerError* Parser::raise_error_and_continue_after_line() {
	Token token = m_tokenizer.peek();
	while (!token.is(TokenType::StmtEndToken) && !token.is(TokenType::EOFToken)) {
		m_tokenizer.eat();
		token = m_tokenizer.peek();
	}
	if (token.is(TokenType::StmtEndToken)) m_tokenizer.eat();
	return m_module_compiler->raise_error();
}

void Parser::required_stmt_end_or_raise_garbage_error_and_continue_after_line() {
	if (!optional(TokenType::StmtEndToken)) {
		raise_error_and_continue_after_line()
			->message("Garbage at end of line, expected a stmt-end token. ")
			->highlight_token(m_tokenizer.peek());
	}
}

Parser::Context Parser::update_context(ast::Scope* scope) {
	Context old_ctx = m_ctx;
	m_ctx.scope = scope;
	m_ctx.first_decl_in_scope_index = (i32)m_declerations_stack.size();
	m_ctx.first_stmt_in_block_index = (i32)m_statements_stack.size();
	m_ctx.first_local_variable_in_scope_index = (i32)m_local_variables_stack.size();
	return old_ctx;
}

Parser::Context Parser::update_context(ast::Function* function) {
	Context old_ctx = m_ctx;
	m_ctx.decl = function;
	m_ctx.first_decl_in_scope_index = (i32)m_declerations_stack.size();
	m_ctx.first_stmt_in_block_index = (i32)m_statements_stack.size();
	m_ctx.first_local_variable_in_scope_index = (i32)m_local_variables_stack.size();
	return old_ctx;
}

Parser::Context Parser::update_context(ast::Struct* structure) {
	Context old_ctx = m_ctx;
	m_ctx.decl  = structure;
	m_ctx.scope = structure->scope;
	m_ctx.first_decl_in_scope_index = (i32)m_declerations_stack.size();
	m_ctx.first_stmt_in_block_index = (i32)m_statements_stack.size();
	m_ctx.first_local_variable_in_scope_index = (i32)m_local_variables_stack.size();
	return old_ctx;
}

void Parser::get_decls(ast::Decl**& decls, i32& count) {
	count = (i32)m_declerations_stack.size() - m_ctx.first_decl_in_scope_index;
	decls = count == 0 ? nullptr : &m_declerations_stack[m_ctx.first_decl_in_scope_index];
}

void Parser::get_stmts(ast::Stmt**& stmts, i32& count) {
	count = (i32)m_statements_stack.size() - m_ctx.first_stmt_in_block_index;
	stmts = count == 0 ? nullptr : &m_statements_stack[m_ctx.first_stmt_in_block_index];
}

void Parser::get_local_variables(ast::Variable**& variables, i32& count) {
	count = (i32)m_local_variables_stack.size() - m_ctx.first_local_variable_in_scope_index;
	variables = count == 0 ? nullptr : &m_local_variables_stack[m_ctx.first_local_variable_in_scope_index];
}

void Parser::restore_context(Context& ctx) {
	m_declerations_stack.resize(m_ctx.first_decl_in_scope_index);
	m_statements_stack.resize(m_ctx.first_stmt_in_block_index);
	m_local_variables_stack.resize(m_ctx.first_local_variable_in_scope_index);
	m_ctx = ctx;
}
