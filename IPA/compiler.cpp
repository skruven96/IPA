
#include "compiler.h"
#include "project.h"

#include "ast_printer.h"


#include <iostream>
#include <assert.h>


bool Link::resolve(Module* module, ast::Decl* decl) {
	bool resolved = false;
	if (m_target_decl) {
		*m_target_decl = decl;
		resolved = true;
	} else {
		if (decl->as_or_null<ast::Struct>()) {
			*m_target_type = decl->as_or_null<ast::Struct>()->type;
			resolved = true;
		} else
			assert(false);
	}
	return resolved;
}


CompilerError* CompilerError::Create(CompilerAllocator* allocator, Module* module) {
	CompilerError* memory = allocator->allocate_one<CompilerError>();
	return new (memory)(CompilerError)(allocator, module);
}

CompilerError::CompilerError(CompilerAllocator* allocator, Module* module) {
	m_next_part = nullptr;
	m_part_type = PartType::MODULE;
	m_allocator = allocator;
	m_module = module;
}

CompilerError::CompilerError(const char* message) {
	m_next_part = nullptr;
	m_part_type = PartType::MESSAGE;
	m_message = message;
}

CompilerError::CompilerError(Token token) {
	m_next_part = nullptr;
	m_part_type = PartType::TOKEN;
	m_token = token;
}

CompilerError::CompilerError(TokenType type) {
	m_next_part = nullptr;
	m_part_type = PartType::TOKEN_TYPE;
	m_token_type = type;
}

CompilerError::CompilerError(Operand operand) {
	m_next_part = nullptr;
	m_part_type = PartType::OPERAND;
	m_operand = operand;
}

CompilerError::CompilerError(Keyword keyword) {
	m_next_part = nullptr;
	m_part_type = PartType::KEYWORD;
	m_keyword = keyword;
}

CompilerError* CompilerError::token_type(TokenType type) {
	CompilerError* next_part = m_allocator->allocate_one<CompilerError>();
	new (next_part)(CompilerError)(type);
	append_part(next_part);
	return this;
}

CompilerError* CompilerError::operand(Operand operand) {
	CompilerError* next_part = m_allocator->allocate_one<CompilerError>();
	new (next_part)(CompilerError)(operand);
	append_part(next_part);
	return this;
}

CompilerError* CompilerError::keyword(Keyword keyword) {
	CompilerError* next_part = m_allocator->allocate_one<CompilerError>();
	new (next_part)(CompilerError)(keyword);
	append_part(next_part);
	return this;
}

CompilerError* CompilerError::message(const char* msg) {
	CompilerError* next_part = m_allocator->allocate_one<CompilerError>();
	new (next_part)(CompilerError)(m_allocator->copy_str(msg));
	append_part(next_part);
	return this;
}

CompilerError* CompilerError::message(const std::string& str) {
	CompilerError* next_part = m_allocator->allocate_one<CompilerError>();
	new (next_part)(CompilerError)(m_allocator->copy_str(str));
	append_part(next_part);
	return this;
}

CompilerError* CompilerError::highlight_token(Token token) {
	CompilerError* next_part = m_allocator->allocate_one<CompilerError>();
	new (next_part)(CompilerError)(token);
	append_part(next_part);
	return this;
}

#include <windows.h>

void CompilerError::print(std::ostream& stream) const {
	switch (m_part_type)
	{
	case(PartType::MODULE):
		stream << std::endl << m_module->path() << std::endl;
		break;
	case(PartType::MESSAGE):
		stream << m_message;
		break;
	case(PartType::TOKEN): {
		stream << "\n";
		const char* line_it = m_token.first_char_ptr();
		if (*line_it == '\n') --line_it;
		if (*line_it == '\r') --line_it;
		while (*line_it != '\n' && *line_it != '\r') --line_it;
		++line_it;

		stream << m_token.line_number() << ": ";

		int index = 0;
		while (line_it < m_token.first_char_ptr()) {
			if (*line_it == '\t')
				stream << "    ";
			else
				stream << *line_it;
			++line_it;
		}
		
		CONSOLE_SCREEN_BUFFER_INFO existing;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hConsole, &existing);
		SetConsoleTextAttribute(hConsole, 12);
		while (line_it < m_token.first_char_ptr() + m_token.length()) {
			stream << *line_it;
			++line_it;
		}
		SetConsoleTextAttribute(hConsole, existing.wAttributes);

		while (true) {
			if (*line_it == '\n' || *line_it == '\r')
				break;
			else if (*line_it == '\0') {
				stream << "\\0";
				break;
			} else
				stream << *line_it;
			++line_it;
		}
	}   break;
	case(PartType::TOKEN_TYPE):

		break;
	case(PartType::OPERAND):

		break;
	case(PartType::KEYWORD):

		break;
		
	default:
		assert(0);
	}
	if (m_next_part)
		m_next_part->print(stream);
	else
		stream << std::endl;
}

void CompilerError::append_part(CompilerError* part) {
	CompilerError* last_part = this;
	while (last_part->m_next_part) last_part = last_part->m_next_part;
	last_part->m_next_part = part;
}


CompilerAllocator::CompilerAllocator() {
	m_current_block = allocate_block(1024 * 1024);
}

CompilerAllocator::~CompilerAllocator() {
	Block* block = m_current_block;
	while (block->m_next != nullptr) block = block->m_next;
	while (block) {
		char* memory = (char*)block;
		block = block->m_prev;
		delete[] memory;
	}
}

const char* CompilerAllocator::copy_str(const char* str) {
	std::size_t size = strlen(str);
	char* result = allocate_array<char>((i32)size + 1);
	memcpy(result, str, size + 1);
	return result;
}

const char* CompilerAllocator::copy_str(const std::string& str) {
	return copy_str(str.c_str());
}

void* CompilerAllocator::allocate_aligned(i64 byte_count, i64 alignment) {
	assert(alignment == 1 || alignment == 2 || alignment == 4 || alignment == 8);
	auto offset = m_current_block->m_used;
	offset = (offset + alignment - 1) & ~(alignment - 1);
	if (offset + byte_count > m_current_block->m_size) {
		i64 new_block_size = 1024 * 1024;
		if (new_block_size < byte_count) new_block_size = byte_count;
		Block* new_block = allocate_block(new_block_size);

		m_current_block->m_next = new_block;
		new_block->m_prev = m_current_block;
		m_current_block = new_block;

		offset = 0;
	}
	m_current_block->m_used = offset + byte_count;
	return &m_current_block->m_data[offset];
}

CompilerAllocator::Block* CompilerAllocator::allocate_block(i64 size) {
	char* memory = new char[size + offsetof(Block, m_data)];
	Block* block = (Block*)memory;
	block->m_used = 0;
	block->m_size = size;
	block->m_next = nullptr;
	block->m_prev = nullptr;
	return block;
}


void TypeInferer::infer_types() {
	visit(m_module_compiler->scope());
}

bool TypeInferer::mark_visited(ast::Node* node) {
	if (node->been_visited)
		return true;
	node->been_visited = true;
	return false;
}

void TypeInferer::jump_to(ast::Node* node) {
	if (node->been_visited) {
		assert(false); // TODO: Find circular dependency.
	}

	accept(node);
}

void TypeInferer::visit(ast::Scope* scope) {
	for (i32 i = 0; i < scope->declerations_count; i++)
		accept(scope->declerations[i]);
}

void TypeInferer::visit(ast::Variable* variable) {
	if (mark_visited(variable))
		return; 
	accept(variable->default_value);
	if (!variable->type)
		variable->type = variable->default_value->type;
}

void TypeInferer::visit(ast::Struct* structure) {
	if (mark_visited(structure))
		return;
}

void TypeInferer::visit(ast::Function* function) {
	if (mark_visited(function))
		return; 
	accept(function->body);
}

void TypeInferer::visit(ast::Block* block) {
	if (mark_visited(block))
		return; 
	accept(block->scope);
	for (i32 i = 0; i < block->statements_count; i++)
		accept(block->statements[i]);
}

void TypeInferer::visit(ast::LoadExpr* expr) {
	if (mark_visited(expr))
		return; 
	if (expr->constant) {
		if (expr->constant.is(TokenType::NumberToken))
			expr->type = ast::Type::GetPrimitiveOrAssert(Primitive::S32Primitive);
		else if (expr->constant.is(TokenType::StringToken))
			assert(false);
		else if (expr->constant.is(Keyword::TrueKeyword) || expr->constant.is(Keyword::FalseKeyword))
			expr->type = ast::Type::GetPrimitiveOrAssert(Primitive::BoolPrimitive);
		else
			assert(false);
	} else if (ast::Variable* var = expr->loaded_decl->as_or_assert<ast::Variable>()) {
		if (!var->type)
			jump_to(var);
		expr->type = var->type;
	} else
		assert(false);
}

void TypeInferer::visit(ast::ReturnStmt* stmt) {
	if (mark_visited(stmt))
		return;
	accept(stmt->return_value);
}

void TypeInferer::visit(ast::OperandExpr* expr) {
	if (mark_visited(expr))
		return; 

	if (expr->operand == Operand::SetOperand) {
		// Store to lhs
		if (!expr->lhs->as_or_null<ast::LoadExpr>() && !expr->lhs->as_or_null<ast::ArrayAccessExpr>()) {
			m_module_compiler->raise_error()
				->message("");
		}

	} else if (expr->operand == Operand::AddSetOperand || expr->operand == Operand::SubSetOperand ||
		       expr->operand == Operand::MulSetOperand || expr->operand == Operand::DivSetOperand ||
		       expr->operand == Operand::ModSetOperand ||
		       expr->operand == Operand::IncrementOperand || expr->operand == Operand::DecrementOperand) {
		// Evaluate lhs for storing and also read it in.
	}

	accept(expr->lhs);
	accept(expr->rhs);

	if (expr->lhs->type == expr->rhs->type)
		expr->type = expr->lhs->type;
	else {
		ast::Type* type = expr->lhs->type->get_explicit_type_conversion_target_or_null(expr->rhs->type);
		if (type) {
			expr->type = type;
			if (expr->lhs->type == type)
				expr->rhs = ast::CastExpr::Create(m_module_compiler->allocator(), expr->rhs, type);
			else if (expr->rhs->type == type)
				expr->lhs = ast::CastExpr::Create(m_module_compiler->allocator(), expr->lhs, type);
		} else
			assert(false);
	}
}

void TypeInferer::visit(ast::CallExpr* expr) {
	if (mark_visited(expr))
		return;
	accept(expr->callable);
	for (i32 i = 0; i < expr->arguments_count; i++)
		accept(expr->arguments[i]);
}

void TypeInferer::visit(ast::ArrayAccessExpr* expr) {
	if (mark_visited(expr))
		return;
	accept(expr->array_expr);
	accept(expr->index_expr);
}

void TypeInferer::visit(ast::CastExpr* expr) {
	if (mark_visited(expr))
		return;
	accept(expr);
}


ModuleCompiler* ModuleCompiler::Create(Compiler* compiler, Module* module) {
	ModuleCompiler* memory = compiler->allocator()->allocate_one<ModuleCompiler>();
	return new (memory)(ModuleCompiler)(module, compiler);
}

ModuleCompiler::ModuleCompiler(Module* module, Compiler* compiler)
	: m_source(module->path()) {
	m_scope = ast::Scope::Create(&m_allocator, compiler->global_scope());
	m_module = module;
	m_compiler = compiler;
}

void ModuleCompiler::parse_and_link_internals() {
	{
		Parser parser(this);
		parser.parse();
	}

	for (i32 i = 0; i < m_unresolved_links.size(); i++) {
		bool found = false;
		Link* link = &m_unresolved_links[i];
		ast::Scope* scope = link->m_scope;
		while (scope && !found) {
			ast::Decl* decl = scope->get_decleration_or_null(link->m_identifier);
			if (decl) {
				if (!link->resolve(m_module, decl)) {
					m_compiler->add_link(link);
					found = true;
				}
			}
			scope = scope->parent;
		}
		if (!found)
			m_compiler->add_link(link);
	}
}

void ModuleCompiler::infer_types_and_do_semantic_analysis() {
	TypeInferer analyser(this);
	analyser.infer_types();
}

void ModuleCompiler::add_link(ast::LoadExpr* load_expr, ast::Scope* scope, const Token& name) {
	Link link = { 0 };
	link.m_target_decl = &load_expr->loaded_decl;
	link.m_identifier = name;
	link.m_scope = scope;
	m_unresolved_links.push_back(link);
}

void ModuleCompiler::import(Module* module, const Token& as) {

}

void ModuleCompiler::import_from(Module* module, const Token& identifier, const Token& as) {

}

void ModuleCompiler::import_everything(Module* module) {

}

CompilerError* ModuleCompiler::raise_error() {
	m_compiler->mark_encoutered_error();
	CompilerError* error = CompilerError::Create(&m_allocator, m_module);
	m_errors.push_back(error);
	return error;
}

void ModuleCompiler::print_errors(std::ostream& stream) {
	for (auto error : m_errors) {
		error->print(stream);
	}
}


Compiler::Compiler(Project* project, Runtime* runtime)
	: m_project(project), m_runtime(runtime), m_global_scope(nullptr) {
	m_encountered_errors = false;
}

Compiler::~Compiler() {
	for (ModuleCompiler* module_compiler : m_module_compilers) {
		module_compiler->~ModuleCompiler();
	}
}

void Compiler::compile() {
	for (auto it = m_project->modules_begin(); it != m_project->modules_end(); ++it) {
		ModuleCompiler* module_compiler = ModuleCompiler::Create(this, it->second);
		m_module_compilers.push_back(module_compiler);
	}

	while (m_module_compilers_index < m_module_compilers.size()) {
		ModuleCompiler* module_compiler = m_module_compilers[m_module_compilers_index++];

		if (m_project->settings().print_tokens) {
			Tokenizer tokenizer(module_compiler);
			Token token;

			std::cout << "Tokens for module '" << module_compiler->source()->path() << "':" << std::endl;
			do {
				token = tokenizer.eat();
				std::string type_str = Token::TokenTypeToString(token.type());
				std::cout << "    " << type_str;
				for (auto i = type_str.size(); i < 17; i++)
					std::cout << ' ';
				std::cout << token.to_str() << std::endl;
			} while (!token.is(TokenType::EOFToken));

		}

		module_compiler->parse_and_link_internals();
	}

	// Link remaining
	if (!encountered_error()) {
		m_module_compilers_index = 0;
		while (m_module_compilers_index < m_module_compilers.size()) {
			ModuleCompiler* module_compiler = m_module_compilers[m_module_compilers_index++];
			module_compiler->infer_types_and_do_semantic_analysis();
		}
	}

	for (auto module_compiler : m_module_compilers) {
		ASTPrinter printer(std::cout);
		printer.visit(module_compiler);
	}
}

void Compiler::add_link(Link* link) {

}

CompilerError* Compiler::raise_error() {
	mark_encoutered_error();
	return CompilerError::Create(&m_allocator, nullptr);
}

void Compiler::print_errors(std::ostream& stream) {
	for (auto error : m_errors) {
		error->print(stream);
	}
	for (auto module : m_module_compilers) {
		module->print_errors(stream);
	}
}

Module* Compiler::resolve_module(const std::string& path) {
	return nullptr;
}


void FunctionCompiler::compile() {
	
}

void FunctionCompiler::visit(ast::Function* function) {
	
}

void FunctionCompiler::visit(ast::Block* block) {
	for (i32 i = 0; i < block->statements_count; i++)
		accept(block->statements[i]);
}

void FunctionCompiler::visit(ast::IfStmt* expr) {

}

void FunctionCompiler::visit(ast::ForStmt* expr) {

}

void FunctionCompiler::visit(ast::WhileStmt* expr) {

}

void FunctionCompiler::visit(ast::ReturnStmt* expr) {
	if (!expr->return_value)
		m_opcodes.push_back(OpCode::OpReturnVoid);
	else {
		accept(expr->return_value);
		m_opcodes.push_back(OpCode::OpReturn);
	}
}

void FunctionCompiler::visit(ast::ExprStmt* expr) {
	accept(expr->expr);
	m_opcodes.push_back(OpCode::OpPop32);
}

void FunctionCompiler::visit(ast::LoadExpr* expr) {
	if (expr->constant) {

	}
	else if (expr->structure_expr) {

	}
	else {

	}
}

void FunctionCompiler::visit(ast::OperandExpr* expr) {

}

void FunctionCompiler::visit(ast::CallExpr* expr) {
	accept(expr->callable);
	for (i32 i = 0; i < expr->arguments_count; i++) {
		accept(expr->arguments[i]);
	}
	m_opcodes.push_back(OpCode::OpCall);
}

void FunctionCompiler::visit(ast::ArrayAccessExpr* expr) {
	accept(expr->array_expr);
	accept(expr->index_expr);
	
}

void FunctionCompiler::visit(ast::CastExpr* expr) {
	if (expr->expr->type->is_integer()) {
		if (expr->type->is_integer()) {

		}
	}
}
