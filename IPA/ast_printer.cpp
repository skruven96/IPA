#include "ast_printer.h"
#include "project.h"

#include <assert.h>


void ASTPrinter::visit(ModuleCompiler* module) {
	indent() << "\nModule: " << module->source()->path();
	accept(module->scope());
	m_stream << "\n";										
}

void ASTPrinter::visit(ast::Scope* scope) {
	increment_indention();
	for (i32 i = 0; i < scope->declerations_count; i++) {
		accept(scope->declerations[i]);
	}
	decrement_indention();
}

void ASTPrinter::visit(ast::Variable* variable) {
	indent() << variable->name.to_str() << ": ";
	print_type(variable->type);
	if ((variable->decl_flags & ast::Decl::GLOBAL) == ast::Decl::GLOBAL) m_stream << " (GLOBAL";
	if ((variable->decl_flags & ast::Decl::LOCAL ) == ast::Decl::LOCAL ) m_stream << " (LOCAL";
	if ((variable->decl_flags & ast::Decl::MEMBER) == ast::Decl::MEMBER) m_stream << " (MEMBER";
	m_stream << (((variable->decl_flags & ast::Decl::CONST) == ast::Decl::CONST) ? ", CONSTANT)" : ")");

	ast::Expr* default_value = variable->default_value;
	if (default_value && !((variable->decl_flags & ast::Variable::LOCAL) == ast::Variable::LOCAL)) {
		increment_indention();
		accept(variable->default_value);
		decrement_indention();
	}
}

void ASTPrinter::visit(ast::Struct* structure) {
	indent() << structure->name.to_str() << " :: struct:";
	accept(structure->scope);
}

void ASTPrinter::visit(ast::Function* function) {
	indent() << function->name.to_str() << ": (";
	i32 arg_index = 0;
	while(true) {
		ast::Variable* arg = function->arguments[arg_index];
		m_stream << arg->name.to_str() << ": ";
		print_type(arg->type);

		if (++arg_index != function->arguments_count)
			m_stream << ", ";
		else
			break;
	}
	m_stream << ") -> ";
	print_type(function->return_type);
	m_stream << ":";
	increment_indention();
	accept(function->body);
	decrement_indention();
}

void ASTPrinter::visit(ast::Block* block) {
	accept(block->scope);
	increment_indention();
	for (i32 i = 0; i < block->local_variables_count; i++)
		accept(block->local_variables[i]);
	for (i32 i = 0; i < block->statements_count; i++)
		accept(block->statements[i]);
	decrement_indention();
}

void ASTPrinter::visit(ast::LoadExpr* load_expr) {
	if (load_expr->constant)
	{	indent() << "load constant " << load_expr->constant.to_str() << " -> "; print_type(load_expr->type); }
	else if (!load_expr->structure_expr)
	{	indent() << "load variable " ; print_decl_name(load_expr->loaded_decl); m_stream << " -> "; print_type(load_expr->type); }
	else {
		indent() << "load member "; print_decl_name(load_expr->loaded_decl); m_stream << " -> "; print_type(load_expr->type);
		accept(load_expr->structure_expr);
		decrement_indention();
	}
}

void ASTPrinter::visit(ast::IfStmt* expr) {
	indent() << "if";
	increment_indention(); accept(expr->condition); decrement_indention();
	indent() << "true:";
	increment_indention(); accept(expr->true_block); decrement_indention();
	if (expr->false_block) {
		indent() << "false:";
		increment_indention(); accept(expr->false_block); decrement_indention();
	}
}

void ASTPrinter::visit(ast::ForStmt* stmt) {
	indent() << "for";
	increment_indention();
	if (stmt->array_expr) {
		accept(stmt->array_expr);
	} else {
		accept(stmt->low_expr);
		indent() << "..";
		accept(stmt->high_expr);
	}
	decrement_indention();
	indent() << "body:";
	accept(stmt->block);
}

void ASTPrinter::visit(ast::WhileStmt* stmt) {
	indent() << "while";
	increment_indention(); accept(stmt->condition); decrement_indention();
	indent() << "body:";
	accept(stmt->loop_body);
}

void ASTPrinter::visit(ast::ReturnStmt* return_stmt) {
	indent() << "return";
	if (return_stmt->return_value) {
		increment_indention();
		accept(return_stmt->return_value);
		decrement_indention();
	}
}

void ASTPrinter::visit(ast::ExprStmt* stmt) {
	indent() << "expr-stmt";
	increment_indention(); accept(stmt->expr); decrement_indention();
}

void ASTPrinter::visit(ast::OperandExpr* operand) {
	if (operand->rhs == nullptr) {
		indent() << "operand unary postfix '" << Token::OperandToString(operand->operand) << "' -> ";
	} else if (operand->lhs == nullptr) {
		indent() << "operand unary prefix '" << Token::OperandToString(operand->operand) << "' -> "; 
	} else {	
		indent() << "operand '" << Token::OperandToString(operand->operand) << "' -> "; 
	}
	print_type(operand->type);
	if (operand->lhs) {
		increment_indention();
		accept(operand->lhs);
		decrement_indention();
	}
	if (operand->rhs) {
		increment_indention();
		accept(operand->rhs);
		decrement_indention();
	}
}

void ASTPrinter::visit(ast::CallExpr* expr) {
	indent() << "call";
	increment_indention(); 
	accept(expr->callable);
	for (i32 i = 0; i < expr->arguments_count; i++)
		accept(expr->arguments[i]);
	decrement_indention();
}

void ASTPrinter::visit(ast::ArrayAccessExpr* expr) {
	indent() << "array access";
}

void ASTPrinter::visit(ast::CastExpr* expr) {
	indent() << "cast -> "; print_type(expr->type); m_stream << ":";
	increment_indention();
	accept(expr->expr);
	decrement_indention();
}

void ASTPrinter::print_type(ast::Type* type) {
	if (!type) m_stream << "<unresolved>";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::VoidPrimitive)) m_stream << "void";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::BoolPrimitive)) m_stream << "bool";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::U8Primitive))   m_stream << "s8";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::U16Primitive))  m_stream << "s16";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::U32Primitive))  m_stream << "s32";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::U64Primitive))  m_stream << "s64";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::S8Primitive))   m_stream << "i8";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::S16Primitive))  m_stream << "i16";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::S32Primitive))  m_stream << "i32";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::S64Primitive))  m_stream << "i64";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::F32Primitive))  m_stream << "f32";
	else if (type == ast::Type::GetPrimitiveOrAssert(Primitive::F64Primitive))  m_stream << "f64";
	else {
		assert(false);
	}
}

void ASTPrinter::print_decl_name(ast::Decl* decl) {
	if (decl) {
		m_stream << decl->name.to_str();
	} else {
		m_stream << "<unresolved>";
	}
}

std::ostream& ASTPrinter::indent() {
	m_stream << m_indention;
	return m_stream; 
}

std::ostream& ASTPrinter::new_segment(const char* name) {
	m_stream << m_indention << name << ": ";
	increment_indention();
	return m_stream;
}

void ASTPrinter::increment_indention() { m_indention += "    "; }
void ASTPrinter::decrement_indention() { m_indention.erase(m_indention.size() - 4); }
