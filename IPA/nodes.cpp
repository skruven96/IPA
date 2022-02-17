#include "project.h"
#include "compiler.h"

#include <limits>
#include <algorithm>
#include <assert.h>
#include <sys/stat.h>



// =========================================================================================================
// Scope
// =========================================================================================================

static std::size_t scope_hash(const char* str, int count) {
	std::size_t hash = 5381;
	for (int i = 0; i < count; i++)
		hash = ((hash << 5) + hash) + (int)str[i]; /* hash * 33 + c */
	return hash;
}

ast::Scope* ast::Scope::Create(CompilerAllocator* allocator, Scope* parent) {
	Scope* result = allocator->allocate_one<Scope>();
	result->parent = parent;
	result->declerations = nullptr;
	result->declerations_count = -1;
	result->declerations_map = nullptr;
	result->declerations_map_size = 0;
	return result;
}

ast::Decl* ast::Scope::get_decleration_or_null(const Token& name) {
	assert(declerations_count != -1 && "Declerations have not been filled out in this scope. ");
	if (declerations_map) {
		auto index = scope_hash(name.first_char_ptr(), name.length()) % declerations_map_size;
		while (declerations_map[index]) {
			if (declerations_map[index]->name.equals(name))
				return declerations_map[index];
			index = (index + 1) % declerations_map_size;
		}
	} else {
		for (i32 i = 0; i < declerations_count; i++) {
			if (declerations[i]->name.equals(name))
				return declerations[i];
		}
	}
	return nullptr;
}

ast::Decl* ast::Scope::get_decleration_or_null(const std::string& name) {
	Token name_token(TokenType::ErrorToken, name.c_str(), (i32)name.size(), -1);
	return get_decleration_or_null(name_token);
}

void ast::Scope::fill_out_declerations(CompilerAllocator* allocator, Decl** given_declerations, i32 count) {
	declerations = allocator->allocate_array<Decl*>(count);
	declerations_count = count;
	memcpy(declerations, given_declerations, sizeof(declerations[0]) * count);
	if (count < 32) {
		declerations_map_size = 0;
		declerations_map = nullptr;
	} else {
		declerations_map_size = (i32)(count * 1.2f);
		declerations_map = allocator->allocate_array<Decl*>(declerations_map_size);
		memset(declerations_map, 0, sizeof(declerations_map[0]) * declerations_map_size);
		for (i32 i = 0; i < declerations_map_size; i++) {
			auto& name_token = declerations[i]->name;
			auto index = scope_hash(name_token.first_char_ptr(), name_token.length()) % declerations_map_size;
			while (!declerations_map[index])
				index = (index + 1) % declerations_map_size;
			declerations_map[index] = declerations[i];
		}
	}
}


// =========================================================================================================
// Node
// =========================================================================================================

void ast::Node::init(u32 node_type) {
	this->node_type = node_type;
	this->been_visited = false;
}

void ast::Node::accept(Visitor* visitor) {
	switch (node_type)
	{
	case(Variable::s_node_type):        visitor->visit(this->as_or_assert<Variable>());        break;
	case(Struct::s_node_type):          visitor->visit(this->as_or_assert<Struct>());          break;
	case(Function::s_node_type):        visitor->visit(this->as_or_assert<Function>());        break;
	case(Block::s_node_type):           visitor->visit(this->as_or_assert<Block>());           break;


	case(IfStmt::s_node_type):          visitor->visit(this->as_or_assert<IfStmt>());          break;
	case(ForStmt::s_node_type):         visitor->visit(this->as_or_assert<ForStmt>());         break;
	case(WhileStmt::s_node_type):       visitor->visit(this->as_or_assert<WhileStmt>());       break;
	case(ReturnStmt::s_node_type):      visitor->visit(this->as_or_assert<ReturnStmt>());      break;
	case(ExprStmt::s_node_type):        visitor->visit(this->as_or_assert<ExprStmt>());        break;

	case(LoadExpr::s_node_type):        visitor->visit(this->as_or_assert<LoadExpr>());        break;
	case(OperandExpr::s_node_type):     visitor->visit(this->as_or_assert<OperandExpr>());     break;
	case(CallExpr::s_node_type):        visitor->visit(this->as_or_assert<CallExpr>());        break;
	case(ArrayAccessExpr::s_node_type): visitor->visit(this->as_or_assert<ArrayAccessExpr>()); break;
	case(CastExpr::s_node_type):        visitor->visit(this->as_or_assert<CastExpr>());        break;

	default:
		assert(0);
	}
}


// =========================================================================================================
// Block
// =========================================================================================================

void ast::Block::init(CompilerAllocator* allocator, Scope* scope, Variable** local_variables, i32 local_variables_count, Stmt** statements, i32 statements_count) {
	Node::init(s_node_type);
	this->scope = scope;
	this->local_variables = allocator->copy_array<Variable*>(local_variables, local_variables_count);
	this->local_variables_count = local_variables_count;
	this->statements = allocator->copy_array<Stmt*>(statements, statements_count);
	this->statements_count = statements_count;
}

ast::Block* ast::Block::Create(CompilerAllocator* allocator, Scope* scope,
					 Variable** local_variables, i32 local_variables_count, Stmt** statements, i32 statements_count) {
	Block* result = allocator->allocate_one<Block>();
	result->init(allocator, scope, local_variables, local_variables_count, statements, statements_count);
	return result;
}

// =========================================================================================================
// Decl
// =========================================================================================================

void ast::Decl::init(u32 node_type, const Token& name, u32 flags) {
	Node::init(node_type);
	this->name = name;
	this->decl_flags = flags;
}


// =========================================================================================================
// Variable
// =========================================================================================================

void ast::Variable::init(Type* type, const Token& name, Expr* default_value, u32 flags) {
	Decl::init(s_node_type, name, flags);
	this->type          = type;
	this->default_value = default_value;
}

ast::Variable* ast::Variable::Create(CompilerAllocator* allocator, Type* type, const Token& name, Expr* default_value, u32 flags) {
	Variable* var = allocator->allocate_one<Variable>();
	var->init(type, name, default_value, flags);
	return var;
}


// =========================================================================================================
// Struct
// =========================================================================================================

void ast::Struct::init(const Token& name) {
	Decl::init(s_node_type, name, GLOBAL | CONST);
	this->scope = nullptr;
}

ast::Struct* ast::Struct::Create(CompilerAllocator* allocator, const Token& name) {
	Struct* structure = allocator->allocate_one<Struct>();
	structure->init(name);
	return structure;
}


// =========================================================================================================
// Function
// =========================================================================================================

void ast::Function::init(CompilerAllocator* allocator, const Token& name, Variable** arguments, i32 arguments_count, Type* return_type) {
	Decl::init(s_node_type, name, GLOBAL | CONST);
	this->type            = nullptr;
	this->return_type     = return_type;
	this->arguments       = allocator->copy_array(arguments, arguments_count);
	this->arguments_count = arguments_count;
	this->body            = nullptr;
	this->next_overload   = nullptr;
}

ast::Function* ast::Function::Create(CompilerAllocator* allocator, const Token& name,
						   Variable** arguments, i32 arguments_count, Type* return_type) {
	Function* result = allocator->allocate_one<Function>();
	result->init(allocator, name, arguments, arguments_count, return_type);
	return result;
}


// =========================================================================================================
// Stmt
// =========================================================================================================

void ast::Stmt::init(u32 node_type) {
	Node::init(node_type);
}


// =========================================================================================================
// Expr
// =========================================================================================================

void ast::Expr::init(u32 node_type, Type* type) {
	Node::init(node_type);
	this->type = type;
}


// =========================================================================================================
// LoadExpr
// =========================================================================================================

void ast::LoadExpr::init(const Token& constant, Variable* variable, Expr* structure_expr) {
	Expr::init(s_node_type, nullptr);
	this->constant = constant;
	this->loaded_decl = variable;
	this->structure_expr = structure_expr;
}

ast::LoadExpr* ast::LoadExpr::CreateLoadVariable(CompilerAllocator* allocator, Variable* variable) {
	LoadExpr* expr = allocator->allocate_one<LoadExpr>();
	expr->init(Token(), variable, nullptr);
	return expr;
}

ast::LoadExpr* ast::LoadExpr::CreateLoadConstant(CompilerAllocator* allocator, const Token& constant) {
	LoadExpr* expr = allocator->allocate_one<LoadExpr>();
	expr->init(constant, nullptr, nullptr);
	return expr;

}

ast::LoadExpr* ast::LoadExpr::CreateLoadMember(CompilerAllocator* allocator, Expr* structure, Variable* member) {
	LoadExpr* expr = allocator->allocate_one<LoadExpr>();
	expr->init(Token(), member, structure);
	return expr;
}


// =========================================================================================================
// IfStmt
// =========================================================================================================

void ast::IfStmt::init(Expr* condition, Block* true_block, Block* false_block) {
	Stmt::init(s_node_type);
	this->condition = condition;
	this->true_block = true_block;
	this->false_block = false_block;
}

ast::IfStmt* ast::IfStmt::Create(CompilerAllocator* allocator, Expr* condition, Block* true_block, Block* false_block) {
	IfStmt* stmt = allocator->allocate_one<IfStmt>();
	stmt->init(condition, true_block, false_block);
	return stmt;
}


// =========================================================================================================
// ForStmt
// =========================================================================================================

void ast::ForStmt::init(Expr* array_expr, Expr* low_expr, Expr* high_expr, Block* block) {
	Stmt::init(s_node_type);
	this->array_expr = array_expr;
	this->low_expr = low_expr;
	this->high_expr = high_expr;
	this->it_var = nullptr;
	this->index_var = nullptr;
	this->block = block;
}

ast::ForStmt* ast::ForStmt::Create(CompilerAllocator* allocator, Expr* array_expr, Block* block) {
	ForStmt* stmt = allocator->allocate_one<ForStmt>();
	stmt->init(array_expr, nullptr, nullptr, block);
	return stmt;
}

ast::ForStmt* ast::ForStmt::Create(CompilerAllocator* allocator, Expr* low_expr, Expr* high_expr, Block* block) {
	ForStmt* stmt = allocator->allocate_one<ForStmt>();
	stmt->init(nullptr, low_expr, high_expr, block);
	return stmt;
}


// =========================================================================================================
// WhileStmt
// =========================================================================================================

void ast::WhileStmt::init(Expr* condition, Block* body) {
	Stmt::init(s_node_type);
	this->condition = condition;
	this->loop_body = body;
}

ast::WhileStmt* ast::WhileStmt::Create(CompilerAllocator* allocator, Expr* condition, Block* body) {
	WhileStmt* stmt = allocator->allocate_one<WhileStmt>();
	stmt->init(condition, body);
	return stmt;
}


// =========================================================================================================
// ReturnStmt
// =========================================================================================================

void ast::ReturnStmt::init(Expr* return_value) {
	Stmt::init(s_node_type);
	this->return_value = return_value;
}

ast::ReturnStmt* ast::ReturnStmt::Create(CompilerAllocator* allocator, Expr* return_value) {
	ReturnStmt* stmt = allocator->allocate_one<ReturnStmt>();
	stmt->init(return_value);
	return stmt;
}


// =========================================================================================================
// ExprStmt
// =========================================================================================================

void ast::ExprStmt::init(Expr* expr) {
	Stmt::init(s_node_type);
	this->expr = expr;
}

ast::ExprStmt* ast::ExprStmt::Create(CompilerAllocator* allocator, Expr* expr) {
	ExprStmt* stmt = allocator->allocate_one<ExprStmt>();
	stmt->init(expr);
	return stmt;
}


// =========================================================================================================
// OperandExpr
// =========================================================================================================

void ast::OperandExpr::init(Operand operand, Expr* lhs, Expr* rhs) {
	Expr::init(s_node_type, nullptr);
	this->operand = operand;
	this->lhs = lhs;
	this->rhs = rhs;
}

ast::OperandExpr* ast::OperandExpr::Create(CompilerAllocator* allocator, Operand operand, Expr* lhs, Expr* rhs) {
	OperandExpr* expr = allocator->allocate_one<OperandExpr>();
	expr->init(operand, lhs, rhs);
	return expr;
}


// =========================================================================================================
// CallExpr
// =========================================================================================================

void ast::CallExpr::init(CompilerAllocator* allocator, Expr* callable, Expr** arguments, i32 arguments_count) {
	Expr::init(s_node_type, nullptr);
	this->callable = callable;
	this->arguments = allocator->copy_array(arguments, arguments_count);
	this->arguments_count = arguments_count;
}

ast::CallExpr* ast::CallExpr::Create(CompilerAllocator* allocator, Expr* callable, Expr** arguments, i32 arguments_count) {
	CallExpr* expr = allocator->allocate_one<CallExpr>();
	expr->init(allocator, callable, arguments, arguments_count);
	return expr;
}


// =========================================================================================================
// ArrayAccessExpr
// =========================================================================================================

void ast::ArrayAccessExpr::init(Expr* array, Expr* index_expr) {
	Expr::init(s_node_type, nullptr);
	this->array_expr = array;
	this->index_expr = index_expr;
}

ast::ArrayAccessExpr* ast::ArrayAccessExpr::Create(CompilerAllocator* allocator, Expr* array, Expr* index_expr) {
	ArrayAccessExpr* expr = allocator->allocate_one<ArrayAccessExpr>();
	expr->init(array, index_expr);
	return expr;
}


// =========================================================================================================
// ArrayAccessExpr
// =========================================================================================================

void ast::CastExpr::init(Expr* expr, Type* target_type) {
	Expr::init(s_node_type, target_type);
	this->expr = expr;
}

ast::CastExpr* ast::CastExpr::Create(CompilerAllocator* allocator, Expr* casted_expr, Type* target_type) {
	CastExpr* expr = allocator->allocate_one<CastExpr>();
	expr->init(casted_expr, target_type);
	return expr;
}

