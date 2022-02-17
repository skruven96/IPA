#ifndef AST_PRINTER_H
#define AST_PRINTER_H
#include "compiler.h"

#include <ostream>
#include <map>

class ModuleCompiler;


class ASTPrinter : public ast::Visitor {
public:

	ASTPrinter(std::ostream& stream)
		: m_stream(stream), m_indention("\n")
	{}

	void visit(ModuleCompiler* module);

	virtual void visit(ast::Scope* scope) override;

	virtual void visit(ast::Variable* variable) override;
	virtual void visit(ast::Struct* structure) override;
	virtual void visit(ast::Function* funnction) override;
	virtual void visit(ast::Block* block) override;

	virtual void visit(ast::LoadExpr* expr) override;

	virtual void visit(ast::IfStmt* expr) override;
	virtual void visit(ast::ForStmt* expr) override;
	virtual void visit(ast::WhileStmt* expr) override;
	virtual void visit(ast::ReturnStmt* expr) override;

	virtual void visit(ast::ExprStmt* expr) override;
	virtual void visit(ast::OperandExpr* expr) override;
	virtual void visit(ast::CallExpr* expr) override;
	virtual void visit(ast::ArrayAccessExpr* expr) override;
	virtual void visit(ast::CastExpr* expr) override;

	void print_type(ast::Type* type);
	void print_decl_name(ast::Decl* decl);

private:
	std::ostream& indent();
	std::ostream& new_segment(const char* name);
	void increment_indention();
	void decrement_indention();

	std::ostream& m_stream;

	bool m_has_printed_indent = false;
	std::string m_indention;
};

#endif // AST_PRINTER_H